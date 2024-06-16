// Fill out your copyright notice in the Description page of Project Settings.


#include "My_Enemy.h"
#include "My_Player.h"
// Sets default values
AMy_Enemy::AMy_Enemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MySound = LoadObject<USoundBase>(nullptr, TEXT("/ Script / Engine.SoundWave'/Game/Audio/Damage_Grunt.Damage_Grunt'"));
	// 创建音频组件并设置声音资源
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->SetSound(MySound);

	GetMesh()->SetupAttachment(GetCapsuleComponent());
	SetCharacterMeshPosition(this);

	CharacterMovement = GetCharacterMovement();
	CharacterMovement->MaxWalkSpeed = 250.0f;

	LoadAssets();
	
	//AMy_Player::SetCharacterMeshPosition();
	Arrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow1"));
	Arrow1->SetupAttachment(GetCapsuleComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
// Called when the game starts or when spawned
void AMy_Enemy::BeginPlay()
{
	Super::BeginPlay();
	AMy_Enemy::InitializeAIController();
	FScriptDelegate PushDelegate;
	FScriptDelegate PushEndDelegate;
	PushDelegate.BindUFunction(this, FName("OnMontageBegin"));
	PushEndDelegate.BindUFunction(this, FName("OnMontageEnd"));
	GetMesh()->GetAnimInstance()->OnMontageEnded.Add(PushEndDelegate);
	////GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &AMy_Enemy::OnMontageBegin);
	GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.Add(PushDelegate);
	//OnMoveCompletedDelegate.AddDynamic(this, &AMy_Enemy::OnMoveCompleted);
}
// Called every frame
void AMy_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AMy_Enemy::ChaseToPlayer();	
	
}
// AEnemyPawn.cpp  
void AMy_Enemy::InitializeAIController()
{
	// 获取当前世界  
	UWorld* world = GetWorld();
	if (world)
	{
		// 在世界中生成 AI 控制器实例  
		// 假设你的 AI 控制器类名为 AMyAIController，并且继承自 AAIController  
		AMy_AIController* newController = world->SpawnActor<AMy_AIController>(AMy_AIController::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		if (newController)
		{
			// 尝试让 AI 控制器占有（Possess）这个敌人  
			// Possess 方法通常在 AAIController 类或其子类中定义  
			newController->Possess(this);
			
				// 占有成功，可以在这里添加其他逻辑  
			UE_LOG(LogTemp, Log, TEXT("AI controller possessed the enemy successfully."));
			
		}
		else
		{
			// 生成 AI 控制器失败  
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn AI controller."));
		}
	}
	else
	{
		// 没有世界，处理错误情况  
		UE_LOG(LogTemp, Error, TEXT("No world found to spawn AI controller."));
	}
}

void AMy_Enemy::ChaseToPlayer()
{	
	if (!chase) return;
	AAIController* AIController = Cast<AAIController>(this->GetController());
	
	if (AIController)
	{
		
		APlayerController * PlayerPawn = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerPawn)
		{
			AIController->MoveToActor(PlayerPawn, 5.0f,true,true,false,0,true);
			if (IsPlayerReached())
			{
				Attack();
			}
		}
	}
}

void AMy_Enemy::OnMontageBegin(FName MontageName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
		
		FVector TraceStart = Arrow1->GetComponentLocation();
		FVector TraceEnd = TraceStart + Arrow1->GetForwardVector() * 100.0f;
		float TraceRadius = 25.0f;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		FHitResult OutHit;

		bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(), 
			TraceStart, 
			TraceEnd, 
			TraceRadius, 
			ObjectTypes, 
			false, 
			IgnoreActors,
			EDrawDebugTrace::None,
			OutHit, 
			true 
		);

		if (bHit)
		{
			//AudioComponent->Play();
			if (AMy_Player* HitPlayerCharacter = Cast<AMy_Player>(OutHit.GetActor()))
			{
				HitPlayerCharacter->take_damage(BaseDamage, this->GetActorForwardVector());
			}
		}
		else
		{
			
		}

}

