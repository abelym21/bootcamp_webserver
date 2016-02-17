#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<pthread.h>


//#define PORT_NUM 8887
#define MESSAGE_MAX_LEN 99999
#define PAYLOAD_MAX_LEN 9999
#define PATH_MAX_LEN 9999
#define CLIENT_NUM 100

char PORT_NUM[4] = "8887";
char _400Error[26] = "HTTP/1.0 400 Bad Request\n";
char _404Error[27] = "HTTP/1.0 404 Not Found\r\n\r\n";
char _405Error[33] = "HTTP/1.0 405 Method Not Allowed\n";
char _200OK[20] = "HTTP/1.0 200 OK\r\n\r\n";
char _404Message[60] = "<!DOCTYPE html>\n<html>\n<body>404 Not Found</body>\n</html>";


struct argu{
	int pid;
	int status;
};

void setServer(char *port, int* socknum)
{
	int sockOpt = 1;
	struct addrinfo hints, *res, *ips;
	
	memset(&hints, 0, sizeof(hints));		//hints를 0으로 초기화.
	hints.ai_family = AF_INET;				//IPv4 주소체계
	hints.ai_socktype = SOCK_STREAM; 		//http는 TCP를 사용하므로.
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, port, &hints, &res)){
		perror("Error : getaddrinfo function error. \n");
		return;
	}

	//이건 공부용으로 해놓은 코드.
	//addrinfo 구조체에 ai_next가 있는 이유 : 하나의 호스트가, 여러 ip주소를 가질 수 있기 때문이다.
	//아래의 코드는 여러 ip주소에 서버 소켓을 생성하는 작업.
	for(ips = res; ips != NULL; ips = ips->ai_next){
		*socknum = socket(ips->ai_family,ips->ai_socktype, 0);
		setsockopt(*socknum, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt));	//bind Error 해결

		if(*socknum == -1){	//소켓 생성 실패 했을 때,
			continue;
		}
		if(bind(*socknum, ips->ai_addr, ips->ai_addrlen) == 0 ){		//소켓이 생성되었고, bind까지 성공했을때,
			break;
		}
	}

	freeaddrinfo(res);		//res 구조체를free.
	
	if(listen(*socknum, 100000) != 0){
		perror("listen() error\n");
		exit(1);
	}
}

//CGI 지원을 위한 함수

int getContentLength(char *c)
{
	int result = 0;
	int i;
	
	for(i = 0 ;i < strlen(c); i++)
	{
		result = result*10 + (c[i]-48);
	}
	return result;
}

//여기까지  CGI  지원을 위한 함수


int response(int clientSockfd, char *root)
{
	char msg[MESSAGE_MAX_LEN];
	char *header[3];
	char payload[PAYLOAD_MAX_LEN];
	int rcvbyte;
	int fd;
	int readbytes;
	//CGI 지원을 위한 변수
	int methodFlag = 0;		//0이면  get, 1이면 post.
	char *queryString = NULL;
	int contentLength = 0;
	char *preContentLength = NULL;
	char *tokContentLength = NULL;
	char *preBody = NULL;
	char *tokBody = NULL;
	
	//여기까지 CGI 지원을 위한 변수
	

	memset( (void*)msg, (int)'\0', MESSAGE_MAX_LEN);	//메시지를 받을 버퍼 초기화.
	rcvbyte = recv(clientSockfd, msg, MESSAGE_MAX_LEN, 0);	//메시지 수신

	if(rcvbyte == -1){
		fprintf(stderr, "Error : recv function Error.\n");
	}
	else if(rcvbyte == 0 ){
		fprintf(stderr, "Error : 0 byte received. \n");
	}
	else{
		printf("--------[REQUEST MESSAGE]-------------\n");
		printf("%s\n", msg);		//수신한 메시지를 출력
		printf("%d\n", strlen(msg));
		printf("--------------------------------------\n");

		header[0] = strtok(msg, " \n\t");	//Request 메시지의 메소드가 들어감. ex) GET
		
		if(strncmp(header[0], "GET", 3) == 0 || strncmp(header[0], "POST", 4) == 0 ){
			header[1] = strtok(NULL, " \t\n");
			header[2] = strtok(NULL, " \t\n");
			
			if(strncmp(header[0], "GET", 3) == 0)
			{
				methodFlag = 0;
			}else{
				methodFlag = 1;
			}

			if(strcmp(header[1], "/") == 0){
				header[1] = "./index.html";
			}

			if( header[2] == NULL || header[1] == NULL){
				printf("%s\n", _400Error);
				write(clientSockfd, _400Error, strlen(_400Error));
			}
			else if(strncmp(header[2], "HTTP/1.0", 8) != 0  && strncmp(header[2], "HTTP/1.1", 8) != 0){	
				printf("%s\n", _400Error);
				write(clientSockfd, _400Error, strlen(_400Error));	
			}
			else{
				//CGI  지원 부분
				if(methodFlag == 0)
				{
					//get일때,
					

				}
				else
				{
					//Post일때,
					preContentLength = strstr(msg, "Content-Length: ");
					tokContentLength = strtok(preContentLength, "\n") + strlen("Content-Length: ");
					contentLength = getContentLength(tokContentLength);
					queryString = (char *)malloc(sizeof(char)*contentLength);
					preBody = strstr(msg, "\n\n") + strlen("\n\n");
					tokBody = strtok(preBody, "\n");

				}




				printf("file open:%s\n", header[1]);	// 파일을 열것임을 출력.
				if((fd = open(header[1], O_RDONLY)) !=  -1){
					printf("%s\n",_200OK);	//200OK 출력
					send(clientSockfd, _200OK, strlen(_200OK), 0);
					while( (readbytes = read(fd,payload, PAYLOAD_MAX_LEN)) > 0 ){
						write(clientSockfd, payload, readbytes);
					}
				}
				else{
					printf("%s\n",_404Error);
					send(clientSockfd, _404Error, strlen(_404Error), 0);
					strcpy(payload, _404Message);
					write(clientSockfd, payload, strlen(_404Message));
				}
			}
		}
		else{
			printf("%s\n", _400Error);
			write(clientSockfd, _400Error, strlen(_400Error));
		}
	}
	shutdown(clientSockfd, SHUT_RDWR);
	close(clientSockfd);
	clientSockfd = -1;
	return 0;
}

