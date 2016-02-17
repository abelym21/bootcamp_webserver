#include <stdio.h>
#include "server.h"

#define MESSAGE_LEN 1024

typedef struct Header{
	char *method;
	char *URI;
	char *HttpVersion;
}requestHeader;

int rcvbyte;
requestHeader *header;
char message[MESSAGE_LEN];

int initHandlingRequest()
{
	initMessage();
	initHeader();
	return 0;
}

int initMessage()
{
	memset( (void *)message, (int)'\0', MESSAGE_LEN);
	return 0;
}

int initHeader()
{
	header = (requestHeader *)malloc(sizeof(requestHeader));
	return 0;
}

int rcvMessage(int clientNumber)
{
	recv(clientNumber, message, MESSAGE_LEN, 0);
	return 0;
}

int parseRequestHeader()
{
	header->method = strtok(message, " \n\t");
	if(header->method == NULL )
	{
		return -1;
	}
	header->URI  = strtok(NULL, " \t\n");
	if(header->URI == NULL)
	{
		return -1;
	}
	header->HttpVersion = strtok(NULL, " \t\n");
	if(header->HttpVersion == NULL)
	{
		return -1;
	}

	return 0;
}

int checkRequestMethod()
{
	if(strncmp(header->method, "GET", 3) == 0)
	{
		return 0;
	}
	else if(strncmp(header->method, "POST", 4) == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int checkRequestURI(char *path)
{	
	if(strstr(header->URI, path) == NULL)		//URI안에 path가 없을 때,
	{
		return -1;	
	}
	else
	{
		if( access(header->URI, 0) == 0)	//file exists.
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
}

int checkRequestHttpVersion()
{
	if( strncmp(header->HttpVersion, "HTTP/1.0", 8) != 0 || strncmp(header->HttpVersion, "HTTP/1.1", 8) != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}



