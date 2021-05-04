
default:
	gcc -g -Wall -Werror -pthread -fsanitize=address mainFin.c -o mainFin -lm

mainedit:
	gcc -g -Wall -Werror -pthread -fsanitize=address mainEdit.c -o mainEdit -lm

sender:
	gcc -g -Wall -Werror -pthread -fsanitize=address sendrecv.c -o sendrecv -lm

echos:
	gcc -g -Wall -Werror -pthread -fsanitize=address echos.c -o echos -lm

debug:
	gcc -g -Wall -Werror -fsanitize=address main.c -DDEBUG -o main

testing:
	gcc -g -Wall -Werror -pthread -fsanitize=address test.c -o test

testing_debug:
	gcc -g -Wall -Werror -pthread -fsanitize=address test.c -DDEBUG -o test


clean:
	rm -f *.o sendrecv
