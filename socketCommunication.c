#define WIN32_LEAN_AND_MEAN

#include "socketCommunication.h"

/*
 *
 * http://wisdom.sakura.ne.jp/system/winapi/winsock/winSock2.html
 */

// 最大クライアント接続数
#define MAX_CLIENT 5

// ソケットオプションOFF
#define SOCK_OPT_OFF 0

// ソケットオプションON
#define SOCK_OPT_ON 1

static TCHAR* getErrorMessage(int errcd);


#define RESPONSE_KIND_MSG TEXT("100")
#define RESPONSE_KIND_USER TEXT("200")

/*
 *
 *
 *
 */
int
watchServerRequest() {
	WSADATA wsaData;
	SOCKET listenSock;
	struct sockaddr_in listenAddr;
	struct sockaddr_in clientAddr;
	int clientAddrlen;
	SOCKET acceptSock;
	TCHAR recvMsg[1024];
	BOOL sockOptVal;
	int errCode;
	TCHAR *errMsg;

	// winsock2の初期化
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		errorlog(TEXT("winsock2の初期化に失敗しました。"));
		return -1;
	}

	// 受付用のSocketを作成
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET) {
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("待ち受けSocketの生成に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
		return -1;
	}

	/*
	 * サーバー側で先にTCPセッションをクローズすると、プロセスで使用していたポートがTIME_WAIT状態となり、
	 * 次、別プロセスが同一ポートを利用しようと場合、使用中と判断されてしまいエラーとなってしまう。
	 * 処理が終了したプロセスで使っていたポート番号をすぐに再利用出来るようにSO_REUSEADDRをONにし、
	 * TIME_WAIT状態のポート番号にSocketをbind出来るようにする
	 */
	sockOptVal = SOCK_OPT_ON;
	setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (LPSTR)&sockOptVal, sizeof(sockOptVal));

	/** 受付を許可するIP情報を設定 **/
	memset(&listenAddr, 0, sizeof(listenAddr));
	// ファミリー
	listenAddr.sin_family = AF_INET;
	// ポート番号
	listenAddr.sin_port = htons(CLIENT_PORT);
	// IPアドレス
	listenAddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

	// 受付用のSocketへ受付を許可するIP情報を紐付ける
	if (bind(listenSock, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) != 0) {
		int errCode = WSAGetLastError();
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("受付用のSocketへ受付を許可するIP情報を紐付けに失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
		return -1;
	}

	// TCPクライアントからの接続要求を待てる状態にする
	if (listen(listenSock, MAX_CLIENT) != 0) {
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("サーバーからの接続要求待ちの準備中にエラーが発生しました。。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
		return -1;
	}


	while(1) {
		// TCPクライアントからの接続要求を受け付ける
		clientAddrlen = sizeof(clientAddr);
		memset(&clientAddr, 0, clientAddrlen);
		acceptSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrlen);

		if (acceptSock == INVALID_SOCKET) {
			errCode = WSAGetLastError();
			errMsg = getErrorMessage(errCode);
			errorlog(TEXT("受付Socketの生成に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
			continue;
		}

		//CreateThread( ClientHandler, arg );

		infolog(TEXT("サーバーからの要求を受け付けました。IPAddress:[%s] Port:[%d]"),
			 inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		memset(recvMsg, 0, sizeof(recvMsg));
		if (recv(acceptSock, (char*)recvMsg, sizeof(recvMsg), 0) <= 0) {
			errCode = WSAGetLastError();
			errMsg = getErrorMessage(errCode);
			errorlog(TEXT("サーバーからのメッセージ受信に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errMsg, errCode);
			continue;
		}

		infolog(TEXT("サーバーからメッセージを受信しました。Message:[%s] Length:[%d] IPAddress:[%s] Port:[%d]"),
				 recvMsg, _tcslen(recvMsg), inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));


		TCHAR code[4];

		_tcsncpy(code, recvMsg, 3);

		// メッセージコードによって条件を分岐させる
	    if (_tcscmp(code, RESPONSE_KIND_USER)) {
			// 受信したメッセージをリストに追加
			time_t timer;
			POST_MSG_T postMsg;
			time(&timer);
			struct tm *ts = localtime(&timer);
			lstrcpy(postMsg.userName, TEXT("メガネ君"));
		    lstrcpy(postMsg.postMsg, recvMsg+1);
		    postMsg.postTime  = *ts;
		    postMsg.next = NULL;
			postMessage(&postMsg);
	    }

	    if (_tcscmp(code, RESPONSE_KIND_MSG)) {
			// 受信したメッセージをリストに追加
			time_t timer;
			POST_MSG_T postMsg;
			time(&timer);
			struct tm *ts = localtime(&timer);
			lstrcpy(postMsg.userName, TEXT("メガネ君"));
		    lstrcpy(postMsg.postMsg, recvMsg+1);
		    postMsg.postTime  = *ts;
		    postMsg.next = NULL;
			postMessage(&postMsg);
		}

		// TCPセッションの終了
		closesocket(acceptSock);

		if(threadStatus != 0) {
			break;
		}
	}

	// winsock2の終了処理
	WSACleanup();

	return 0;
}


/*
 *
 *
 *
 */
int
sendSocketToClient(TCHAR *msg, size_t msgLen) {
	WSADATA wsaData;
	SOCKET consock;
	struct sockaddr_in srvaddr;
	int errCode;
	TCHAR *errmsg;

	// winsock2の初期化
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		errorlog(TEXT("winsock2の初期化に失敗しました。"));
		return -1;
	}

	// コネクションソケットの作成
	consock = socket(AF_INET, SOCK_STREAM, 0);
	if (consock == INVALID_SOCKET) {
		errCode = WSAGetLastError();
		errmsg = getErrorMessage(errCode);
		errorlog(TEXT("コネクションSocketの生成に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errmsg, errCode);
		return -1;
	}

	/*** 接続先サーバのアドレス情報を設定 ***/
	memset(&srvaddr, 0, sizeof(srvaddr));
	// ファミリー
	srvaddr.sin_family = AF_INET;
	// ポート番号
	srvaddr.sin_port = htons(SRV_PORT);
	// IPアドレス
	srvaddr.sin_addr.S_un.S_addr = inet_addr(SRV_IP_ADDR);

	// サーバに接続
	if (connect(consock, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) != 0) {
		int errCode = WSAGetLastError();
		errCode = WSAGetLastError();
		errmsg = getErrorMessage(errCode);
		errorlog(TEXT("サーバーへの接続に失敗しました。エラー詳細:[%s] エラーコード:[%d] IPAddress:[%s] Port:[%d]"),
				errmsg, errCode, inet_ntoa(srvaddr.sin_addr), ntohs(srvaddr.sin_port));
		return -1;
	}

	/*
	// サーバからデータを受信
	 memset(recvMsg, 0, sizeof(recvMsg));
	 if (recv(consock, recvMsg, sizeof(recvMsg), 0) <= 0) {
		 errCode = WSAGetLastError();
		 errmsg = getErrorMessage(errCode);
		 errorlog("サーバーからのメッセージ受信に失敗しました。エラー詳細:[%s] エラーコード:[%d]",
				 errmsg, errCode);
		 return -1;
	 }

	 infolog("サーバーからメッセージを受信しました。Message:[%s] Length:[%d]",
			 recvMsg , strlen(recvMsg));

	 Sleep(1);
	 */

	 // サーバーへメッセージを送信
	 if (send(consock, (char*)msg, msgLen, 0) <= 0) {
		 errCode = WSAGetLastError();
		 errmsg = getErrorMessage(errCode);
		 errorlog(TEXT("サーバーへのメッセージ送信に失敗しました。エラー詳細:[%s] エラーコード:[%d]"), errmsg, errCode);
		 return -1;
	 }

	 infolog(TEXT("サーバーへメッセージを送信しました。Message:[%s] Length:[%d]"), msg, _tcslen(msg));

	// winsock2の終了処理
	WSACleanup();

	return 0;
}


/**
 * WSAGetLastError()で取得したエラーコードからエラーメッセージを取得し、返却します.
 *
 * @param errcd WSAGetLastError()で取得したエラーコード
 * @return エラーメッセージを格納したメモリのポインタ
 *
 */
static TCHAR*
getErrorMessage(int errcd){
	LPVOID msg;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM |
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
		errcd,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
	    (TCHAR*)&msg,
	    0,
	    NULL
	);

	return msg;
}

