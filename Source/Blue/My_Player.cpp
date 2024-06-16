// Fill out your copyright notice in the Description page of Project Settings.


#include "My_Player.h"
#include "My_Enemy.h"
// Sets default values
AMy_Player::AMy_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    LoadAssets(MySkeletalMesh);
	MySound = LoadObject<USoundBase>(nullptr, TEXT("/ Script / Engine.SoundWave'/Game/Audio/Damage_Grunt.Damage_Grunt'"));
	// 创建音频组件并设置声音资源
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->SetSound(MySound);
	SpawnUserWidget();
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	SetCharacterMeshPosition();
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);
	Arrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow1"));
	Arrow1->SetupAttachment(GetCapsuleComponent());

	SpringArm->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
void AMy_Player::SetCharacterMeshPosition()

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
// Called when the game starts or when spawned

void AMy_Player::OnMontageBegin(FName MontageName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
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
		if (AMy_Enemy* HitPlayerCharacter = Cast<AMy_Enemy>(OutHit.GetActor()))
		{
			HitPlayerCharacter->take_damage(BashDamage, this->GetActorForwardVector());
		}
	}
	else
	{

	}
}

void AMy_Player::OnMontageEnd(UAnimMontage* MontageName, bool bInterrupted)
{
	//FString TestHUDString = "Attack";
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TestHUDString);
	Canattack = true;
}

void AMy_Player::BeginPlay()
{
	Super::BeginPlay();
	
	/*FSoftObjectPath AnimAssetPath(TEXT("/Game/Characters/Mannequin_UE4/Our/ABP_Player.ABP_Player"));
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(AnimAssetPath.TryLoad());

	if (AnimBlueprint)
	{
		GetMesh()->SetAnimInstanceClass(AnimBlueprint->GeneratedClass);
		UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Found!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Not Found!"));
	}*/


	FScriptDelegate PushDelegate;
	FScriptDelegate PushEndDelegate;
	PushDelegate.BindUFunction(this, FName("OnMontageBegin"));
	PushEndDelegate.BindUFunction(this, FName("OnMontageEnd"));
	GetMesh()->GetAnimInstance()->OnMontageEnded.Add(PushEndDelegate);
	//GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &AMy_Enemy::OnMontageBegin);
	GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.Add(PushDelegate);
	//OnMoveCompletedDelegate.AddDynamic(this, &AMy_Enemy::OnMoveCompleted);

}
// Called every frame
void AMy_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Called to bind functionality to input
void AMy_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Rotate"), this, &AMy_Player::Rotate);
	PlayerInputComponent->BindAxis(TEXT("Rotatey"), this, &AMy_Player::Rotatey);
	PlayerInputComponent->BindAxis(TEXT("MoveFB"), this, &AMy_Player::MoveFB);
	PlayerInputComponent->BindAxis(TEXT("MoveLR"), this, &AMy_Player::MoveLR);
	PlayerInputComponent->BindAction(TEXT("attack"), IE_Pressed, this, &AMy_Player::AttackInput);
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		FScriptDelegate PushEndDelegate;
		PushEndDelegate.BindUFunction(this, FName("OnMontageEnd"));
		GetMesh()->GetAnimInstance()->OnMontageEnded.Add(PushEndDelegate);
	}
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		FScriptDelegate PushDelegate;
		PushDelegate.BindUFunction(this, FName("OnMontageBegin"));
		GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.Add(PushDelegate);
	}
}

