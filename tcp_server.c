#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "color.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>

//printf("\033[A\033[K");

void serverBoot(int *server_socket);
void *accept_socket(void *arg);
void forward_message(int client_socket, Server * server);
void manage_chat(Server *Server);
void print_all_members(Group* group);

int join_username(int client_socket ,char* username, Group* group);
int add_username(User* new_user, Group* group);
int getColorNumber(int size);
int getColorById(int client_socket,Group* group);


int main()
{
	
	char server_message[MAX_USER_SIZE];

	// create the server socket
	int server_socket;
	serverBoot(&server_socket);

	Server server;

	server.group.size = 0;
	server.server_socket = server_socket;

	manage_chat(&server);


	// close the socket
	close(server_socket);

	return 0;
}

void serverBoot(int *server_socket)
{
    *server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    bind(*server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    listen(*server_socket, 5);
}


void manage_chat(Server *server)
{
	pthread_t accept_socket_t, forward_t;
	
	
	if (pthread_create(&accept_socket_t, NULL, accept_socket, (void *)server) != 0) 
	{
		fprintf(stderr, "Error creating thread\n");
    	}
    	
	
	// Wait for the thread to finish
	if (pthread_join(accept_socket_t, NULL) != 0) {
		fprintf(stderr, "Error joining thread\n");
	}
	

}



void *handle_client(void *arg) {
    AcceptClient *acceptClient = (AcceptClient *) arg;
    
    char username[MAX_USER_SIZE];
		
    
    
    while (1) {
    
    	int bytes_received = recv(acceptClient->client_socket, username, sizeof(username), 0);

        if (bytes_received <= 0) {
            // handle error or client disconnection
            close(acceptClient->client_socket);
            return NULL;
        }
        username[bytes_received] = '\0';

        int val = join_username(acceptClient->client_socket, username, &acceptClient->server->group);
        send(acceptClient->client_socket, &val, sizeof(val), 0);
	
        if (val == 1) {
            printcb(GREEN,"%s Join the chat. \n", username);
            break;
        }
    }
    
    forward_message(acceptClient->client_socket,acceptClient->server);
    
    free(arg);  // Free the memory allocated for the client socket
    
    return NULL;
}



void *accept_socket(void *arg)
{
    int server_socket, client_socket;
    char username[MAX_USER_SIZE];
    
    Server *server = (Server *)arg;
    
    server_socket = server->server_socket;
    

    while (1)
    {
        
  	AcceptClient *acceptClient = malloc(sizeof(AcceptClient));
  	
  	if (acceptClient == NULL) {
            // handle memory allocation failure
            continue;
        }
    	
        acceptClient->client_socket = accept(server_socket, NULL, NULL);
        if (acceptClient->client_socket < 0) {
            // handle error
            free(acceptClient);
            continue;
        } 
        
        acceptClient->server = server;
        
        printc(YELLOW,"A new connection has entered the server ID : %d\n", acceptClient->client_socket);
	
		
		
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, handle_client, acceptClient) != 0) {
	    // handle thread creation failure
	    close(acceptClient->client_socket);
	    free(acceptClient);
	}
	
        
    }

}



int join_username(int client_socket ,char* username, Group* group)
{

	User* users_temp = group->users;
	
	for(int i=0; i<group->size; i++)
	{
		if(strcmp(users_temp[i].username, username) == 0)
			return ERROR;
	}

	// Allocate memory for a new User
	User* new_user = malloc(sizeof(User));

	// Check if memory allocation was successful
	if (new_user == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return ERROR; // Return with error code
	}

	// Initialize the new User
	new_user->id = client_socket; // Example ID
	strcpy(new_user->username, username); 
	new_user->code_color = getColorNumber(group->size);
	
	if(add_username(new_user, group) != OK)
		return ERROR;


	return OK;
}



void forward_message(int client_socket, Server *server) {
    int i;


    while (1) {
    
    	Message *msg = (Message *)malloc(sizeof(Message));
	char * username;
	if (msg == NULL) {
		fprintf(stderr, "Failed to allocate memory for Message\n");
		continue;
	}
        
        ssize_t bytes_received = recv(client_socket, msg, sizeof(Message), 0);
        if (bytes_received <= 0) { // Check for disconnect or error
            if (bytes_received == 0) {
                printf("Client with id %d disconnected\n",client_socket);
            } else {
                perror("recv failed");
            }
            break; // Exit the loop on disconnect or error
        }
        
        printc(BLUE,"[%s]: %s\n",msg->username,msg->message);
        
        msg->code_color = getColorById(client_socket,&server->group);
        	
        for (i = 0; i < server->group.size; i++) {
  	    
  	    	User user = server->group.users[i];
  	    
            if (user.id == client_socket) {
                //printf("Skipping sender: %s : %s\n", msg->username, msg->message);
                continue;
            }
  
	    
            ssize_t bytes_sent = send(user.id, msg, sizeof(Message), 0);
            if (bytes_sent == -1) {
                perror("send failed");
                // Consider whether to break here or just log the error
                continue;
            }
        }
        
         free(msg); // Ensure msg is freed before returning
    }

   
}



int add_username(User* new_user, Group* group) {
    if (new_user == NULL || group == NULL) {
        return -1; // Error: Null pointer
    }
    
    User* temp; // Declare temp outside the if-else block

    if (group->size == 0) {
        temp = malloc(sizeof(User)); // Correct usage of malloc
    } else {
        // Resize the group's users array to accommodate the new user
        temp = realloc(group->users, (group->size + 1) * sizeof(User));
    }

    if (temp == NULL) {
        return -2; // Error: Memory allocation failed
    }

    group->users = temp;

    // Copy the new user into the group's users array
    group->users[group->size] = *new_user;
    group->size++;

    return OK; // Assuming OK is defined elsewhere as a success indicator
}



void print_all_members(Group* group)
{
	
	for (int i=0; i< group->size; i++)
	{
		User user = group->users[i];
		printf("id: %d username: %s\n",user.id,user.username);
	
	}


}

int getColorNumber(int size)
{   
    return size % 7; // random number between 0 and 6
}



int getColorById(int client_socket,Group* group)
{
	for (int i = 0; i < group->size; i++) {
	    
		if (group->users[i].id == client_socket) {
			return group->users[i].code_color;
		}


	}

	return 0;
}



















