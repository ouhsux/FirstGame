// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CanvasTypes.h" // 包含 Canvas 类型定义的头文件
#include "Engine/Canvas.h" // 包含 Canvas 类的头文件
#include "GameFramework/HUD.h" 
#include "GameFramework/Character.h"
#include "Internationalization/Text.h"
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
#include "My_Player.generated.h"

UCLASS()
class BLUE_API AMy_Player : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AMy_Player();
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
	class UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly)
	class UAnimInstance* AnimInstance;
	

protected:
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArm;
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* FollowCamera;
	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* Arrow1;
protected:
	void SetCharacterMeshPosition(ACharacter* Character);
	void LoadAssets(FString SkeletalMesh);
	FString MySkeletalMesh = TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_03.MESH_PC_03'");
	
private:
	void MoveFB(float Value);
	void MoveLR(float Value);
	void Rotate(float Value);
	void Rotatey(float Value);
	void AttackInput();

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1.0f;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 1.0f;

	UPROPERTY(EditAnywhere)
	bool Canattack = true;

private:
	UFUNCTION()
	void OnMontageBegin(FName MontageName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnMontageEnd(UAnimMontage* MontageName, bool bInterrupted);

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 100;
	void take_damage(float value,FVector ImpulseDirection);
	float BashDamage = 10; 

	void SpawnUserWidget();

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> WavesWidgetClass;

	UUserWidget* HealthBarWidget;
	UUserWidget* WavesWidget;

	UFUNCTION(BlueprintImplementableEvent)
	void DamageScreen();
public:
	USoundBase* MySound;
	UAudioComponent* AudioComponent;
};
