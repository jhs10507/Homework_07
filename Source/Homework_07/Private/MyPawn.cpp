#include "MyPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMyPawn::AMyPawn()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->InitCapsuleSize(35.f, 90.f); // 캡슐의 높이와 반경 값을 조절함 (실제 크기 변경이 아님)
	CapsuleComponent->SetSimulatePhysics(false);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT(
		"/Game/Resources/Characters/Meshes/SKM_Manny.SKM_Manny"));
	if (MeshAsset.Succeeded())
	{
		SkeletalMeshComponent->SetSkeletalMesh(MeshAsset.Object);
	}
	
	FQuat SKMeshLocalRotation = FQuat(FRotator(0.0f, -90.0f, 0.0f));
	SkeletalMeshComponent->AddLocalRotation(SKMeshLocalRotation);
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);
	SkeletalMeshComponent->AddLocalOffset(FVector(0.0f, 0.0f, -90.0f));
	SkeletalMeshComponent->SetSimulatePhysics(false);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(CapsuleComponent);
	SpringArmComponent->AddLocalOffset(FVector(0.0f, 0.0f, 80.0f));
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->SetFieldOfView(45.0f);

	PrimaryActorTick.bCanEverTick = false;
}

void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Move
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Look
				);
			}
		}
	}
}

void AMyPawn::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	float MoveSpeed = 20.f;

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		FVector SideMove = FVector(10.f, 0.f, 0.f);
		float DeltaSeconds = GetWorld()->GetDeltaSeconds();
		FVector SideMovement = (SideMove * MoveInput.X) * MoveSpeed * DeltaSeconds;
		AddActorLocalOffset(SideMovement);

		/*if (!FMath::IsNegative(MoveInput.X))
		{
			FVector LeftMove = FVector(10.f, 0.f, 0.f);
			float DeltaSeconds = GetWorld()->GetDeltaSeconds();
			FVector LeftMovement = (LeftMove * MoveInput.X) * MoveSpeed * DeltaSeconds;
			AddActorLocalOffset(LeftMovement);

		}
		else
		{
			FVector RightMove = FVector(10.f, 0.f, 0.f);
			float DeltaSeconds = GetWorld()->GetDeltaSeconds();
			FVector RightMovement = (RightMove * MoveInput.X) * MoveSpeed * DeltaSeconds;
			AddActorLocalOffset(RightMovement);

		}*/
		//AddMovementInput(GetActorForwardVector(), MoveInput.X); 사용하지 않았을 때의 코드
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		if (!FMath::IsNegative(MoveInput.Y))
		{
			FVector FrontMove = FVector(0.f, 10.f, 0.f);
			float DeltaSeconds = GetWorld()->GetDeltaSeconds();
			FVector FrontMovement = (FrontMove * MoveInput.Y) * MoveSpeed * DeltaSeconds;
			AddActorLocalOffset(FrontMovement);
			
		}
		else
		{
			FVector BackMove = FVector(0.f, 10.f, 0.f);
			float DeltaSeconds = GetWorld()->GetDeltaSeconds();
			FVector BackMovement = (BackMove * MoveInput.Y) * MoveSpeed * DeltaSeconds;
			AddActorLocalOffset(BackMovement);
		}
		//AddMovementInput(GetActorForwardVector(), MoveInput.Y); 사용하지 않았을 때의 코드
	}
}

void AMyPawn::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	
	// SpringArmComponent가 null이 아닐 경우에만 실행
	if (SpringArmComponent)
	{
		// IsNearlyZero : 거의 0인지에 대해 판별 (부동소수점 오차로 인해 완벽한 0이 아닐 수 있음)
		if (!FMath::IsNearlyZero(LookInput.X))
		{
			//AddControllerYawInput(LookInput.X); 를 사용하지 않았을 때
			// 마우스의 좌우 입력이 들어왔을 때
			float YawRotation = LookInput.X * GetWorld()->GetDeltaSeconds() * 100.f;
			AddActorLocalRotation(FRotator(0.0f, YawRotation, 0.0f)); // Pitch, Yaw, Roll
		}
		if (!FMath::IsNearlyZero(LookInput.Y))
		{
			//AddControllerPitchInput(LookInput.Y); 를 사용하지 않았을 때
			FRotator CurrentRotation = SpringArmComponent->GetRelativeRotation();
			float PithRotation = LookInput.Y * GetWorld()->GetDeltaSeconds() * 100.f;
			PithRotation = CurrentRotation.Pitch - PithRotation;
			PithRotation = FMath::Clamp(PithRotation, -60.f, 80.f);
			CurrentRotation.Pitch = PithRotation;
			SpringArmComponent->SetRelativeRotation(CurrentRotation);
			//UE_LOG(LogTemp, Warning, TEXT("회전"));
		}
	}
}