/*
* (C) Radim Kolar 1997-2004
* This is free software, see GNU Public License version 2 for
* details.
*
* Simple forking WWW Server benchmark:
*
* Usage:
*   webbench --help
*
* Return codes:
*    0 - sucess
*    1 - benchmark failed (server is not on-line)
*    2 - bad param
*    3 - internal error, fork failed
* 
*/ 

//#include "socket.h" // 没有看错，是include
#include <unistd.h>
#include <sys/param.h>
#include <rpc/types.h>
#include <getopt.h>
#include <strings.h>
#include <time.h>
#include <signal.h>
#include <string>
#include <iostream>

/* globals */
// http的版本 @http
/* 0 - http/0.9, 1 - http/1.0, 2 - http/1.1
 * 问题是： 这版本之间有什么区别呢？
 *
 * */

/* internal */

static void benchcore(const char* host,const int port, const char *request);
static int bench(void);
static void alarm_handler(int signal)
{
    timerexpired=1;
}

volatile int timerexpired=0;
int speed=0;
int failed=0;
int bytes=0;

int mypipe[2];
int clients=1; // 这是并发请求客户端的数量
static int bench(void)
{
    int i,j,k;
    pid_t pid=0;
    FILE *f;

    /* check avaibility of target server */
	// step（1）: 首先测试TCP连接是否合法；
    i=Socket(proxyhost==NULL?host:proxyhost,proxyport);
    if(i<0) {
        fprintf(stderr,"\nConnect to server failed. Aborting benchmark.\n");
        return 1;
    }
    close(i);
	// step2 (2）:调用pipe建立管道；
    /* create pipe */
    if(pipe(mypipe))
    {
        perror("pipe failed.");
        return 3;
    }

    /* not needed, since we have alarm() in childrens */
    /* wait 4 next system clock tick */
    /*
    cas=time(NULL);
    while(time(NULL)==cas)
    sched_yield();
    */

    /* fork childs */
    for(i=0;i<clients;i++)
    {
        pid=fork();
        if(pid <= (pid_t) 0) // TODO
        {
            /* child process or error*/
            sleep(1); /* make childs faster */
            break;
        }
    }

    if( pid < (pid_t) 0)
    {
        fprintf(stderr,"problems forking worker no. %d\n",i);
        perror("fork failed.");
        return 3;
    }

    if(pid == (pid_t) 0)
    {
        /* I am a child */
        if(proxyhost==NULL) // 启用代理
            benchcore(host,proxyport,request);
        else
            benchcore(proxyhost,proxyport,request);

        /* write results to pipe */
        f=fdopen(mypipe[1],"w");
        if(f==NULL){
            perror("open pipe for writing failed.");
            return 3;
        }
        /* fprintf(stderr,"Child - %d %d\n",speed,failed); */
        fprintf(f,"%d %d %d\n",speed,failed,bytes);
        fclose(f);
        return 0;
    }
    else
    {
        f=fdopen(mypipe[0],"r");
        if(f==NULL)
        {
            perror("open pipe for reading failed.");
            return 3;
        }

        setvbuf(f,NULL,_IONBF,0);

        speed=0;
        failed=0;
        bytes=0;

        while(1)
        {
            pid=fscanf(f,"%d %d %d",&i,&j,&k); // 从管道中读入3个数字
            if(pid<2) // 只有至多一个进程了
            {
                fprintf(stderr,"Some of our childrens died.\n");
                break;
            }

            speed+=i;
            failed+=j;
            bytes+=k;

            /* fprintf(stderr,"*Knock* %d %d read=%d\n",speed,failed,pid); */
            if(--clients==0) break;
        }

        fclose(f);

        printf("\nSpeed=%d pages/min, %d bytes/sec.\nRequests: %d susceed, %d failed.\n",
            (int)((speed+failed)/(benchtime/60.0f)),
            (int)(bytes/(float)benchtime),
            speed,
            failed);
    }

    return i;
}
//


void benchcore(const char *host,const int port,const char *req){
    int rlen;
    char buf[1500];
    int s,i;
    struct sigaction sa;

    /* setup alarm signal handler */
    sa.sa_handler=alarm_handler;
    sa.sa_flags=0;
    if(sigaction(SIGALRM,&sa,NULL))
        exit(3);

    alarm(benchtime); // after benchtime,then exit

    rlen=strlen(req);
    nexttry:while(1){
        if(timerexpired){
            if(failed>0){
                /* fprintf(stderr,"Correcting failed by signal\n"); */
                failed--;
            }
            return;
        }

        s=Socket(host,port);
        if(s<0) {failed++;continue;}
        if(rlen!=write(s,req,rlen)) {failed++;close(s);continue;}
        if(http10==0)
        if(shutdown(s,1)) { failed++;close(s);continue;}

        if(force==0){ /* read all available data from socket */
            while(1){
                if(timerexpired) break;
                i=read(s,buf,1500);
				/* fprintf(stderr,"%d\n",i); */
				if(i<0) {
					failed++;
					close(s);
					goto nexttry;
				} else if(i==0) break;
				else{
					bytes+=i;
				}
            }
        }

        if(close(s)) { /* 连接提前断开，这里有点看不懂 */
        	failed++;
        	continue;
        }
        speed++;
    }// nexttry end
}

