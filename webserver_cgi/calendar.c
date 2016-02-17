#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define REQUEST_METHOD "REQUEST_METHOD"
#define QUERY_STRING "QUERY_STRING"
#define CONTENT_LENGTH "CONTENT_LENGTH"
#define METHOD_GET "GET"
#define METHOD_POST "POST"
#define MAX_PARAMETER 1024
#define ERROR -1
#define INCORRECT -2
#define SUCCESS 1
#define FAIL 0


static int normal[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static int leap[12]   = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct calendar{
	int day;
	int month;
	int year;
	int* dayspermonth;
};

// CGI functions
char* getParamString();
int getParameter(char* buf, struct calendar* c1);
void printPage(int status, int days, struct calendar* birthday, struct calendar* today);

// Calendar functions
int verifyDay(struct calendar* c1);
void getToday(struct calendar* c1);
int isLeapYear(int year);	
int computeDays(struct calendar c1, struct calendar c2);	
int isGreaterThan(struct calendar c1, struct calendar c2);	// c1 > c2?

int main(void)
{
	struct calendar birthday, today;
	int days;
	char* param;
	
	param = getParamString();
	if (param == NULL){
		printPage(ERROR, 0, NULL, NULL);
		return 0;
	}

	getParameter(param, &birthday);
	if (verifyDay(&birthday) == FAIL){
		perror("Birthday verification failed");
		printPage(INCORRECT, 0, NULL, NULL);
		return 0;
	}
	
	getToday(&today);
	if (verifyDay(&today) == FAIL){
		perror("Today verification failed");
		printPage(INCORRECT, 0, NULL, NULL);
		return 0;
	}
	
	if (isGreaterThan(birthday, today)){	// Birthday is future
		days = -computeDays(birthday, today);
	}else{
		days = computeDays(today, birthday);
	}

	printPage(SUCCESS, days, &birthday, &today);
	return 0;
}

char* getParamString()
{
	char* param;
	char* buf;
	int length, count=0, total, offset;
	
	param = getenv(REQUEST_METHOD);
	if (param == NULL){
		perror("REQUEST_METHOD env. variable doesn't exist");
		return NULL;
	}

	if (!strcmp(param, METHOD_GET)){
		buf = malloc(MAX_PARAMETER);
		param = getenv(QUERY_STRING);
		
		if (param == NULL || buf == NULL){
			perror("Memory allocation failed");
			return NULL;
		}

		strncpy(buf, param, MAX_PARAMETER-1);
		buf[MAX_PARAMETER] = '\0';
	}else if(!strcmp(param, METHOD_POST)){
		param = getenv(CONTENT_LENGTH);
		if (param == NULL){
			perror("CONTENT_LENGTH env. variable doesn't exist");
			return NULL;
		}

		length = atoi(param);
		if (length <= 0){
			perror("Content Length is negative");
			return NULL;
		}

		buf = malloc(length + 1);	// To include null character
		
		while(1){
			offset = 0;
			total = 0;
			count = read(STDIN_FILENO, buf + offset, length);
			total += count;
			if (count == 0 || total == length){
				buf[total] = '\0';
				break;
			}
			offset = count;
		}

	}else{
		perror("Requested method is unrecognizable");
		return NULL;
	}
	
	return buf;
}

int getParameter(char* buf, struct calendar *c1)
{
	char* token[3];
	char* temp;
	int i;

	temp = strtok(buf, "&");
	for(i=0; i<3; i++){
		if (temp == NULL){
			perror("& parsing error");
			c1 = NULL;
			return FAIL;
		}
		
		token[i] = malloc(strlen(temp)+1);
		strcpy(token[i], temp);
		
		temp = strtok(NULL, "&");
	}

	
	for(i=0; i<3; i++){
		temp = strtok(token[i], "=");
		if (temp == NULL){
			perror("= parsing error");
			c1 = NULL;
			break;
		}

		if (!strcmp(temp, "year")){
			temp = strtok(NULL, "=");
			c1->year = atoi(temp);
		}else if (!strcmp(temp, "month")){
			temp = strtok(NULL, "=");
			c1->month = atoi(temp);
		}else if (!strcmp(temp, "day")){
			temp = strtok(NULL, "=");
			c1->day = atoi(temp);
		}else{
			perror("Parameters match none of year, month, or day");
			c1 = NULL;
			break;
		}
	}

	free(token[0]);
	free(token[1]);
	free(token[2]);

	if (c1 == NULL){
		return FAIL;
	}else{
		return SUCCESS;
	}
}
		
void printPage(int status, int days, struct calendar* birthday, struct calendar* today)
{
	// print header
	printf("Content-Type: text/html\n\n\n");
	printf("<HTML>\n<HEAD></HEAD>\n<BODY>\n");
	// print body
	switch(status){
		case ERROR:
			printf("I'm sorry but there were some internal problems.<BR>\n");
			printf("Calculation has been failed.<BR>\n");
			break;
		case INCORRECT:
			printf("Your birthday is invalid.<BR>\n");
			printf("Please check and try again.<BR>\n");
			break;
		case SUCCESS:
			printf("Your birthday is %d.%d.%d.<BR>\n", 
					birthday->year, birthday->month, birthday->day);
			printf("Today is %d.%d.%d.<BR>\n",
					today->year, today->month, today->day);
			
			if (days >= 0){
				printf("<BR>You have lived %d days since your birthday.<BR>\n",
						days);
			}else{
				printf("<BR>You have %d days left until your birthday.--;<BR>\n",
						days);
			}
			break;
	}
	// print tail
	printf("</BODY>\n</HTML>\n");
}


int verifyDay(struct calendar *c1)
{
	if (c1->year <= 0){
		perror("The year must be a positive number.\n");
		return FAIL;
	}

	if ( (c1->month <= 0) || (c1->month > 12) ){
		perror("The month is out of range.\n");
		return FAIL;
	}

	if (isLeapYear(c1->year)){
		c1->dayspermonth = leap;
	}else{
		c1->dayspermonth = normal;
	}
	
	if ( (c1->day <= 0) || (c1->day > (c1->dayspermonth[c1->month-1])) ){
		return FAIL;
	}

	return SUCCESS;

}

void getToday(struct calendar* c1)
{
	time_t t;
	struct tm* now;

	t = time(NULL);
	now = localtime(&t);
	
	c1->year  = now->tm_year+ 1900;
	c1->month = now->tm_mon + 1;	// since tm_mon is zero-based
	c1->day   = now->tm_mday;

}
	
int isGreaterThan(struct calendar c1, struct calendar c2)
{
	// If years differ
	if (c1.year > c2.year){
		return 1;
	}else if (c1.year < c2.year){
		return 0;
	}

	// If months differ
	if (c1.month > c2.month){
		return 1;
	}else if (c1.month < c2.month){
		return 0;
	}

	// If days differ
	if (c1.day > c2.day){
		return 1;
	}else{			// including the same days
		return 0;
	}
}

int isLeapYear(int year)
{
	// Definition of a leap year: 
	// a year which is a multiple of 4 and not a multiple of 100.
	// But a year is a leap year if it is a multiple of 400.

	if ( (!(year % 4) && (year % 100)) || (!(year % 400)) ){
		return 1;
	}else{
		return 0;
	}
}

int computeDays(struct calendar c1, struct calendar c2)
{
	int days = 0;
	int i;

	days = c1.day - c2.day;

	for(i=0; i<(c1.month-1); i++){
		days += c1.dayspermonth[i];
	}
	for(i=0; i<(c2.month-1); i++){
		days -= c2.dayspermonth[i];
	}

	for(i=c2.year; i<(c1.year); i++){
		if (isLeapYear(i)){
			days += 366;
		}else{
			days += 365;
		}
	}

	return days;
}


		
