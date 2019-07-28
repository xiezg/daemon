/*************************************************************************
 # File Name: main.c
 # Author: xiezg
 # Mail: xzghyd2008@hotmail.com 
 # Created Time: 2019-07-28 11:36:53
 # Last modified: 2019-07-28 11:45:45
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DOCSAFE_LOG_FILE_PATH "../logs/daemon/daemon.log"

#define WRITE_PID_TO_FILE()\
    FILE * fp_pidfile;\
    fp_pidfile = fopen( "/var/run/ddr.pid", "w" );\
    fprintf( fp_pidfile, "%d\n", getpid() );\
    fclose( fp_pidfile );

int main( int argc, char** argv ){

    int fd = -1;
    pid_t pid = -1;

    if ( ( fd = open( DOCSAFE_LOG_FILE_PATH, O_RDWR|O_CREAT|O_APPEND, 0644 ) ) == -1 ){
        fprintf( stderr, "open [%s] failed. errno:%d errmsg:%s\n", DOCSAFE_LOG_FILE_PATH, errno, strerror(errno) );
        return -1;
    }

    if( daemon( 1, 1 ) == -1 ){
        fprintf( stderr, "daemon failed. errno:%d errmsg:%s\n", errno, strerror(errno) );
        return -1;
    }

    dup2( fd, STDOUT_FILENO );
    dup2( fd, STDERR_FILENO );

    WRITE_PID_TO_FILE();

RESTART_WORK_PROCESS:
    if( ( pid = fork() )== -1 ){
        fprintf( stderr, "fork workprocess failed. errno:%d errmsg:%s\n", errno, strerror(errno) );
        return -1;
    }

    if( pid == 0 ){
        goto WORKER_PROCESS;
    }

    if( ( pid = wait( NULL ) ) == -1 ){
        fprintf( stderr, "master process wait failed. errno:%d errmsg:%s\n", errno, strerror(errno) );
        exit(-1);
    }

    fprintf( stderr, "work process quit\n" );
    sleep( 5 );
    goto RESTART_WORK_PROCESS;

WORKER_PROCESS:
    if( prctl(PR_SET_PDEATHSIG, SIGTERM) == -1 ){
        fprintf( stderr, "work process PR_SET_PDEATHSIG SIGTERM. errno:%d errmsg:%s\n", errno, strerror( errno ) );
    }

    execl( "dbbak", "dbbak", "-mode", "http", NULL );

    fprintf( stderr, "work process execl[%s] failed. errno:%d errmsg:%s\n", "docsafe-master", errno, strerror( errno ) );

    return 0;
}


