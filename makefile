all : bin/main

bin/main : src/main.c src/encryption/*.c src/shared/*.c src/aes/*.c src/keyExpansion/*.c bin/
	clang src/main.c src/encryption/*.c src/shared/*.c src/aes/*.c src/keyExpansion/*.c -o bin/main

bin/ :
	mkdir bin/

clean : bin/
	rm -r bin/