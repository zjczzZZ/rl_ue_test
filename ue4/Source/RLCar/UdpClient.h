// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Runtime/Sockets/Public/Sockets.h"
#include "Sockets/Public/SocketSubsystem.h"

#include "Runtime/Networking/Public/Common/UdpSocketBuilder.h"
#include "UdpClient.generated.h"

UCLASS()
class RLCAR_API AUdpClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUdpClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//发送的Socket
	FSocket* SenderSocket;

	//远程的地址
	TSharedPtr<FInternetAddr> RemoteAddr;
	UFUNCTION(BlueprintCallable, Category = "UDP")
	bool StartUDPSender(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, bool UDP);
	
public:
	bool IsUDP;
	UFUNCTION(BlueprintCallable, Category = "UDP")
	bool RamaUDPSender_SendString(FString ToSend);

	UFUNCTION(BlueprintCallable, Category = "UDP")
	void DataRecv(FString& str, bool& success);
};
