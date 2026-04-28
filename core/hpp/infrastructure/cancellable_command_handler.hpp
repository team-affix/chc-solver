#ifndef CANCELLABLE_EVENT_HANDLER_HPP
#define CANCELLABLE_EVENT_HANDLER_HPP

#include "command_handler.hpp"
#include "event_handler.hpp"

template<typename Command, typename CancellationEvent, typename ResetCancellationEvent>
struct cancellable_command_handler :
        command_handler<Command>,
        event_handler<CancellationEvent>,
        event_handler<ResetCancellationEvent> {
    virtual ~cancellable_command_handler() = default;
    cancellable_command_handler();
    virtual void execute(const Command&) = 0;
#ifndef DEBUG
private:
#endif
    void operator()(const Command&) final override;
    void operator()(const CancellationEvent&) final override;
    void operator()(const ResetCancellationEvent&) final override;
    bool cancelled;
};

template<typename Command, typename CancellationEvent, typename ResetCancellationEvent>
cancellable_command_handler<Command, CancellationEvent, ResetCancellationEvent>::cancellable_command_handler() :
    command_handler<Command>(),
    event_handler<CancellationEvent>(),
    event_handler<ResetCancellationEvent>(),
    cancelled(false) {
}

template<typename Command, typename CancellationEvent, typename ResetCancellationEvent>
void cancellable_command_handler<Command, CancellationEvent, ResetCancellationEvent>::operator()(const Command& command) {
    if (cancelled)
        return;
    execute(command);
}

template<typename Command, typename CancellationEvent, typename ResetCancellationEvent>
void cancellable_command_handler<Command, CancellationEvent, ResetCancellationEvent>::operator()(const CancellationEvent& event) {
    cancelled = true;
}

template<typename Command, typename CancellationEvent, typename ResetCancellationEvent>
void cancellable_command_handler<Command, CancellationEvent, ResetCancellationEvent>::operator()(const ResetCancellationEvent& event) {
    cancelled = false;
}

#endif
