#include "properties.h"


static void strim(TCHAR *tobuf, LPCTSTR frombuf);

int
loadPropery(
	LPCTSTR filePath, PROP_T *prop) {

	TCHAR linebuf[1024];
	TCHAR trimedbuf[1024];
	memset(prop->keys, 0, sizeof(prop->keys));
	memset(prop->vals, 0, sizeof(prop->vals));

	memset(linebuf, 0, sizeof(linebuf));
	memset(trimedbuf, 0, sizeof(trimedbuf));
	size_t count = 0;

	FILE *fp;
	fp = _tfopen(filePath, TEXT("r"));
	if(fp == NULL) {
		return -1;
	}

	TCHAR *tp;

	while(_fgetts(linebuf, sizeof(linebuf), fp) != 0) {
		// 前後スペースをトリム
		strim(trimedbuf, linebuf);

		// 空行は無視
		// 必ず key + "=" で2文字以上は確定しているため、1文字の場合も無視する
		if(_tcslen(trimedbuf) <= 1) {
			continue;
		}

		// コメント行は無視
		if(trimedbuf[0] == TEXT('#')) {
			continue;
		}

		tp = _tcstok(trimedbuf, TEXT("="));
		_tcscpy(prop->keys[count], tp);

		tp += _tcslen(tp) + 1;
		_tcscpy(prop->vals[count], tp);
		count++;
	}

	prop->count = count;

	fclose(fp);

	return 0;
}

void
getValue (const PROP_T *prop, LPCTSTR key, TCHAR *buf) {
	for(size_t i = 0; i < prop->count; i++) {
		if(_tcscmp(prop->keys[i], key) == 0) {
			_tcscpy(buf, prop->vals[i]);
			break;
		}
	}
}

/**
 * 文字型配列から半角スペース・改行をトリムする。
 *
 * @param *tobuf   トリム後の文字型配列
 * @param *frombuf トリム対象の文字型配列
 *
 */
static void
strim(TCHAR *tobuf, LPCTSTR frombuf) {
	int startidx = 0;
	int endidx = 0;
	int fromsize = _tcslen(frombuf) - 1;

	for(int i = 0; i < fromsize; i++) {
		if (frombuf[i] == TEXT('\0')) {
			break;
		}

		if (frombuf[i] != TEXT(' ') &&
			frombuf[i] != TEXT('\r') &&
			frombuf[i] != TEXT('\n')) {
			startidx = i;
			break;
		}
	}

	for(int i = fromsize; 0 <= i; i--) {
		if (frombuf[i] != TEXT(' ') &&
			frombuf[i] != TEXT('\r') &&
			frombuf[i] != TEXT('\n')) {
			endidx = i;
			break;
		}
	}

	_tcsncpy(tobuf, frombuf+startidx, endidx - startidx + 1);
	tobuf[endidx+1] = TEXT('\0');
	tobuf[endidx+2] = TEXT('\0');
}
