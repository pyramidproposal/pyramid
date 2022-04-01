PARAMS = pyramid-shake256-test_d_3
THASH = simple

# Anything that can be shared between distinct implementations.
COMMONDIR=../../common
COMMONC=         randombytes.c address.c convert.c wots.c merkle.c utils.c
COMMONH=params.h randombytes.h address.h convert.h wots.h merkle.h utils.h hash.h thash.h

CC=/usr/bin/gcc
CFLAGS=-Wall -Wextra -Wpedantic -O3 -std=c99 -DPARAMS=$(PARAMS) $(EXTRA_CFLAGS) -I$(COMMONDIR)

# Files that alter the way in which signing, verifying, keygen is performed.
SOURCES = sign.c format.c ext_algo.c
HEADERS = api.h  format.h ext_algo.h

# External algorithm
SOURCES += pyr_simple.c 
HEADERS += pyr_simple.h 

ifneq (,$(findstring shake256,$(PARAMS)))
	COMMONC += fips202.c hash_shake256.c thash_shake256_$(THASH).c
	COMMONH += fips202.h
endif
ifneq (,$(findstring haraka,$(PARAMS)))
	COMMONC += haraka.c hash_haraka.c thash_haraka_$(THASH).c
	COMMONH += haraka.h
endif
ifneq (,$(findstring sha256,$(PARAMS)))
	COMMONC += sha256.c hash_sha256.c thash_sha256_$(THASH).c
	COMMONH += sha256.h
endif

SOURCES += $(addprefix $(COMMONDIR)/,$(COMMONC))
HEADERS += $(addprefix $(COMMONDIR)/,$(COMMONH))

TESTS =         test/pyramid
              # test/haraka

.PHONY: clean test 

default: test

all: tests 

tests: $(TESTS)

test: $(TESTS:=.exec)

test/%: test/%.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $< $(LDLIBS)

# The SPHINCS+ haraka test includes the .c file.
test/haraka: test/haraka.c $(COMMONDIR)/randombytes.c $(COMMONDIR)/randombytes.h
	$(CC) $(CFLAGS) -o $@ $(COMMONDIR)/randombytes.c $< $(LDLIBS)

test/%.exec: test/%
	@$<

clean:
	-$(RM) $(TESTS)
	-$(RM) test/dump_sig
	-$(RM) test/haraka
