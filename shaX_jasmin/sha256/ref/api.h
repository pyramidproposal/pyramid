#define CRYPTO_STATEBYTES 32
#define CRYPTO_BLOCKBYTES 64

int crypto_hashblocks(unsigned char *statebytes,const unsigned char *in,unsigned long long inlen);
int crypto_hash(unsigned char *out,const unsigned char *in,unsigned long long inlen);
