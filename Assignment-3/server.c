#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>
#include <sys/socket.h>
#define Max(x, y) (((x) > (y)) ? (x) : (y))

int
pbproxy_server(int pb_port, char *real_server_ip, int real_server_port, char *keyfile)
{
    printf("inside server");
    int server_fd, new_socket, num_bytes_read, num_bytes_write,num_bytes_sent;
    struct sockaddr_in address, actual_server_address;
    int opt = 1;
    struct hostent* s;
    struct hostent*    s1;
    fd_set readfds;
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


    //-----------------------------------------------------------------------------------


    //create TCP,ipv4,ip socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        printf("server socket creation failed");
        return -1;
    }

    //clear address
    bzero((char *) &listen_address, sizeof(listen_address));

    //set port and other details
    listen_address.sin_family = AF_INET;
    listen_address.sin_addr.s_addr = INADDR_ANY;
    listen_address.sin_port = htons(pb_port);

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

    //---------------------------------------------------------------------------------------

    //create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        printf("pb server socket creation error \n");
        return -1;
    }

    //Get server address
    s1 = gethostbyname(real_server_ip);
    if (s1 == NULL)
    {
        printf("get server address failed");
        return -1;
    }

    //set port and other details
    bzero((char *) &actual_server_address, sizeof(actual_server_address));
    actual_server_address.sin_family = AF_INET;
    bcopy((char *) s1->h_addr, (char *) &actual_server_address.sin_addr.s_addr, s1->h_length);
    actual_server_address.sin_port = htons(real_server_port);


    //connect to pb_server
    if (connect(server_fd, (struct sockaddr *) &actual_server_address, sizeof(actual_server_address)) < 0)
    {
        printf("connect to actual server failed  \n");
        return -1;
    }
    //-------------------------------------------------------------------------------------------------------------------
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
        FD_ZERO(&readfds);
        FD_SET(fd_new, &readfds);
        FD_SET(server_fd, &readfds);
        select(Max(fd_new, server_fd) + 1, &readfds, NULL, NULL, NULL);
        while (1)
        {
            printf("enterened in while 1");
            if (FD_ISSET(server_fd, &readfds))
            {
                bzero(read_buffer, 2048);
                num_bytes_read = read(server_fd, read_buffer, 2048);
                printf("data read from server %s",read_buffer);
                if (num_bytes_read <= 0)
                {
                    printf("Error reading data from server");
                    return -1;
                }
                else
                {
                    printf("data sent to client %s",read_buffer);
                    num_bytes_sent = write(fd_new, read_buffer, 2048);
                    if (num_bytes_sent < 0)
                    {
                        printf("Error sending data to client \n");
                        return -1;
                    }
                }
            }
            if (FD_ISSET(fd_new, &readfds))
            {
                bzero(read_buffer, 2048);
                num_bytes_read = read(fd_new, read_buffer, 2048);
                printf("data read from client %s",read_buffer);
                if (num_bytes_read <= 0)
                {
                    close(fd_new);
                    break;
                }
                else
                {
                    num_bytes_sent = write(server_fd, read_buffer, 2048);
                    printf("data sent to server %s",read_buffer);
                    if (num_bytes_sent < 0)
                    {
                        printf("error sending message to socket pb_client \n");
                        return -1;
                    }
                }
            }
        }
        close(fd);

    }

    return 0;
}