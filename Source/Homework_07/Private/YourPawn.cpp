// Fill out your copyright notice in the Description page of Project Settings.


#include "YourPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// Sets default values
AYourPawn::AYourPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	CapsuleComponent->SetSimulatePhysics(false);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->SetSimulatePhysics(false);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AYourPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>
			(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AYourPawn::Move(const FInputActionValue& Value)
{
	CurrentMoveInput = Value.Get<FVector2D>();
}

void AYourPawn::StopMove(const FInputActionValue& Value)
{
	CurrentMoveInput = FVector2D::ZeroVector;
}

void AYourPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// SpringArmComponent가 null이 아닐 경우에만 실행
	if (SpringArm)
	{
		// IsNearlyZero : 거의 0인지에 대해 판별 (부동소수점 오차로 인해 완벽한 0이 아닐 수 있음)
		if (!FMath::IsNearlyZero(LookAxisVector.X))
		{
			//AddControllerYawInput(LookInput.X); 를 사용하지 않았을 때
			// 마우스의 좌우 입력이 들어왔을 때
			float YawRotation = LookAxisVector.X * GetWorld()->GetDeltaSeconds() * 100.f;
			AddActorLocalRotation(FRotator(0.0f, YawRotation, 0.0f)); // Pitch, Yaw, Roll
		}
		if (!FMath::IsNearlyZero(LookAxisVector.Y))
		{
			//AddControllerPitchInput(LookInput.Y); 를 사용하지 않았을 때
			FRotator CurrentRotation = SpringArm->GetRelativeRotation();
			float PithRotation = LookAxisVector.Y * GetWorld()->GetDeltaSeconds() * 100.f;
			PithRotation = CurrentRotation.Pitch - PithRotation;
			PithRotation = FMath::Clamp(PithRotation, -60.f, 80.f);
			CurrentRotation.Pitch = PithRotation;
			SpringArm->SetRelativeRotation(CurrentRotation);
			//UE_LOG(LogTemp, Warning, TEXT("회전"));
		}
		//if (!FMath::IsNearlyZero(LookAxisVector.Z))
		//{
		//	//AddControllerYawInput(LookInput.X); 를 사용하지 않았을 때
		//	// 마우스의 좌우 입력이 들어왔을 때
		//	float RollRotation = LookInput.Z * GetWorld()->GetDeltaSeconds() * 100.f;
		//	AddActorLocalRotation(FRotator(0.0f, 0.0f, RollRotation)); // Pitch, Yaw, Roll
		//}
	}

	/*AddActorLocalRotation(FRotator(
		0.0f, LookAxisVector.X * LookSpeed * GetWorld()->GetDeltaSeconds(), 0.0f));

	FRotator CurrentSpringArmRotation = SpringArm->GetRelativeRotation();

	float NewPitch = 
		CurrentSpringArmRotation.Pitch - LookAxisVector.Y * LookSpeed * GetWorld()->GetDeltaSeconds();

	NewPitch = FMath::Clamp(NewPitch, -75.f, 0.f);

	SpringArm->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));*/
}

void AYourPawn::Jump(const FInputActionValue& Value)
{
	if (bIsOnGround)
	{
		Velocity.Z = JumpForce;
		bIsOnGround = false;
	}
}


// Called every frame
void AYourPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;
	FVector StartTrace = GetActorLocation();

	FVector EndTrace = StartTrace + FVector(
		0.f, 0.f, -(CapsuleComponent->GetScaledCapsuleHalfHeight() + 10.f));
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bIsOnGround = GetWorld()->LineTraceSingleByChannel(
		HitResult, StartTrace, EndTrace, ECC_Visibility, CollisionParams);

	DrawDebugLine(
		GetWorld(),
		StartTrace,
		EndTrace,
		bIsOnGround ? FColor::Green : FColor::Red,
		false,
		0.0f,
		0,
		1.0f
	);

	if (!bIsOnGround)
	{
		Velocity.Z += Gravity * DeltaTime;
	}

	float CurrentMoveSpeed = bIsOnGround ? MoveSpeed : MoveSpeed * AirControl;
	FVector DesireMovement = FVector(
		CurrentMoveInput.X, CurrentMoveInput.Y, 0.0f).GetSafeNormal() * CurrentMoveSpeed;

	if (bIsOnGround)
	{
		Velocity.X = DesireMovement.X;
		Velocity.Y = DesireMovement.Y;
		if (Velocity.Z < 0.f) Velocity.Z = 0.f;
	}
	else
	{
		Velocity.X = FMath::Lerp(Velocity.X, DesireMovement.X, AirControl);
		Velocity.Y = FMath::Lerp(Velocity.Y, DesireMovement.Y, AirControl);
	}

	if (bIsOnGround && Velocity.Z < 0.0f)
	{
		Velocity.Z = 0.0f;
		SetActorLocation(
			HitResult.ImpactPoint + FVector(
			0.0f, 0.0f, 
			CapsuleComponent->GetScaledCapsuleHalfHeight()));
	}

	FVector WorldVelocity = GetActorRotation().RotateVector(Velocity);
	AddActorWorldOffset(WorldVelocity * DeltaTime, true);
}

// Called to bind functionality to input
void AYourPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&AYourPawn::Move
		);
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Completed,
			this,
			&AYourPawn::StopMove
		);
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&AYourPawn::Look
		);
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&AYourPawn::Jump
		);
	}
}