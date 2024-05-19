// Header file includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define MAX_SONGS 3

// Struct to store arguments of send_msg function which needs to be passed to the threads
typedef struct thread_args
{
	int client_socket;
	char **song_names;	
}
thread_args;

// Error function
void error(const char* msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

// Function to modify an array of pointers to chars to store paths to the mp3 files
void get_mp3_path_list(char *song_names[], char *directory)
{
	struct dirent *file_in_d;
	DIR *d = opendir(directory);
	if (d)
	{
		int i=0;
		char *file_name;
		while((file_in_d = readdir(d)) != NULL)
		{
			file_name = file_in_d->d_name;
			// Compare last 4 characters of file name with .mp3
			if (strcmp((file_name+strlen(file_name)-4),".mp3")==0)
			{	
				//Malloc enough space for full path length and NULL character
				song_names[i]=malloc((strlen(directory)+strlen(file_name)+2)*sizeof(char));
				strcpy(song_names[i],directory);
				strcat(song_names[i],"/");
				strcat(song_names[i],file_name);
				i++;
			}
		}
		closedir(d);
	}
	else
	{
		error("Invalid path");
	}
}


// Function to send dong data to client
void send_msg(int chosen, char *song_names[],int client_socket) 
{
	if(!((1<=chosen) && (chosen<=MAX_SONGS)))
	{
		error("Invalid input from client");
	}
	FILE *fp;
	if(!(fp = fopen(song_names[chosen-1], "rb")))
	{
		error("Error opening requested file");
	}
	char msg[4096];
	ssize_t bytes_read, bytes_sent;
	// Read the file in packets of 4096 bytes each and send it to client
	while ((bytes_read = fread(msg, 1, 4096, fp)) > 0)
	{
		if((bytes_sent=send(client_socket, msg, bytes_read, 0))==-1)
		{
			error("Error sending msg to client");
		}
        }
        fclose(fp);	
}

// Function called by pthread_create
void *thread_handle_client(void *t_args)
{
	// Get the arguments from the void pointer
	thread_args *args = (thread_args *)t_args;
	int c_sock = args->client_socket;
	char **song_names = args->song_names;
	
	// Wait for a message from client and store it in buffer
	char buffer[BUFFER_SIZE];
	while (1)
	{
		if((recv(c_sock, buffer, BUFFER_SIZE, 0) > 0))
		{
			printf("Song selected: %s",buffer);
			break;
		}
		bzero(buffer, BUFFER_SIZE);
	}

	//Send the song data to client based in client's choice
	send_msg(atoi(buffer), song_names, c_sock);

	// Close sockets and free the arguments struct
	close(c_sock);
	free(args);
	
	return NULL;
}

int main(int argc, char*argv[]) 
{
	// Take Port number and root directory of the mp3 song folder as arguments
	// Print error if no. of arguments is not 2
	if (argc!=3) 
	{
		error("Please give port number (P) and root directory (DIR) only as arguments");
	}	
	int P=atoi(argv[1]);
	char *directory = argv[2];
	
	// Get Path to mp3 files
	char *song_names[3];
	get_mp3_path_list(song_names, directory);
	
	// Initialising 7 thread ids and index to track threads
	pthread_t thread_id[7];
	int thread_index = 0;
	
	// Initialising the buffer and the client and server sockets, sizes and addresses
	int server_socket, client_socket;
	socklen_t server_length, client_length;
	struct sockaddr_in server_address, client_address;

	// Create socket and Print error on failure
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		error("Error creating socket");
	}

	// Initialize server address structure
	server_length = sizeof(server_address);
	memset(&server_address, 0, server_length);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(P);

	// Bind socket to address and port and print error on failure
	if (bind(server_socket, (struct sockaddr*)&server_address, server_length) == -1) 
	{
		perror("Error binding socket");
		exit(EXIT_FAILURE);
	}

	// Listen for connections and print error on failure
	// Maximum 7 requests backlog as I have 7 threads
	if (listen(server_socket, 7) == -1) 
	{
		perror("Error listening on socket");
		exit(EXIT_FAILURE);
	}

	printf("Listening for connections on port %d... Please ctrl+C to exit the server application\n" ,P);

	// Accept incoming connection and print error on failure
	while(1)
	{
		client_length = sizeof(client_address);
		client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_length); // Blocks until accept
		if (client_socket == -1) 
		{
			printf("Error accepting connection");
			continue;
		}
		else
		{
			thread_args *t_args = malloc(sizeof(thread_args));
			t_args->client_socket = client_socket;
			t_args->song_names = song_names;		
			if(pthread_create(&thread_id[thread_index], NULL, thread_handle_client, (void *)t_args) != 0)
			{
				printf("Error creating thread");
				close(client_socket);
				free(t_args);
				continue;
			}
			
			// Convert IP address of client to string and print
			printf("Connected successfully and waiting for action from client with IP address %s\n" ,inet_ntoa(client_address.sin_addr));
			
			pthread_detach(thread_id[thread_index]);
			thread_index++;
		}
		
		thread_index = (thread_index + 1) % 7;
		continue;
	}
	
	return 0;
}
