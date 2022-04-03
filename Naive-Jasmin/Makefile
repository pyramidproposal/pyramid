# Note: this makefile / project setup is derived from the one by M.C.F.H.P. Meijers.
# The original can be found at https://github.com/MM45/SABER-Jasmin.

# "Simple" is used to refer to either "pyr_simple" or "simple thash mode".

# The thash strategy used in tests.
PARAMS = pyramid-shake256-test_d_3
THASH = robust

# GENERALDIRECTORY
GD = ref

# REGULARDIRECTORY
RD = src

CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -Wpedantic -O3 -std=c99 -DPARAMS=$(PARAMS) -Itest/util
LDFLAGS = -lssl -lcrypto
JASMIN = jasminc
JADDFLAGS = 
JFLAGS = -lea -pasm $(JADDFLAGS)

FIPS202 = \
test_fips202_store64 \
test_fips202_KeccakF1600_StatePermute \
test_fips202_keccak_squeezeblocks_256_256 \
test_fips202_shake256_PYR_N_PRF_INLEN \
test_fips202_shake256_PYR_N_TH1_INLEN \
test_fips202_shake256_PYR_N_TH2_INLEN\
test_fips202_shake256_PYR_N_THLEN_INLEN \
test_fips202_shake256_TH1_MASK_INLEN \
test_fips202_shake256_TH2_MASK_INLEN \
test_fips202_shake256_THLEN_MASK_INLEN \
test_fips202_keccak_absorb_256_PRF_INLEN \
test_fips202_keccak_absorb_256_TH1_INLEN \
test_fips202_keccak_absorb_256_TH2_INLEN \
test_fips202_keccak_absorb_256_THLEN_INLEN \
test_fips202_keccak_absorb_256_MASK_INLEN \
test_fips202_keccak_squeezeblocks_256_PYR_N \
test_fips202_keccak_squeezeblocks_256_TH1 \
test_fips202_keccak_squeezeblocks_256_TH2 \
test_fips202_keccak_squeezeblocks_256_THLEN \
test_fips202_keccak_inc_absorb_256 \
test_fips202_keccak_inc_absorb_256_HMSG_CONST_INLEN \
test_fips202_keccak_inc \
test_fips202_shake256_PYR_N_variable

PYR = \
test_pyr_address \
test_pyr_hash_shake256 \
test_pyr_thash_shake256_simple \
test_pyr_thash_shake256_robust \
test_pyr_wots \
test_pyr_compute_root

CRYPTO = \
test_crypto_sign_seed_keypair \
test_crypto_sign_signature \
test_crypto_sign_verify

TESTS = $(FIPS202) $(PYR) $(CRYPTO)

default: prep test	


##### GENERAL #####

# Anything that can be shared between distinct implementations.
HEADERS = params.h randombytes.h address.h convert.h wots.h merkle.h hash.h thash.h
SOURCES =          randombytes.c address.c convert.c wots.c merkle.c 

# Files that alter the way in which signing, verifying, keygen is performed.
HEADERS += api.h format.h
SOURCES += sign.c 

# External algorithm.
HEADERS += treehash.h
SOURCES += treehash.c

# This is SHAKE256 by default.
HEADERS += fips202.h shake256_offsets.h
SOURCES += fips202.c hash_shake256.c

# Final lists without prefix
SIMPLE_SOURCES = $(SOURCES) thash_shake256_simple.c
ROBUST_SOURCES = $(SOURCES) thash_shake256_robust.c

# For testing both implementations.
simple_SG = $(addprefix $(GD)/,$(SIMPLE_SOURCES))
robust_SG = $(addprefix $(GD)/,$(ROBUST_SOURCES))

# For testing/building everything else.
HEADERSGENERAL = $(addprefix $(GD)/,$(HEADERS))
SOURCESGENERAL = $($(THASH)_SG)

# Test utilities
HEADERSTEST = $(HEADERSGENERAL) $(wildcard test/util/*.h)
SOURCESTEST = $(SOURCESGENERAL) $(wildcard test/util/*.c)
SIMPLE_ST   = $(simple_SG)      $(wildcard test/util/*.c)
ROBUST_ST   = $(robust_SG)      $(wildcard test/util/*.c)


test/bin/test_fips202_%: \
test/test_fips202_%.c $(HEADERSTEST) $(SOURCESTEST) asm/jfips202.s
	$(CC) $(CFLAGS) -o $@ $(SOURCESTEST) asm/jfips202.s $< $(LDFLAGS)

test/bin/test_pyr_thash_shake256_robust: \
test/test_pyr_thash_shake256_robust.c $(HEADERSTEST) $(ROBUST_ST) asm/jpyr_thash_shake256_robust.s
	$(CC) $(CFLAGS) -o $@ $(ROBUST_ST) asm/jpyr_thash_shake256_robust.s $< $(LDFLAGS)

test/bin/test_pyr_thash_shake256_simple: \
test/test_pyr_thash_shake256_simple.c $(HEADERSTEST) $(SIMPLE_ST) asm/jpyr_thash_shake256_simple.s
	$(CC) $(CFLAGS) -o $@ $(SIMPLE_ST) asm/jpyr_thash_shake256_simple.s $< $(LDFLAGS)

test/bin/test_pyr_%: \
test/test_pyr_%.c $(HEADERSTEST) $(SOURCESTEST) asm/jpyr_%.s
	$(CC) $(CFLAGS) -o $@ $(SOURCESTEST) asm/jpyr_$*.s $< $(LDFLAGS)

test/bin/test_crypto_%: \
test/test_crypto_%.c $(HEADERSTEST) $(SOURCESTEST) asm/jcrypto.s
	$(CC) $(CFLAGS) -o $@ $(SOURCESTEST) asm/jcrypto.s $< $(LDFLAGS)

# The sanity check protects against accidental test parameter mismatches.
test/bin/test_sanity_check: test/sanity_check.c ref/params.h asm/jsanity_check.s
	$(CC) $(CFLAGS) -o $@ asm/jsanity_check.s $< $(LDFLAGS)

asm/j%.s: $(RD)/%.jazz
	  $(JASMIN) $(JFLAGS) $^ -o $@

test: $(addsuffix .exec, $(addprefix test/bin/, $(TESTS)))

test/%.exec: test/%
	     @$<

.PHONY: clean

prep:
	ln -s params/params-$(PARAMS).jahh src/pyramid_params.jahh
	ln -s pyr_thash_shake256_$(THASH).jahh src/pyr_thash_shake256.jahh

clean:
	-rm src/pyr_thash_shake256.jahh
	-rm src/pyramid_params.jahh
	-rm asm/j*.s
	-rm test/bin/test_*
