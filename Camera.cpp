#include "Camera.hpp"
#include "Arduino.h"

#define MAX_ZOOM 147;
#define MIN_ZOOM 0;

enum LancCommand
{
    ZoomWide0 = 0x10,
    ZoomWide1 = 0x12,
    ZoomWide2 = 0x14,
    ZoomWide3 = 0x16,
    ZoomWide4 = 0x18,
    ZoomWide5 = 0x1A,
    ZoomWide6 = 0x1C,
    ZoomWide7 = 0x1E,
    ZoomTele0 = 0x00,
    ZoomTele1 = 0x02,
    ZoomTele2 = 0x04,
    ZoomTele3 = 0x06,
    ZoomTele4 = 0x08,
    ZoomTele5 = 0x0A,
    ZoomTele6 = 0x0C,
    ZoomTele7 = 0x0E,
    FocusFar = 0x45,
    FocusNear = 0x47,
    AutoFocus = 0x41,
    IrisOpen = 0x55,
    IrisClose = 0x53,
    IrisAuto = 0xAF,
    Menu = 0x9A,
    MenuUp = 0x84,
    MenuDown = 0x86,
    MenuRight = 0xC2,
    MenuLeft = 0xC4,
    MenuEnter = 0xA2,
    Off = 0x5E,
    Sleep = 0x6C
};

void Camera::_zoomWide(int speed)
{
    switch (speed)
    {
    case 0:
        _lanc.sendCommand(_videoCamMode, ZoomWide0);
        break;
    case 1:
        _lanc.sendCommand(_videoCamMode, ZoomWide1);
        break;
    case 2:
        _lanc.sendCommand(_videoCamMode, ZoomWide2);
        break;
    case 3:
        _lanc.sendCommand(_videoCamMode, ZoomWide3);
        break;
    case 4:
        _lanc.sendCommand(_videoCamMode, ZoomWide4);
        break;
    case 5:
        _lanc.sendCommand(_videoCamMode, ZoomWide5);
        break;
    case 6:
        _lanc.sendCommand(_videoCamMode, ZoomWide6);
        break;
    case 7:
        _lanc.sendCommand(_videoCamMode, ZoomWide7);
        break;

    default:
        _lanc.sendCommand(_videoCamMode, ZoomWide3);
    }

    _zoom -= speed;

    if (_zoom < 0)
    {
        _zoom = MIN_ZOOM;
    }
}

void Camera::_zoomTele(int speed)
{
    switch (speed)
    {
    case 0:
        _lanc.sendCommand(_videoCamMode, ZoomTele0);
        break;
    case 1:
        _lanc.sendCommand(_videoCamMode, ZoomTele1);
        break;
    case 2:
        _lanc.sendCommand(_videoCamMode, ZoomTele2);
        break;
    case 3:
        _lanc.sendCommand(_videoCamMode, ZoomTele3);
        break;
    case 4:
        _lanc.sendCommand(_videoCamMode, ZoomTele4);
        break;
    case 5:
        _lanc.sendCommand(_videoCamMode, ZoomTele5);
        break;
    case 6:
        _lanc.sendCommand(_videoCamMode, ZoomTele6);
        break;
    case 7:
        _lanc.sendCommand(_videoCamMode, ZoomTele7);
        break;

    default:
        _lanc.sendCommand(_videoCamMode, ZoomTele3);
    }

    _zoom += speed;

    if (_zoom > 147)
    {
        _zoom = MAX_ZOOM;
    }
}

void Camera::focusFar()
{
    _lanc.sendCommand(_videoCamMode, FocusFar);
    _autoFocus = false;
}

void Camera::focusNear()
{
    _lanc.sendCommand(_videoCamMode, FocusNear);
    _autoFocus = false;
}

void Camera::focusDirect(int value)
{
}

void Camera::focusStop()
{
}

void Camera::toggleAutoFocus()
{
    _lanc.sendCommand(_videoCamMode, AutoFocus);
    _autoFocus = !_autoFocus;
}

void Camera::autoFocus(bool on)
{
    if (_autoFocus == on)
    {
        return;
    }
    toggleAutoFocus();
}

void Camera::irisOpen()
{
    _lanc.sendCommand(_videoCamMode, IrisOpen);
    _irisAuto = false;
}

void Camera::irisClose()
{
    _lanc.sendCommand(_videoCamMode, IrisClose);
    _irisAuto = false;
}

void Camera::irisDirect(int value)
{
}

void Camera::irisAuto()
{
    _lanc.sendCommand(_videoCamMode, IrisAuto);
    _irisAuto = true;
}

void Camera::menu()
{
    _lanc.sendCommand(_vtrMode, Menu);
}

void Camera::menuUp()
{
    _lanc.sendCommand(_vtrMode, MenuUp);
}
void Camera::menuDown()
{
    _lanc.sendCommand(_vtrMode, MenuDown);
}
void Camera::menuLeft()
{
    _lanc.sendCommand(_vtrMode, MenuLeft);
}
void Camera::menuRight()
{
    _lanc.sendCommand(_vtrMode, MenuRight);
}
void Camera::menuEnter()
{
    _lanc.sendCommand(_vtrMode, MenuEnter);
}

void Camera::on()
{
    _lanc.on();
    _on = true;
}

void Camera::off()
{
    _lanc.sendCommand(_vtrMode, Off);
    _on = false;
}

void Camera::sleep()
{
    _lanc.sendCommand(_vtrMode, Sleep);
}



void Camera::zoomWide(int speed)
{
    _zoomSpeed = -speed;
}

void Camera::zoomTele(int speed)
{
    _zoomSpeed = speed;
}

void Camera::zoomDirect(int value)
{
    if (value < 0)
    {
        value = MIN_ZOOM;
    }

    if (value > 147)
    {
        value = MAX_ZOOM;
    }

    int diff = value - _zoom;

    while (diff != 0)
    {
        if (diff > 0)
        {
            _zoomTele(min(diff, 4));
        }
        else
        {
            _zoomWide(min(-diff, 4));
        }

        diff = value - _zoom;
    }
}

void Camera::zoomStop()
{
    _zoomSpeed = 0;
}

void Camera::setup()
{
    _lanc.setup();

    for (int i = 0; i < 25; i++)
    {
        _zoomWide(7);
    }

    zoomStop();
    _zoom = MIN_ZOOM;
}

void Camera::loop()
{

    if (_zoomSpeed != 0)
    {
        if (_zoomSpeed > 0)
        {
            _zoomTele(_zoomSpeed);
        }
        else
        {
            _zoomWide(-_zoomSpeed);
        }
    }
}

Camera::~Camera()
{
}
