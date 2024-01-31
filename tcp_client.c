#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include "data.h"
#include "color.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <pthread.h>

int signin(int network_socket, char *username, size_t username_size);
int validate_username(const char *username);

void chatting(int network_socket, char* username);
void *send_message(void *arg);
void *recv_message(void *arg);
void remove_last_line();

int main()
{
	// create a socket
	int network_socket;
	char username[MAX_USER_SIZE];

	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	// specify an address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));

	// check for error with the connection
	if (connection_status == -1)
	{
	printf("There was an error making a connection to the remote socket\n\n");
	return 1; // exit with an error
	}

	signin(network_socket, username, MAX_USER_SIZE);
	
	chatting(network_socket, username);

	// close the socket
	close(network_socket);

	return 0;
}


int signin(int network_socket, char *username, size_t username_size) {
	int buffer = 0;    
	printf("Pick a username: ");

	int valid;
    
	while (1) {

		fgets(username, username_size, stdin);

		// Remove newline character if present
		username[strcspn(username, "\n")] = '\0';

		valid = validate_username(username);

		if(valid)
		{
			send(network_socket, username, strlen(username), 0);
			// Read the integer sent by the server
			ssize_t valread = recv(network_socket, &buffer, sizeof(buffer), 0);

			if (valread < 0) {
				perror("recv failed");
				exit(EXIT_FAILURE);
			}

			if (buffer) {
				printcb(GREEN,"You join the chat..\n");
				break;
			}

			printc(RED,"Existing username, pick another username: ");

		} else  
			printc(RED,"Invalid username [A-Za-z0-9] 4-12 letters, pick another username: ");

		memset(username, '\0', sizeof(username));
	}
}



void chatting(int network_socket, char* username)
{
	pthread_t send_t,recv_t;
	
	MHelper* helper = (MHelper*)malloc(sizeof(MHelper));
	if (helper == NULL) {
		// Handle allocation failure
		fprintf(stderr, "Failed to allocate memory for MHelper\n");
		return;
	}

	helper->username = username;
	helper->network_socket = network_socket;
	

	if (pthread_create(&send_t, NULL, send_message, (void *)helper) != 0) 
	{
		fprintf(stderr, "Error creating thread\n");
    	}
    	
    	if (pthread_create(&recv_t, NULL, recv_message, (void *)helper) != 0) 
	{
		fprintf(stderr, "Error creating thread\n");
    	}
    	
    	// Wait for the thread to finish
	if (pthread_join(send_t, NULL) != 0) {
		fprintf(stderr, "Error joining thread\n");
	}
	
	    	// Wait for the thread to finish
	if (pthread_join(recv_t, NULL) != 0) {
		fprintf(stderr, "Error joining thread\n");
	}

}


void *send_message(void *arg)	
{
	MHelper * mHelper = (MHelper *)arg;
	
	int network_socket = mHelper->network_socket;
	
	
	while(1)
	{
		Message *msg = (Message *)malloc(sizeof(Message));
		if (msg == NULL) {
			fprintf(stderr, "Failed to allocate memory for Message\n");
		}
		
		strncpy(msg->username, mHelper->username, sizeof(msg->username) - 1);
		msg->username[sizeof(msg->username) - 1] = '\0'; // Ensure null-termination

		fgets(msg->message, sizeof(msg->message), stdin);
		
		// Remove the newline character if present
		size_t len = strlen(msg->message);
		if (len > 0 && msg->message[len - 1] == '\n') {
		    msg->message[len - 1] = '\0';
		}
		
		ssize_t bytes_sent = send(network_socket, msg, sizeof(Message), 0);
		if (bytes_sent == -1) {
			perror("send failed");
			continue; // Indicates an error occurred
		}
		
		remove_last_line();
		
		printf("[%s]: %s\n", mHelper->username, msg->message);
		
		free(msg);
		
	}
	
	
}

void *recv_message(void *arg) {

    MHelper * mHelper = (MHelper *)arg;
    
    int network_socket = mHelper->network_socket;
    

    while (1) {
        
        Message *msg = (Message *)malloc(sizeof(Message));
	if (msg == NULL) {
	    fprintf(stderr, "Failed to allocate memory for Message\n");
	    continue;
	}
		

        // Correctly receive data directly into the allocated Message structure
        int bytes_received = recv(network_socket, msg, sizeof(Message), 0);

        if (bytes_received <= 0) {
            // Check for errors or closed connection
            if (bytes_received == 0) {
                printf("The client has closed the connection.\n");
            } else {
                perror("recv failed");
            }
            free(msg);
            break; // Exit the loop if connection is closed or an error occurred
        }
	
        // Ensure the message is null-terminated to prevent printf from reading out of bounds
        msg->username[sizeof(msg->username) - 1] = '\0';
        msg->message[sizeof(msg->message) - 1] = '\0';

        printc(msg->code_color,"[%s]: %s\n", msg->username, msg->message);
        printf("");

	free(msg);
    }
    
    

    return NULL;
}


void remove_last_line()
{
	printf("\033[A\033[2K"); // Move up and clear line
}


int validate_username(const char *username) 
{
	regex_t regex;
	int reti;

	// Compile regular expression
	// ^[A-Za-z0-9]{5,16}$ explains:
	// ^ : start of string
	// [A-Za-z0-9] : allowed characters (letters and numbers)
	// {5,16} : minimum 5 characters and maximum 16 characters
	// $ : end of string
	reti = regcomp(&regex, "^[A-Za-z0-9]{4,12}$", REG_EXTENDED);

	if (reti) {
		fprintf(stderr, "Could not compile regex\n");
		return 0; // 0 indicates invalid username
	}

	// Execute regular expression
	reti = regexec(&regex, username, 0, NULL, 0);

	// Free compiled regular expression if you want to use the regex_t again
	regfree(&regex);

	if (!reti) {
		return 1; // Match found - valid username
	} else if (reti == REG_NOMATCH) {
		return 0; // No match - invalid username
	} else {
		char msgbuf[100];
		regerror(reti, &regex, msgbuf, sizeof(msgbuf));
		fprintf(stderr, "Regex match failed: %s\n", msgbuf);
		return 0; // Error occurred
	}

}
	
