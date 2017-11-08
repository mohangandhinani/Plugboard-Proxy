
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client.h"
#include "server.h"
#include <stdlib.h>
#include <ctype.h>

int
main(int argc, char **argv)
{

    int server_mode_flag = 0;
    int key_flag =0 ;
    int s_proxy_in_port=0;
    char* destination_server_ip=NULL;
    int destination_server_port = 0;
    char* keyfilepath = NULL;

    char opt;
    while ((opt = getopt(argc, argv, "k:l:")) != -1)
    {
        switch (opt)
        {
            case 'k':
            {
                key_flag = 1;
                keyfilepath = optarg;
                break;
            }

            case 'l':
            {
                server_mode_flag = 1;
                s_proxy_in_port = atoi(optarg);
                break;
            }

            case '?':
            {
                printf("please pass valid options \n");
                return 0;

            }
        }
    }
    if(key_flag==0)
    {
        printf("Please pass mandatory argument key file");
        return 0;
    }
    int entry = 0;
    for (int argv_index = optind; argv_index < argc; argv_index++)
    {
        if(entry ==0)
        {
            destination_server_ip = argv[argv_index];
        }
        if(entry == 1)
        {
            destination_server_port = atoi(argv[argv_index]);
        }
        entry++;
    }

    if(entry<2)
    {
        printf("Destination ip and port parameters missing");
    }

//    /* print input info */
//    printf("s_proxy_in_port: %d\n", s_proxy_in_port);
//    printf("keyfilepath: %s\n", keyfilepath);
//    printf("destination_server_ip: %s\n", destination_server_ip);
//    printf("destination_server_port : %d\n", destination_server_port);

    if(server_mode_flag)
    {
        pbproxy_server(s_proxy_in_port,destination_server_ip,destination_server_port, keyfilepath);


    }
    else
    {
        pbproxy_client(destination_server_ip,destination_server_port,keyfilepath);
    }

}