// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shmAdt.h>

struct shmCdt {
    char   name[SHM_NAME_SIZE];
    char   buf[SHM_BUFF_SIZE];   /* Data being transferred */
    sem_t  sem;            /* POSIX unnamed semaphore */
    size_t readIndex;
    size_t writeIndex;
};

shmAdt newShm(const char *name/* , int oflag, mode_t mode */) {
    int fd = shm_open(name, O_CREAT | O_TRUNC | O_RDWR, 0600);
    if (fd < 0) {
        perror("Error while creating shared memory");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof(struct shmCdt)) == -1) {
        perror("Error while truncating shared memory");
        exit(EXIT_FAILURE);
    }

    /* Map the object into the caller's address space */

    struct shmCdt *shmp = mmap(NULL, sizeof(*shmp),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("Error while mapping shared memory");
        exit(EXIT_FAILURE);
    }

    strcpy(shmp->name, name);
    shmp->readIndex = 0;
    shmp->writeIndex = 0;

    /* Initialize semaphores as process-shared, with value 0 */

    if (sem_init(&shmp->sem, 1, 0) == -1) {
        perror("Error while initializing semaphore");
        exit(EXIT_FAILURE);
    }

    return shmp;
}

void deleteShm(shmAdt shmp) {
    char shmpName[SHM_NAME_SIZE];
    strcpy(shmpName, shmp->name);

    if (sem_destroy(&shmp->sem) == -1) {
        perror("Error while destroying semaphore");
        exit(EXIT_FAILURE);
    }

    if (munmap(shmp, sizeof(*shmp)) == -1) {
        perror("Error while unmapping shared memory");
        exit(EXIT_FAILURE);
    }

    if (shm_unlink(shmpName) < 0) {
        perror("Error while unlinking shared memory");
        exit(EXIT_FAILURE);
    }
}

shmAdt connectShm(const char *shmpath) {
    /* Open the existing shared memory object and map it
        into the caller's address space */

    int fd = shm_open(shmpath, O_RDWR, 0);
    if (fd == -1) {
        perror("Error while opening shared memory");
        exit(EXIT_FAILURE);
    }

    struct shmCdt *shmp = mmap(NULL, sizeof(*shmp),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("Error while mapping shared memory");
        exit(EXIT_FAILURE);
    }

    return shmp;
}

int closeShm(shmAdt shmp) {
    if (munmap(shmp, sizeof(*shmp)) == -1) {
        perror("Error while unmapping shared memory");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void writeShm(shmAdt shmp, char * string, size_t len) {
    if (len > SHM_BUFF_SIZE) {
        perror("String is too long\n");
        exit(EXIT_FAILURE);
    }

    if (shmp->writeIndex + len > SHM_BUFF_SIZE) {
        perror("Buffer is full\n");
        exit(EXIT_FAILURE);
    }

    // Check if string is valid
    if (string[len] != '\0') {
        perror("String is not null terminated\n");
        exit(EXIT_FAILURE);
    }

    snprintf(&(shmp->buf[shmp->writeIndex]), len + 1, "%s", string);
    shmp->writeIndex += len + 1;

    /* Tell peer that it can now access shared memory */
    if (sem_post(&shmp->sem) == -1) {
        perror("Error while doing sem_post");
        exit(EXIT_FAILURE);
    }
}

void readShm(shmAdt shmp, char * buff) {
    /* Wait until peer finishes its read operation */
    if (sem_wait(&shmp->sem) == -1) {
        perror("Error while doing sem_wait");
        exit(EXIT_FAILURE);
    }
   
    shmp->readIndex += sprintf(buff, "%s", &(shmp->buf[shmp->readIndex])) + 1;
    if (shmp->readIndex >= shmp->writeIndex) {
        shmp->readIndex = 0;
        shmp->writeIndex = 0;
    }
}