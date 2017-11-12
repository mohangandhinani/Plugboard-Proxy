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
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encrypt_decrypt.h"
int
pbproxy_server(int pb_port, char *real_server_ip, int real_server_port, char *key_value)
{
    printf("inside server\n");
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
    char enc_write_buffer[2048];
    char dec_read_buffer[2048];
    memset(read_buffer, 0,2048);
    memset(write_buffer, 0,2048);
    AES_KEY key_struct;
    AES_KEY key_struct1;
    struct ctr_state initial_state;
    char enc_iv[AES_BLOCK_SIZE];
    char dec_iv[AES_BLOCK_SIZE];

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

    while (1)
    {
        //Accept the 1st connection in queue and create a socket for that
        len = sizeof(listen_address);
        fd_new = accept(fd, (struct sockaddr *) &listen_address, &len);
        if (fd_new < 0)
        {
            printf("Accept connection failed");
        }

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

        //connect to actual server
        if (connect(server_fd, (struct sockaddr *) &actual_server_address, sizeof(actual_server_address)) < 0)
        {
            printf("connect to actual server failed  \n");
            exit(EXIT_FAILURE);
            return -1;
        }

        //initialise decryption
        AES_KEY key_struct1;
        struct ctr_state initial_state1;
        if (AES_set_encrypt_key((unsigned const char *) key_value, 128, &key_struct) < 0)
        {
            printf("decrypt key setting failed\n");
        }
        num_bytes_read = read(fd_new, dec_iv, AES_BLOCK_SIZE);
        if (num_bytes_read < 0)
        {
            printf("error reading iv from client \n");
            exit(EXIT_FAILURE);
        }
        init_ctr(&initial_state1, dec_iv);

        //initialise encryption(generate iv and init ctr)
        if (!RAND_bytes(enc_iv, AES_BLOCK_SIZE))
        {
            printf("Random byte creation failed\n");
        }

        if (AES_set_encrypt_key((unsigned const char *) key_value, 128, &key_struct) < 0)
        {
            printf("encrypt key setting failed\n");
        }

        init_ctr(&initial_state, enc_iv);
        num_bytes_sent = write(fd_new, enc_iv, AES_BLOCK_SIZE);
        if (num_bytes_sent < 0)
        {
            printf("error sending iv to client \n");
            exit(EXIT_FAILURE);
        }

        while (1)
        {
            FD_ZERO(&readfds);
            FD_SET(fd_new, &readfds);
            FD_SET(server_fd, &readfds);
            select(Max(fd_new, server_fd) + 1, &readfds, NULL, NULL, NULL);
            if (FD_ISSET(server_fd, &readfds))
            {
                bzero(read_buffer, 2048);
                num_bytes_read = read(server_fd, read_buffer, 2048);
                printf("data read from server %s",read_buffer);
                if (num_bytes_read <= 0)
                {
                    printf("Error reading data from server");
                    break;
                }
                else
                {

                    AES_ctr128_encrypt(read_buffer,
                                       enc_write_buffer,
                                       num_bytes_read,
                                       &key_struct,
                                       initial_state.ivec,
                                       initial_state.ecount,
                                       &initial_state.num);
                    num_bytes_sent = write(fd_new, enc_write_buffer, num_bytes_read);
                    printf("data sent to client %s\n",enc_write_buffer);
                    if (num_bytes_sent <= 0)
                    {
                        printf("Error sending data to client \n");
                        break;
                    }
                }
            }
            if (FD_ISSET(fd_new, &readfds))
            {
                bzero(read_buffer, 2048);
                num_bytes_read = read(fd_new, read_buffer, 2048);
                printf("data received from client %s\n",read_buffer);
                AES_ctr128_encrypt(read_buffer,
                                   dec_read_buffer,
                                   num_bytes_read,
                                   &key_struct,
                                   initial_state1.ivec,
                                   initial_state1.ecount,
                                   &initial_state1.num);
                if (num_bytes_read <= 0)
                {
                    break;
                }
                else
                {
                    num_bytes_sent = write(server_fd, dec_read_buffer, num_bytes_read);
                    printf("data sent to server %s",dec_read_buffer);
                    if (num_bytes_sent <= 0)
                    {
                        printf("error sending message to socket pb_client \n");
                        break;
                    }
                }
            }
        }
        printf("outside while");
        close(fd_new);
        close(server_fd);

    }

    return 0;
}
