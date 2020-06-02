
#include "PanTilt.hpp"

#include <SparkFun_TB6612.h>
#include <Arduino.h>

#define SPEED_THRESHOLD 5

void PanTilt::setup()
{
    pinMode(A1, INPUT_PULLUP);
    pinMode(A0, INPUT_PULLUP);
}

void PanTilt::loop()
{
    loopTilt();
    loopPan();
}

void PanTilt::loopTilt()
{
    float diff = _targetTiltSpeed - _currentTiltSpeed;

    if (abs(diff) < SPEED_THRESHOLD)
    {
        _currentTiltSpeed = _targetTiltSpeed;
    }
    else
    {
        _currentTiltSpeed += diff / 20.0;
    }

    if (tiltLimit())
    {
        _tiltMotor.brake();
        return;
    }

    _tiltMotor.drive(_currentTiltSpeed);
}

void PanTilt::loopPan()
{
    float diff = _targetPanSpeed - _currentPanSpeed;

    if (abs(diff) > SPEED_THRESHOLD)
    {
        _currentPanSpeed = _targetPanSpeed;
    }
    else
    {
        _currentPanSpeed += diff / 50.0;
    }

    if (panLimit())
    {
        _panMotor.brake();
        return;
    }

    _panMotor.drive(_currentPanSpeed);
}

bool PanTilt::tiltLimit()
{
    if (_currentTiltSpeed > 0)
    {
        return !digitalRead(A0);
    }

    return !digitalRead(A1);
}

bool PanTilt::panLimit()
{
    return false;
}

void PanTilt::tilt(TiltDirection dir, unsigned char speed)
{
    if (speed == 0)
    {
        _targetTiltSpeed = 0;
        return;
    };

    _targetTiltSpeed = dir * (speed * 8 + 75);
}

void PanTilt::pan(PanDirection dir, unsigned char speed)
{
    if (speed == 0)
    {
        _targetPanSpeed = 0;
        return;
    }

    _targetPanSpeed = dir * (speed * 9 + 39);
}

void PanTilt::stop()
{
    _targetTiltSpeed = 0;
    _targetPanSpeed = 0;
}

void PanTilt::home()
{
}

void PanTilt::position(unsigned char tiltSpeed, unsigned char panSpeed, int pan, int tilt, bool relative) {
    
}

PanTilt::PanTilt(/* args */)
{
}

PanTilt::~PanTilt()
{
}
