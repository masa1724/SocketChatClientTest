
typedef struct __CHAT_RESPONSE_HEADER__ {
	TCHAR responceCode[5];
	TCHAR resultCode[10];
} CHAT_RESPONSE_HEADER;


typedef struct __CHAT_REQUEST_HEADER__ {
	TCHAR responceCode[5];
	TCHAR resultCode[10];
} CHAT_REQUEST_HEADER;



typedef struct __CHAT_REQUEST_BODY_MSG__ {
	TCHAR msg[5];
} CHAT_REQUEST_BODY_MSG;




int
requestMsg(TCHAR *sendMsg, size_t msgLen, TCHAR *cliIpAddr, int cliPort) {

}
