#ifndef I2C_HISTORY_HPP
#define I2C_HISTORY_HPP

#include "Arduino.h"

#define HISTORY_CAPACITY 8192

enum class Event : byte
{
    START,
    BIT_LOW,
    BIT_HIGH,
    STOP,
    NO_EVENT,
};

String event_str(Event event);

class History
{

public:
    bool is_writable() volatile const;
    void write(Event event) volatile;
    void reset() volatile;
    size_t length() volatile const;

    Event read(size_t id) volatile const;

    String dump() volatile const;

private:
    Event events[HISTORY_CAPACITY];
    size_t len;
};

#endif