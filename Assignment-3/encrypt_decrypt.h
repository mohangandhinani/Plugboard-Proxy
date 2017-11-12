//
// Created by mohan on 11/11/17.
//

#ifndef ASSIGNMENTS_ENCRYPT_DECRYPT_H
#define ASSIGNMENTS_ENCRYPT_DECRYPT_H
struct ctr_state
{
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned int num;
    unsigned char ecount[AES_BLOCK_SIZE];
};

void
init_ctr(struct ctr_state *state, const unsigned char iv[16]);
#endif //ASSIGNMENTS_ENCRYPT_DECRYPT_H
