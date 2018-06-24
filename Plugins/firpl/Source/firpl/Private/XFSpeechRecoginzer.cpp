#pragma  once

#include "XFSpeechRecoginzer.h"


#define DEFAULT_SESSION_PARA \
	 "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = json, result_encoding = UTF-8"

#define DEFAULT_FORMAT		\
{\
	WAVE_FORMAT_PCM,	\
	1,					\
	16000,				\
	32000,				\
	2,					\
	16,					\
	sizeof(WAVEFORMATEX)	\
}

/* internal state */
enum {
	SR_STATE_INIT,
	SR_STATE_STARTED
};

#define SR_MALLOC malloc
#define SR_MFREE  free
#define SR_MEMSET	memset

FString _TempResult;

XFSpeechRecoginzer::XFSpeechRecoginzer()
{
}

XFSpeechRecoginzer::XFSpeechRecoginzer(FString s)
{
	UE_LOG(LogTemp, Error, TEXT("XFSpeechRecoginzer() Create :%s\n"),*s);
}

XFSpeechRecoginzer::~XFSpeechRecoginzer()
{
}

int XFSpeechRecoginzer::sr_init(speech_rec * sr, const char * session_begin_params, sr_audsrc aud_src, int devid, speech_rec_notifier * notifier)
{
	int errcode;
	size_t param_size;
	WAVEFORMATEX wavfmt = DEFAULT_FORMAT;

	if (aud_src == SR_MIC && winrec->get_input_dev_num() == 0) {
		return -E_SR_NOACTIVEDEVICE;
	}

	if (!sr)
		return -E_SR_INVAL;

	if (session_begin_params == NULL) {
		session_begin_params = DEFAULT_SESSION_PARA;
	}

	SR_MEMSET(sr, 0, sizeof(struct speech_rec));
	sr->state = SR_STATE_INIT;

	sr->aud_src = aud_src;
	sr->ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
	sr->rec_stat = MSP_REC_STATUS_SUCCESS;
	sr->audio_status = MSP_AUDIO_SAMPLE_FIRST;

	param_size = strlen(session_begin_params) + 1;
	sr->session_begin_params = (char*)SR_MALLOC(param_size);
	if (sr->session_begin_params == NULL) {
		UE_LOG(LogTemp, Error, TEXT("mem alloc failed\n"));
		return -E_SR_NOMEM;
	}
	strncpy(sr->session_begin_params, session_begin_params, param_size - 1);

	sr->notif = *notifier;

	if (aud_src == SR_MIC) {
		//����¼����
		errcode = winrec->create_recorder(&sr->recorder, iat_cb, (void*)sr);
		if (sr->recorder == NULL || errcode != 0) {
			UE_LOG(LogTemp, Error, TEXT("create recorder failed:%d \n"),errcode);
			errcode = -E_SR_RECORDFAIL;
			goto fail;
		}
		update_format_from_sessionparam(session_begin_params, &wavfmt);
		//��¼����
		errcode = winrec->open_recorder(sr->recorder, devid, &wavfmt);
		if (errcode != 0) {
			UE_LOG(LogTemp, Error, TEXT("recorder open failed:%d \n"), errcode);
			errcode = -E_SR_RECORDFAIL;
			goto fail;
		}
	}

	return 0;

fail:
	if (sr->recorder) {
		winrec->destroy_recorder(sr->recorder);
		sr->recorder = NULL;
	}

	if (sr->session_begin_params) {
		SR_MFREE(sr->session_begin_params);
		sr->session_begin_params = NULL;
	}
	SR_MEMSET(&sr->notif, 0, sizeof(sr->notif));

	return errcode;
}

