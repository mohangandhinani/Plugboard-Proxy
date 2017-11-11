//
// Created by mohan on 8/11/17.
//

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

int
server(char *real_server_ip, int real_server_port)
{
    int server_fd, new_socket, num_bytes_read, num_bytes_write,num_bytes_sent;
    struct sockaddr_in address;
    int opt = 1;
    struct hostent* s;
    int addrlen = sizeof(address);
    char server_read_buffer[2048] = {0};
    char *server_reply = "From server :: Received ur message";
    char read_buffer[2048];
    char write_buffer[2048];
    memset(read_buffer, 0,2048);
    memset(write_buffer, 0,2048);

    //variable declaration
    int fd, bind_value, len, fd_new;
    struct sockaddr_in listen_address;



    //create TCP,ipv4,ip socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        printf("server socket creation failed");
        return -1;
    }

    //Get server address
    s = gethostbyname(real_server_ip);
    if(s==NULL)
    {
        printf("get server address failed");
        return -1;
    }

    //set port and other details
    bzero((char*)&listen_address, sizeof(listen_address));
    listen_address.sin_family = AF_INET;
    bcopy((char*)s->h_addr,(char*)&listen_address.sin_addr.s_addr,s->h_length);
    listen_address.sin_port = htons(real_server_port);

    // Bind socket
    bind_value = bind(fd, (struct sockaddr *) &listen_address, sizeof(listen_address));
    if (bind_value < 0)
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

    while (1)
    {
        //Accept the 1st connection in queue and create a socket for that
        len = sizeof(listen_address);
        fd_new = accept(fd, (struct sockaddr *) &listen_address, &len);
        if (fd_new < 0)
        {
            printf("Accept connection failed");
            return -1;
        }

        while (1)
        {
            bzero(read_buffer,2048);
            num_bytes_read = read(fd_new, read_buffer, 2048);
            if (num_bytes_read <= 0)
            {
                close(fd_new);
                break;
            }
            else
            {
                num_bytes_sent = write(fd_new, read_buffer, 2048);
                if (num_bytes_sent < 0)
                {
                    printf("error sending message to socket pb_client \n");
                    return -1;
                }
            }
        }

    }

    return 0;
}

int
main()
{
    server("127.0.0.1", 8084);
    return 0;
}