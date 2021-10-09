#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(MinesweeperGameUILog, Log, All);

class MINESWEEPERGAMEUI_API FMinesweeperGameUIModule
	: public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};