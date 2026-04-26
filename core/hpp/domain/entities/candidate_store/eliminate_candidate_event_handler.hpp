#ifndef ELIMINATE_CANDIDATE_EVENT_HANDLER_HPP
#define ELIMINATE_CANDIDATE_EVENT_HANDLER_HPP

#include "../../events/eliminate_candidate_event.hpp"

struct eliminate_candidate_event_handler {
    void operator()(const eliminate_candidate_event& e);
};

#endif
