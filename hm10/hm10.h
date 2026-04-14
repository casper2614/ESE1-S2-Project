#ifndef HM10_H
#define HM10_H

#define BUFFER_SIZE 2048
#define UUID_TO_FIND "REPLACE"

int init(void);
void send(char cmd[]);
void receive(void);
void wipe(void);

#endif