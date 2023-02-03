#include "dispatcher.h"

namespace bigsaddle {

bool Dispatcher::Dispatch(const SDL_Event& event) {
    if(event.type >= SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EVENT_WINDOW_LAST)
        return DispatchWindowEvent(event);
    switch (event.type) {
    case SDL_EVENT_QUIT:
        return false;
    /*case SDL_WINDOWEVENT:
        return DispatchWindowEvent(event);*/
    }
    return true;
}

} //namespace bigsaddle