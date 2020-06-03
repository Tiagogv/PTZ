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

    unsigned long lastSequence = _sequence;

    bool valid = parseCommand(packetSize);

    fillReplySequence();

    if (_payloadType == PayloadType::ControlCommand)
    {
        if (_commandType == ControlCommand::Reset)
        {
            _sequence = 0;
            sendControlAck();
        }
        return;
    }

    if (_sequence <= lastSequence)
    {
        _sequence = lastSequence;
        sendControlError();
        return;
    }

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
    _replyBuffer[0] = 0x01;
    _replyBuffer[1] = 0x11;
    fillReplyPayloadLength(size + 3);

    _replyBuffer[8] = 0x90;
    _replyBuffer[9] = 0x50;

    for (int i = 0; i < size; i++)
    {
        _replyBuffer[i + 10] = value[i];
    }

    _replyBuffer[size + 10] = 0xFF;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, size + 11);
    Udp.endPacket();
}

void ViscaProtocol::sendError(VISCAError error)
{
    _replyBuffer[0] = 0x01;
    _replyBuffer[1] = 0x11;
    fillReplyPayloadLength(4);

    _replyBuffer[8] = 0x90;
    _replyBuffer[9] = 0x60;
    _replyBuffer[10] = error;
    _replyBuffer[11] = 0xFF;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, 12);
    Udp.endPacket();
}

void ViscaProtocol::sendControlError()
{
    _replyBuffer[0] = 0x02;
    _replyBuffer[1] = 0x00;

    fillReplyPayloadLength(2);

    _replyBuffer[8] = 0x0F;
    _replyBuffer[9] = 0x01;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, 10);
    Udp.endPacket();
}

void ViscaProtocol::sendAck()
{
    _replyBuffer[0] = 0x01;
    _replyBuffer[1] = 0x11;
    fillReplyPayloadLength(3);

    _replyBuffer[8] = 0x90;
    _replyBuffer[9] = 0x41;
    _replyBuffer[10] = 0xFF;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, 11);
    Udp.endPacket();
}

void ViscaProtocol::sendControlAck()
{
    _replyBuffer[0] = 0x02;
    _replyBuffer[1] = 0x01;
    fillReplyPayloadLength(1);

    _replyBuffer[8] = 0x01;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, 9);
    Udp.endPacket();
}

void ViscaProtocol::sendCompletion()
{
    _replyBuffer[0] = 0x01;
    _replyBuffer[1] = 0x11;
    fillReplyPayloadLength(3);

    _replyBuffer[8] = 0x90;
    _replyBuffer[9] = 0x51;
    _replyBuffer[10] = 0xFF;

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(_replyBuffer, 11);
    Udp.endPacket();
}

void ViscaProtocol::fillReplySequence()
{
    unsigned long sequence = _sequence + 1;

    _replyBuffer[4] = sequence >> 24 & 0xFF;
    _replyBuffer[5] = sequence >> 16 & 0xFF;
    _replyBuffer[6] = sequence >> 8 & 0xFF;
    _replyBuffer[7] = sequence & 0xFF;
}

void ViscaProtocol::fillReplyPayloadLength(unsigned int length)
{
    _replyBuffer[2] = (length >> 8) & 0xFF;
    _replyBuffer[3] = length & 0xFF;
}

bool ViscaProtocol::parseCommand(int size)
{
    if (size < 9 || size > 24)
    {
        return false;
    }

    _payloadLength = (_packetBuffer[2] << 8) + _packetBuffer[3];

    if (_payloadLength > 16 || _payloadLength < 1)
    {
        return false;
    }

    _sequence = (_packetBuffer[4] << 24) + (_packetBuffer[5] << 16) + (_packetBuffer[6] << 8) + _packetBuffer[7];

    _payloadType = (_packetBuffer[0] << 8) + _packetBuffer[1];

    switch (_payloadType)
    {
    case PayloadType::VISCACommand:
        return parseVISCA(VISCAType::Command);
    case PayloadType::VISCAInquiry:
        return parseVISCA(VISCAType::Inquiry);
    case PayloadType::ControlCommand:
        return parseControlCommand();
    default:
        return false;
    }
}

bool ViscaProtocol::parseVISCA(VISCAType type)
{
    if (_payloadLength < 3 || _payloadLength > 16)
    {
        return false;
    }

    if (_packetBuffer[8] != 0x81 || _packetBuffer[_payloadLength + 7] != 0xFF)
    {
        return false;
    }

    if (_packetBuffer[9] != type)
    {
        return false;
    }

    for (unsigned int i = 0; i < _payloadLength - 4; i++)
    {
        _commandBuffer[i] = _packetBuffer[i + 11];
    }

    _commandType = _packetBuffer[10];

    return true;
}

bool ViscaProtocol::parseControlCommand()
{

    if (_payloadLength > 2)
    {
        return false;
    }

    unsigned int cmd = _packetBuffer[8];

    if (_payloadLength == 2)
    {
        cmd = (cmd << 8) + _packetBuffer[9];
    }

    if (cmd != ControlCommand::Reset && cmd != ControlCommand::ErrorMessage && cmd != ControlCommand::ErrorSequence)
    {
        return false;
    }

    _commandType = cmd;
    return true;
}

ViscaCommand ViscaProtocol::getCommand()
{
    _hasCommand = false;

    ViscaCommand command;

    command.inquiry = _payloadType == PayloadType::VISCAInquiry;

    for (int i = 0; i < _payloadLength - 4; i++)
    {
        command.message[i] = _commandBuffer[i];
    }

    command.messageLength = _payloadLength - 4;
    command.type = VISCAType(_commandType & 0xFF);

    return command;
}

ViscaProtocol::~ViscaProtocol()
{
}
