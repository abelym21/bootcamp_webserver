#include "webserver.h"


int writeResponseMessage(int HttpStatusCode, char *URI, int *client)
{
	int fileDescriptor = 0;
	int bytesToRead = 0;
	char *HttpStatusMessage = NULL;
	char payload[PAYLOAD_MAX_LEN];

	HttpStatusMessage = getHttpStatusMessage(HttpStatusCode);

	printf("\ndebugging point1\n");
	printf("URI: %s\n", URI);

	if( (fileDescriptor = open(URI, O_RDONLY)) != -1 )
	{
		send(*client, HttpStatusMessage, strlen(HttpStatusMessage), 0);
		while( (bytesToRead = read(fileDescriptor, payload, PAYLOAD_MAX_LEN)) > 0)
		{
			write(*client, payload, bytesToRead);
		}
	}

	return 0;
}

int writeResponseHeader(int HttpStatusCode, int *client)
{
	char * HttpStatusMessage = NULL;

	HttpStatusMessage = getHttpStatusMessage(HttpStatusCode);
	send(*client, HttpStatusMessage, strlen(HttpStatusMessage), 0);
	
	return 0;
}


