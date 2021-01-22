/* $Id: socket.c 1.1 1995/01/01 07:11:14 cthuang Exp $
 *
 * This module has been modified by Radim Kolar for OS/2 emx
 */

/***********************************************************************
  module:       socket.c
  program:      popclient
  SCCS ID:      @(#)socket.c    1.5  4/1/94
  programmer:   Virginia Tech Computing Center
  compiler:     DEC RISC C compiler (Ultrix 4.1)
  environment:  DEC Ultrix 4.3
  description:  UNIX sockets code.
 ***********************************************************************/


#ifndef CPP_LEARN_A_H
#define CPP_LEARN_A_H

#include <cstdlib>


extern char* PROGRAM_VERSION;
#define METHOD_GET 0
#define METHOD_HEAD 1
#define METHOD_OPTIONS 2
#define METHOD_TRACE 3

struct bench_request{
	int method;
	char* url; // 用户传进来的url
	int http10; // http版本，根据请求方法来定
	char* proxyhost;
	int proxyport; // 接受的代理服务器
	char* host;
	char *request;// 生成的请求报文

	int clients;
	int benchtime;

	int force;
	int force_reload;

	bench_request(){
		http10=1;
		method=METHOD_GET;
		clients=1; // 这是一个整数
		force=0;
		force_reload=0;
		proxyport=80;
		proxyhost=NULL;
		host= NULL ;
		benchtime=30; // 默认执行30秒
		request= NULL ;

	}
};

int Socket(const char *host, int clientPort);

#endif