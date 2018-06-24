// Fill out your copyright notice in the Description page of Project Settings.
//Ϊ��ͼ��¶��������:
//create_recorder   	����¼������
//open_recorder      	��¼����������¼����ʽ
//start_record       	��ʼ¼����¼��������ע��Ļص������л�ȡ
//stop_record        	ֹͣ¼��
//close_recorder   	 	�ر�¼��������Ӧopen_recorder
//destroy_recorder  	����¼�����󣬶�Ӧ create_recorder


#pragma once

#include "SpeechTask.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XFActor.generated.h"

UCLASS()
class FIRPL_API AXFActor : public AActor
{
	GENERATED_BODY()
	
private:
	FString Result;;//�����������ʶ����

public:	
	// Sets default values for this actor's properties
	AXFActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*---  ��չ����    --*/
public:
	UFUNCTION(BlueprintCallable, Category = "XunFei", meta = (DisplayName = "XFInit", Keywords = "Init"))
		void XFInit();//�������ܳ�ʼ��
	UFUNCTION(BlueprintCallable, Category = "XunFei", meta = (DisplayName = "XFOpen", Keywords = "Open"))
		void XFOpen();//����¼������
	UFUNCTION(BlueprintCallable, Category = "XunFei", meta = (DisplayName = "XFStop", Keywords = "Stop"))
		void XFStop();//ֹͣ¼������
	UFUNCTION(BlueprintCallable, Category = "XunFei", meta = (DisplayName = "XFQuit", Keywords = "Quit"))
		void XFQuit();//�˳�¼�������ͷ���Դ
	UFUNCTION(BlueprintCallable, Category = "XunFei", meta = (DisplayName = "XFResult", Keywords = "GetResult"))
		FString XFResult();//��ȡ¼�����
};
