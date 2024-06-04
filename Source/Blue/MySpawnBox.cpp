// Fill out your copyright notice in the Description page of Project Settings.


#include "MySpawnBox.h"
#include "EngineUtils.h" // ���� EngineUtils.h ͷ�ļ�
#include "TimerManager.h"

// Sets default values
AMySpawnBox::AMySpawnBox()
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
	FVector Location1 = FVector(-360, -2300, 478);
	FVector Location2 = FVector(-360, 2460, 88);
	FVector Location3 = FVector(1450, 540, 248);

	// �����ĸ� FTransform ����
	TransformArray[0] = FTransform(Location1);
	TransformArray[1] = FTransform(Location2);
	TransformArray[2] = FTransform(Location3);

	MyDictionary.Add(TEXT("Wave1"), 1);
	MyDictionary.Add(TEXT("Wave2"), 2);
	MyDictionary.Add(TEXT("Wave3"), 3);
}

// Called when the game starts or when spawned
void AMySpawnBox::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMySpawnBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CanMake)
	{
		if (Check())
		{
			CanMake = false;
			SpawnYin();
		}
			
	}
}
void AMySpawnBox::SpawnYin()
{
	//int num = 0;
	//for (auto& Pair : MyDictionary)
	//{
	//	if (CurrentWave != num)
	//	{
	//		++num;
	//		continue;
	//	}
	//	int32 Value = Pair.Value;
	//	CurrentName = Pair.Key;
	//	float DelayTime = 3.0f;
	//	CanMake = false;
	//	FTimerHandle DelayTimerHandle;
	//	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, [this, Value]()
	//		{SpawnWave(Value);}, DelayTime, false);
	//	break;
	//}
	int32 IndexToFind = CurrentWave; // ����Ҫ��������Ϊ 1 ��ֵ
	// �����ֵ�����������������Ӧ��ֵ
	int32 FoundValue = -1; // Ĭ��Ϊ -1 ��ʾδ�ҵ�
	for (auto& Pair : MyDictionary) {
		// ����ҵ���Ӧ����������
		if (IndexToFind == 0) {
			FoundValue = Pair.Value;
			CurrentName = Pair.Key;
			break;
		}
		IndexToFind--;
	}

	// ������
	if (FoundValue != -1) {
		SpawnWave(FoundValue);
	}
	else {
		
	}

}

void AMySpawnBox::SpawnWave(int32 num)
{
	AudioComponent->Play();
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
	if(CurrentWave < MaxWave)
		CurrentWave++;
	else
	{
		return;
	}
	CanMake = true;
}

bool AMySpawnBox::Check()
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



