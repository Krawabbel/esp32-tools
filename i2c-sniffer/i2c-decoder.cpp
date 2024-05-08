#include "Arduino.h"

#include "i2c-decoder.hpp"

static constexpr size_t expected_bit_len = 10;

Message::Message(byte data, bool ack, bool is_valid, size_t n_bits, String raw_snippet)
    : data{data}, ack{ack}, is_valid{is_valid}, n_bits{n_bits}, raw_snippet{raw_snippet}
{
    // empty
}

Decoder::Decoder(Recorder const volatile &recorder) : recorder{recorder}
{
    // empty
}

static size_t find_next_start_after_stop(Recorder const volatile &recorder, size_t begin)
{
    for (size_t i = begin; i < recorder.length(); i++)
    {
        const Signal signal = recorder.read(i);
        if (signal == Signal::STOP)
        {
            const Signal peek = i + 1 < recorder.length() ? recorder.read(i + 1) : Signal::NO_SIGNAL;
            if (peek == Signal::START)
            {
                return i + 1;
            }
        }
    }
    return recorder.length();
}

Message Decoder::next()
{
    const size_t start_ptr = this->next_ptr;

    const size_t end_ptr = find_next_start_after_stop(this->recorder, start_ptr);
    this->next_ptr = end_ptr;

    // Serial.printf("%i\n", end_ptr);

    bool is_valid = (this->recorder.read(start_ptr) == Signal::START) && (this->recorder.read(end_ptr - 1) == Signal::STOP);

    size_t bit_counter = 0;

    byte data = 0;
    bool ack = false;

    for (size_t i = start_ptr; i < end_ptr; i++)
    {
        Signal signal = this->recorder.read(i);
        switch (signal)
        {
        case Signal::BIT_HIGH:
            if (bit_counter < 8)
            {
                data |= (1U << (7 - bit_counter));
            }
            else if (bit_counter == 8)
            {
                ack = false;
            }
            else if (bit_counter == 9)
            {
                // 10th bit must be LOW
                is_valid = false;
            }
            bit_counter++;
            break;
        case Signal::BIT_LOW:
            if (bit_counter == 8)
            {
                ack = true;
            }
            // low bits are default for data and ack
            // low bit is expected for 10th bit (due to measurement mode)
            bit_counter++;
            break;
        case Signal::START:
            is_valid = is_valid && (i == start_ptr);
            break;
        case Signal::STOP:
            is_valid = is_valid && (i == end_ptr - 1);
            break;
        case Signal::NO_SIGNAL:
        default:
            is_valid = false;
            break;
        }
    }

    // final sanity checks
    is_valid = is_valid && (bit_counter == expected_bit_len);

    const String raw = this->recorder.snippet(start_ptr, end_ptr);

    return Message(data, ack, is_valid, bit_counter - 1, raw);
}

bool Decoder::eof()
{
    return this->next_ptr >= this->recorder.length();
}
