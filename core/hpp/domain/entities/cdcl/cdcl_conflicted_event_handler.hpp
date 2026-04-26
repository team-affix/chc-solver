#ifndef CDCL_CONFLICTED_EVENT_HANDLER_HPP
#define CDCL_CONFLICTED_EVENT_HANDLER_HPP

#include "../../../infrastructure/event_handler.hpp"
#include "../../events/conflicted_event.hpp"
#include "cdcl.hpp"

struct cdcl_conflicted_event_handler : event_handler<conflicted_event> {
    cdcl_conflicted_event_handler();
    void operator()(const conflicted_event& e) override;
#ifndef DEBUG
private:
#endif
    cdcl& c;
};

#endif
