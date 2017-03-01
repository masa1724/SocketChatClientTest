/*
 * message.h
 *
 *  Created on: 2017/01/31
 *      Author: sgym2835win
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <tchar.h>
#include <sys/time.h>


// チャット情報の構造体
typedef struct _POST_MSG_T_ {
	TCHAR userName[100];
	TCHAR postMsg[200];
	struct _POST_MSG_T_ *next;
	struct tm postTime;
} POST_MSG_T;

#endif /* MESSAGE_H_ */
