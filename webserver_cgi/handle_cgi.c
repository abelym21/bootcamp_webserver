#include "webserver.h"

int preProcessCGI(char *message)
{
	char msg[MESSAGE_MAX_LEN];
	
	memcpy(msg, message, MESSAGE_MAX_LEN);
	setCgiEnv(msg);	

	return 0;
}

int setCgiEnv(char *message)
{
	char msg[MESSAGE_MAX_LEN];

	memcpy(msg, message, MESSAGE_MAX_LEN);

	setenv("REQUEST_METHOD", getMethod(msg), 1);
	setenv("CONTENT_LENGTH", getContentLength(msg), 1);
	setenv("QUERY_STRING", getQueryString(msg), 1);
	return 0;
}

char *getMethod(char *message)
{
	char *tokMethod;
	char msg[MESSAGE_MAX_LEN];

	memcpy(msg, message, MESSAGE_MAX_LEN);
	tokMethod = strtok(msg, "");
	strcat(tokMethod, "");

	return tokMethod;
}

char *getContentLength(char *message)
{
	char *pContentLength1;
	char *pContentLength2;
	char *preContentLength;
	char *tokContentLength;
	char msg[MESSAGE_MAX_LEN];

	memcpy(msg, message, MESSAGE_MAX_LEN);

	pContentLength1 = strtok(msg, "");
	while(pContentLength1 != NULL)
	{
		pContentLength2 = strstr(pContentLength1, "Content-Length: ");
		if(pContentLength2 != NULL)
		{
			preContentLength = pContentLength2;
			break;
		}
		else
		{
			pContentLength1 = strchr(pContentLength1, '\0')+2;
		}
	}
	tokContentLength  = strtok(preContentLength, "\r") + strlen("Content-Length: ");
	strcat(tokContentLength, "");

	return tokContentLength;
}

char *getQueryString(char *message)
{
	char *pQueryString1;
	char *pQueryString2;
	char *preQueryString;
	char *tokQueryString;
	char msg[MESSAGE_MAX_LEN];

	memcpy(msg, message, MESSAGE_MAX_LEN);
	
	pQueryString1 = msg;
	while(pQueryString1 != NULL)
	{
		pQueryString2 = strstr(pQueryString1, "\r\n\r\n");
		if(pQueryString2 != NULL)
		{
			preQueryString = pQueryString2;
			break;
		}
		else
		{
			pQueryString1 = strchr(pQueryString1, '\0')+1;
		}
	}
	tokQueryString = strtok(preQueryString, "\"")+4; 
	strcat(tokQueryString,"");
	
	return tokQueryString;
}

char *getCgiExeName(char *URI, char *path)
{
	char *tokExeName;
	
	tokExeName = URI + strlen(path);
	
	return tokExeName;
}

int makePipe(int *fileDes)
{
	if(pipe(fileDes) == -1)
	{
		printf("Error : cannot create pipe.\n");
		return -1;
	}
	
	return 0; 
}

int isBoundary(char *message)
{
	char *pBoundary1 = NULL;

	pBoundary1 = strstr(message, "");
	
	while(pBoundary1 != NULL)
	{
		pBoundary1 = pBoundary1+1;
		if(strstr(pBoundary1, "\r\n\r\n") == NULL)
		{
			pBoundary1 = strstr(message, "");
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

char *removeBoundary(char *message, int size)
{
	char *pBoundary = NULL;
	char msg[MESSAGE_MAX_LEN];

	memcpy(msg, message, size);

	pBoundary = strstr(msg, "");

	while(pBoundary != NULL)
	{
		pBoundary = pBoundary+1;
		if(strstr(pBoundary, "\r\n\r\n") == NULL)
		{
			pBoundary = strstr(msg, "");
		}
		else
		{
			return pBoundary+strlen("\r\n\r\n");
		}
	}
	return "";
}

