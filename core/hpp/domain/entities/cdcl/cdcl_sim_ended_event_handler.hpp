#ifndef CDCL_SIM_ENDED_EVENT_HANDLER_HPP
#define CDCL_SIM_ENDED_EVENT_HANDLER_HPP

#include "../../../infrastructure/event_handler.hpp"
#include "../../events/sim_ended_event.hpp"
#include "cdcl.hpp"

struct cdcl_sim_ended_event_handler : event_handler<sim_ended_event> {
    cdcl_sim_ended_event_handler();
    void operator()(const sim_ended_event& e) override;
#ifndef DEBUG
private:
#endif
    cdcl& c;
};

#endif
