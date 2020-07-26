#ifndef TALLY_H
#define TALLY_H

#include <Ethernet.h>

class Tally
{
private:
    EthernetClient client;
    int _ledPin;
    int _input;
    IPAddress _ip;
    char _buffer[101];
    int _idx = 0;

    void connect();
    void process();
    void on();
    void off();

public:
    Tally(int ledPin): _ledPin(ledPin){}
    ~Tally();

    void setup(IPAddress ip, int input);
    void loop();
};

#endif // TALLY_H
