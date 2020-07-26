#ifndef VISCA
#define VISCA

#include <SPI.h>
#include <EthernetUdp.h>

namespace Visca
{

    enum PayloadType
    {
        VISCACommand = 0x01,
        VISCAInquiry = 0x09,
        VISCAReply = 0x50
    };

    enum VISCAType
    {
        Command = 0x01,
        Inquiry = 0x09,
        Camera = 0x04,
        Position = 0x06
    };

    enum VISCAError
    {
        SyntaxError = 0x01,
        BufferFullError = 0x03,
        MessageLengthError = 0x02
    };

    enum ControlCommand
    {
        Reset = 0x01,
        ErrorSequence = 0x0F01,
        ErrorMessage = 0x0F02
    };

    enum VISCAPositionCommand
    {
        NormalPosition = 0x01,
        AbsolutePosition = 0x02,
        RelativePosition = 0x03,
        Home = 0x04,
        ResetPosition = 0x05,
        Limit = 0x07,
        LimitSet = 0x00,
        LimitClear = 0x01,
        PositionLess = 0x02,
        PositionMore = 0x01,
        PositionStop = 0x03,
        SlowMode = 0x44
    };

    enum VISCAPositionInquiry
    {
        PositionInq = 0x12,
        SlowModeInq = 0x44,
        MaxSpeed = 0x11
    };

    enum VISCACameraCommand
    {
        Power = 0x00,
        Zoom = 0x07,
        Focus = 0x08,
        Iris = 0x0B,
        Direct = 0x40,
        Stop = 0x00,
        MoreStandard = 0x02,
        LessStandard = 0x03,
        MoreVariable = 0x20,
        LessVariable = 0x30,
        AutoFocus = 0x38
    };

    enum VISCACameraInquiry
    {
        PowerInq = 0x00,
        ZoomInq = 0x47,
        FocusInq = 0x48,
        FocusModeInq = 0x38,
        IrisInq = 0x4B
    };

    enum VISCABoolean
    {
        ON = 0x02,
        OFF = 0x03,
        Toggle = 0x10
    };

    struct ViscaCommand
    {
        bool inquiry;
        unsigned char message[12];
        int messageLength;
        VISCAType type;
    };

    class ViscaProtocol
    {
    public:
        ViscaProtocol(unsigned int udpPort = 52381) : _udpPort(udpPort){};
        ~ViscaProtocol();

        void begin();

        void loop();

        bool hasCommand() { return _hasCommand; }
        ViscaCommand getCommand();

        void sendInquiryResult(unsigned char *value, int size);
        void sendError(VISCAError error);
        void sendCompletion();

    private:
        unsigned int _udpPort;
        unsigned char _packetBuffer[UDP_TX_PACKET_MAX_SIZE];
        unsigned char _commandBuffer[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        bool parseCommand(unsigned int size);
        bool parseVISCA(VISCAType type);

        EthernetUDP Udp;
        unsigned int _payloadType = 0;
        unsigned int _payloadLength = 0;
        unsigned int _commandType = 0;
        bool _hasCommand = false;

        unsigned char _replyBuffer[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        void sendAck();
    };

} // namespace Visca

#endif //VISCA
