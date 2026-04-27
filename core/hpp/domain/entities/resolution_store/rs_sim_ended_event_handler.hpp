#ifndef RS_SIM_ENDED_EVENT_HANDLER_HPP
#define RS_SIM_ENDED_EVENT_HANDLER_HPP

#include "../../events/sim_ended_event.hpp"
#include "../resolution_store/resolution_store.hpp"
#include "../../../infrastructure/event_handler.hpp"

struct rs_sim_ended_event_handler : event_handler<sim_ended_event> {
    rs_sim_ended_event_handler();
    void operator()(const sim_ended_event&) override;
#ifndef DEBUG
private:
#endif
    resolution_store& rs;
};

#endif