int initClientSockFd(int *clientSockFd)
{	
	int i= 0;
	for(i = 0; i <CLIENT_NUM ; i++)
	{
		clientSockFd[i] = -1;
	}
	return 0;
}

void *deleteJombieProcess(void * arguments)
{
	int child_pid;
	struct argu *argus =(struct argu *)arguments;
	while(1){
		child_pid = waitpid(argus->pid, &(argus->status), WNOHANG);
		if(child_pid != 0)
		{
			printf("자식프로세스가 종료되었습니다. \n");
		}
		sleep(1);
	}
}

int main(int argc, char argv[])
{
	int socknum;		//소켓 식별자
	struct sockaddr_in clientaddr;
	char port[6]; 	//strcpy를 사용해야되나?
	char root[PATH_MAX_LEN];
	int clientSockfd;
	int clientSockFd[CLIENT_NUM];
	socklen_t addrlen;
	int conPosition = 0;
	pid_t child_pid;
	pid_t pid;
	int count = 0 ;		//무한 포크 방지용
	int status;
	struct argu arguments;
	pthread_t thread_t;


	strcpy(root, getenv("PWD"));//현재 디렉토리를 root에 받아옴
	strcpy(port, PORT_NUM);		//port번호 설정
	(char*)malloc(strlen(root)*sizeof(char));	//root 설정
	
	printf("--------------[CURRENT DIRECTORY PATH]------------\n");
	printf("%s\n", root);
	printf("--------------------------------------------------\n");

	initClientSockFd(clientSockFd);	//배열 초기화.

	printf("서버 설정을 시작합니다.\n");
	setServer(port, &socknum);		//서버를 설정한다.
	printf("서버 설정이 완료됐습니다.\n");

	while(count < 2){
		addrlen = sizeof(clientaddr);
		clientSockFd[conPosition] = accept(socknum, (struct sockaddr *)&clientaddr, &addrlen);
		
		if(clientSockFd[conPosition] == -1){
			printf("Error : accept 에러\n");
		}
		else{
			if( (pid = fork())  == 0)
			{	
				//자식 프로세스
				response(clientSockFd[conPosition], root);
				printf("프로세스를 종료합니다. \n");
				exit(0);
			}
			else
			{	//부모프로세스
				//좀비 프로세스 제거할 쓰레드 생성
				arguments.pid = pid;
				arguments.status = status;
				if(pthread_create(&thread_t, NULL, deleteJombieProcess, (void *)&arguments) < 0)
				{
					printf("thread  생성 에러 \n");
					return -1;
				}
			}
			printf("abcdefg");
		}
		while(clientSockFd[conPosition] != -1)
		{
			conPosition = ( conPosition + 1)%CLIENT_NUM;
		}
		count++;
		printf("count : %d\n", count);
		//response(clientSockfd, root);		//respond
	}

	printf("서버가 종료되었습니다\n");
	
	return 0;
}
