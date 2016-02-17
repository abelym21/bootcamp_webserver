#include "webserver.h"

#define PORT_NUM "8887"


int main()
{
	char *port = PORT_NUM;
	char path[PATH_MAX_LEN];
	int serverSockDes;
	struct addrinfo hints, *res;
	clientsInfo *clients;
	char message[MESSAGE_MAX_LEN];
	//char *message;
	char *msg = NULL;
	char *tmpMessage;
	char *headerToken[3];
	int HttpStatusCode = 0;
	pid_t pid;
	pid_t exePid;
	int count = 0;
	int pipes1[2];
	int pipes2[2];
	int contentLength = 0;
	int bodyLength = 0;
	int toRcvBytes = 0;
	char buffer[2048];
	char *preDir1;
	char *preDir2;
	char *isHeader;
	
	//message = (char *)malloc(MESSAGE_MAX_LEN);

	setAddress(port, &hints, &res);
	createServerSocket(&serverSockDes, &res);
	setListenState(&serverSockDes);
	setPath(path);
	
	clients = (clientsInfo *)malloc(sizeof(clientsInfo));
	initClientsInfo(clients);
	while(count <5)
	{
		acceptClient(&serverSockDes, clients);
		rcvRequest(&clients->clientSd[clients->cliNum], message);

		printf("reqeust messsage : \n");
		printf("%s\n", message);

		if(parseRequestHeader(headerToken, message) == -1 )
		{
			HttpStatusCode = 400;
		}
		else if(checkRequestMethod(headerToken[0]) == NULL)
		{
			HttpStatusCode = 400;
		}
		else if(checkRequestHttpVersion(headerToken[2]) == -1)
		{
			HttpStatusCode = 400;
		}
		else if(checkRequestURI(&headerToken[1], path) == -1)
		{
			HttpStatusCode = 404;
		}
		else
		{
			HttpStatusCode = 200;
			
			if(checkRequestMethod(headerToken[0]) == "POST")	
			{
				contentLength = getContentLengthNum(getContentLength(message));
				bodyLength = getBodyLength(message);
				if(contentLength > bodyLength)
				{
					toRcvBytes = contentLength;

					while(toRcvBytes > 0)
					{
						 rcvRequest(&clients->clientSd[clients->cliNum], message);
						 isHeader = strstr(message, "HTTP");
						 if(isHeader != NULL)
						 {
						 	tmpMessage = &message[strlen(getQueryString(message))];
							toRcvBytes = toRcvBytes - strlen(tmpMessage);
						 }
						 else
						 {
						 	tmpMessage = realloc(tmpMessage, strlen(tmpMessage) + strlen(message));
							tmpMessage = strcat(tmpMessage, message);
							toRcvBytes = toRcvBytes - strlen(message);
						 }
					}
				}
			/*	if(isBoundary(message))
				{
					message = realloc(message, strlen(tmpMessage));
					//strcpy(message, removeBoydartmpMessage)
					
					//message = strdup(removeBoundary(tmpMessage), stlen(tmpMessage));
					strcpy(message, removeBoundary(tmpMessage));
					printf("result : %s", message);
				}*/
			}
			pid = fork();
			if( pid  == 0)
			{

				if(isCallCGI(headerToken[1]) == 0)
				{
				printf("\nDebugging : hexa address URI = %.4x\n", headerToken[1]);
				printf("Debugging: headertoken_1 : %s\n", headerToken[1]);
					writeResponseMessage(HttpStatusCode, headerToken[1],&clients->clientSd[clients->cliNum]);
				}
				else
				{	
					preProcessCGI(message);
					makePipe(pipes1);
					makePipe(pipes2);
					
					if( (exePid = fork()) == 0 )
					{
						//child process
						pipes1[0] = dup2(pipes1[0], STDIN_FILENO);
						pipes2[1] = dup2(pipes2[1], STDOUT_FILENO);
						close(pipes1[1]);		//use pipes1[0] = read mode
						close(pipes2[0]);		//use pipes2[1] = write mode
						
						preDir1 = strrchr(headerToken[1], '/');
						preDir2 = malloc(strlen(".")+strlen(preDir1));
						preDir1 = malloc(strlen(preDir1));
						strcpy(preDir1, strrchr(headerToken[1], '/'));
						strcpy(preDir2, ".");
						strcat(preDir2, preDir1);
						
						if(execvp(preDir2, NULL) == -1)
						{
							perror("fail\n");
						}
						printf("pid\n");
						exit(0);
					}
					else
					{	
					
						pipes1[1] = dup2(pipes1[1], STDOUT_FILENO);
						pipes2[0] = dup2(pipes2[0], STDIN_FILENO);
						close(pipes1[0]);   //use pipes1[1] = write mode
						close(pipes2[1]);   //use pipes2[0] = read mode
						write(pipes1[1], getQueryString(message), strlen(getQueryString(message)));
						read(pipes2[0], buffer, 1024);
						close(pipes1[1]);	
						close(pipes2[0]);
						writeResponseHeader(HttpStatusCode, &clients->clientSd[clients->cliNum]);
						send(clients->clientSd[clients->cliNum],buffer ,strlen(buffer) ,0);
					}
				
				}
				closeConnection(&clients->clientSd[clients->cliNum]);
				arrangeClientsInfo(clients);
			}else{
				count++;
			}
		}
	}

	return 0;
}

