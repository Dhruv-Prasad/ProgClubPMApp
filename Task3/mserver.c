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

#define BUFFER_SIZE 1024

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
	if(!((1<=chosen) && (chosen<=3)))
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
	
	// Initialising the buffer and the client and server sockets, sizes and addresses
	int server_socket, client_socket;
	socklen_t server_length, client_length;
	char buffer[BUFFER_SIZE];
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
	if (listen(server_socket, 5) == -1) 
	{
		perror("Error listening on socket");
		exit(EXIT_FAILURE);
	}

	printf("Listening for connections on port %d...\n" ,P);

	// Accept incoming connection and print error on failure
	client_length = sizeof(client_address);
	client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_length);
	if (client_socket == -1) 
	{
		perror("Error accepting connection");
		exit(EXIT_FAILURE);
	}
	
	// Convert IP address of client to string and print
	printf("IP address is %s\n" ,inet_ntoa(client_address.sin_addr)); //in arpa/inet.h
	
	// Wait for a message from client and store it in buffer
	printf("Waiting for client to request a song...\n");
	while (1)
	{
		if((recv(client_socket, buffer, BUFFER_SIZE, 0) > 0))
		{
			printf("Song selected: %s",buffer);
			break;
		}
		bzero(buffer, BUFFER_SIZE);
	}

	//Send the song data to client based in client's choice
	send_msg(atoi(buffer), song_names, client_socket);

	// Close sockets
	close(client_socket);
	close(server_socket);

	return 0;
}
