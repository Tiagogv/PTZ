#ifndef LANC
#define LANC

class Lanc
{
private:
    int _input;
    int _output;
    bool _connected = false;

    void writeByte(unsigned char byte);
    unsigned char readByte();

public:
    Lanc(int inputPin, int outputPin) : _input(inputPin), _output(outputPin){};
    void setup();
    void sendCommand(unsigned char mode, unsigned char cmd);
    void on();
    ~Lanc();
};

#endif // LANC
