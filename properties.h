
#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "log.h"


/* �v���p�e�B�\���� */
typedef struct __PROP_T__{
	TCHAR keys[100][300];
	TCHAR vals[1024][300];
	size_t count;
} PROP_T;


/**
 *  �����^�z�񂩂甼�p�X�y�[�X�E���s���g��������B
 *
 *  @param *tobuf   �g������̕�����̃R�s�[�敶���^�z��
 *  @param *frombuf �g�����Ώۂ̕����^�z��
 *  @return �v���p�e�B�t�@�C���̓Ǎ��� ����(0) ���s(0�ȊO)
 *
 */
int loadPropery(LPCTSTR filePath, PROP_T *prop);


/**
 * PROP�\���̂���w�肳�ꂽ�L�[�ɑΉ�����v���p�e�B�l���擾���A�ԋp���܂��B
 *
 * @param *prop PROP�\���̂̃C���X�^���X
 * @param *key  �L�[
 * @param *buf  �擾�����v���p�e�B�l�̊i�[��
 *
 */
void getValue (const PROP_T *prop, LPCTSTR key, TCHAR *buf);


#endif /* PROPERTIES_H_ */
