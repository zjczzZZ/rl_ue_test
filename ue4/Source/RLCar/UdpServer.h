// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Sockets/Public/SocketSubsystem.h"
#include "Runtime/Networking/Public/Common/UdpSocketBuilder.h"
#include "Runtime/Networking/Public/Common/UdpSocketReceiver.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "UdpServer.generated.h"

UCLASS()
class RLCAR_API AUdpServer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUdpServer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:	

	FSocket* ListenSocket;
	FUdpSocketReceiver* UDPReceiver = nullptr;

	//Զ�̵ĵ�ַ
	TSharedPtr<FInternetAddr> RemoteAddr;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION(BlueprintCallable, Category = "UDP")
	void StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const FString& sendIP, const int32 sendPort, bool& success);

	UFUNCTION(BlueprintCallable, Category = "UDP")
	void DataRecv(FString& str, bool& success);

	UFUNCTION(BlueprintCallable, Category = "UDP")
	bool RamaUDPSender_SendString(FString ToSend);
};
