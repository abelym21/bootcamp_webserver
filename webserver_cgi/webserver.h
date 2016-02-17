#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

#define PATH_MAX_LEN 2048
#define CLIENT_MAX_NUM 1024
#define MESSAGE_MAX_LEN 100000 
#define PAYLOAD_MAX_LEN 2048

#define _200MESSAGE "HTTP/1.0 200 OK\r\n\r\n"
#define _400MESSAGE "HTTP/1.0 400 Bad Request"
#define _404MESSAGE "HTTP/1.0 404 Not Found"

typedef struct clientsInfoStructure{
     int clientSd[CLIENT_MAX_NUM];
	 int cliNum;
}clientsInfo;


//server.c
int createServerSocket(int* serverSocketDes, struct addrinfo **res);
int setListenState(int *serverSocketDes);
int setAddress(char *port, struct addrinfo *hints, struct addrinfo **res);
int acceptClient(int *serverSocketDes, clientsInfo *clients);
int setPath(char *path);

//connection.c

int acceptClient(int *serverSocketDes, clientsInfo *clients);
int initClientsInfo(clientsInfo *clients);
int arrangeClientsInfo(clientsInfo *clients);
int closeConnection(int *client);


//handle_request.c

int  parseRequestHeader(char **headerToken, char *message);
int  rcvRequest(int *client, char *message);
char *checkRequestMethod(char *method);
int checkRequestURI(char **URI, char *path);
int checkRequestHttpVersion(char *version);

//response.c

int writeResponseMessage(int HttpStatusCode, char *URI, int *client);

//handle_cgi.c

int preProcessCGI(char *message);
int setCgiEnv(char *message);
char *getMethod(char *message);
char *getContentLength(char *message);
char *getQueryString(char *message);
char *getCgiExeName(char *URI, char *path);

//HttpStatusCode.c

char* getHttpStatusMessage(int HttpStatusCode);
#endif
