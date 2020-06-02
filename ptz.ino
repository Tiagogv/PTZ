
#include "Config.hpp"
#include "Lanc.hpp"
#include "Camera.hpp"
#include "Visca.hpp"
#include "PanTilt.hpp"

#include <SPI.h>
#include <Ethernet.h>

Config config;
Lanc lanc(A7, A6);
Camera camera(lanc);
PanTilt panTilt;
Visca::ViscaProtocol visca(52381);

using namespace Visca;

void cameraStopCommand(VISCACameraCommand type)
{
    switch (type)
    {
    case VISCACameraCommand::Zoom:
        camera.zoomStop();
        break;
    case VISCACameraCommand::Focus:
        camera.focusStop();
        break;
    case VISCACameraCommand::Iris:
        camera.irisAuto();
        break;
    }
}

void cameraLessStandardCommand(VISCACameraCommand type)
{
    switch (type)
    {
    case VISCACameraCommand::Zoom:
        camera.zoomWide(3);
        break;
    case VISCACameraCommand::Focus:
        camera.focusFar();
        break;
    case VISCACameraCommand::Iris:
        camera.irisOpen();
        break;
    }
}

void cameraMoreStandardCommand(VISCACameraCommand type)
{
    switch (type)
    {
    case VISCACameraCommand::Zoom:
        camera.zoomTele(3);
        break;
    case VISCACameraCommand::Focus:
        camera.focusNear();
        break;
    case VISCACameraCommand::Iris:
        camera.irisClose();
        break;
    }
}

void cameraLessVariableCommand(VISCACameraCommand type, int speed)
{
    switch (type)
    {
    case VISCACameraCommand::Zoom:
        camera.zoomWide(speed);
        break;
    case VISCACameraCommand::Focus:
        camera.focusFar();
        break;
    }
}

void cameraMoreVariableCommand(VISCACameraCommand type, int speed)
{
    switch (type)
    {
    case VISCACameraCommand::Zoom:
        camera.zoomTele(speed);
        break;
    case VISCACameraCommand::Focus:
        camera.focusNear();
        break;
    }
}

void cameraDirectCommand(VISCACameraCommand type, int value)
{
    switch (type)
    {
    case VISCACameraCommand::Zoom:
        camera.zoomDirect(value);
        break;
    case VISCACameraCommand::Focus:
        camera.focusDirect(value);
        break;
    case VISCACameraCommand::Iris:
        camera.irisDirect(value);
        break;
    }
}

void executeCameraCommand(ViscaCommand command)
{
    switch (command.message[0])
    {
    case VISCACameraCommand::AutoFocus:
        if (command.message[1] == VISCABoolean::Toggle)
        {
            camera.toggleAutoFocus();
            return;
        }

        camera.autoFocus(command.message[1] == VISCABoolean::ON);
        return;
    case VISCACameraCommand::Power:
        if (command.message[1] == VISCABoolean::ON)
        {
            camera.on();
            return;
        }
        camera.off();
        return;
    }

    unsigned char cmdType = command.message[0] & 0x0F;
    unsigned char msb = command.message[0] & 0xF0;

    bool direct = msb == VISCACameraCommand::Direct;

    if (direct)
    {
        int directValue = (command.message[1] << 12) + (command.message[2] << 8) + (command.message[3] << 4) + command.message[4];
        cameraDirectCommand(cmdType, directValue);
        return;
    }

    unsigned char variableByte = command.message[1] & 0xF0;

    bool variableMore = variableByte == VISCACameraCommand::MoreVariable;
    bool variableLess = variableByte == VISCACameraCommand::LessVariable;

    if (variableMore || variableLess)
    {
        int speed = command.message[1] & 0x0F;

        if (variableMore)
        {
            cameraMoreVariableCommand(cmdType, speed);
            return;
        }

        cameraLessVariableCommand(cmdType, speed);
        return;
    }

    switch (command.message[1])
    {
    case VISCACameraCommand::LessStandard:
        cameraMoreStandardCommand(cmdType);
        break;
    case VISCACameraCommand::MoreStandard:
        cameraLessStandardCommand(cmdType);
        break;
    case VISCACameraCommand::Stop:
        cameraStopCommand(cmdType);
        break;
    }
}

