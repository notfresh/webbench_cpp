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

#define METHOD_GET 0
#define METHOD_HEAD 1
#define METHOD_OPTIONS 2
#define METHOD_TRACE 3
int method=METHOD_GET;

int force=0;
int force_reload=0;
int http10=1;

int proxyport=80;
char *proxyhost=NULL;
int benchtime=30; // 默认执行30秒
const char* PROGRAM_VERSION = "1.5";

int clients=1; // 这是一个整数

static const struct option long_options[]=
		{
				{"force",no_argument,&force,1},
				{"reload",no_argument,&force_reload,1},
				{"time",required_argument,NULL,'t'},
				{"help",no_argument,NULL,'?'},
				{"http09",no_argument,NULL,'9'},
				{"http10",no_argument,NULL,'1'},
				{"http11",no_argument,NULL,'2'},
				{"get",no_argument,&method,METHOD_GET},
				{"head",no_argument,&method,METHOD_HEAD},
				{"options",no_argument,&method,METHOD_OPTIONS},
				{"trace",no_argument,&method,METHOD_TRACE},
				{"version",no_argument,NULL,'V'},
				{"proxy",required_argument,NULL,'p'},
				{"clients",required_argument,NULL,'c'},
				{NULL,0,NULL,0}
		};

void usage(void);

int main(int argc, char *argv[]){
	char *tmp=NULL;
	if(argc==1)
	{
		usage();
		return 2;
	}

	int opt=0;
	int options_index=0;
	while((opt=getopt_long(argc,argv,"912Vfrt:p:c:?h",long_options,&options_index))!=EOF )
	{
		switch(opt)
		{
			case  0 : break;
			case 'f': force=1;break;
			case 'r': force_reload=1;break;
			case '9': http10=0;break;
			case '1': http10=1;break;
			case '2': http10=2;break;
			case 'V': printf("%s\n", PROGRAM_VERSION);exit(0);
			case 't': benchtime=atoi(optarg);break;
			case 'p':
				/* proxy server parsing server:port */
				tmp=strrchr(optarg,':');
				proxyhost=optarg;
				if(tmp==NULL)
				{
					break;
				}
				if(tmp==optarg)
				{￿
					fprintf(stderr,"Error in option --proxy %s: Missing hostname.\n",optarg);
					return 2;
				}
				if(tmp==optarg+strlen(optarg)-1)
				{
					printf("@tmp: %s", tmp);
					fprintf(stderr,"Error in option --proxy %s Port number is missing.\n",optarg);
					return 2;
				}
				*tmp='\0'; // 正常取到port了
				proxyport=atoi(tmp+1);break;
			case ':':
			case 'h':
			case '?': usage();return 2;break;
			case 'c': clients=atoi(optarg);break;
		}
	}

	if(optind==argc) {
		fprintf(stderr,"webbench: Missing URL!\n");
//		usage();
		printf("usage ");
		return 2;
	}

	if(clients==0) clients=1;
	if(benchtime==0) benchtime=30;

	/* Copyright */
	fprintf(stderr, "Webbench - Simple Web Benchmark %s %s \n", PROGRAM_VERSION,
			"Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.\n"
	);

//    build_request(argv[optind]);
	printf("Runing info: ");

	if(clients==1)
		printf("1 client");
	else
		printf("%d clients",clients);

	printf(", running %d sec", benchtime);

	if(force) printf(", early socket close");
	if(proxyhost!=NULL) printf(", via proxy server %s:%d",proxyhost,proxyport);
	if(force_reload) printf(", forcing reload");

	printf(".\n");
	return 0;
//    return bench();
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