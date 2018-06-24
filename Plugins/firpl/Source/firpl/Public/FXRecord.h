#pragma once
/*

����iat_record_sample.c

* ������д(iFly Auto Transform)�����ܹ�ʵʱ�ؽ�����ת���ɶ�Ӧ�����֡�
*/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <errno.h>
#include <process.h>

#include "msp_cmn.h"
#include "msp_errors.h"

#include "XFSpeechRecoginzer.h"


#define FRAME_LEN	640 
#define	BUFFER_SIZE	4096

//
enum {
	EVT_START = 0,
	EVT_STOP,
	EVT_QUIT,
	EVT_TOTAL
};


class FXRecord
{
	public:
		FXRecord();
		FXRecord(FString);
		~FXRecord();

		void Start();//��ʼ¼��
		void Stop();//����
		void Quit();//�˳�����

		//��ʾʶ��������˷����Ƶ
		void demo_mic(const char* session_begin_params);
		//��ʾ���ļ��з�����Ƶ����
		void demo_file(const char* audio_file, const char* session_begin_params);

		//��ʶ��������
		//const char* get_result() const;
		FString& get_result() const;

		
};

static FXRecord * xunfeispeech = new FXRecord(FString("static FXRecord be created !"));