int XFSpeechRecoginzer::sr_start_listening(speech_rec * sr)
{
	int ret;
	const char*		session_id = NULL;
	int				errcode = MSP_SUCCESS;

	if (sr->state >= SR_STATE_STARTED) {
		//sr_dbg("already STARTED.\n");
		UE_LOG(LogTemp, Warning, TEXT("already STARTED.\n"));
		return -E_SR_ALREADY;
	}
	//����һ��ʶ�����Ự��ʶ����Ƶ����
	session_id = QISRSessionBegin(NULL, sr->session_begin_params, &errcode); //��д����Ҫ�﷨����һ������ΪNULL���������������������óɹ�����ֵΪMSP_SUCCESS�����򷵻ش������
	if (MSP_SUCCESS != errcode)
	{
		//sr_dbg("\nQISRSessionBegin failed! error code:%d\n", errcode);
		UE_LOG(LogTemp, Warning, TEXT("\nQISRSessionBegin failed! error code:%d\n"),errcode);
		return errcode;
	}
	sr->session_id = session_id;//��������ʶ�����ľ��
	sr->ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
	sr->rec_stat = MSP_REC_STATUS_SUCCESS;
	sr->audio_status = MSP_AUDIO_SAMPLE_FIRST;

	if (sr->aud_src == SR_MIC) {
		//��ʼ¼��
		ret = winrec-> start_record(sr->recorder);
		UE_LOG(LogTemp, Error, TEXT("----------Start Record ------- \n"));
		if (ret != 0) {
			//sr_dbg("start record failed: %d\n", ret);
			UE_LOG(LogTemp, Error, TEXT("start record failed:%d \n"),ret);
			//����ʶ�����Ự���ͷ�������Դ��
			QISRSessionEnd(session_id, "start record fail");
			sr->session_id = NULL;
			return -E_SR_RECORDFAIL;
		}

	}

	sr->state = SR_STATE_STARTED;

	if (sr->notif.on_speech_begin)
		sr->notif.on_speech_begin();//��ʼ����ʶ��,���һ���µĽ��(��) �ȴ������

	return 0;
}

