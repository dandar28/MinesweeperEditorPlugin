#pragma once

/**
 * \brief - Interface for hosting a generic game session.
 */
class MINESWEEPERGAMELOGICS_API IGameSession {
public:
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual bool IsRunning() const = 0;
};