#include <shmAdt.h>

struct shmCdt {
    char * name;
    sem_t  sem;            /* POSIX unnamed semaphore */
    size_t cnt;             /* Number of bytes used in 'buf' */
    char   buf[BUF_SIZE];   /* Data being transferred */
};

shmAdt newShm(const char *name/* , int oflag, mode_t mode */) {
    shmAdt adt = malloc(sizeof(struct shmCdt));

    adt->name = malloc(strlen(name) + 1);

    int fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("Error while creating shared memory");
        exit(1);
    }

    if (ftruncate(fd, sizeof(struct shmCdt)) == -1)
        errExit("ftruncate");

    /* Map the object into the caller's address space */

    struct shmCdt *shmp = mmap(NULL, sizeof(*shmp),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");

    /* Initialize semaphores as process-shared, with value 0 */

    if (sem_init(&shmp->sem, 1, 0) == -1)
        errExit("sem_init-sem1");

    return adt;
}

int unlinkShm(shmAdt shmp) {
    int r = shm_unlink(shmp->name);
    if (r < 0) {
        perror("Error while unlinking shared memory");
        exit(1);
    }

    free(shmp->name);
    free(shmp);

    return r;
}

/* /* Wait for 'sem1' to be posted by peer before touching
    shared memory * /

if (sem_wait(&shmp->sem1) == -1)
    errExit("sem_wait");

/* Convert data in shared memory into upper case * /

for (int j = 0; j < shmp->cnt; j++)
    shmp->buf[j] = toupper((unsigned char) shmp->buf[j]);

/* Post 'sem2' to tell the to tell peer that it can now
    access the modified data in shared memory * /

if (sem_post(&shmp->sem2) == -1)
    errExit("sem_post");

/* Unlink the shared memory object. Even if the peer process
    is still using the object, this is okay. The object will
    be removed only after all open references are closed. * /

shm_unlink(shmpath);

exit(EXIT_SUCCESS); */

shmAdt connectShm(const char *shmpath) {
    /* Open the existing shared memory object and map it
        into the caller's address space */

    int fd = shm_open(shmpath, O_RDWR, 0);
    if (fd == -1)
        errExit("shm_open");

    struct shmCdt *shmp = mmap(NULL, sizeof(*shmp),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");

    return shmp;
}

int writeShm(shmAdt shmp, char * string, size_t len) {
    if (len > BUF_SIZE) {
        perror("String is too long\n");
        exit(EXIT_FAILURE);
    }

    /* Copy data into the shared memory object */
    shmp->cnt = len;
    memcpy(&shmp->buf, string, len);

    /* Tell peer that it can now access shared memory */
    if (sem_post(&shmp->sem) == -1)
        errExit("sem_post");
}

int readShm(shmAdt shmp, char * buff) {
    /* Wait until peer finishes its read operation */
    if (sem_wait(&shmp->sem) == -1)
        errExit("sem_wait");

    /* Display modified data in shared memory */
    return write(buff, shmp->buf, shmp->cnt);
}