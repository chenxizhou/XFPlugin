
/*
��winrec.h
ֱ�Ӵ��п���  ��װ¼��
* @file
* @brief a record interface in windows
*
* it encapsluate the windows API waveInxxx;
* Common steps:
*	create_recorder,
*	open_recorder,
*	start_record,
*	stop_record,
*	close_recorder,
*	destroy_recorder
*
* @author		hzy
* @date		2018/06/15
*/

#pragma once
#include "firpl.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <mmsystem.h>   
#include <process.h>
#include <errno.h>



/* error code */
enum {
	RECORD_ERR_BASE = 0,
	RECORD_ERR_GENERAL,
	RECORD_ERR_MEMFAIL,
	RECORD_ERR_INVAL,
	RECORD_ERR_NOT_READY
};



//DEFINE_LOG_CATEGORY_STATIC()

/* ����һ��¼��(��¼��)���� */
struct recorder {
	void(*on_data_ind)(char *data, unsigned long len, void *user_para);
	void * user_cb_para;
	volatile int state;		/* ��ʼ��¼������״̬ */

	void * wavein_hdl;
	void * rec_thread_hdl;
	void * bufheader;
	unsigned int bufcount;
};

//DECLARE_LOG_CATEGORY_EXTERN(UseXFLog,Warning,ALL);//������ǩΪ�ⲿ��,�����ظ�����

class UseXF
{
	//������Ԫ����  �ص�����
	friend static unsigned int  __stdcall record_thread_proc(void * para);

public:
	UseXF();
	UseXF(FString);
	~UseXF();


public:
	/**
	* @fn
	* @brief	Get the default input device ID
	*
	* @return	returns WAVE_MAPPER in windows.
	*/
	int get_default_input_dev(); //��ȡĬ�������豸ID

	/**
	* @fn
	* @brief	Get the total number of active input devices.
	* @return	the number. 0 means no active device.
	*/
	unsigned int get_input_dev_num();//��ȡ¼���豸��������

	/**
	* @fn
	* @brief	Create a recorder object.
	* @return	int			- Return 0 in success, otherwise return error code.
	* @param	out_rec		- [out] recorder object holder
	* @param	on_data_ind	- [in]	callback. called when data coming.
	* @param	user_cb_para	- [in] user params for the callback.
	* @see
	*/
	int create_recorder(struct recorder ** out_rec,
		void(*on_data_ind)(char *data, unsigned long len, void *user_para),
		void* user_cb_para);//����һ��¼��������

	/**
	* @fn
	* @brief	Destroy recorder object. free memory.
	* @param	rec	- [in]recorder object
	*/
	void destroy_recorder(struct recorder *rec);//����¼��������.�ͷ��ڴ档

	/**
	* @fn
	* @brief	open the device.
	* @return	int			- Return 0 in success, otherwise return error code.
	* @param	rec			- [in] recorder object
	* @param	dev			- [in] device id, from 0.
	* @param	fmt			- [in] record format.
	* @see
	* 	get_default_input_dev()
	*/
	int open_recorder(struct recorder * rec, unsigned int dev, WAVEFORMATEX * fmt);//����¼���豸��¼����ʽ������¼������

	/**
	* @fn
	* @brief	close the device.
	* @param	rec			- [in] recorder object
	*/

	void close_recorder(struct recorder *rec);//�ر�¼����

	/**
	* @fn
	* @brief	start record.
	* @return	int			- Return 0 in success, otherwise return error code.
	* @param	rec			- [in] recorder object
	*/
	int start_record(struct recorder * rec);//��ʼ¼����

	/**
	* @fn
	* @brief	stop record.
	* @return	int			- Return 0 in success, otherwise return error code.
	* @param	rec			- [in] recorder object
	*/
	int stop_record(struct recorder * rec);//ֹͣ¼��

	/**
	* @fn
	* @brief	test if the recording has been stopped.
	* @return	int			- 1: stopped. 0 : recording.
	* @param	rec			- [in] recorder object
	*/
	int is_record_stopped(struct recorder *rec);//��ѯ¼���Ƿ��Ѿ�����ֹͣ����

	//һЩ�������� ������winrec.c
private:
	void dbg_wave_header(WAVEHDR * buf);
	int create_callback_thread(void *thread_proc_para, HANDLE *thread_hdl_out);
	void close_callback_thread(HANDLE thread);
	int open_rec_device(int dev, WAVEFORMATEX *format, HANDLE thread, HWAVEIN *wave_hdl_out);
	int prepare_rec_buffer(HWAVEIN wi, WAVEHDR ** bufheader_out, unsigned int headercount, unsigned int bufsize);
	void free_rec_buffer(HWAVEIN wi, WAVEHDR *first_header, unsigned headercount);
	void close_rec_device(HWAVEIN wi);
	int start_record_internal(HWAVEIN wi, WAVEHDR *header, unsigned int bufcount);
	int stop_record_internal(HWAVEIN wi);
	void data_proc(struct recorder *rec, MSG *msg);
	int is_stopped_internal(struct recorder *rec);
	int open_recorder_internal(struct recorder * rec, unsigned int dev, WAVEFORMATEX * fmt);
	void close_recorder_internal(struct recorder *rec);

};
static UseXF* winrec = new UseXF(FString("Static winrec be created")); //������static  function �з��ʳ�Ա