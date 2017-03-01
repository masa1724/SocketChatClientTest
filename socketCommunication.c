#define WIN32_LEAN_AND_MEAN

#include "socketCommunication.h"

/*
 *
 * http://wisdom.sakura.ne.jp/system/winapi/winsock/winSock2.html
 */

// �ő�N���C�A���g�ڑ���
#define MAX_CLIENT 5

// �\�P�b�g�I�v�V����OFF
#define SOCK_OPT_OFF 0

// �\�P�b�g�I�v�V����ON
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

	// winsock2�̏�����
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		errorlog(TEXT("winsock2�̏������Ɏ��s���܂����B"));
		return -1;
	}

	// ��t�p��Socket���쐬
	listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET) {
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("�҂���Socket�̐����Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
		return -1;
	}

	/*
	 * �T�[�o�[���Ő��TCP�Z�b�V�������N���[�Y����ƁA�v���Z�X�Ŏg�p���Ă����|�[�g��TIME_WAIT��ԂƂȂ�A
	 * ���A�ʃv���Z�X������|�[�g�𗘗p���悤�Əꍇ�A�g�p���Ɣ��f����Ă��܂��G���[�ƂȂ��Ă��܂��B
	 * �������I�������v���Z�X�Ŏg���Ă����|�[�g�ԍ��������ɍė��p�o����悤��SO_REUSEADDR��ON�ɂ��A
	 * TIME_WAIT��Ԃ̃|�[�g�ԍ���Socket��bind�o����悤�ɂ���
	 */
	sockOptVal = SOCK_OPT_ON;
	setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, (LPSTR)&sockOptVal, sizeof(sockOptVal));

	/** ��t��������IP����ݒ� **/
	memset(&listenAddr, 0, sizeof(listenAddr));
	// �t�@�~���[
	listenAddr.sin_family = AF_INET;
	// �|�[�g�ԍ�
	listenAddr.sin_port = htons(CLIENT_PORT);
	// IP�A�h���X
	listenAddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

	// ��t�p��Socket�֎�t��������IP����R�t����
	if (bind(listenSock, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) != 0) {
		int errCode = WSAGetLastError();
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("��t�p��Socket�֎�t��������IP����R�t���Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
		return -1;
	}

	// TCP�N���C�A���g����̐ڑ��v����҂Ă��Ԃɂ���
	if (listen(listenSock, MAX_CLIENT) != 0) {
		errCode = WSAGetLastError();
		errMsg = getErrorMessage(errCode);
		errorlog(TEXT("�T�[�o�[����̐ڑ��v���҂��̏������ɃG���[���������܂����B�B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
		return -1;
	}


	while(1) {
		// TCP�N���C�A���g����̐ڑ��v�����󂯕t����
		clientAddrlen = sizeof(clientAddr);
		memset(&clientAddr, 0, clientAddrlen);
		acceptSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientAddrlen);

		if (acceptSock == INVALID_SOCKET) {
			errCode = WSAGetLastError();
			errMsg = getErrorMessage(errCode);
			errorlog(TEXT("��tSocket�̐����Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
			continue;
		}

		//CreateThread( ClientHandler, arg );

		infolog(TEXT("�T�[�o�[����̗v�����󂯕t���܂����BIPAddress:[%s] Port:[%d]"),
			 inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		memset(recvMsg, 0, sizeof(recvMsg));
		if (recv(acceptSock, (char*)recvMsg, sizeof(recvMsg), 0) <= 0) {
			errCode = WSAGetLastError();
			errMsg = getErrorMessage(errCode);
			errorlog(TEXT("�T�[�o�[����̃��b�Z�[�W��M�Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errMsg, errCode);
			continue;
		}

		infolog(TEXT("�T�[�o�[���烁�b�Z�[�W����M���܂����BMessage:[%s] Length:[%d] IPAddress:[%s] Port:[%d]"),
				 recvMsg, _tcslen(recvMsg), inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));


		TCHAR code[4];

		_tcsncpy(code, recvMsg, 3);

		// ���b�Z�[�W�R�[�h�ɂ���ď����𕪊򂳂���
	    if (_tcscmp(code, RESPONSE_KIND_USER)) {
			// ��M�������b�Z�[�W�����X�g�ɒǉ�
			time_t timer;
			POST_MSG_T postMsg;
			time(&timer);
			struct tm *ts = localtime(&timer);
			lstrcpy(postMsg.userName, TEXT("���K�l�N"));
		    lstrcpy(postMsg.postMsg, recvMsg+1);
		    postMsg.postTime  = *ts;
		    postMsg.next = NULL;
			postMessage(&postMsg);
	    }

	    if (_tcscmp(code, RESPONSE_KIND_MSG)) {
			// ��M�������b�Z�[�W�����X�g�ɒǉ�
			time_t timer;
			POST_MSG_T postMsg;
			time(&timer);
			struct tm *ts = localtime(&timer);
			lstrcpy(postMsg.userName, TEXT("���K�l�N"));
		    lstrcpy(postMsg.postMsg, recvMsg+1);
		    postMsg.postTime  = *ts;
		    postMsg.next = NULL;
			postMessage(&postMsg);
		}

		// TCP�Z�b�V�����̏I��
		closesocket(acceptSock);

		if(threadStatus != 0) {
			break;
		}
	}

	// winsock2�̏I������
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

	// winsock2�̏�����
	if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
		errorlog(TEXT("winsock2�̏������Ɏ��s���܂����B"));
		return -1;
	}

	// �R�l�N�V�����\�P�b�g�̍쐬
	consock = socket(AF_INET, SOCK_STREAM, 0);
	if (consock == INVALID_SOCKET) {
		errCode = WSAGetLastError();
		errmsg = getErrorMessage(errCode);
		errorlog(TEXT("�R�l�N�V����Socket�̐����Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errmsg, errCode);
		return -1;
	}

	/*** �ڑ���T�[�o�̃A�h���X����ݒ� ***/
	memset(&srvaddr, 0, sizeof(srvaddr));
	// �t�@�~���[
	srvaddr.sin_family = AF_INET;
	// �|�[�g�ԍ�
	srvaddr.sin_port = htons(SRV_PORT);
	// IP�A�h���X
	srvaddr.sin_addr.S_un.S_addr = inet_addr(SRV_IP_ADDR);

	// �T�[�o�ɐڑ�
	if (connect(consock, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) != 0) {
		int errCode = WSAGetLastError();
		errCode = WSAGetLastError();
		errmsg = getErrorMessage(errCode);
		errorlog(TEXT("�T�[�o�[�ւ̐ڑ��Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d] IPAddress:[%s] Port:[%d]"),
				errmsg, errCode, inet_ntoa(srvaddr.sin_addr), ntohs(srvaddr.sin_port));
		return -1;
	}

	/*
	// �T�[�o����f�[�^����M
	 memset(recvMsg, 0, sizeof(recvMsg));
	 if (recv(consock, recvMsg, sizeof(recvMsg), 0) <= 0) {
		 errCode = WSAGetLastError();
		 errmsg = getErrorMessage(errCode);
		 errorlog("�T�[�o�[����̃��b�Z�[�W��M�Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]",
				 errmsg, errCode);
		 return -1;
	 }

	 infolog("�T�[�o�[���烁�b�Z�[�W����M���܂����BMessage:[%s] Length:[%d]",
			 recvMsg , strlen(recvMsg));

	 Sleep(1);
	 */

	 // �T�[�o�[�փ��b�Z�[�W�𑗐M
	 if (send(consock, (char*)msg, msgLen, 0) <= 0) {
		 errCode = WSAGetLastError();
		 errmsg = getErrorMessage(errCode);
		 errorlog(TEXT("�T�[�o�[�ւ̃��b�Z�[�W���M�Ɏ��s���܂����B�G���[�ڍ�:[%s] �G���[�R�[�h:[%d]"), errmsg, errCode);
		 return -1;
	 }

	 infolog(TEXT("�T�[�o�[�փ��b�Z�[�W�𑗐M���܂����BMessage:[%s] Length:[%d]"), msg, _tcslen(msg));

	// winsock2�̏I������
	WSACleanup();

	return 0;
}


/**
 * WSAGetLastError()�Ŏ擾�����G���[�R�[�h����G���[���b�Z�[�W���擾���A�ԋp���܂�.
 *
 * @param errcd WSAGetLastError()�Ŏ擾�����G���[�R�[�h
 * @return �G���[���b�Z�[�W���i�[�����������̃|�C���^
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
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // ����̌���
	    (TCHAR*)&msg,
	    0,
	    NULL
	);

	return msg;
}

