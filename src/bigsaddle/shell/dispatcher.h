#include "SDL.h"

class Dispatcher {
public:
    virtual bool Dispatch(const SDL_Event& event);
    virtual bool DispatchWindowEvent(const SDL_Event& event) { return true; }
};