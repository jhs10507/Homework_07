#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class HOMEWORK_07_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	AMyPawn();

	UCapsuleComponent* CapsuleComponent;
	USpringArmComponent* SpringArmComponent;
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	USkeletalMeshComponent* SkeletalMeshComponent;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);

private:
	float MoveSpeed;
	float ForwardInput;
	FVector Direction;
};
