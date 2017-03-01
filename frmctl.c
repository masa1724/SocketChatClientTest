#include "frmctl.h"

#ifdef UNICODE
#define __stprintf(dist, format, ...) _stprintf(dist, _tcslen(dist)+2, format, ##__VA_ARGS__)
#else
#define __stprintf(dist, format, ...) _stprintf(dist, format, ##__VA_ARGS__)
#endif


/*
 *  ���͂��ꂽ���b�Z�[�W�𑗐M����.
 *    - ���X�g�r���[�ւ̓o�^
 *    - �T�[�o�[�ւ̃��b�Z�[�W���M
 *
 *  @param *post : ���M���b�Z�[�W�̏��
 *
 */
int postMessage(POST_MSG_T *post) {
	TCHAR dateTimeStr[20];
    convDateStr(&post->postTime, dateTimeStr);

    // ���݂̃��b�Z�[�W�����擾
    int chatCount =  ListView_GetItemCount(hListMsg);

    // ���X�g�r���[�Ƀ��b�Z�[�W��ݒ�
    // ���t
    LVITEM liDateStr;
    liDateStr.mask = LVIF_TEXT;
    liDateStr.pszText = dateTimeStr;
	liDateStr.iSubItem = 0;
	liDateStr.iItem = chatCount;
	ListView_InsertItem(hListMsg, &liDateStr);

	// ���[�U��
	LVITEM liUserName;
	liUserName.mask = LVIF_TEXT;
	liUserName.pszText = post->userName;
	liUserName.iSubItem = 1;
	liUserName.iItem = chatCount;
	ListView_SetItem(hListMsg, &liUserName);

	// ���b�Z�[�W
	LVITEM liMsg;
    liMsg.mask = LVIF_TEXT;
    liMsg.pszText = post->postMsg;
    liMsg.iSubItem = 2;
    liMsg.iItem = chatCount;
    ListView_SetItem(hListMsg, &liMsg);
	chatCount++;

	// �X�N���[���ʒu�����[�Ɉړ�������
	ListView_Scroll(hListMsg, 0, chatCount);

	return 0;
}

/**
 * tm�\���̂̃C���X�^���X����t������ɕϊ�����
 *   �ϊ��� ��) 2016/12/01 23:59:59
 *
 * @param *ts  tm�̃C���X�^���X
 * @param *buf �R�s�[��̕����^�z��
 *
 */
void
convDateStr(struct tm *ts, LPTSTR buf) {
	__stprintf(buf, TEXT("%04d/%02d/%02d %02d:%02d:%02d"),
			ts->tm_year+1900, ts->tm_mon+1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec);
}
