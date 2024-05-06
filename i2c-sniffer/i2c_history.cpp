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
