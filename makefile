all : src/aes.c bin/
	gcc src/aes.c -o bin/aes

bin/ :
	mkdir bin/

clean : bin/
	rm -r bin/