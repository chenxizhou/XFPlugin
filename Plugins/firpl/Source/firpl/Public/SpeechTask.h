#pragma once
//����������ʶ�� ��Ϊ�����߳����������⿪������ʱ����������


#include "FXRecord.h"
#include "AsyncWork.h"

class FSpeechTask:public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FSpeechTask>;

	FSpeechTask()
	{
		UE_LOG(LogTemp, Warning, TEXT("-------Speech Task  Create!-----------."));
	}
	void DoWork();//����ʶ�������
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FSpeechTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};