// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Sockets/Public/SocketSubsystem.h"
#include "Networking/Public/Networking.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"

#include "tcpclient.generated.h"

UCLASS()
class RLCAR_API Atcpclient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Atcpclient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Զ�̵ĵ�ַ
	TSharedPtr<FInternetAddr> RemoteAddr;

	FSocket* SocketClient;

	FIPv4Endpoint RemoteAddressForConnection;

	FTimerHandle TCPSocketListenerTimerHandle;

	FTimerHandle TCPConnectionListenerTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "tcpclient")
	bool SocketCreate(FString IPStr, int32 port);

	UFUNCTION(BlueprintCallable, Category = "tcpclient")
	void SocketSend(FString meesage);

	UFUNCTION(BlueprintCallable, Category = "tcpclient")
	void SocketReceive(bool& bReceive, FString& recvMessage);

	FString StringFromBinaryArray(TArray<uint8> BinaryArray);

};
