#include "../../../../hpp/domain/entities/cdcl/cdcl_conflicted_event_handler.hpp"
#include "../../../../hpp/infrastructure/locator.hpp"

cdcl_conflicted_event_handler::cdcl_conflicted_event_handler() :
    c(locator::locate<cdcl>()) {
}

void cdcl_conflicted_event_handler::operator()(const conflicted_event& e) {
    c.learn();
}
