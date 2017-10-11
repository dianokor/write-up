/*
        The Lord of the BOF : The Fellowship of the BOF
        - dark knight
        - remote BOF
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <dumpcode.h>

main()
{
	char buffer[40];

	int server_fd, client_fd;  
	struct sockaddr_in server_addr;   
	struct sockaddr_in client_addr; 
	int sin_size;

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;        
	server_addr.sin_port = htons(6666);   
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server_addr.sin_zero), 8);   

	if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
		perror("bind");
		exit(1);
	}

	if(listen(server_fd, 10) == -1){
		perror("listen");
		exit(1);
	}
        
	while(1) {  
		sin_size = sizeof(struct sockaddr_in);
		if((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size)) == -1){
			perror("accept");
			continue;
		}
            
		if (!fork()){ 
			send(client_fd, "Death Knight : Not even death can save you from me!\n", 52, 0);
			send(client_fd, "You : ", 6, 0);
			recv(client_fd, buffer, 256, 0);
			close(client_fd);
			break;
		}
            
		close(client_fd);  
		while(waitpid(-1,NULL,WNOHANG) > 0);
	}
	close(server_fd);
}

