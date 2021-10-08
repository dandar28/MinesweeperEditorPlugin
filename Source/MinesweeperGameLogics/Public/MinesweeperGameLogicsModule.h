#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(MinesweeperGameLogicsLog, Log, All);

class MINESWEEPERGAMELOGICS_API FMinesweeperGameLogicsModule
	: public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};