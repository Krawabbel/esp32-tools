#ifndef I2C_EMULATOR_HPP
#define I2C_EMULATOR_HPP

#include "Arduino.h"
#include "i2c-recorder.hpp"

struct Message
{
    Message(byte data, bool ack, bool is_valid, size_t n_bits, String raw_snippet);
    String str() const;

    byte data;
    bool ack;
    bool is_valid;
    size_t n_bits;
    String raw_snippet;
};

class Decoder
{

public:
    Decoder(Recorder const volatile &recorder);
    Message next();
    bool eof();

private:
    Recorder const volatile &recorder;
    size_t next_ptr = 0;
};

#endif