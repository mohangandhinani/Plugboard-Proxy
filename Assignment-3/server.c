#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

int pbproxy_server(int pb_port,char* real_server_ip,int real_server_port, char* keyfile)
{
    int server_fd, new_socket, num_bytes_read,num_bytes_write;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char server_read_buffer[2048] = {0};
    char *server_reply = "From server :: Received ur message";

    //variable declaration
    int fd,bind_value,len,fd_new;
    struct sockaddr_in listen_address;

    //create TCP,ipv4,ip socket
    fd=socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0)
    {
        printf("server socket creation failed");
        return -1;
    }

    //clear address
    bzero((char*)&listen_address, sizeof(listen_address));

    //set port and other details
    listen_address.sin_family = AF_INET;
    listen_address.sin_addr.s_addr = INADDR_ANY;
    listen_address.sin_port = htons(pb_port );

    // Bind socket
    bind_value = bind(fd, (struct sockaddr *)&listen_address, sizeof(listen_address));
    if (bind_value<0)
    {
        printf("Bind failed");
        return -1;
    }

    //Listen for connections on this socket
    if (listen(fd, 6) < 0)
    {
        printf("Listen failed");
        return -1;
    }

    //Accept the 1st connection in queue and create a socket for that
    len = sizeof(listen_address);
    fd_new = accept(fd, (struct sockaddr *)&listen_address,&len);
    if (fd_new<0)
    {
        printf("Accept connection failed");
        return -1;
    }

    //Read Message
    bzero(server_read_buffer,2048);
    num_bytes_read = read( fd_new , server_read_buffer, 2048);
    if(num_bytes_read<0)
    {
        printf("Read fron socket failed");
        return -1;
    }
    printf("%s\n",server_read_buffer );

    //Send Message
    num_bytes_write = write(fd_new , server_reply , strlen(server_reply));
    if(num_bytes_read<0)
    {
        printf("Read fron socket failed");
        return -1;
    }

    return 0;
}