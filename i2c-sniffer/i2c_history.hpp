#ifndef I2C_HISTORY_HPP
#define I2C_HISTORY_HPP

#include "Arduino.h"

#define HISTORY_CAPACITY 8192

enum class Event : byte
{
    SDA_RISE,
    SDA_FALL,
    SCL_RISE,
    SCL_FALL,
    NO_EVENT,
};

class History
{

public:
    bool is_writable() volatile const;
    void write(Event event) volatile;
    void reset() volatile;
    size_t length() volatile const;

    Event read(size_t id) volatile const;

private:
    Event events[HISTORY_CAPACITY];
    size_t len;
};

#endif