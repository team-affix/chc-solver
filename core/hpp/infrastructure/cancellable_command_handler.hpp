#ifndef CANCELLABLE_EVENT_HANDLER_HPP
#define CANCELLABLE_EVENT_HANDLER_HPP

#include "cancellation_token.hpp"
#include "command_handler.hpp"
#include "../bootstrap/resolver.hpp"

template<typename Command>
struct cancellable_command_handler : command_handler<Command> {
    virtual ~cancellable_command_handler() = default;
    cancellable_command_handler();
    virtual void execute(const Command&) = 0;
#ifndef DEBUG
private:
#endif
    void operator()(const Command&) final override;
    const cancellation_token& token;
};

template<typename Command>
cancellable_command_handler<Command>::cancellable_command_handler() :
    command_handler<Command>(),
    token(resolver::resolve<cancellation_token>()) {
}

template<typename Command>
void cancellable_command_handler<Command>::operator()(const Command& command) {
    if (token.is_cancelled())
        return;
    execute(command);
}

#endif
