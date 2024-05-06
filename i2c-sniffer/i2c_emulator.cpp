#include "Arduino.h"

#include "i2c_emulator.hpp"

Emulator::Emulator(History volatile *history)
{
    this->history = history;
    history_start = 0;
    history_curr = 0;

    signals_start = 0;
    signals_curr = 0;

    this->state = State::Unknown;
    this->scl = Line::Invalid;
    this->sda = Line::Invalid;
}

void Emulator::log_signal(Signal signal)
{
    this->signals[this->signals_curr++] = signal;
}

bool Emulator::next()
{
    this->history_start = this->history_curr;
    this->signals_start = this->signals_curr;

    while ((this->history_curr < this->history->length()) && (this->state == State::Stopped))
    {
        const Event event = this->history->read(this->history_curr);
        const Signal signal = this->transition(event);
        this->log_signal(signal);
        this->history_curr++;
    }

    return this->history_curr < this->history->length();
}

Signal Emulator::transition(Event event)
{

    // transition lines
    switch (event)
    {
    case Event::SCL_FALL:
        this->scl = Line::Low;
    case Event::SCL_RISE:
        this->scl = Line::High;
    case Event::SDA_FALL:
        this->sda = Line::Low;
    case Event::SDA_RISE:
        this->sda = Line::High;
    case Event::NO_EVENT:
    default:
        this->sda = Line::Invalid;
        this->scl = Line::Invalid;
    }

    // transition states and emit signal
    if (this->sda == Line::Invalid || this->scl == Line::Invalid)
    {
        this->state = State::Unknown;
        return Signal::Ignore;
    }
    else if (event == Event::SCL_FALL)
    {
        this->state = State::Waiting;
        switch (this->state)
        {
        case State::RecordingLow:
            return Signal::Low;
        case State::RecordingHigh:
            return Signal::High;
        default:
            return Signal::Ignore;
        }
    }
    else if (this->scl == Line::Low)
    {
        this->state = State::Waiting;
        return Signal::Ignore;
    }
    else if (event == Event::SDA_FALL)
    {
        this->state = State::Started;
        return Signal::Start;
    }
    else if (event == Event::SDA_RISE)
    {
        this->state = State::Stopped;
        return Signal::Stop;
    }
    else if (event == Event::SCL_RISE)
    {
        switch (this->sda)
        {
        case Line::Low:
            this->state = State::RecordingLow;
            break;
        case Line::High:
            this->state = State::RecordingHigh;
            break;
        default:
            break;
        }
        return Signal::Ignore;
    }
    else
    {
        this->state = State::Unknown;
        return Signal::Ignore;
    }
}

static String event_str(Event event)
{
    switch (event)
    {
    case Event::SDA_FALL:
        return "-";
    case Event::SDA_RISE:
        return "+";
    case Event::SCL_FALL:
        return "\\";
    case Event::SCL_RISE:
        return "/";
    case Event::NO_EVENT:
        return "?";
    }
}

String Emulator::event_log() const
{
    String log = "";
    for (size_t i = history_start; i < history_curr; i++)
    {
        log += event_str(this->history->read(i));
    }
    return log;
}

static String signal_str(Signal signal)
{
    switch (signal)
    {
    case Signal::Start:
        return " START ";
    case Signal::Stop:
        return " STOP ";
    case Signal::Low:
        return "0";
    case Signal::High:
        return "1";
    case Signal::Ignore:
    default:
        return "";
    }
}

String Emulator::signal_log() const
{
    String log = "";
    for (size_t i = this->signals_start; i < signals_curr; i++)
    {
        log += signal_str(this->signals[i]);
    }
    return log;
}

String Emulator::data_log() const
{
    return "";
}

String Emulator::ack_log() const
{
    return "";
}

String Emulator::add_log() const
{
    return "";
}

// String emulator_data_log(Emulator const &emu)
// {
//     byte data = 0U;
//     bool is_data_valid = true;
//     for (size_t i = 0; i < 8; i++)
//     {
//         switch (this->bit_log[i])
//         {
//         case Bit::Low:
//             break;
//         case Bit::High:
//             data |= (1U << (7 - i));
//             break;
//         case Bit::Unset:
//         case Bit::Invalid:
//         default:
//             is_data_valid = false;
//             break;
//         }
//     }

//     char data_buffer[10];
//     if (is_data_valid)
//     {
//         sprintf(data_buffer, "0x%02X", data);
//     }
//     else
//     {
//         sprintf(data_buffer, "N/A");
//     }

//     return String(data_buffer);
// }

// String emulator_ack_log(Emulator const &emu)
// {
//     char ack_buffer[10];
//     switch (this->bit_log[8])
//     {
//     case Bit::Low:
//         return "----------------------------> ACK";
//     case Bit::High:
//         return "NACK";
//     case Bit::Unset:
//         return "UNSET";
//     case Bit::Invalid:
//     default:
//         return "ERROR";
//     }
// }

// String emulator_add_log(Emulator const &emu)
// {
//     if (this->bit_log_ptr > 9)
//     {
//         char add_buffer[30];
//         sprintf(add_buffer, "(%d additional bits ignored)", this->bit_log_ptr - 9);
//         return String(add_buffer);
//     }
//     else
//     {
//         return "";
//     }
// }
