# SO_TP1
System in C language which calculates md5 hashes of files using slaves.
The OS consist of three processes (app.c, view.c, and slave.c).

## Project Structure

* app.c is the main process, it creates the slaves required in order to calculate all md5 hashes and it opens the shared memory required in order to communicate with the view.c file. 
* slave.c is the function that control the slaves' actions. Receives multiple files and delegates individual files to it child for them to calculate md5 hashes.
* view.c accesses the output of the application process in real time using the shared memory created by the app process
* include folder includes app.h, shmAdt.h, slave.h, utils.h and view.h libraries to code app, slave, shmAdt, utils and view
* utils folder includes functions of files and file descriptors manipulations.


## Requirements
Programs have to be compiled and run using the following docker container:
```
user@linux:~$ docker pull agodio/itba-so:2.0
user@linux:~$ cd ./path/to/SO_TP1
user@linux:~$ docker run -v ${PWD}:/root --privileged -ti --name SO agodio/itba-so:2.0
user@docker:~$ cd root
```

