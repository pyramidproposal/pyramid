PARAMS = pyramid-shake256-test_d_3
THASH = simple

SOURCES =          randombytes.c address.c convert.c wots.c merkle.c 
HEADERS = params.h randombytes.h address.h convert.h wots.h merkle.h hash.h thash.h

CC=/usr/bin/gcc
CFLAGS=-g -Wall -Wextra -Wpedantic -O3 -std=c99 -DPARAMS=$(PARAMS) $(EXTRA_CFLAGS)

SOURCES += sign.c format.c ext_algo.c
HEADERS += api.h  format.h ext_algo.h

SOURCES += pyr_simple.c 
HEADERS += pyr_simple.h 

SOURCES += fips202.c hash_shake256.c thash_shake256_$(THASH).c
HEADERS += fips202.h

DET_SOURCES = $(SOURCES:randombytes.%=rng.%)
DET_HEADERS = $(HEADERS:randombytes.%=rng.%)

TESTS = test/pyramid

.PHONY: clean test

default: all

all: tests

tests: $(TESTS)

test: $(TESTS:=.exec)

test/%: test/%.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $< $(LDLIBS)

test/%.exec: test/%
	@$<

clean:
	-$(RM) $(TESTS)
