#ifndef FRMCTL_H_
#define FRMCTL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <unistd.h>
#include <WInerror.h>
#include <ws2tcpip.h>

#include <commctrl.h>
#include "log.h"
#include "message.h"
#include "socketCommunication.h"

extern HWND hListMsg;

int postMessage(POST_MSG_T *post);
void convDateStr(struct tm *ts, TCHAR *buf);

#endif /* FRMCTL_H_ */
