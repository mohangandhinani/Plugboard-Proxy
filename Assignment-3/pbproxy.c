
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client.h"
#include "server.h"
#include <stdlib.h>
#include <ctype.h>

int char_int(char c)
{
    if(c>'9')
    {
        return 10+c-'A';
    }
    else
    {
        return c-'0';
    }
    return 0;
}

void convert_key(char* input_key,char* output_key)
{
    int v1,v2,v3;
    for(int i=0;i<31;i+=2)
    {
        v1 = char_int(input_key[i]);
        v2 = char_int(input_key[i+1]);
        v3 = v1<<4|v2;
        output_key[i/2] = (char)v3;
    }
    output_key[16]='\0';
}

int
main(int argc, char **argv)
{

    int server_mode_flag = 0;
    int key_flag =0 ;
    int s_proxy_in_port=0;
    char* destination_server_ip=NULL;
    int destination_server_port = 0;
    char* keyfilepath = NULL;
    long l;
    FILE *f;
    char *key_value = "";
    char *final_key_value = malloc(17* sizeof(char));

//    char final_key_value[17] ={'1','2','3','1','2','3','1','2','3','1','2','3','1','2','3','1','\0'};

    char opt;
    while ((opt = getopt(argc, argv, "k:l:")) != -1)
    {
        switch (opt)
        {
            case 'k':
            {
                key_flag = 1;
                keyfilepath = optarg;
                //initialize the enc_key

                FILE *key_file = fopen(keyfilepath, "rb");
                if(f)
                {
                    fseek(key_file, 0, SEEK_END);
                    l = ftell(key_file);
                    key_value = malloc(l);

                    fseek(key_file, 0, SEEK_SET);

                    if(key_value)
                    {
                        fread(key_value, 1, l, key_file);
                    }
                    fclose(key_file);
                }
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
        key_value = "41414141414141414141414141414141414141";
    }

//    printf("0 key value is %s",key_value);

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
//    printf("ist key is %s",key_value);
    convert_key(key_value,final_key_value);
    printf("key value is %s\n",final_key_value);
    if(server_mode_flag)
    {
        pbproxy_server(s_proxy_in_port,destination_server_ip,destination_server_port, final_key_value);


    }
    else
    {
        pbproxy_client(destination_server_ip,destination_server_port,final_key_value);
    }

}
