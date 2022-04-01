JASMIN=jasminc
JFLAGS=-lea

default: 	test_iterate_hash \
		test_sha256_INLEN \
		test_crypto_hashblocks \
		test_crypto_hash


test_iterate_hash: test_iterate_hash.c sha256.s
	gcc -Wall -Wextra -Wpedantic test_iterate_hash.c sha256.s -o $@

test_sha256_INLEN: test_sha256_INLEN.c sha256.s
	gcc -Wall -Wextra -Wpedantic test_sha256_INLEN.c sha256.s -o $@

test_crypto_hashblocks: test_crypto_hashblocks.c ref/api.h ref/blocks.c crypto_hashblocks.s
	gcc -Wall -Wextra -Wpedantic test_crypto_hashblocks.c ref/api.h ref/blocks.c crypto_hashblocks.s -o $@

test_crypto_hash: test_crypto_hash.c ref/api.h ref/hash.c ref/blocks.c crypto_hash.s
	gcc -Wall -Wextra -Wpedantic test_crypto_hash.c ref/api.h ref/blocks.c ref/hash.c crypto_hash.s -o $@

%.s: %.jazz
	$(JASMIN) -o $@ $(JFLAGS) $^

.PHONY: clean test

test: 	test_iterate_hash test_sha256_INLEN test_crypto_hashblocks test_crypto_hash
	./test_iterate_hash
	./test_sha256_INLEN
	./test_crypto_hashblocks
	./test_crypto_hash

clean:
	-rm *.s
	-rm test_iterate_hash
	-rm test_sha256_INLEN
	-rm test_crypto_hashblocks
	-rm test_crypto_hash
