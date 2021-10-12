#include "MinesweeperGameUIModule.h"

#include "MinesweeperGameUIStyle.h"

IMPLEMENT_MODULE(FMinesweeperGameUIModule, MinesweeperGameUI);

DEFINE_LOG_CATEGORY(MinesweeperGameUILog);

void FMinesweeperGameUIModule::StartupModule() {
	FMinesweeperGameUIStyle::Initialize();
	FMinesweeperGameUIStyle::ReloadTextures();
}
void FMinesweeperGameUIModule::ShutdownModule() {
	FMinesweeperGameUIStyle::Shutdown();
}