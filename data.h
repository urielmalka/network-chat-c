#ifndef DATA_H
#define DATA_H

#define OK 1
#define ERROR 0

#define PORT 9008
#define MAX_USER_SIZE 64
#define MAX_SIZE_MESSAGE 1024

#include <time.h>


typedef struct user_s
{
	int id; // ID socket
	int code_color;
	char username[32];
} User;

typedef struct group_s
{
	int size;
	User* users;
} Group;

typedef struct server_s
{
	int server_socket;
	Group group;
} Server;


typedef struct accept_client_s
{
	int client_socket;
	Server* server;
} AcceptClient;

typedef struct message_s
{
	int code_color;
	char username[32]; 
	char message[1024];
} Message;

typedef struct serverForward_s
{
	Server* Server;
	Message* message;
} ServerForward;

typedef struct messageHelper_s
{
	int network_socket;
	char* username;
} MHelper;


int getRandomNumber();



#endif

