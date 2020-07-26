
#ifndef PAN_TILT
#define PAN_TILT

#include <SparkFun_TB6612.h>
#include <Arduino.h>

#define AIN1 5
#define AIN2 4
#define PWMA 3
#define STBY 2
#define BIN1 7
#define BIN2 8
#define PWMB 6

enum PanDirection
{
    Left = -1,
    Right = 1
};

enum TiltDirection
{
    Down = -1,
    Up = 1
};

class PanTilt
{
private:
    Motor _tiltMotor = Motor(AIN1, AIN2, PWMA, 1, STBY);
    Motor _panMotor = Motor(BIN1, BIN2, PWMB, 1, STBY);

    float _panAngularSpeed[3];
    float _tiltAngularSpeed[3];

    void loopPan();
    void loopTilt();

    long _loopTime;
    long _elapsedTime;

    bool tiltLimit();
    bool panLimit();

    float _currentTilt;
    float _currentPan;

    int _currentTiltSpeed = 0;
    int _currentPanSpeed = 0;
    int _targetTiltSpeed = 0;
    int _targetPanSpeed = 0;

    void calibrate();
    void calibratePan();
    void calibrateTilt();

public:
    PanTilt(/* args */);
    ~PanTilt();

    void setup();

    void pan(PanDirection dir, unsigned char speed);
    void tilt(TiltDirection dir, unsigned char speed);
    void home();
    void position(unsigned char tiltSpeed, unsigned char panSpeed, int pan, int tilt, bool relative = false);

    void stop();
    void loop();

    int getPan() { return _currentPan; }
    int getTilt() { return _currentTilt; }
};

#endif // PAN_TILT
