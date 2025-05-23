// Copyright © 2025 CronoGames. All Rights Reserved.
// Author: Matt Brock

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../Auth/UserRecord.h"
#include "../Auth/KingdomTypes.h"
#include "../Kingdom/EtherealKingdom.h"
#include "EKPlayerController.generated.h"

// User Authentication Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginResult, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegisterResult, bool, bWasSuccessful);

// Kingdom Management Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateKingdomResult, bool, bWasSuccessful);

UCLASS()
class ETHEREALKINGDOMSUE_API AEKPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEKPlayerController();

protected:
	virtual void BeginPlay() override;

/*********************************************************************************************/
// User: Public Methods

public:
	UFUNCTION(BlueprintCallable, Category = "Auth")
	void AttemptLogin(const FString& Username, const FString& Password);

	UFUNCTION(BlueprintCallable, Category = "Auth")
	void RegisterUser(const FString& Username, const FString& Password);

	UFUNCTION(BlueprintCallable)
	bool IsLoggedIn() const;

	UFUNCTION(BlueprintCallable)
	const FUserRecord& GetUserProfile() const;

	UFUNCTION(BlueprintCallable, Category = "Auth")
	void LogoutUser();

/*********************************************************************************************/
// User: Server RPCs

protected:
	UFUNCTION(Server, Reliable)
	void ServerAttemptLogin(const FString& Username, const FString& Password);

	UFUNCTION(Server, Reliable)
	void ServerAttemptRegisterUser(const FString& Username, const FString& Password);

/*********************************************************************************************/
// User: Server Logic

protected:
	void HandleLoginResult(bool bSuccess, const FUserRecord& User);
	void HandleRegisterResult(bool bSuccess);

/*********************************************************************************************/
// User: Client RPCs

protected:
	UFUNCTION(Client, Reliable)
	void ClientReceiveLoginResult(bool bWasSuccessful, const FUserRecord& User);

	UFUNCTION(Client, Reliable)
	void ClientReceiveRegisterResult(bool bWasSuccessful);

	UFUNCTION(Client, Reliable)
	void ClientUpdateUserProfile(const FUserRecord& UpdatedUser);

/*********************************************************************************************/
// Internal Utility

protected:
	void SetUserProfile(const FUserRecord& User);

/*********************************************************************************************/
// Kingdom: Public Methods

public:
	UFUNCTION(BlueprintCallable, Category = "Kingdom")
	void CreateKingdom(const FString& KingdomName);

	UFUNCTION(BlueprintCallable, Category = "Kingdom")
	const UEtherealKingdom* GetKingdom() const;

	UFUNCTION(BlueprintCallable)
	bool SpendResource(const FString& ResourceName, int32 Amount);

	void SetKingdomData(UEtherealKingdom* Kingdom);

	UFUNCTION(BlueprintCallable, Category = "Kingdom")
	void SaveKingdom();

/*********************************************************************************************/
// Kingdom: Server RPCs

protected:
	UFUNCTION(Server, Reliable)
	void ServerCreateKingdom(const FString& KingdomName);

/*********************************************************************************************/
// Kingdom: Client RPCs

protected:
	UFUNCTION(Client, Reliable)
	void ClientReceiveCreateKingdomResult(bool bSuccess);

	UFUNCTION(Client, Reliable)
	void ClientUpdateKingdomData(const FKingdomData& Data);

/*********************************************************************************************/
// Delegates

public:
	UPROPERTY(BlueprintAssignable, Category = "Auth")
	FOnLoginResult OnLoginResult;

	UPROPERTY(BlueprintAssignable, Category = "Auth")
	FOnRegisterResult OnRegisterResult;

	UPROPERTY(BlueprintAssignable, Category = "Kingdom")
	FOnCreateKingdomResult OnCreateKingdomResult;

/*********************************************************************************************/
// Internal State

protected:
	bool bIsLoggedIn = false;

private:
	FUserRecord CurrentUser;

	UPROPERTY()
	UEtherealKingdom* CurrentKingdom;
};