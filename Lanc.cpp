
#include "Lanc.hpp"
#include <Arduino.h>

#define bitDuration 104

//Reading to the digital port takes about 4 microseconds so only 100 microseconds are left for each bit
#define digitalReadDuration 4
#define readBitDuration (bitDuration - digitalReadDuration)

//Writing to the digital port takes about 8 microseconds so only 96 microseconds are left for each bit
#define digitalWriteDuration 8
#define writeBitDuration (bitDuration - digitalWriteDuration)

#define cmdRepeatTimes 5

void Lanc::setup()
{
    on();

    Serial.begin(9600);
    delay(2000);

    long pulse = pulseIn(_input, HIGH, 10000);

    if (pulse < 6000 && pulse > 0)
    {
        _connected = true;
    }

    pinMode(_output, OUTPUT);
    digitalWrite(_output, LOW);
}

void Lanc::on()
{
    pinMode(_input, OUTPUT);
    digitalWrite(_input, LOW);
    delay(200);
    pinMode(_input, INPUT);
}

//Write

void Lanc::writeByte(unsigned char byte)
{
    delayMicroseconds(bitDuration - 7); // wait START bit duration

    for (int i = 0; i < 8; i++)
    {
        digitalWrite(_output, bitRead(byte, i));
        delayMicroseconds(writeBitDuration);
    }
    digitalWrite(_output, LOW);
}

unsigned char Lanc::readByte()
{
    unsigned char byte = 0x00;

    delayMicroseconds(bitDuration); // wait START bit duration

    for (int i = 0; i < 8; i++)
    {
        delayMicroseconds(50);
        bitWrite(byte, i, digitalRead(_input));
        delayMicroseconds(50);
    }
    digitalWrite(_output, LOW);
    return byte;
}

void Lanc::sendCommand(unsigned char mode, unsigned char cmd)
{
    if (!_connected)
    {
        return;
    }

    unsigned char lancCmd[] = {mode, cmd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    int repeat = cmdRepeatTimes;

    while (pulseIn(_input, HIGH) < 5000)
    {
    }

    while (repeat > 0)
    {

        for (int byt = 0; byt < 8; byt++)
        {
            if (byt < 2)
            {
                writeByte(lancCmd[byt]);
            }
            else
            {
                lancCmd[byt] = readByte();
            }

            while (digitalRead(_input))
            {
            }
        }

        if (!bitRead(lancCmd[5], 0))
        {
            break;
        }

        repeat--;
    }
}

Lanc::~Lanc()
{
}
