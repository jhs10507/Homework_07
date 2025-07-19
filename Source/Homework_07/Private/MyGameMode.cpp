#include "MyGameMode.h"
#include "MyPlayerController.h"
#include "MyPawn.h"

AMyGameMode::AMyGameMode()
{
	// 기본 폰 클래스를 정적 클래스로 호출
	DefaultPawnClass = AMyPawn::StaticClass();

	// 기본 컨트롤러를 정적 클래스로 호출
	PlayerControllerClass = AMyPlayerController::StaticClass();
}