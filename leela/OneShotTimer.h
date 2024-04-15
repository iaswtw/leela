#pragma once

#include <SDL.h>

class OneShotTimer
{
public:
    OneShotTimer()
    {
    }

    void set(int timeout_ms)
    {
        endTime = SDL_GetTicks() + timeout_ms;
        _bActive = true;
    }

    /* This must be called in the application main loop. */
    void tick()
    {
        if (_bActive)
        {
            if (endTime <= SDL_GetTicks())
            {
                _bActive = false;
            }
        }
        else
        {
            // ignore
        }
    }

    bool get()
    {
        return _bActive;
    }

private:
    unsigned int endTime = 0;
    bool _bActive = false;          // true if oneshot timer is active
};