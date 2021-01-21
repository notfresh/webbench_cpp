#include <string>
#include <zxlib/using_std.h>

using namespace std;
const char* PROGRAM_VERSION = "1.5";

string request; // REQUEST_SIZE= 2048; // 一个请求 2K 字节啊
string host;
int proxyport=80;
int http10=1;

enum method_t { METHOD_GET, METHOD_HEAD, METHOD_OPTIONS, METHOD_TRACE };
method_t method = METHOD_GET;
int force_reload = 0;
string proxyhost ;


// 根据参数行命令建立一个Http请求，并将http请求放入到全局数据request中。
void build_request(const char *url){
	string url_str(url);
	// 这坨代码什么意思呢？
	if(force_reload && proxyhost!="" && http10<1) http10=1;
	if(method==METHOD_HEAD && http10<1) http10=1;
	if(method==METHOD_OPTIONS && http10<2) http10=2;
	if(method==METHOD_TRACE && http10<2) http10=2;

	switch(method)
	{
		default:
		case METHOD_GET: request.append("GET");break;
		case METHOD_HEAD: request.append("HEAD");break;
		case METHOD_OPTIONS: request.append("OPTIONS");break;
		case METHOD_TRACE: request.append("TRACE");break;
	}

	request.append(" ");

	if(url_str.find("://") == -1){
		cerr << "\n" << url_str << ": is not a valid URL." << endl;
		exit(2);
	}
	if(url_str.size()>1500){
		cerr << "URL is too long." << endl;
		exit(2);
	}
	if ( url_str.substr(0,7) != "http://"){
		cerr << "Only HTTP protocol is directly supported, set --proxy for others." << endl;
		exit(2);
	}

	/* protocol/host delimiter */
	int i=url_str.find("://")+3; //举个例子， http://www.baidu.com, 现在等于www的下标


	if(url_str.find('/', i)==-1) {
		cerr << "Invalid URL syntax - hostname don't ends with '/'." << endl;
		exit(2);
	}

	cout << "@proxy" << proxyhost << endl;

	if(proxyhost == ""){
		/* http://www.jianshu.com/ */
		int port_start = url_str.find(":", i);
		int uri_start = url_str.find("/", i);
		if(port_start != -1 && port_start < uri_start) // i就是 baidu.com的下标
		{
			host = url_str.substr(i, port_start-i);
			string port = url_str.substr(port_start+1, uri_start-port_start-1);
			proxyport=stoi(port);
			cout << "host: " << host << ", port: " << port << endl;
			if(proxyport==0) proxyport=80;
		}
		else{
			host = url_str.substr(i, uri_start-i);
		}
		request.append(url_str.substr(uri_start));
	}
	else{
		// printf("ProxyHost=%s\nProxyPort=%d\n",proxyhost,proxyport);
		request.append(url_str);
	}

	if(http10==1)
		request.append(" HTTP/1.0");
	else if (http10==2)
		request.append(" HTTP/1.1");
	request.append("\r\n"); // 拼接换行符

	if(http10>0){
		request.append(string("User-Agent: WebBench") + PROGRAM_VERSION + "\r\n" );
	}

	if(proxyhost=="" && http10>0){
		request.append(string("Host: ") + host + "\r\n");
	}

	if(force_reload && proxyhost!=""){
		request.append("Pragma: no-cache\r\n");
	}

	if(http10>1)
		request.append("Connection: close\r\n");

	/* add empty line at end */
	if(http10>0) request.append("\r\n");

	cout << "\nRequest:\n" << request << endl;
}

int test_buid_request(){
	char* url = "http://www.jianshu.com/";
	build_request(url);
	cout << host << endl;
}

int main(){
	test_buid_request();
}