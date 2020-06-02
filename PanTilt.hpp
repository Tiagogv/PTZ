
#ifndef PAN_TILT
#define PAN_TILT

#include <SparkFun_TB6612.h>
#include <Arduino.h>

#define AIN1 7
#define AIN2 8
#define PWMA 9
#define STBY 6
#define BIN1 5
#define BIN2 4
#define PWMB 3

enum PanDirection
{
    Left = 1,
    Right = -1
};

enum TiltDirection
{
    Down = 1,
    Up = -1
};

class PanTilt
{
private:
    Motor _tiltMotor = Motor(AIN1, AIN2, PWMA, 1, STBY);
    Motor _panMotor = Motor(BIN1, BIN2, PWMB, 1, STBY);

    void loopPan();
    void loopTilt();

    bool tiltLimit();
    bool panLimit();

    int _currentTilt;
    int _currentPan;

    int _currentTiltSpeed = 0;
    int _currentPanSpeed = 0;
    int _targetTiltSpeed = 0;
    int _targetPanSpeed = 0;

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
