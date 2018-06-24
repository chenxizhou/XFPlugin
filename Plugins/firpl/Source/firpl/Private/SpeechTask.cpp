#pragma  once

#include "SpeechTask.h"

//����iat_record_sample  main ����
void FSpeechTask::DoWork()
{
	int			ret = MSP_SUCCESS;
	int			upload_on = 0; //�Ƿ��ϴ��û��ʱ� ѡ���ϴ�
	const char* login_params = "appid = 5b22eaf4, work_dir = ."; // ��¼������appid��msc���,��������Ķ�
	int aud_src = 0;

	/*
	* sub:				����ҵ������
	* domain:			����
	* language:			����
	* accent:			����
	* sample_rate:		��Ƶ������
	* result_type:		ʶ������ʽ
	* result_encoding:	��������ʽ
	*
	*/
	const char* session_begin_params = "sub = iat,domain = iat,language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = json, result_encoding = UTF-8";

	/* �û���¼ */
	ret = MSPLogin(NULL, NULL, login_params); //��һ���������û������ڶ������������룬����NULL���ɣ������������ǵ�¼����	
	if (MSP_SUCCESS != ret) {
		UE_LOG(LogTemp, Error, TEXT("MSPLogin failed , Error code .\n"));
		goto exit; //��¼ʧ�ܣ��˳���¼
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MSPLogin succ!\n"));
	}

	xunfeispeech->demo_mic(session_begin_params);

exit:
	MSPLogout(); //�˳���¼

	return;
}
