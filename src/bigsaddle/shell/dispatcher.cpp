#include "dispatcher.h"

namespace bigsaddle {

bool Dispatcher::Dispatch(const SDL_Event& event) {
    switch (event.type) {
    case SDL_QUIT:
        return false;
    case SDL_WINDOWEVENT:
        return DispatchWindowEvent(event);
    }
    return true;
}

} //namespace bigsaddle