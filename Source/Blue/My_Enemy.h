// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h" 
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMesh.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "My_AIController.h"
#include "My_Enemy.generated.h"


UCLASS()
class BLUE_API AMy_Enemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMy_Enemy();
public:
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveCompletedDelegate);
//FOnMoveCompletedDelegate OnMoveCompletedDelegate;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* Arrow1;
	UCharacterMovementComponent* CharacterMovement;
	UPROPERTY(EditAnyWhere)
	int SkeletalMeshNum = 4;
	int32 index;
protected:


	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly)
	class UAnimInstance* AnimInstance;

protected:
	void SetCharacterMeshPosition(ACharacter* Character);
	void LoadAssets();

protected:
	UPROPERTY(EditAnywhere)
	bool Canattack = true;

	void InitializeAIController();
	void ChaseToPlayer();

	bool IsPlayerReached();
private:
	UFUNCTION()
	void OnMontageBegin(FName MontageName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* MontageName, bool bInterrupted);
	void Attack();
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
private:
	float Health = 20.0f;
public:
	UFUNCTION()
	void take_damage(float value,FVector ImpulseDirection);
	float BaseDamage = 10.0f;
	bool chase = true;

public:
	USoundBase* MySound;
	UAudioComponent* AudioComponent;
};
