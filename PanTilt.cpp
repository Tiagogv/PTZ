
#include "PanTilt.hpp"

#include <SparkFun_TB6612.h>
#include <Arduino.h>

#define SPEED_THRESHOLD 5
#define STOP_DOWN A0
#define STOP_UP A1
#define STOP_0 A2
#define STOP_45 A3
#define STOP_90 A4
#define STOP_340 A5

#define PAN_STEP 8
#define PAN_RANGE PAN_STEP * 0x18
#define PAN_OFFSET 255 - PAN_RANGE
#define PAN_MIN_SPEED PAN_OFFSET + PAN_STEP
#define TILT_STEP 7
#define TILT_RANGE TILT_STEP * 0X14
#define TILT_OFFSET 255 - TILT_RANGE

void PanTilt::setup()
{
    pinMode(STOP_UP, INPUT_PULLUP);
    pinMode(STOP_DOWN, INPUT_PULLUP);
    pinMode(STOP_0, INPUT_PULLUP);
    pinMode(STOP_45, INPUT_PULLUP);
    pinMode(STOP_90, INPUT_PULLUP);
    pinMode(STOP_340, INPUT_PULLUP);

    // calibrate();
    // home();

    // _loopTime = millis();
}

void PanTilt::loop()
{
    // _elapsedTime = millis() - _loopTime;

    // if (_currentPanSpeed != 0)
    // {
    //     float panAngularSpeed = ((abs(_currentPanSpeed) - PAN_MIN_SPEED) * _panAngularSpeed[2]) + _panAngularSpeed[0];
    //     float panAngle = panAngularSpeed * _elapsedTime;

    //     if (_currentPanSpeed < 0)
    //     {
    //         _currentPan -= panAngle;
    //     }
    //     else
    //     {
    //         _currentPan += panAngle;
    //     }
    // }

    // if (_currentTiltSpeed > 0)
    // {
    //     _currentTilt += ((_currentTiltSpeed - TILT_OFFSET - TILT_STEP) * _tiltAngularSpeed[2]) + _tiltAngularSpeed[0];
    // }

    loopTilt();
    loopPan();

    // _loopTime = millis();
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
        _currentTiltSpeed = 0;
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
        _currentPanSpeed = 0;
        _panMotor.brake();
        return;
    }

    _panMotor.drive(_currentPanSpeed);
}

bool PanTilt::tiltLimit()
{
    if (_currentTiltSpeed > 0)
    {
        return !digitalRead(STOP_DOWN);
    }

    return !digitalRead(STOP_UP);
}

bool PanTilt::panLimit()
{
    if (_currentPanSpeed > 0)
    {
        return !digitalRead(STOP_0);
    }

    return !digitalRead(STOP_340);
}

void PanTilt::tilt(TiltDirection dir, unsigned char speed)
{
    if (speed == 0)
    {
        _targetTiltSpeed = 0;
        return;
    };

    _targetTiltSpeed = dir * (speed * TILT_STEP + TILT_OFFSET);
}

void PanTilt::pan(PanDirection dir, unsigned char speed)
{
    if (speed == 0)
    {
        _targetPanSpeed = 0;
        return;
    }

    _targetPanSpeed = dir * (speed * PAN_STEP + PAN_OFFSET);
}

void PanTilt::stop()
{
    _targetTiltSpeed = 0;
    _targetPanSpeed = 0;
}

void PanTilt::home()
{
    // _tiltMotor.brake();
    // TiltDirection tiltDir = TiltDirection::Up;
    // PanDirection panDir = PanDirection::Left;

    // if (_currentTilt > 0)
    // {
    //     tiltDir = TiltDirection::Down;
    // }

    // if (_currentPan > 0)
    // {
    //     panDir = PanDirection::Right;
    // }

    // unsigned long timeTilt = round(abs(_currentTilt) / _tiltAngularSpeed[1]);
    // unsigned long timePan = round(abs(_currentPan) / _panAngularSpeed[1]);

    // unsigned long diff = timePan - timeTilt;

    // _tiltMotor.drive(tiltDir * 255);
    // _panMotor.drive(panDir * 255);

    // if (diff > 0)
    // {
    //     delay(timeTilt);
    //     _tiltMotor.brake();
    //     delay(diff);
    //     _panMotor.brake();
    // }
    // else
    // {
    //     delay(timePan);
    //     _panMotor.brake();
    //     delay(-diff);
    //     _tiltMotor.brake();
    // }

    // _currentTilt = 0;
    // _currentPan = 0;
}

