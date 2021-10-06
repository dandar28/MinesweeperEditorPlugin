#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(MinesweeperGameLog, Log, All);

class MINESWEEPERGAME_API FMinesweeperGameModule 
	: public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};