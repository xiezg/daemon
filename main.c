/*************************************************************************
 # File Name: main.c
 # Author: xiezg
 # Mail: xzghyd2008@hotmail.com 
 # Created Time: 2019-07-28 11:36:53
 # Last modified: 2019-08-16 11:39:39
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
#include <sys/stat.h>
#include <sys/types.h>

#define DBBAK_CONFIG_PATH "../conf/config.ini"
#define DOCSAFE_LOG_FILE_PATH "../logs/daemon/daemon.log"

int main( int argc, char** argv ){

    int fd = -1;
    pid_t pid = -1;

    if( ( mkdir( "../logs/http", 0755 ) == -1 ) && ( errno != EEXIST ) ){
        fprintf( stderr, "mkdir  [../logs/http] failed. errno:%d errmsg:%s\n", errno, strerror(errno) );
        return -1;
    }

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
    execl( "dbbak", "dbbak", "-mode", "http", "-v=10", "-log_dir=../logs/http/",  NULL );

    fprintf( stderr, "work process execl[%s] failed. errno:%d errmsg:%s\n", "docsafe-master", errno, strerror( errno ) );

    return 0;
}


