#include "i2c_history.hpp"

bool History::is_writable() volatile const
{
    return this->len < HISTORY_CAPACITY;
}

void History::write(Event event) volatile
{
    if (this->is_writable())
    {
        this->events[this->len++] = event;
    }
}

Event History::read(size_t id) volatile const
{
    return id < this->len ? this->events[id] : Event::NO_EVENT;
}

void History::reset() volatile
{
    this->len = 0;
}

size_t History::length() volatile const
{
    return this->len;
}

String History::dump() const volatile
{
    String log = "";
    for (size_t i = 0; i < this->len; i++)
    {
        log += event_str(this->events[i]);
    }
    return log;
}

String event_str(Event event)
{
    switch (event)
    {
    case Event::START:
        return " START ";
    case Event::BIT_LOW:
        return "0";
    case Event::BIT_HIGH:
        return "1";
    case Event::STOP:
        return " STOP ";
    case Event::NO_EVENT:
        return " ? ";
    }
}
