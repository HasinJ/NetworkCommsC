
default:
	gcc -g -Wall -Werror -pthread -fsanitize=address sendrecv.c -o sendrecv -lm

echos:
	gcc -g -Wall -Werror -pthread -fsanitize=address echos.c -o echos -lm

debug:
	gcc -g -Wall -Werror -fsanitize=address sendrecv.c -DDEBUG -o sendrecv

testing:
	gcc -g -Wall -Werror -pthread -fsanitize=address test.c -o test

testing_debug:
	gcc -g -Wall -Werror -pthread -fsanitize=address test.c -DDEBUG -o test


clean:
	rm -f *.o sendrecv
