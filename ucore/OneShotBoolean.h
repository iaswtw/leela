#pragma once

#include <SDL.h>

class OneShotBoolean
{
public:
    OneShotBoolean()
    {
    }

    void set(int timeout_ms)
    {
        endTime = SDL_GetTicks() + timeout_ms;
        _bValue = true;
    }

    /* This must be called in the application main loop. */
    void tick()
    {
        if (_bValue)
        {
            if (endTime <= SDL_GetTicks())
            {
                _bValue = false;
            }
        }
        else
        {
            // ignore
        }
    }

    bool get()
    {
        return _bValue;
    }

private:
    unsigned int endTime = 0;
    bool _bValue = false;
};