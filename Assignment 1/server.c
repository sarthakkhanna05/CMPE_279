// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>

#define PORT 8080

int privilage_drop(){
    struct passwd* pass_ptr;
    pid_t pId, cId;
    int cur_status;
    
    cId= fork();
   
    if (cId==0){
        // cId is 0 means it is successful
        printf("\n Successfully Forked \n");
        //printf("Comment: forked child_pid = %d\n", cId);
        //printf("Comment: Current pid = %d\n",getpid());

        //Changing user to nobody
        pass_ptr = getpwnam("nobody");
        pId= setuid(pass_ptr->pw_uid);
        //printf("Comment:  UID of nobody=%ld\n",(long) pass_ptr->pw_uid);
        if (pId != 0)   
            perror("setuid() error");
        else
            //printf("Comment: UID after setuid() = %ld\n",(long) getuid());
	
	 sleep(1);
        return 1;

    }
    else if (cId > 0){
        //fork return PID of child process inside parent
        //Waiting for child process to complete
        //printf("Waiting for child process to complete\n");
        if((pId = wait(&cur_status)) < 0){
            perror(" Error in wait");
            _exit(1);
        }
        //printf("Comment:  Forked child pid: %d\n", cId); 
        //printf("Comment: : Current pid = %d\n",getpid());
        //printf("Comment: Current uid = %ld\n",(long) getuid());   
        //printf("Comment:  Completed\n");

        return 2;

    }

    return 0;


}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Show ASLR
    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    int pd = privilage_drop();
    if(pd==1){
         //message processing
            valread = read(new_socket, buffer, 1024);
            printf("Read %d bytes: %s\n", valread, buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent\n");
    }
    else if (pd==0) 
    {
        perror("fork() failed and returned value of childId less than zero");
    }
    wait(0);	
    return 0;
}