void executeCameraInquiry(ViscaCommand command)
{
    unsigned char reply[] = {0x00, 0x00, 0x00, 0x00};
    int replyLength = 0;

    switch (command.message[0])
    {
    case VISCACameraInquiry::PowerInq:
        replyLength = 1;
        if (camera.isOn())
        {
            reply[0] = VISCABoolean::ON;
        }
        else
        {
            reply[0] = VISCABoolean::OFF;
        }
        break;
    case VISCACameraInquiry::FocusModeInq:
        replyLength = 1;
        if (camera.getAutoFocus())
        {
            reply[0] = VISCABoolean::ON;
        }
        else
        {
            reply[0] = VISCABoolean::OFF;
        }
        break;
    case VISCACameraInquiry::ZoomInq:
        replyLength = 4;
        int zoom = camera.getZoom();

        reply[0] = (zoom >> 12) & 0x0F;
        reply[1] = (zoom >> 8) & 0x0F;
        reply[2] = (zoom >> 4) & 0x0F;
        reply[3] = zoom & 0x0F;

        break;
    }

    if (replyLength)
    {
        visca.sendInquiryResult(reply, replyLength);
        return;
    }

    visca.sendError(VISCAError::SyntaxError);
}

void executeNormalPositionCommand(ViscaCommand command)
{
    bool panStop = command.message[3] == VISCAPositionCommand::PositionStop;
    bool tiltStop = command.message[4] == VISCAPositionCommand::PositionStop;

    if (panStop && tiltStop)
    {
        panTilt.stop();
        return;
    }

    if (!panStop)
    {
        panTilt.pan(command.message[3] - 2, command.message[1]);
    }

    if (!tiltStop)
    {
        panTilt.tilt(command.message[4] - 2, command.message[2]);
    }
}

void executeAbsRelPositionCommand(ViscaCommand command)
{
    int pan = (command.message[3] << 12) + (command.message[4] << 8) + (command.message[5] << 4) + command.message[6];
    int tilt = (command.message[7] << 12) + (command.message[8] << 8) + (command.message[9] << 4) + command.message[10];

    panTilt.position(command.message[1], command.message[2], pan, tilt, command.message[0] == VISCAPositionCommand::RelativePosition);
}

void executePositionCommand(ViscaCommand command)
{
    switch (command.message[0])
    {
    case VISCAPositionCommand::NormalPosition:
        executeNormalPositionCommand(command);
        break;
    case VISCAPositionCommand::AbsolutePosition:
    case VISCAPositionCommand::RelativePosition:
        executeAbsRelPositionCommand(command);
        break;
    case VISCAPositionCommand::Home:
    case VISCAPositionCommand::ResetPosition:
        panTilt.home();
        break;
    }
}

void executePositionInquiry(ViscaCommand command)
{
    Serial.println("Pos Inq");
    Serial.println(command.message[0], HEX);

    switch (command.message[0])
    {
    case VISCAPositionInquiry::PositionInq:
        int pan = panTilt.getPan();
        int tilt = panTilt.getTilt();

        unsigned char reply[] = {(pan >> 12) & 0x0F, (pan >> 8) & 0x0F, (pan >> 4) & 0x0F, pan & 0x0F, (tilt >> 12) & 0x0F, (tilt >> 8) & 0x0F, (tilt >> 4) & 0x0F, tilt & 0x0F};

        visca.sendInquiryResult(reply, 8);

        break;
    }
}

void executeCommand(ViscaCommand command)
{
    switch (command.type)
    {
    case VISCAType::Camera:
        executeCameraCommand(command);
        break;
    case VISCAType::Position:
        executePositionCommand(command);
        break;
    }
}

void executeInquiry(ViscaCommand command)
{
    switch (command.type)
    {
    case VISCAType::Position:
        executePositionInquiry(command);
        break;
    case VISCAType::Camera:
        executeCameraInquiry(command);
        break;
    }
}

void setup()
{
    Serial.begin(9600);

    byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    Ethernet.begin(mac);

    Serial.println(Ethernet.localIP());

    config.setup();
    camera.setup();
    visca.begin();
    panTilt.setup();
}

void loop()
{
    visca.loop();

    if (visca.hasCommand())
    {
        ViscaCommand command = visca.getCommand();

        if (command.inquiry)
        {
            executeInquiry(command);
        }
        else
        {
            executeCommand(command);
            visca.sendCompletion();
        }
    }

    panTilt.loop();
    camera.loop();

    config.loop();
}
