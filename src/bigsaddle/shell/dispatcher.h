#include "SDL.h"

namespace bigsaddle {

class Dispatcher {
public:
    virtual bool Dispatch(const SDL_Event& event);
    virtual bool DispatchWindowEvent(const SDL_Event& event) { return true; }
};

} //namespace bigsaddle
