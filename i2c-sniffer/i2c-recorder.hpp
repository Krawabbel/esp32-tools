#ifndef I2C_RECORDER_HPP
#define I2C_RECORDER_HPP

#include "Arduino.h"

#define RECORDER_CAPACITY 0x1000

enum class Signal : byte
{
    START,
    BIT_LOW,
    BIT_HIGH,
    STOP,
    NO_SIGNAL,
};

String signal_str(Signal signal);

class Recorder
{

public:
    bool is_writable() volatile const;
    void write(const Signal signal) volatile;

    size_t length() volatile const;
    Signal read(const size_t id) volatile const;

    String snippet(const size_t start, const size_t end) const volatile;

    void reset() volatile;

private:
    Signal signals[RECORDER_CAPACITY];
    size_t len = 0;
};

#endif