#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
int main(int argc, char *argv[]) {
    char *keyfile = NULL;
    char *port = NULL;
    char *dest = NULL;
    char *d_port = NULL;
    int rev_mode=0;
    int has_key = 0;
    char c;
    int index;
    while ((c = getopt (argc, argv, "k:l:")) != -1)
    {
        switch (c)
        {
            case 'l':
                rev_mode =1;//server
                port = optarg;
                printf("%s",port);
                break;
            case 'k':
                keyfile = optarg;
                has_key =1;
                printf("%s",keyfile);
                break;
            case 'h':
                //help(1);
                return 0;
                break;
            case '?':
                if (optopt == 'k')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'l')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                //help(-1);
                return(0);
                break;
        }
    }
}