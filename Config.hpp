#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Ethernet.h>

#include <aWOT.h>

class Config
{
private:
    void load();

public:
    Config(/* args */);

    void loop();
    void setup();

    bool getDHCP();
    IPAddress getIP();
    byte *getMAC();
    int getVmixInput();

    ~Config();
};

#endif // CONFIG_H
