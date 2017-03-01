
#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "log.h"


/* プロパティ構造体 */
typedef struct __PROP_T__{
	TCHAR keys[100][300];
	TCHAR vals[1024][300];
	size_t count;
} PROP_T;


/**
 *  文字型配列から半角スペース・改行をトリムする。
 *
 *  @param *tobuf   トリム後の文字列のコピー先文字型配列
 *  @param *frombuf トリム対象の文字型配列
 *  @return プロパティファイルの読込み 成功(0) 失敗(0以外)
 *
 */
int loadPropery(LPCTSTR filePath, PROP_T *prop);


/**
 * PROP構造体から指定されたキーに対応するプロパティ値を取得し、返却します。
 *
 * @param *prop PROP構造体のインスタンス
 * @param *key  キー
 * @param *buf  取得したプロパティ値の格納先
 *
 */
void getValue (const PROP_T *prop, LPCTSTR key, TCHAR *buf);


#endif /* PROPERTIES_H_ */
