#ifndef KEYBOARD_H
#define KEYBOARD_H

#define BUFFLEN 128
void initialize_keyboard();
char get_ascii_char();
uint8_t kb_buff_hd;
int ret;
uint8_t commandIndex;
char *commands[BUFFLEN];
uint8_t kb_buff[BUFFLEN];

char currentCommand[BUFFLEN];
int index;
#endif
