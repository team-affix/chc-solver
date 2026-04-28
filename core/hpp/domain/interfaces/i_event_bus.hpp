#ifndef I_EVENT_BUS_HPP
#define I_EVENT_BUS_HPP

template<typename Event>
struct i_event_bus {
    virtual ~i_event_bus() = default;
    virtual void produce(const Event&) = 0;
};

#endif
