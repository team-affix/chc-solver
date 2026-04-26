#include "../../../../hpp/domain/entities/initial_condition_detector/check_initial_conditions_command_handler.hpp"
#include "../../../../hpp/infrastructure/locator.hpp"

check_initial_conditions_command_handler::check_initial_conditions_command_handler() :
    icd(locator::locate<initial_condition_detector>()) {
}

void check_initial_conditions_command_handler::operator()(const check_initial_conditions_command& c) {
    icd.check_initial_conditions(c.gl);
}
