// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWave.h"
#include "EngineUtils.h" // ���� EngineUtils.h ͷ�ļ�
#include "TimerManager.h"

// Sets default values
AMyWave::AMyWave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// �ڹ��캯������ BeginPlay() �����м���������Դ
	MySound = LoadObject<USoundBase>(nullptr, TEXT("/Script/Engine.SoundWave'/Game/Audio/Wave.Wave'"));

	// ������Ƶ���������������Դ
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->SetSound(MySound);

	// �����ĸ����λ�õ� FVector
	FVector Location1 = FVector(-360,-2300,478);
	FVector Location2 = FVector(-360, 2460, 88);
	FVector Location3 = FVector(1450, 540, 248);

	// �����ĸ� FTransform ����
	TransformArray[0] = FTransform( Location1);
	TransformArray[1] = FTransform( Location2);
	TransformArray[2] = FTransform( Location3);


	MyDictionary.Add(TEXT("Wave1"), 1);
	MyDictionary.Add(TEXT("Wave2"), 2);
	MyDictionary.Add(TEXT("Wave3"), 3);
}

// Called when the game starts or when spawned
void AMyWave::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyWave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CanMake)
	{
		if (Check())
			SpawnYin();
	}
}
	/*FString TestHUDString = "hellohello";
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TestHUDString);*/
void AMyWave::SpawnYin()
{
	
	int num = 0;
	for (auto& Pair : MyDictionary)
	{
		if (CurrentWave != num)
		{
			num++;
			continue;
		}
		int32 Value = Pair.Value;
		CurrentName = Pair.Key;
		float DelayTime = 3.0f;
		CanMake = false;
		FTimerHandle DelayTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, [this, Value]()
			{SpawnWave(Value);  }, DelayTime, false);
		break;
	}
	
}

void AMyWave::SetMyLocation()
{
	float MinX = -1000.f;
	float MaxX = 1000.f;
	float MinY = -1000.f;
	float MaxY = 1000.f;
	float MinZ = 0.f;
	float MaxZ = 1000.f;

	// ���岽�����������ȣ�
	float StepSize = 100.f;

	// �洢�ҵ��ĺ���λ��
	TArray<FVector> SuitableLocations;

	// �����ռ�
	for (float X = MinX; X <= MaxX; X += StepSize)
	{
		for (float Y = MinY; Y <= MaxY; Y += StepSize)
		{
			for (float Z = MinZ; Z <= MaxZ; Z += StepSize)
			{
				// ��鵱ǰλ���Ƿ����
				FVector CurrentLocation = FVector(X, Y, Z);
		/*		if (IsLocationSuitable(CurrentLocation))
				{
					SuitableLocations.Add(CurrentLocation);
				}*/
			}
		}
	}
}

void AMyWave::SpawnWave(int32 num)
{
	AudioComponent->Play();
	FString MyString = FString::Printf(TEXT("%d"), num);
	FString TestHUDString = CurrentName+" "+ MyString;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TestHUDString);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	int index = FMath::RandRange(0, 2);	
	while (num--)
	{	
		if (GetWorld()->SpawnActor<AMy_Enemy>(AMy_Enemy::StaticClass(), TransformArray[index], SpawnParams))
		{
			index = FMath::RandRange(0, 2);
		}
		else
		{
			FString notify = "sheng cheng yi ge";
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, notify);
		}
	}	
	CurrentWave++;	
	CanMake = true;
}

bool AMyWave::Check()
{
	int NumInstances = 0;
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// ��� Actor �Ƿ���Ŀ�����ʵ��
		if (ActorItr->IsA(AMy_Enemy::StaticClass()))
		{
			// ����ʵ������
			++NumInstances;
		}
	}
	if (NumInstances)
		return false;
	else
		return true;
}



