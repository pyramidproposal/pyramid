#ifndef COMPARE_H
#define COMPARE_H

int cmp_SK_CORE(const unsigned char *sk_c, const unsigned char *sk_jazz);
int cmp_PK(const unsigned char *pk_c, const unsigned char *pk_jazz);
int cmp_SK(const unsigned char *sk_c, const unsigned char *sk_jazz);
int cmp_SIG(const unsigned char *sig_c, const unsigned char *sig_jazz);

#endif
