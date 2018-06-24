// Fill out your copyright notice in the Description page of Project Settings.

#pragma  once

#include "XFActor.h"
//����UE4  JSON�� F:\UE418\UE_4.18\Engine\Source\Runtime\Json
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "Serialization//JsonSerializer.h"


#include "firpl.h"



// Sets default values
AXFActor::AXFActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AXFActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("-----BeginPlay-----"));
}

// Called every frame
void AXFActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
//	UE_LOG(LogTemp, Log, TEXT("-----Tick-----"));
}

void AXFActor::XFInit()
{
	//����һ��speechtask ʵ�� ue4���๹����ʽ
	FAutoDeleteAsyncTask<FSpeechTask>*SpeechTask = new FAutoDeleteAsyncTask<FSpeechTask>();

	if (SpeechTask)
	{
		SpeechTask->StartBackgroundTask();//�����߳��첽ִ��SpeechTask�е�DoWork����
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("XF task object could not be create!\n"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("XF task object Stopped !\n"));
	return;

}

void AXFActor::XFOpen()
{
	xunfeispeech->Start();


}

void AXFActor::XFStop()
{
	xunfeispeech->Stop();
}

void AXFActor::XFQuit()
{
	xunfeispeech->Quit();
	Sleep(300);//�ӳ�0.3��
}

FString AXFActor::XFResult()
{
	Result = xunfeispeech->get_result();


	return Result;
}

