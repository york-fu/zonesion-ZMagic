#ifndef _phonedlg_h_
#define _phonedlg_h_

#include "DIALOG.h"
#include "App.h"
#include "View.h"
#include "lte/lte-ec20.h"

#define NUMBUF_SIZE           14
#define CONTACTS_NAME_SIZE    16
#define PHONE_CONTACTS_MAX    4

typedef struct
{
  char name[CONTACTS_NAME_SIZE];
  char number[NUMBUF_SIZE];
}PhoneContacts_t;


extern char PhoneNumber[NUMBUF_SIZE];
extern PhoneContacts_t ContactsList[PHONE_CONTACTS_MAX];

PROCESS_NAME(PhoneProcess);

void Phone_ZxbeeGet(char* buf);
void Phone_ZxbeeSet(char* str);
short PhoneOpenContacts(void);

#endif