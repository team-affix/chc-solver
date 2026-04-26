#ifndef CHECK_INITIAL_CONDITIONS_COMMAND_HANDLER_HPP
#define CHECK_INITIAL_CONDITIONS_COMMAND_HANDLER_HPP

#include "../../../infrastructure/command_handler.hpp"
#include "../../commands/check_initial_conditions_command.hpp"
#include "initial_condition_detector.hpp"

struct check_initial_conditions_command_handler : command_handler<check_initial_conditions_command> {
    check_initial_conditions_command_handler();
    void operator()(const check_initial_conditions_command& c) override;
#ifndef DEBUG
private:
#endif
    initial_condition_detector& icd;
};

#endif
