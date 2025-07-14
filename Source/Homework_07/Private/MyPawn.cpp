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
	//CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	//RootComponent = CapsuleComponent;
	//CapsuleComponent->AddLocalOffset(FVector(0.0f, 0.0f, 90.0f));
	//CapsuleComponent->InitCapsuleSize(35.0f, 90.0f); // 캡슐의 높이와 반경 값을 조절함 (실제 크기 변경이 아님)
	//CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	//
	//SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArmComponent->SetupAttachment(RootComponent);
	//SpringArmComponent->AddLocalOffset(FVector(0.0f, 0.0f, 80.0f));
	//SpringArmComponent->TargetArmLength = 300.0f;
	//SpringArmComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	//SpringArmComponent->bUsePawnControlRotation = true;
	//SpringArmComponent->SocketOffset = FVector(0.0f, 40.0f, 0.0f);
	//
	//CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	//CameraComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	//CameraComponent->SetFieldOfView(45.0f);
	//CameraComponent->bUsePawnControlRotation = false;

	//SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT(
	//	"/Game/Resources/Characters/Meshes/SKM_Manny.SKM_Manny"));
	//if (MeshAsset.Succeeded())
	//{
	//	SkeletalMeshComponent->SetSkeletalMesh(MeshAsset.Object);
	//}
	//
	//FQuat SKMeshLocalRotation = FQuat(FRotator(0.0f, -90.0f, 0.0f));
	//SkeletalMeshComponent->AddLocalRotation(SKMeshLocalRotation);
	//SkeletalMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	//SkeletalMeshComponent->SetupAttachment(RootComponent);
	//SkeletalMeshComponent->AddLocalOffset(FVector(0.0f, 0.0f, -90.0f));

	// 1) Capsule Component: 루트 컴포넌트로 사용 (충돌, 이동 기준)
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->InitCapsuleSize(34.f, 88.f);
	CapsuleComponent->SetSimulatePhysics(false);

	// 2) Skeletal Mesh: Pawn의 외형 표현
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);
	SkeletalMeshComponent->SetSimulatePhysics(false);

	// 3) SpringArm: 카메라의 거리를 제어하며, 회전도 가능하도록 설정
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->SetupAttachment(CapsuleComponent);
	SpringArmComponent->TargetArmLength = 300.f;
	// Pawn의 회전과 별개로 카메라가 독립적으로 회전하도록 함
	SpringArmComponent->bUsePawnControlRotation = false;

	// 4) Camera: 실제 화면에 보일 카메라
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	PrimaryActorTick.bCanEverTick = true;
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

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		if (MoveInput.X < 0)
		{
			AddActorLocalOffset(FVector(1.0f, 0.0f, 0.0f));
		}
		else if (MoveInput.X > 0)
		{
			AddActorLocalOffset(FVector(-1.0f, 0.0f, 0.0f));
		}
		//AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddActorLocalOffset(FVector(0.0f, 1.0f, 0.0f));
		//AddMovementInput(GetActorForwardVector(), MoveInput.Y);
	}
}

void AMyPawn::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	//UCameraComponent* camera = CameraComponent;
	
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
			UE_LOG(LogTemp, Warning, TEXT("회전"));
		}
	}
}