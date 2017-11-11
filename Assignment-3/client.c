
#include  <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>

int pbproxy_client(char* dest_ip,int dest_port, char* key_file_path)
{
    //variable declarations
    int pb_c_fd = 0,num_bytes_read,num_bytes_sent;
    struct sockaddr_in pb_server_address;
    struct hostent* s;
    char *hello = "Hello from client";
    fd_set readfds;
    char read_buffer[2048];
    char write_buffer[2048];
    memset(read_buffer, 0,2048);
    memset(write_buffer, 0,2048);

    //create socket
    pb_c_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (pb_c_fd < 0)
    {
        printf("pb client socket creation error \n");
        exit(EXIT_FAILURE);
        return -1;
    }

    //Get server address
    s = gethostbyname(dest_ip);
    if(s==NULL)
    {
        printf("get server address failed");
        exit(EXIT_FAILURE);
        return -1;
    }

    //set port and other details
    bzero((char*)&pb_server_address, sizeof(pb_server_address));
    pb_server_address.sin_family = AF_INET;
    bcopy((char*)s->h_addr,(char*)&pb_server_address.sin_addr.s_addr,s->h_length);
    pb_server_address.sin_port = htons(dest_port);


    //connect to pb_server
    if (connect(pb_c_fd, (struct sockaddr *)&pb_server_address, sizeof(pb_server_address)) < 0)
    {
        printf("connect to pb server failed pb_proxy_c -> pb_proxy_s \n");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(pb_c_fd, &readfds);
        select(pb_c_fd+1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(0, &readfds))
        {
            //read from std in and send to server

            bzero(write_buffer,2048);
            num_bytes_read = read( 0 , write_buffer, 2048);
            if(num_bytes_read<=0)
            {
                printf("error reading stdin \n");
                break;
            }
//            fgets(write_buffer,2048,0);


            num_bytes_sent = write(pb_c_fd , write_buffer , num_bytes_read);
            if(num_bytes_sent<0)
            {
                printf("error sending message to socket pb_client \n");
                exit(EXIT_FAILURE);
            }
        }

        if(FD_ISSET(pb_c_fd, &readfds))
        {
            //read message from server and send to stdout
            bzero(read_buffer,2048);
            num_bytes_read = read( pb_c_fd , read_buffer, 2048);
            if(num_bytes_read<=0)
            {
                printf("error reading socket pb_client \n");
                break;
            }
//            printf("%s",read_buffer );

            num_bytes_sent = write(1 , read_buffer , num_bytes_read);
            if(num_bytes_sent<=0)
            {
                printf("error writing to stdout\n");
                exit(EXIT_FAILURE);
            }
        }

    }
    close(pb_c_fd);
    return 0;
}
