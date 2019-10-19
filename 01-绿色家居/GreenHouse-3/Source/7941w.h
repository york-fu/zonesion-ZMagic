#ifndef __7941W_H__
#define __7941W_H__

int RFID7941CheckCard125kHz(char *cid);
int RFID7941CheckCard1356MHz(char *cid);
int RFID7941ReadCard1356MHz(char blk, char* key, char* buf);
int RFID7941WriteCard1356MHz(char blk, char* key, char* buf);
void RFID7941Init(void);

void RFID7941CheckCard125kHzReq(void);
int RFID7941CheckCard125kHzRsp(char *cid);

void RFID7941CheckCard1356MHzReq(void);
int RFID7941CheckCard1356MHzRsp(char *cid);

void RFID7941ReadCard1356MHzReq(char blk, char* key);
int RFID7941ReadCard1356MHzRsp(char* buf);

void RFID7941WriteCard1356MHzReq(char blk, char* key, char* buf);
int RFID7941WriteCard1356MHzRsp(void);
#endif