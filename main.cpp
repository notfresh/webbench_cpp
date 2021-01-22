//
// Created by zxzx on 2021/1/21.
//

#include <unistd.h>
#include <sys/param.h>
#include <rpc/types.h>
#include <getopt.h>
#include <strings.h>
#include <cstdio>
#include <cstdlib>

#include "socket.h"

bench_request bench_req;

static const struct option long_options[]=
{
		{"force",no_argument,& bench_req.force,1},
		{"reload",no_argument,& bench_req.force_reload,1},
		{"time",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'?'},
		{"http09",no_argument,NULL,'9'},
		{"http10",no_argument,NULL,'1'},
		{"http11",no_argument,NULL,'2'},
		{"get",no_argument,& bench_req.method,METHOD_GET},
		{"head",no_argument,& bench_req.method,METHOD_HEAD},
		{"options",no_argument,& bench_req.method,METHOD_OPTIONS},
		{"trace",no_argument,& bench_req.method,METHOD_TRACE},
		{"version",no_argument,NULL,'V'},
		{"proxy",required_argument,NULL,'p'},
		{"clients",required_argument,NULL,'c'},
		{NULL,0,NULL,0}
};

//
void usage(void);
//// 外部的三个方法
void build_request(bench_request *req);
int bench(bench_request* req);
void f1(char* chars);
int main(int argc, char *argv[]){
	bench_req.proxyport = 80;
	bench_req.clients = 1;
	bench_req.method=METHOD_GET;
	if(argc==1)
	{
		usage();
		return 2;
	}

	int opt=0;
	int options_index=0;
	char* tmp;
	while((opt=getopt_long(argc,argv,"912Vfrt:p:c:?h",long_options,&options_index))!=EOF ){
		switch(opt){
			case 'c':
				bench_req.clients=atoi(optarg);
				break;
			case  0 :
				break;
			case 'f':
				bench_req.force=1;
				break;
			case 'r':
				bench_req.force_reload=1;
				break;
			case '9':
				bench_req.http10=0;
				break;
			case '1':
				bench_req.http10=1;
				break;
			case '2':
				bench_req.http10=2;
				break;
			case 'V':
				printf("%s\n", PROGRAM_VERSION);
				exit(0);
			case 't':
				bench_req.benchtime=atoi(optarg);
				break;
			case 'p':
				/* proxy server parsing server:port */
				tmp=strrchr(optarg,':');
				bench_req.proxyhost=optarg;
				if(tmp==NULL){
					break;
				}
				if(tmp==optarg){
					fprintf(stderr,"Error in option --proxy %s: Missing hostname.\n",optarg);
					return 2;
				}
				if(tmp==optarg+strlen(optarg)-1){
					fprintf(stderr,"Error in option --proxy %s Port number is missing.\n",optarg);
					return 2;
				}
				*tmp='\0'; // 截断proxyhost和proxyport
				bench_req.proxyport=atoi(tmp+1);
				printf("\nproxy host: %s, proxy port: %d\n", bench_req.proxyhost, bench_req.proxyport );
				break;
			case ':':
			case 'h':
			case '?':
				usage();
				return 2;
				break;// 为了语法通过，加的break,其实根本没用
		}
	}

	if(optind==argc) {
		fprintf(stderr,"webbench: Missing URL!\n");
		usage();
		printf("usage ");
		return 2;
	}

	if(bench_req.clients==0) bench_req.clients=1;
	if(bench_req.benchtime==0) bench_req.benchtime=30;

	/* Copyright */
	fprintf(stderr, "Webbench - Simple Web Benchmark %s %s \n", PROGRAM_VERSION,
			"Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.\n"
	);
	bench_req.url = argv[optind];
    build_request(&bench_req); // 构建请求体
	f1("abcd");

	printf("Runing info: ");

	if(bench_req.clients==1)
		printf("1 client");
	else
		printf("%d clients",bench_req.clients);

	printf(", running %d sec", bench_req.benchtime);

	if(bench_req.force) printf(", early socket close");
	if(bench_req.proxyhost!=NULL) printf(", via proxy server %s:%d",bench_req.proxyhost,bench_req.proxyport);
	if(bench_req.force_reload) printf(", forcing reload");

	printf(".\n");
//	return 0;
    return bench(&bench_req);
}


void usage(void)
{
	fprintf(stderr,
			"webbench [option]... URL\n"
			"  -f|--force               Don't wait for reply from server.\n"
			"  -r|--reload              Send reload request - Pragma: no-cache.\n"
			"  -t|--time <sec>          Run benchmark for <sec> seconds. Default 30.\n"
			"  -p|--proxy <server:port> Use proxy server for request.\n"
			"  -c|--clients <n>         Run <n> HTTP clients at once. Default one.\n"
			"  -9|--http09              Use HTTP/0.9 style requests.\n"
			"  -1|--http10              Use HTTP/1.0 protocol.\n"
			"  -2|--http11              Use HTTP/1.1 protocol.\n"
			"  --get                    Use GET request method.\n"
			"  --head                   Use HEAD request method.\n"
			"  --options                Use OPTIONS request method.\n"
			"  --trace                  Use TRACE request method.\n"
			"  -?|-h|--help             This information.\n"
			"  -V|--version             Display program version.\n"
	);
}

