// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Math/Transform.h"   // 包含 FTransform 头文件
#include "Containers/Array.h" // 包含 TArray 头文件
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_Enemy.h"
#include "MySpawnBox.generated.h"

// 声明一个指向声音资源的指针变量

UCLASS()
class BLUE_API AMySpawnBox : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMySpawnBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere);
	USoundBase* MySound;
	UPROPERTY(EditAnywhere);
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentWave = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxWave = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CurrentName;

private:
	// 添加多个 Transform 到数组中
	// 创建 FTransform 对象数组
	FTransform TransformArray[3];

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int32> MyDictionary;
	// 向字典中添加键值对
	bool CanMake = true;
	bool Check();
	void SpawnWave(int32 num);
	void SpawnYin();
};
