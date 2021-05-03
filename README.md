# NetworkCommsC

Project 3 Systems Network Communication

Norman Ligums Clark
NetID: nal94

Hasin Choudhury
NetID: hmc94

Our underlying data structure (an alphabetized linked list) was tested by running multiple threads and simultaneously inserting, fetching, and deleting random key and value entries. We checked for consistency in functionality and data stored in memory across multiple threads. Once we were satisfied with the underlying data structure, we incorporated that into the preliminary server structure created from modifying the sample code provided from class. Originally, functionality was tested using separate client code similar to send and sendrecv, where individual messages were modified before being compiled and executed. Once the NC utility was discussed in class, further testing was conducted via netcat. Multithread functionality was tested by running nc in multiple shell instances on several computers simultaneously including the ilab and inputting different sets of commands. After we were satisfied with functionality under normal conditions, we began testing various error conditions. These included inputting the following messages to our server: incorrect message headers, invalid values for message length, message payloads which did not match message length (both too long and too short), and various unusual payload values (such as empty strings, numbers, and strings with blank spaces among others). The read statements initially read multiple bytes at a time but by storing it by characters, we were able to manage for every situation. This led to availability whenever we wanted to add a new condition, such as the different parameters for SET requests. Memory leaks were checked using address sanitizer and all malloc instances were conducted using a macro (safe_malloc) that would exit the program if malloc failed. 

We chose to not allow empty key strings, but we did allow empty key values.
