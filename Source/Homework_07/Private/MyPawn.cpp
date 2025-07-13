#include "MyPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyPlayerController.h"
#include "Components/CapsuleComponent.h"

AMyPawn::AMyPawn()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->AddLocalOffset(FVector(0.0f, 0.0f, 90.0f));
	CapsuleComponent->InitCapsuleSize(35.0f, 90.0f); // 캡슐의 높이와 반경 값을 조절함 (실제 크기 변경이 아님)
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->AddLocalOffset(FVector(0.0f, 0.0f, 80.0f));
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SocketOffset = FVector(0.0f, 40.0f, 0.0f);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CameraComponent->SetFieldOfView(45.0f);
	CameraComponent->bUsePawnControlRotation = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT(
		"/Game/Resources/Characters/Meshes/SKM_Manny.SKM_Manny"));
	if (MeshAsset.Succeeded())
	{
		SkeletalMeshComponent->SetSkeletalMesh(MeshAsset.Object);
	}
	
	FQuat SKMeshLocalRotation = FQuat(FRotator(0.0f, -90.0f, 0.0f));
	SkeletalMeshComponent->AddLocalRotation(SKMeshLocalRotation);
	SkeletalMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->AddLocalOffset(FVector(0.0f, 0.0f, -90.0f));

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

		}
	}
}

void AMyPawn::Move(const FInputActionValue& value)
{
}

void AMyPawn::Look(const FInputActionValue& value)
{
}