int XFSpeechRecoginzer::sr_stop_listening(speech_rec * sr)
{
	int ret = 0;
	const char * rslt = NULL;

	//�ж��Ƿ��Ѿ�����
	if (sr->state < SR_STATE_STARTED) {
		//sr_dbg("Not started or already stopped.\n");
		UE_LOG(LogTemp, Error, TEXT("Not started or already stopped.\n"));
		return 0;
	}

	if (sr->aud_src == SR_MIC) {
		//ֹͣ¼��
		ret = winrec->stop_record(sr->recorder);
		UE_LOG(LogTemp, Error, TEXT("------Have Stop Record!----- \n"));
		if (ret != 0) {
			//sr_dbg("Stop failed! \n");
			UE_LOG(LogTemp, Error, TEXT("Stop failed! \n"));
			return -E_SR_RECORDFAIL;
		}
		wait_for_rec_stop(sr->recorder, (unsigned int)-1);
	}
	sr->state = SR_STATE_INIT;//����״̬Ϊδ����
	UE_LOG(LogTemp, Error, TEXT("------Have SetState=SR_STATE_INIT!  in stop function----- \n"));
	//����������Ƶ����д��ʶ������
	/*�����Ҫ���ϵ��ã�ֱ����Ƶȫ��д��Ϊֹ.
	�ϴ���Ƶʱ����Ҫ���µ�4��������ֵ��������˵:
	��д���׿���Ƶʱ,��audioStatus��ΪMSP_AUDIO_SAMPLE_FIRST
	��д�����һ����Ƶʱ,��audioStatus��ΪMSP_AUDIO_SAMPLE_LAST
	���������,��audioStatus��ΪMSP_AUDIO_SAMPLE_CONTINUE
	ͬʱ���趨ʱ�������������epStatus��rsltStatus��������˵:
	��epStatus��ʾ�Ѽ�⵽��˵�ʱ��MSC�Ѳ��ٽ�����Ƶ��Ӧ��ʱֹͣ��Ƶд��
	��rsltStatus��ʾ��ʶ��������ʱ�����ɴ�MSC�����л�ȡ���
	*/
	ret = QISRAudioWrite(sr->session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &sr->ep_stat, &sr->rec_stat);
	UE_LOG(LogTemp, Error, TEXT("------Have QISRAudioWrite!  in stop function----- \n"));
	if (ret != 0) {
		//sr_dbg("write LAST_SAMPLE failed: %d\n", ret);
		UE_LOG(LogTemp, Error, TEXT("write LAST_SAMPLE failed: %d\n"), ret);
		//����ʶ�����Ự���ͷ�������Դ��
		QISRSessionEnd(sr->session_id, "write err");
		return ret;
	}

	while (sr->rec_stat != MSP_REC_STATUS_COMPLETE) {
		//���ָ����ʽ��ʶ������
		rslt = QISRGetResult(sr->session_id, &sr->rec_stat, 0, &ret);
		FString temprslt = FString(UTF8_TO_TCHAR(rslt));
		UE_LOG(LogTemp, Error, TEXT("------Have QISRGetResult=%s!  in stop function----- \n"),*temprslt);
		if (MSP_SUCCESS != ret) {
			//sr_dbg("\nQISRGetResult failed! error code: %d\n", ret);
			UE_LOG(LogTemp, Error, TEXT("\nQISRGetResult failed! error code: %d\n"), ret);
			end_sr_on_error(sr, ret);
			return ret;
		}
		if (NULL != rslt && sr->notif.on_result)
		{
			sr->notif.on_result(rslt, sr->rec_stat == MSP_REC_STATUS_COMPLETE ? 1 : 0);
			UE_LOG(LogTemp, Error, TEXT("------Have call on_result! in stop function----- \n"));
		}
		Sleep(100);
	}
	//����ʶ�����Ự���ͷ�������Դ��
	sr->notif.on_speech_end(END_REASON_VAD_DETECT);

	UE_LOG(LogTemp, Error, TEXT("------Now need QISRSessionEnd!----- \n"));
	QISRSessionEnd(sr->session_id, "normal");
	sr->session_id = NULL;

	//_TempResult.Reset();

	return 0;
}

int XFSpeechRecoginzer::sr_write_audio_data(speech_rec * sr, char * data, unsigned int len)
{
	const char *rslt = NULL;
	int ret = 0;
	if (!sr)
		return -E_SR_INVAL;
	if (!data || !len)
		return 0;

	ret = QISRAudioWrite(sr->session_id, data, len, sr->audio_status, &sr->ep_stat, &sr->rec_stat);
	if (ret) {
		end_sr_on_error(sr, ret);
		return ret;
	}
	sr->audio_status = MSP_AUDIO_SAMPLE_CONTINUE;

	if (MSP_REC_STATUS_SUCCESS == sr->rec_stat) { //�Ѿ��в�����д���
		rslt = QISRGetResult(sr->session_id, &sr->rec_stat, 0, &ret);
		if (MSP_SUCCESS != ret) {
			UE_LOG(LogTemp, Error, TEXT("\nQISRGetResult failed! error code: %d\n"), ret);
			end_sr_on_error(sr, ret);
			return ret;
		}
		if (NULL != rslt && sr->notif.on_result)
			sr->notif.on_result(rslt, sr->rec_stat == MSP_REC_STATUS_COMPLETE ? 1 : 0);
	}

	if (MSP_EP_AFTER_SPEECH == sr->ep_stat)//�ж��Ƿ��⵽�˶���Ƶ�ĺ�˵㣬��̵���Ƶ�ᱻMSC����
		end_sr_on_vad(sr);

	return 0;
}

