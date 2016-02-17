#include "webserver.h"

char* getHttpStatusMessage(int HttpStatusCode)
{
	if(HttpStatusCode == 400)
	{
		return _400MESSAGE;
	}
	else if(HttpStatusCode == 404)
	{
		return _404MESSAGE;
	}
	else if(HttpStatusCode == 200)
	{
		return _200MESSAGE;
	}
	else
	{
		fprintf(stderr, "Wrong HttpStatusCode.\n");
		return NULL;
	}

}
