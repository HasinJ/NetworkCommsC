#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>


#define BACKLOG 5
#define BUFSIZE 1


struct connection {
  struct sockaddr_storage addr;
  socklen_t addr_len;
  int fd;
};


void reset(char* word, int word_length, int* pos){
  free(word);
  word=calloc(word_length+1,sizeof(char));
  *pos=0;
}

void *echo(void *arg){
  char host[100], port[10];
  struct connection *c = (struct connection *) arg;
  int error, ch, newlines_max=3, newlines_read=0, pos=0, word_length=4, choice;
  char* word = calloc(word_length+1,sizeof(char));
  char* key=0;

  error = getnameinfo((struct sockaddr *) &c->addr, c->addr_len, host, 100, port, 10, NI_NUMERICSERV);
  if (error != 0) {
    fprintf(stderr, "getnameinfo: %s", gai_strerror(error));
    close(c->fd);
    return NULL;
  }

  printf("[%s:%s] connection\n", host, port);

  FILE *fin = fdopen(dup(c->fd), "r");  // copy socket & open in read mode
  FILE *fout = fdopen(c->fd, "w");  // open in write mode

  ch = getc(fin);
  while (ch != EOF){
    if(ch==0){
      fprintf(fout, "ERR\nBAD\n");
      fflush(fout);
      break;
    }
    if(isspace(ch)){
      if(ch==10){
        if(++newlines_read==newlines_max){
          printf("newlines maxed out\n");
          if (choice==1) { // GET
            printf("get key: %s\n", word);
            /* get key and value here
            char* result = 0;
            if(result = ll_read(L, word))  printf("key found: %s", result); // idk if this works
            else printf("KNF") //no error can continue connection
            */
          }
          else if (choice==3) { // DEL
            printf("delete key: %s\n", word);
            /* delete here
            char* result = ll_del(L, word); //idk if this works
            printf("deleted: %s", result);
            */
          }
          else if (choice==2){ //SET
            printf("set key: [%s] to value: [%s]\n", key, word);
            /* set here
            char* result = 0;
            if(result = ll_read(L, word))  printf("key found: %s", result); // idk if this works
            else ll_ins(L, key, word) //no error can continue connection
            */
          }
          else{
            printf("error with server, no chocie\n"); // should never happen;
          }
          printf("resetting...\n");
          free(word);
          newlines_max=3, newlines_read=0, pos=0, word_length=4, choice=0;
          word=calloc(word_length+1,sizeof(char));
        }
        else if(newlines_read==1){
          printf("first newline -->");
          if(pos!=3) { //too short
            fprintf(fout, "ERR\nBAD\n"); //idk about this
            fflush(fout);
            break;
          }
          if(strcmp(word,"GET")==0){
            printf("get command pos: %d \n", pos);
            choice=1;
          }
          else if(strcmp(word,"SET")==0){
            printf("set command\n");
            newlines_max=4;
            choice=2;
          }
          else if(strcmp(word,"DEL")==0){
            printf("del command\n");
            choice=3;
          }
          else {
            printf("error BAD its not GET, SET, or DEL\n"); //BAD
            break;
          }
        }
        else if(newlines_read==2){
          printf("second newline pos: %d number (word) : %d \n", pos, atoi(word));
          word_length=atoi(word);
        }
        else if(newlines_read==3){
          printf("third newline, means we're in SET mode |pos: %d| \n",pos);
          key=calloc((word_length-=(pos+1))+1,sizeof(char));
          strcpy(key,word);
        }
        free(word);
        word=calloc(word_length+1,sizeof(char));
        pos=0;
      }
    }
    else{
      printf("\nbuilding word...\n");
      if(newlines_read==1 && !(ch >= '0' && ch <= '9')){
        printf("error BAD, second input isnt a number\n");
        break;
      }

      word[pos++]=ch;

      if(pos==word_length) { //too long
        if(newlines_read==0) {
          printf("error LEN first set of message is too long\n");
          break;
        }
        else if(newlines_read==2 || (newlines_read==3 && choice==2) ){
          printf("error LEN, too many letters were given\n");
          break;
        }
        else { //this should never be called
          printf("THIS SHOULDNT BE CALLED: realloc");
          word=realloc(word,sizeof(char)*((word_length*=2)+1));
        }
      }

    }
    ch = getc(fin);
  }

  printf("[%s:%s] got EOF\n", host, port);
  free(word);
  fclose(fin);
  fclose(fout);
  close(c->fd);
  free(c);
  return NULL;
}

int server(char *port){
    struct addrinfo hints, *info_list, *info;
    struct connection *con;
    int error, sfd;
    pthread_t tid;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // listener

    error = getaddrinfo(NULL, port, &hints, &info_list); // NULL = localhost
    if (error != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // attempt to create socket
    for (info = info_list; info != NULL; info = info->ai_next) {
        sfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        // if we couldn't create the socket, try the next method
        if (sfd == -1) {
            continue;
        }

        // if we were able to create the socket, try to set it up for incoming connections;
        // two steps:
        // - bind associates the socket with the specified port on the local host
        // - listen sets up a queue for incoming connections and allows us to use accept
        if ((bind(sfd, info->ai_addr, info->ai_addrlen) == 0) &&
            (listen(sfd, BACKLOG) == 0)) {
            break;
        }

        // unable to set it up, so try the next method
        close(sfd);
    }

    if (info == NULL) {
        // we reached the end of result without successfuly binding a socket
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(info_list);

    // at this point sfd is bound and listening
    printf("Waiting for connection\n");
    for (;;) {
  		con = malloc(sizeof(struct connection)); // create argument struct for child thread
      con->addr_len = sizeof(struct sockaddr_storage);
      	// addr_len is a read/write parameter to accept
      	// we set the initial value, saying how much space is available
      	// after the call to accept, this field will contain the actual address length

      con->fd = accept(sfd, (struct sockaddr *) &con->addr, &con->addr_len);
      	// sfd - the listening socket
      	// &con->addr - a location to write the address of the remote host
      	// &con->addr_len - a location to write the length of the address
      	//
      	// accept will block until a remote host tries to connect
      	// it returns a new socket that can be used to communicate with the remote
      	// host, and writes the address of the remote hist into the provided location

      if (con->fd == -1) {
          perror("accept");
          continue;
      }

      // spin off a worker thread to handle the remote connection
      error = pthread_create(&tid, NULL, echo, con);

      // if we couldn't spin off the thread, clean up and wait for another connection
      if (error != 0) {
          fprintf(stderr, "Unable to create thread: %d\n", error);
          close(con->fd);
          free(con);
          continue;
      }

      // otherwise, detach the thread and wait for the next connection request
      pthread_detach(tid);
    }

    // never reach here
    return 0;
}


int main(int argc, char **argv){
	if (argc != 2) {
		printf("Usage: %s [port]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

  (void) server(argv[1]);
  return EXIT_SUCCESS;
}
