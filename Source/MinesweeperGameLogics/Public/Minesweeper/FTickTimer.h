#pragma once

#include "CoreMinimal.h"

/**
 * \brief - Class representing a timer that can be started/stopped to calculate elapsed time.
 */
class MINESWEEPERGAMELOGICS_API FTickTimer {
public:
	/**
	 * \brief - Set the Time Start at Now so that the timer can take track of the elapsed time from now on.
	 */
	void StartTimer();

	/**
	 * \brief - Set the Time End at Now so that the timer can take track of the elapsed time from the beginning to this moment.
	 */
	void StopTimer();

	/**
	 * \return The Time Start. Call this after calling StartTimer()
	 */
	FDateTime GetTimeStart() const;

	/**
	 * \return The Time End. Call this after calling StopTimer()
	 */
	FDateTime GetTimeEnd() const;

	/**
	 * \return The time elapsed from Start to now (or to End if the timer has been stopped). Call this after calling StartTimer() at least once.
	 */
	FTimespan GetTimeElapsedFromStart() const;

	/**
	 * \return True if the timer has been started but not stopped yet.
	 */
	bool IsTimerRunning() const;

	/**
	 * \return True if StartTimer() has been called.
	 */
	bool HasTimerBeenStarted() const;

	/**
	 * \return True if StopTimer() has been called.
	 */
	bool HasTimerBeenStopped() const;

private:
	/**
	 * \brief - Time on which the game started, obtained when StartTimer() is called.
	 */
	TOptional<FDateTime> _timeStart;

	/**
	 * \brief - Time on which the game ended, obtained when StopTimer() is called.
	 */
	TOptional<FDateTime> _timeEnd;
};