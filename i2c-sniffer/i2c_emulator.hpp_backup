#ifndef I2C_EMULATOR_HPP
#define I2C_EMULATOR_HPP

#include "Arduino.h"
#include "i2c_history.hpp"

enum class State : byte
{
    Unknown,
    Started,
    Waiting,
    RecordingHigh,
    RecordingLow,
    Stopped,
    Idle,
};

enum class Line
{
    Low,
    High,
    Invalid,
};

enum class Signal
{
    Start,
    Stop,
    Low,
    High,
    Ignore,
};

class Emulator
{

public:
    Emulator(History volatile *history);

    bool next();

    String event_log() const;
    String signal_log() const;
    String data_log() const;
    String ack_log() const;
    String add_log() const;

private:
    Signal transition(Event event);

    void log_signal(Signal signal);

    History volatile *history;
    size_t history_start, history_curr;

    Signal signals[HISTORY_CAPACITY];
    size_t signals_start, signals_curr;

    State state;
    Line scl;
    Line sda;
};

#endif