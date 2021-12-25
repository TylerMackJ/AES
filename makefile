all : bin/sEncrypt

bin/sEncrypt : src/sEncrypt.c src/encryption/*.c src/shared/*.c bin/
	clang src/sEncrypt.c src/encryption/*.c src/shared/*.c -o bin/sEncrypt

bin/ :
	mkdir bin/

clean : bin/
	rm -r bin/