void AMy_Player::LoadAssets(FString SkeletalMesh)
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(*SkeletalMesh);
	if (MeshAsset.Succeeded()) {
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh Not Found!"));
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance>AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Characters/Mannequin_UE4/Our/ABP_Player.ABP_Player_C'"));
	if (AnimAsset.Succeeded()) {
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Not Found!"));
	}

	/*FSoftObjectPath AnimAssetPath(TEXT("/Game/Characters/Mannequin_UE4/Our/ABP_Player.ABP_Player"));
	UObject* LoadedObject = StaticLoadObject(UAnimBlueprint::StaticClass(), nullptr, *AnimAssetPath.ToString());
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(LoadedObject);
	if (AnimBlueprint)
	{
		GetMesh()->SetAnimInstanceClass(AnimBlueprint->GeneratedClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Not Found!"));
	}*/

	//FSoftObjectPath AnimAssetPath(TEXT("/Game/UI/ABP_Player2.ABP_Player2'"));
	//FString PackageName = FPackageName::ObjectPathToPackageName(AnimAssetPath.ToString());
	//UAnimBlueprint* AnimBlueprint = LoadObject<UAnimBlueprint>(nullptr, *PackageName);
	//if (AnimBlueprint)
	//{
	//	GetMesh()->SetAnimInstanceClass(AnimBlueprint->GeneratedClass);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Not Found!"));
	//}

	
	//UClass* AnimBlueprintClass = LoadClass<UAnimBlueprint>(nullptr, TEXT("/Game/Characters/Mannequin_UE4/Our/ABP_Player.ABP_Player"), nullptr, LOAD_None, nullptr);
	//if (AnimBlueprintClass)
	//{
	//	GetMesh()->SetAnimInstanceClass(AnimBlueprintClass);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Not Found!"));
	//}



	//FSoftObjectPath AnimAssetPath(TEXT("/Game/UI/ABP_Player2.ABP_Player2"));
	//FString PackageName = FPackageName::ObjectPathToPackageName(AnimAssetPath.ToString());

	//// 异步加载动画蓝图
	//TSoftObjectPtr<UAnimBlueprint> AnimBlueprintPtr(AnimAssetPath);
	//AnimBlueprintPtr.LoadSynchronous();

	////// 检查是否成功加载动画蓝图
	//if (UAnimBlueprint* AnimBlueprint = AnimBlueprintPtr.Get())
	//{
	////	// 设置动画实例类
	//	GetMesh()->SetAnimInstanceClass(AnimBlueprint->GeneratedClass);
	//	UE_LOG(LogTemp, Warning, TEXT("动画蓝图成功加载：%s"), *AnimBlueprint->GetName());
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%s动画蓝图未找到："), *AnimAssetPath.ToString());
	//}

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

void AMy_Player::MoveFB(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which direction is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value * MoveSpeed);
	}
	/*AddMovementInput(GetActorForwardVector(), Value * MoveSpeed);*/
}

void AMy_Player::MoveLR(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which direction is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement in that direction
		AddMovementInput(Direction, Value * MoveSpeed);
	}
	//AddMovementInput(-GetActorRightVector(), Value * MoveSpeed);
}

void AMy_Player::Rotate(float Value)
{
	AddControllerYawInput(Value * RotationSpeed);
}

void AMy_Player::Rotatey(float Value)
{
	AddControllerPitchInput(Value * RotationSpeed);
}

void AMy_Player::AttackInput()
{
	if (GetMesh()->GetAnimInstance() && AttackMontage)
	{
		
		//GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &AMy_Player::OnMontageBegin);
		if (Canattack)
		{
			/*FString TestHUDString = "Attack";
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TestHUDString);*/
			Canattack = false;
			GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage, 1.f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("meng tai qi chu cuo"));
	}
}

void AMy_Player::take_damage(float value,FVector ImpulseDirection)
{
	AudioComponent->Play();
	UFunction* function = WavesWidget->FindFunction(FName("Damage Screen"));
	if (function)
	{	
		WavesWidget->ProcessEvent(function, nullptr); // 对于静态函数或没有参数的实例函数    
	}
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
		PrimaryActorTick.bCanEverTick = false;
		// 在需要延迟销毁对象的地方调用下面的代码
		GetCapsuleComponent()->SetSimulatePhysics(true);
		float DelayTime = 3.0f;
		GetMesh()->GetAnimInstance()->Montage_Stop(3.0f);
		FTimerHandle DelayTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, [this]()
			{
				// 在延迟时间到期后执行销毁操作
				Destroy();
			}, DelayTime, false);
	}
}

void AMy_Player::SpawnUserWidget()
{
	HealthBarWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WB_HealthBar.WB_HealthBar_C'"));
	if (HealthBarWidgetClass)
	{	
		HealthBarWidget = CreateWidget<UUserWidget>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
		
			HealthBarWidget->AddToViewport(-1000);
		}
	}
	WavesWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WB_HUD.WB_HUD_C'"));
	if (WavesWidgetClass)
	{
		WavesWidget = CreateWidget<UUserWidget>(GetWorld(), WavesWidgetClass);
		if (WavesWidget)
		{
			WavesWidget->AddToViewport(-1000);
		}
	}
}
