#ifndef __USERMANAGER_H__
#define __USERMANAGER_H__

#include "protocol.h"
void login(Message *, Message *);
void signup(Message *, Message *);
void record(Message *, Message *);
void win(Message *, Message *);
void lose(Message *, Message *);

#endif