void PanTilt::position(unsigned char tiltSpeed, unsigned char panSpeed, int pan, int tilt, bool relative)
{
}

void PanTilt::calibrate()
{
    _tiltMotor.brake();
    _panMotor.brake();

    // calibratePan();
    // calibrateTilt();

    // stop();
}

void PanTilt::calibratePan()
{
    // unsigned long _startTime;
    // unsigned long _totalTime;

    // pan(PanDirection::Left, 0x18);
    // _currentPanSpeed = _targetPanSpeed;
    // _panMotor.drive(_targetPanSpeed);

    // while (!digitalRead(STOP_0))
    // {
    //     delay(1);
    // }
    // _panMotor.brake();

    // pan(PanDirection::Right, 0x01);
    // _currentPanSpeed = _targetPanSpeed;
    // _startTime = millis();
    // _panMotor.drive(_targetPanSpeed);

    // while (!digitalRead(STOP_45))
    // {
    //     delay(1);
    // }
    // _panMotor.brake();

    // _totalTime = millis() - _startTime;

    // _panAngularSpeed[0] = 330.0 / _totalTime;

    // pan(PanDirection::Left, 0x18);
    // _currentPanSpeed = _targetPanSpeed;

    // _startTime = millis();
    // _panMotor.drive(_targetPanSpeed);

    // while (!digitalRead(STOP_0))
    // {
    //     delay(1);
    // }
    // _panMotor.brake();

    // _totalTime = millis() - _startTime;

    // _panAngularSpeed[1] = 45.0 / _totalTime;
    // _panAngularSpeed[2] = (_panAngularSpeed[1] - _panAngularSpeed[0]) / (PAN_RANGE - PAN_STEP);

    // _currentPan = 165;
    // _currentPanSpeed = 0;
}

void PanTilt::calibrateTilt()
{
    // unsigned long _startTime;
    // unsigned long _totalTime;

    // tilt(TiltDirection::Up, 0x14);
    // _currentTiltSpeed = _targetTiltSpeed;
    // _tiltMotor.drive(_targetTiltSpeed);

    // while (!tiltLimit())
    // {
    //     delay(1);
    // }
    // _tiltMotor.brake();

    // tilt(TiltDirection::Down, 0x01);
    // _currentTiltSpeed = _targetTiltSpeed;
    // _startTime = millis();
    // _tiltMotor.drive(_targetTiltSpeed);

    // while (!tiltLimit())
    // {
    //     delay(1);
    // }
    // _tiltMotor.brake();

    // _totalTime = millis() - _startTime;

    // _tiltAngularSpeed[0] = 30.0 / _totalTime;

    // tilt(TiltDirection::Up, 0x14);
    // _currentTiltSpeed = _targetTiltSpeed;

    // _startTime = millis();
    // _tiltMotor.drive(_targetTiltSpeed);

    // while (!tiltLimit())
    // {
    //     delay(1);
    // }
    // _tiltMotor.brake();

    // _totalTime = millis() - _startTime;

    // _tiltAngularSpeed[1] = 30.0 / _totalTime;
    // _tiltAngularSpeed[2] = (_tiltAngularSpeed[1] - _tiltAngularSpeed[0]) / (TILT_RANGE - TILT_STEP);

    // _currentTilt = 15.0;
    // _currentTiltSpeed = 0;
}

PanTilt::PanTilt(/* args */)
{
}

PanTilt::~PanTilt()
{
}
