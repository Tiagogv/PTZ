#ifndef CAMERA
#define CAMERA

#include "Lanc.hpp"

class Camera
{
private:
    int _videoCamMode = 0x28;
    int _vtrMode = 0x18;
    Lanc _lanc;
    bool _irisAuto = true;
    bool _autoFocus = true;
    int _zoom = 0;
    int _zoomSpeed = 0;

    bool _on = false;

    void _zoomWide(int speed);
    void _zoomTele(int speed);

public:
    Camera(Lanc lanc) : _lanc(lanc) {}
    ~Camera();

    void loop();
    void setup();

    void zoomWide(int speed);
    void zoomTele(int speed);
    void zoomStop();
    void zoomDirect(int value);

    int getZoom() { return _zoom; };

    void focusFar();
    void focusNear();
    void focusDirect(int value);
    void focusStop();

    void autoFocus(bool on);
    void toggleAutoFocus();
    bool getAutoFocus() { return _autoFocus; };

    void irisOpen();
    void irisClose();
    void irisDirect(int value);

    void irisAuto();
    bool getIrisAuto() { return _irisAuto; };

    void menu();
    void menuUp();
    void menuDown();
    void menuLeft();
    void menuRight();
    void menuEnter();

    void on();
    void off();
    void sleep();
    bool isOn() { return _on; };
};

#endif //CAMERA
