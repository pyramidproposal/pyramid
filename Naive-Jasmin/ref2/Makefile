PARAMS = pyramid-shake256-test_d_3

# Make sure to use "make prep" in parent directory before building!

CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -Wpedantic -O3 -std=c99 -DPARAMS=$(PARAMS)
LDFLAGS = -lssl -lcrypto
JASMIN = jasminc
JADDFLAGS = 
JFLAGS = -lea -pasm $(JADDFLAGS)

SOURCES = randombytes.c sign.c
HEADERS = randombytes.h api.h params.h format.h export.h

pyramid: test/pyramid.c $(HEADERS) $(SOURCES) jexport.s
	$(CC) $(CFLAGS) -o $@ $(SOURCES) jexport.s $< $(LDFLAGS)

j%.s: ../src/%.jazz
	  $(JASMIN) $(JFLAGS) $^ -o $@

sanity_check: sanity_check.c params.h jsanity_check.s
	$(CC) $(CFLAGS) -o $@ jsanity_check.s $< $(LDFLAGS)

.PHONY: clean test

test: pyramid.exec

pyramid.exec: sanity_check pyramid
	@./sanity_check
	@./pyramid

clean:
	-rm pyramid
	-rm sanity_check
	-rm jexport.s
	-rm jsanity_check.s
