#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

UCLASS()
class HOMEWORK_07_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	AMyPawn();

protected:
	virtual void BeginPlay() override;

private:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
