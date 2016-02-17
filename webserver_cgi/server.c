#include "webserver.h"

int createServerSocket(int *serverSocketDes, struct addrinfo **res)
{
	struct addrinfo *ip;
	int sockOpt = 1;
	int returnValue = -1;

	for(ip = *res;ip != NULL; ip = ip->ai_next)
	{
		*serverSocketDes = socket(ip->ai_family, ip->ai_socktype, 0);
		setsockopt(*serverSocketDes, SOL_SOCKET, SO_REUSEADDR, &sockOpt, sizeof(sockOpt));
		if(*serverSocketDes == -1)
		{
			continue;
		}
		if(bind(*serverSocketDes, ip->ai_addr, ip->ai_addrlen) == 0)
		{
			break;
		}
		returnValue = 0;
	}

	return returnValue;
}

int setListenState(int *serverSocketDes)
{
	if(listen(*serverSocketDes, 100000) != 0)
	{
		printf("Error : listen error.\n");
		return -1;
	}

	return 0;
}

int setAddress(char *port, struct addrinfo *hints, struct addrinfo **res)
{
	memset(hints, 0, sizeof(*hints));
	(*hints).ai_family = AF_INET;
	(*hints).ai_socktype = SOCK_STREAM;
	(*hints).ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, port, hints, *(&res)))
	{
		printf("Error : getaddrinfo error\n");
		return -1;
	}

	return 0;
}

int setPath(char *path)
{
	strcpy(path, getenv("PWD"));
	
	return 0;
}
