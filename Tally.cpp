#include "Tally.hpp"

#include <Arduino.h>
#include <Ethernet.h>

void Tally::setup(IPAddress ip, int input)
{
    _input = input;
    _ip = ip;

    pinMode(_ledPin, OUTPUT);

    off();
    connect();
}

void Tally::on()
{
    digitalWrite(_ledPin, HIGH);
}

void Tally::off()
{
    digitalWrite(_ledPin, LOW);
}

void Tally::process()
{
    if (_idx < (9 + _input) || _buffer[0] != 'T')
    {
        return;
    }

    if (_buffer[_input + 8] == '1')
    {
        on();
        return;
    }

    off();
}

void Tally::loop()
{
    if (client.available())
    {
        _buffer[_idx] = client.read();
        _idx++;

        if (_buffer[_idx - 1] == '\n' || _idx > 99)
        {
            _buffer[_idx] = '\0';
            process();
            _idx = 0;
        }
    }

    connect();
}

void Tally::connect()
{
    if (client.connected())
    {
        return;
    }

    if (client.connect(_ip, 8099))
    {
        client.println("SUBSCRIBE TALLY");
    }
}

Tally::~Tally()
{
}
