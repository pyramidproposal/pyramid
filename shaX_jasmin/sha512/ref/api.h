#define CRYPTO_STATEBYTES 64
#define CRYPTO_BLOCKBYTES 128

int crypto_hashblocks(unsigned char *statebytes,const unsigned char *in,unsigned long long inlen);
int crypto_hash(unsigned char *out,const unsigned char *in,unsigned long long inlen);