void AMy_Enemy::OnMontageEnd(UAnimMontage* MontageName, bool bInterrupted)
{
	Canattack = true;

}

void AMy_Enemy::Attack()
{
	if (GetMesh()->GetAnimInstance() && AttackMontage)
	{
		if (Canattack)
		{
			Canattack = false;
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage, 1.f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("meng tai qi chu cuo"));
	}
}

void AMy_Enemy::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{	
		Attack();
	}
	else
	{
		ChaseToPlayer();
	}
}

void AMy_Enemy::take_damage(float value,FVector ImpulseDirection)
{
	AudioComponent->Play();
	Health -= value;
	if (Health > 0)
	{
		float DistanceToMove = 300.0f;
		FVector DesiredLocation = GetActorLocation() + ImpulseDirection * DistanceToMove;
		LaunchCharacter(ImpulseDirection * 200 + this->GetActorUpVector() * 100, false, false);
	}
	else
	{
		
		Canattack = false;
		chase = false;
		GetCapsuleComponent()->SetSimulatePhysics(true);
		float DelayTime = 3.0f; // 延迟时间
		FTimerHandle DelayTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, [this]()
			{
				// 在延迟时间到期后执行销毁操作
				Destroy();
			}, DelayTime, false);
	}
}
// Called to bind functionality to input
void AMy_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMy_Enemy::SetCharacterMeshPosition(ACharacter* Character)
{
	if (Character)
	{
		USkeletalMeshComponent* MeshComponent = GetMesh();
		if (MeshComponent)
		{
			FVector NewLocation = FVector(0.0f, 0.0f, -89.0f);
			MeshComponent->SetRelativeLocation(NewLocation);
			FQuat NewRotation = FQuat(FRotator(0.0f, -90.0f, 0.0f)); // 旋转角度自定义
			MeshComponent->SetRelativeRotation(NewRotation);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Mesh Component is nullptr!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("character is nullptr!"));
	}
}

void AMy_Enemy::LoadAssets()
{	
	TArray<ConstructorHelpers::FObjectFinder<USkeletalMesh>>MeshAssets
	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_00.MESH_PC_00'")),
		ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_01.MESH_PC_01'")),
		ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_02.MESH_PC_02'")),
		ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_03.MESH_PC_03'"))
	};
	index = FMath::RandRange(0, SkeletalMeshNum - 1);
	if (MeshAssets[index].Succeeded()){
		GetMesh()->SetSkeletalMesh(MeshAssets[index].Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh Not Found!"));
	}
	//static ConstructorHelpers::FObjectFinder<UAnimBlueprint>AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Characters/Mannequin_UE4/Our/ABP_Player.ABP_Player'"));
	//if (AnimAsset.Succeeded()) {
	//	GetMesh()->SetAnimInstanceClass(AnimAsset.Object->GeneratedClass);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Not Found!"));
	//}

	static ConstructorHelpers::FClassFinder<UAnimInstance>AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Characters/Mannequin_UE4/Our/ABP_Player.ABP_Player_C'"));
	if (AnimAsset.Succeeded()) {
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Not Found!"));
	}


	static ConstructorHelpers::FObjectFinder<UAnimMontage>PushMontageAsset(TEXT("/Script/Engine.AnimMontage'/Game/Characters/Mannequin_UE4/Our/Push_Montage.Push_Montage'"));
	if (PushMontageAsset.Succeeded())
	{
		AttackMontage = PushMontageAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PushMontageAsset Not Found!"));
	}
}

bool AMy_Enemy::IsPlayerReached()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		float Distance = FVector::Distance(PlayerPawn->GetActorLocation(), this->GetActorLocation());
		if (Distance <= 150.0f)
		{
			return true; // 追到了玩家角色
		}
	}
	return false; // 没有追到玩家角色
}