#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct ctr_state
{
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned int num;
    unsigned char ecount[AES_BLOCK_SIZE];
};

void
init_ctr(struct ctr_state *state, const unsigned char iv[16])
{
    /* aes_ctr128_encrypt requires 'num' and 'ecount' set to zero on the
    * first call. */
    state->num = 0;
    memset(state->ecount, 0, AES_BLOCK_SIZE);

    /* Initialise counter in 'ivec' to 0 */
    memset(state->ivec + 8, 0, 8);

    /* Copy IV into 'ivec' */
    memcpy(state->ivec, iv, 8);
}

void
encrypt_text(char *input_text, int len,char *encrypted_text, char *encryption_key,char* iv)
{

    AES_KEY key_struct;
    struct ctr_state initial_state;

    if (!RAND_bytes(iv, AES_BLOCK_SIZE))
    {
        printf("Random byte creation failed\n");
    }

    if (AES_set_encrypt_key((unsigned const char *) encryption_key, 128, &key_struct) < 0)
    {
        printf("decrypt key setting failed\n");
    }

    init_ctr(&initial_state, iv);
    AES_ctr128_encrypt(input_text,
                       encrypted_text,
                       len,
                       &key_struct,
                       initial_state.ivec,
                       initial_state.ecount,
                       &initial_state.num);

}

void
decrypt_text(char *input_text, int len,char *decrypted_text, char *encryption_key,char* iv)
{
    AES_KEY key_struct;
    struct ctr_state initial_state;
    if (AES_set_encrypt_key((unsigned const char *) encryption_key, 128, &key_struct) < 0)
    {
        printf("decrypt key setting failed\n");
    }

    init_ctr(&initial_state, iv);
    AES_ctr128_encrypt(input_text,
                       decrypted_text,
                       len,
                       &key_struct,
                       initial_state.ivec,
                       initial_state.ecount,
                       &initial_state.num);

}

int
main()
{
    char *input_text = "abcdef";
    char iv[AES_BLOCK_SIZE];
    char *encrypted_text = (char *) malloc(strlen(input_text) * sizeof(char) + 1);
    char *decrypted_text = (char *) malloc(strlen(input_text) * sizeof(char) + 1);
    char *key = "1234567812345678";
    encrypt_text(input_text,strlen(input_text),encrypted_text, key,iv);
    printf("encrypted text is %s\n", encrypted_text);
    decrypt_text(encrypted_text,strlen(input_text), decrypted_text, key,iv);
    printf("decrypted text is %s\n", decrypted_text);
    return 0;
}