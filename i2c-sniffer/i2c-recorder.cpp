#include "i2c-recorder.hpp"

bool Recorder::is_writable() volatile const
{
    return this->len < RECORDER_CAPACITY;
}

void Recorder::write(Signal signal) volatile
{
    if (this->is_writable())
    {
        this->signals[this->len++] = signal;
    }
}

void Recorder::reset() volatile
{
    this->len = 0;
}

size_t Recorder::length() volatile const
{
    return this->len;
}

Signal Recorder::read(const size_t id) const volatile
{
    return id < this->len ? this->signals[id] : Signal::NO_SIGNAL;
}

String Recorder::snippet(const size_t start, const size_t end) const volatile
{
    String log = "";
    for (size_t i = start; i < end; i++)
    {
        log += signal_str(this->signals[i]);
    }
    return log;
}

String signal_str(Signal signal)
{
    switch (signal)
    {
    case Signal::START:
        return " START ";
    case Signal::BIT_LOW:
        return "0";
    case Signal::BIT_HIGH:
        return "1";
    case Signal::STOP:
        return " STOP ";
    case Signal::NO_SIGNAL:
        return " X ";
    }
}