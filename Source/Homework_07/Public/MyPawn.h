#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class HOMEWORK_07_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	AMyPawn();

	UCapsuleComponent* CapsuleComponent
		= CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	USkeletalMeshComponent* SkeletalMeshComponent;
	USpringArmComponent* SpringArmComponent;
	UCameraComponent* CameraComponent;

protected:
	virtual void BeginPlay() override;

private:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
