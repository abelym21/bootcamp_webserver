#include "webserver.h"

int initClientsInfo(clientsInfo *clients)
{
	int i = 0;

	for(i=0 ; i < CLIENT_MAX_NUM ; i++)
	{
		clients->clientSd[i] = -1;
	}
	clients->cliNum = 0;

	return 0;
}

int acceptClient(int *serverSocketDes, clientsInfo *clients)
{
	socklen_t addrlen;
	struct sockaddr_in clientaddr;

	addrlen = sizeof(clientaddr);
	clients->clientSd[clients->cliNum] = accept(*serverSocketDes, (struct sockaddr *)&clientaddr, &addrlen);
	if(clients->clientSd[clients->cliNum] == -1)
	{
		printf("Error : accept error.\n");
		return -1;
	}

	return 0;
}

int arrangeClientsInfo(clientsInfo *clients)
{
	while(clients->clientSd[clients->cliNum] != -1)
	{
		clients->cliNum = (clients->cliNum + 1) & CLIENT_MAX_NUM;
	}

	return 0;
}

int closeConnection(int *client)
{
	shutdown(*client, SHUT_RDWR);
	close(*client);
	*client = -1;

	return 0;
}

void *deleteJombieProcess(int* pid, int* status)
{
	int child_pid;
	
	while(1)
	{
		child_pid = wait(pid, &(*status), WNOHANG );
	}
}
