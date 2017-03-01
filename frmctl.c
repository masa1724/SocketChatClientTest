#include "frmctl.h"

#ifdef UNICODE
#define __stprintf(dist, format, ...) _stprintf(dist, _tcslen(dist)+2, format, ##__VA_ARGS__)
#else
#define __stprintf(dist, format, ...) _stprintf(dist, format, ##__VA_ARGS__)
#endif


/*
 *  入力されたメッセージを送信する.
 *    - リストビューへの登録
 *    - サーバーへのメッセージ送信
 *
 *  @param *post : 送信メッセージの情報
 *
 */
int postMessage(POST_MSG_T *post) {
	TCHAR dateTimeStr[20];
    convDateStr(&post->postTime, dateTimeStr);

    // 現在のメッセージ数を取得
    int chatCount =  ListView_GetItemCount(hListMsg);

    // リストビューにメッセージを設定
    // 日付
    LVITEM liDateStr;
    liDateStr.mask = LVIF_TEXT;
    liDateStr.pszText = dateTimeStr;
	liDateStr.iSubItem = 0;
	liDateStr.iItem = chatCount;
	ListView_InsertItem(hListMsg, &liDateStr);

	// ユーザ名
	LVITEM liUserName;
	liUserName.mask = LVIF_TEXT;
	liUserName.pszText = post->userName;
	liUserName.iSubItem = 1;
	liUserName.iItem = chatCount;
	ListView_SetItem(hListMsg, &liUserName);

	// メッセージ
	LVITEM liMsg;
    liMsg.mask = LVIF_TEXT;
    liMsg.pszText = post->postMsg;
    liMsg.iSubItem = 2;
    liMsg.iItem = chatCount;
    ListView_SetItem(hListMsg, &liMsg);
	chatCount++;

	// スクロール位置を下端に移動させる
	ListView_Scroll(hListMsg, 0, chatCount);

	return 0;
}

/**
 * tm構造体のインスタンスを日付文字列に変換する
 *   変換後 例) 2016/12/01 23:59:59
 *
 * @param *ts  tmのインスタンス
 * @param *buf コピー先の文字型配列
 *
 */
void
convDateStr(struct tm *ts, LPTSTR buf) {
	__stprintf(buf, TEXT("%04d/%02d/%02d %02d:%02d:%02d"),
			ts->tm_year+1900, ts->tm_mon+1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec);
}
