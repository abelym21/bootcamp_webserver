#include "webserver.h"

int  rcvRequest(int *client, char *message)
{
	int rcvbytes = 0;

	memset( (void*)message, (int)'\0', MESSAGE_MAX_LEN);
	rcvbytes = recv(*client, message, MESSAGE_MAX_LEN, 0);

	if(rcvbytes == -1)
	{
		fprintf(stderr, "Error : recv error.\n");
		exit(1);
	}
	else if(rcvbytes == 0)
	{
		fprintf(stderr, "error : revc 0 bytes.\n");
		return -1;
	}
	else
	{
		return rcvbytes;
	}
}

int getContentLengthNum(char *contentLength)
{
	int i = 0;
	int result = 0;
	for(i = 0 ; i < strlen(contentLength); i++)
	{
		result = result*10 + (contentLength[i]-48);
	}

	return result;
}

int getBodyLength(char *message)
{
	char *pBody1;
	char *pBody2;
	char *preBody;
	char *tokBody;
	char msg[MESSAGE_MAX_LEN];

	memcpy(msg, message, MESSAGE_MAX_LEN);

	pBody1 = msg;

	while(pBody2 != NULL)
	{
		pBody2 = strstr(pBody1, "\r\n\r\n");
		if(pBody2 != NULL)
		{
			preBody = pBody2;
			break;
		}
		else
		{
			pBody1 = strchr(pBody1, '\0')+1;
		}
	}
	tokBody = pBody1+4;

	return strlen(tokBody);
}

int parseRequestHeader(char **headerToken, char *message )
{	
	headerToken[0] =  strtok(message, " \n\t");
	if(headerToken[0] == NULL)
	{
		fprintf(stderr, "Error : wrong method.\n");
		return -1;
	}
	headerToken[1] = strtok(NULL, " \n\t");
	if(headerToken[1] == NULL)
	{
		fprintf(stderr, "Error : wrong URI.\n");
		return -1;
	}
	headerToken[2] = strtok(NULL, " \r\n\t");
	if(headerToken[2] == NULL)
	{
		fprintf(stderr, "Error : wrong HTTP Version\n");
		return -1;
	}
	
	return 0;
}

char *checkRequestMethod(char *method)
{
	if(strncmp(method, "GET", 3) == 0)
	{
		return "GET";
	}
	else if(strncmp(method, "POST", 4) == 0)
	{
		return "POST";
	}
	else
	{
		return NULL;
	}
}

int checkRequestURI(char **URI, char *path)
{
	char root[2048] = {'\0'};
	char *tmp;// = malloc(sizeof(char) * 2048);
	if(*URI[0] == '/' && strlen(*URI) != 1)
	{
		memset(root, 0x00, 2048);
		strcpy(root, getenv("PWD"));
		strcat(root, *URI);
	
		tmp = malloc(sizeof(char) * strlen(root));
		strcpy(tmp, root);
		*URI = tmp;

		printf("\nDebugging : hexa address URI = %.4x\n", *URI);
		printf("*********************");
		printf("\nnew URI ;  %s\n", *URI);

		
		return 1;
	}

	if(strcmp(*URI, "/") == 0 || *URI == NULL)
	{
		memset(root, 0x00, 2048);
		strcpy(root, getenv("PWD"));
		*URI =  strcat(root,"/index.html");
		printf("*********************");
		printf("\nnew URI ;  %s\n", *URI);

		return 1;
	}

	//현재 디랙토리가 아니면 접근 못하게 하는 코드.
	/*
	else if(strstr(URI, path) == NULL)
	{
		fprintf(stderr, "Error : wrong URI.\n");
		fprintf(stderr, "Wrong URI : %s\n", URI);
		return -1;
	}
	*/
/*	else if(isCallCGI(URI) == 1)
	{
		return 0;
	} */
	else if(access(*URI, F_OK) != 0)
	{
		fprintf(stderr, "Error : cannot find file.\n");
		return -1;
	}
	else
	{
		return 0;
	}
}

int checkRequestHttpVersion(char *version)
{
	if(strncmp(version, "HTTP/1.0", 8) == 0 )
	{
		return 0;
	}
	else if(strncmp(version, "HTTP/1.1", 8) == 0 )
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


int isCallCGI(char * URI)
{
	if(strstr(URI, ".html") != NULL || strstr(URI, ".htm") != NULL || strstr(URI, ".bmp") ||  strstr(URI, ".jpg"))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