void XFSpeechRecoginzer::sr_uninit(speech_rec * sr)
{
	if (sr->recorder) {
		if (!winrec->is_record_stopped(sr->recorder))
			winrec->stop_record(sr->recorder);
		//�ر�¼����
		winrec->close_recorder(sr->recorder);
		UE_LOG(LogTemp, Error, TEXT("-----Close Recorder --- \n"));
		//����¼��������
		winrec->destroy_recorder(sr->recorder);
		UE_LOG(LogTemp, Error, TEXT("-----Destroy Recorder --- \n"));
		sr->recorder = NULL;
	}

	if (sr->session_begin_params) {
		SR_MFREE(sr->session_begin_params);
		sr->session_begin_params = NULL;
	}
}

void XFSpeechRecoginzer::end_sr_on_error(speech_rec * sr, int errcode)
{
	if (sr->aud_src == SR_MIC)
		winrec->stop_record(sr->recorder);

	if (sr->session_id) {
		if (sr->notif.on_speech_end)
			sr->notif.on_speech_end(errcode);

		QISRSessionEnd(sr->session_id, "err");
		sr->session_id = NULL;
	}
	sr->state = SR_STATE_INIT;
}

void XFSpeechRecoginzer::end_sr_on_vad(speech_rec * sr)
{
	int errcode;
	const char *rslt;

	if (sr->aud_src == SR_MIC)
		winrec->stop_record(sr->recorder);

	while (sr->rec_stat != MSP_REC_STATUS_COMPLETE) {
		rslt = QISRGetResult(sr->session_id, &sr->rec_stat, 0, &errcode);
		FString temprslt = FString(UTF8_TO_TCHAR(rslt));
		UE_LOG(LogTemp, Error, TEXT("------Have QISRGetResult=%s!  in end_sr_on_vad function----- \n"), *temprslt);
		if (rslt && sr->notif.on_result)
		{
			sr->notif.on_result(rslt, sr->rec_stat == MSP_REC_STATUS_COMPLETE ? 1 : 0);
			UE_LOG(LogTemp, Error, TEXT("------Have call on_result! in end_sr_on_vad function----- \n"));
		}
		Sleep(100); /* for cpu occupy, should sleep here */
	}

	if (sr->session_id) {
		if (sr->notif.on_speech_end)
			sr->notif.on_speech_end(END_REASON_VAD_DETECT);
		QISRSessionEnd(sr->session_id, "VAD Normal");
		sr->session_id = NULL;
	}
	sr->state = SR_STATE_INIT;



}

char * XFSpeechRecoginzer::skip_space(char * s)
{
	while (s && *s != ' ' && *s != '\0')
		s++;
	return s;
}

int XFSpeechRecoginzer::update_format_from_sessionparam(const char * session_para, WAVEFORMATEX * wavefmt)
{
	char *s{ (char*)strstr(session_para, "sample_rate") };
	if (s) {
		s = strstr(s, "=");
		if (s) {
			s = skip_space(s);
			if (s && *s) {
				wavefmt->nSamplesPerSec = atoi(s);
				wavefmt->nAvgBytesPerSec = wavefmt->nBlockAlign * wavefmt->nSamplesPerSec;
			}
		}
		else
			return -1;
	}
	else {
		return -1;
	}

	return 0;
}

void XFSpeechRecoginzer::wait_for_rec_stop(recorder * rec, unsigned int timeout_ms)
{
	while (!winrec->is_record_stopped(rec)) {
		Sleep(1);
		if (timeout_ms != (unsigned int)-1)
			if (0 == timeout_ms--)
				break;
	}
}



void iat_cb(char * data, unsigned long len, void * user_para)
{
	int errcode;
	struct speech_rec *sr;

	if (len == 0 || data == NULL)
		return;

	sr = (struct speech_rec *)user_para;

	if (sr == NULL || sr->ep_stat >= MSP_EP_AFTER_SPEECH)
		return;

	errcode = speechrecoginzer->sr_write_audio_data(sr, data, len);
	if (errcode) {
		speechrecoginzer->end_sr_on_error(sr, errcode);
		return;
	}
}
