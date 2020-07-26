#include "Visca.hpp"

using namespace Visca;

void ViscaProtocol::begin()
{
    Udp.begin(_udpPort);
}

void ViscaProtocol::loop()
{
    int packetSize = Udp.parsePacket();

    if (!packetSize)
    {
        return;
    }

    Udp.read(_packetBuffer, UDP_TX_PACKET_MAX_SIZE);

    bool valid = parseCommand(packetSize);

    if (!valid)
    {
        sendError(VISCAError::SyntaxError);
        return;
    }

    if ((_payloadType == PayloadType::VISCACommand || _payloadType == PayloadType::VISCAInquiry) && _hasCommand)
    {
        sendError(VISCAError::BufferFullError);
        return;
    }

    if (_payloadType == PayloadType::VISCACommand)
    {
        sendAck();
    }

    _hasCommand = true;
}

void ViscaProtocol::sendInquiryResult(unsigned char *value, int size)
{
    _replyBuffer[0] = 0x90;
    _replyBuffer[1] = 0x50;

    for (int i = 0; i < size; i++)
    {
        _replyBuffer[i + 2] = value[i];
    }

    _replyBuffer[size + 2] = 0xFF;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, size + 3);
    Udp.endPacket();
}

void ViscaProtocol::sendError(VISCAError error)
{
    _replyBuffer[0] = 0x90;
    _replyBuffer[1] = 0x60;
    _replyBuffer[2] = error;
    _replyBuffer[3] = 0xFF;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, 4);
    Udp.endPacket();
}

void ViscaProtocol::sendAck()
{
    _replyBuffer[0] = 0x90;
    _replyBuffer[1] = 0x41;
    _replyBuffer[2] = 0xFF;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, 3);
    Udp.endPacket();
}

void ViscaProtocol::sendCompletion()
{
    _replyBuffer[0] = 0x90;
    _replyBuffer[1] = 0x51;
    _replyBuffer[2] = 0xFF;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, 3);
    Udp.endPacket();
}

bool ViscaProtocol::parseCommand(unsigned int size)
{
    if (size < 5 || size > 16)
    {
        return false;
    }

    if (_packetBuffer[size - 1] != 0xFF)
    {
        return false;
    }

    _payloadLength = size - 3;
    _payloadType = _packetBuffer[1];

    switch (_payloadType)
    {
    case PayloadType::VISCACommand:
        return parseVISCA(VISCAType::Command);
    case PayloadType::VISCAInquiry:
        return parseVISCA(VISCAType::Inquiry);
    default:
        return false;
    }
}

bool ViscaProtocol::parseVISCA(VISCAType type)
{
    if (_payloadLength < 2 || _payloadLength > 13)
    {
        return false;
    }

    if (_packetBuffer[0] != 0x81)
    {
        return false;
    }

    for (unsigned int i = 0; i < _payloadLength; i++)
    {
        _commandBuffer[i] = _packetBuffer[i + 2];
    }

    _commandType = _packetBuffer[2];

    return true;
}


ViscaCommand ViscaProtocol::getCommand()
{
    _hasCommand = false;

    ViscaCommand command;

    command.inquiry = _payloadType == PayloadType::VISCAInquiry;

    for (int i = 0; i < _payloadLength-1; i++)
    {
        command.message[i] = _commandBuffer[i+1];
    }

    command.messageLength = _payloadLength-1;
    command.type = VISCAType(_commandType & 0xFF);

    return command;
}

ViscaProtocol::~ViscaProtocol()
{
}
