#include "Leela.h"



constexpr SDL_Keycode SHIFT = SDLK_LSHIFT | SDLK_RSHIFT;

void Leela::createActions()
{

    actionMap.emplace(
        std::make_tuple(SDLK_l, SHIFT),			Action(
                                                    [this]() { cycleLabelModes(); },
                                                    "Cycle labels"
                                                )
    );

}
