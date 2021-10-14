#include "..\..\Public\Minesweeper\FTickTimer.h"
#include "..\..\Public\Minesweeper\FTickTimer.h"
#include "..\..\Public\Minesweeper\FTickTimer.h"
#include "Minesweeper/FTickTimer.h"

void FTickTimer::StartTimer() {
	_timeStart = FDateTime::Now();
	_timeEnd.Reset();
}

void FTickTimer::StopTimer() {
	if (HasTimerBeenStopped()) {
		return;
	}
	check(HasTimerBeenStarted());
	_timeEnd = FDateTime::Now();
}

FDateTime FTickTimer::GetTimeStart() const {
	check(HasTimerBeenStarted());
	return _timeStart.GetValue();
}

FDateTime FTickTimer::GetTimeEnd() const {
	check(HasTimerBeenStopped());
	return _timeEnd.GetValue();
}

FTimespan FTickTimer::GetTimeElapsedFromStart() const {
	check(HasTimerBeenStarted());
	return _timeEnd.Get(FDateTime::Now()) - _timeStart.GetValue();
}

bool FTickTimer::IsTimerRunning() const {
	return HasTimerBeenStarted() && !HasTimerBeenStopped();
}

bool FTickTimer::HasTimerBeenStarted() const {
	return _timeStart.IsSet();
}

bool FTickTimer::HasTimerBeenStopped() const {
	return _timeEnd.IsSet();
}
