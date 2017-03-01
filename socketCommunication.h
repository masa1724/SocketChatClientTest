
#ifndef SOCKETCOMMUNICATION_H_
#define SOCKETCOMMUNICATION_H_

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <unistd.h>
#include <WInerror.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "log.h"
#include "message.h"
#include "frmctl.h"

// サーバーのIPアドレス
#define SRV_IP_ADDR TEXT("127.0.0.1")
// ポート番号
#define SRV_PORT 12345


// クライアントのポート番号
#define CLIENT_PORT 12346


int watchServerRequest();
int sendSocketToClient(TCHAR *msg, size_t msgLen);


extern int threadStatus;

#endif /* SOCKETCOMMUNICATION_H_ */
