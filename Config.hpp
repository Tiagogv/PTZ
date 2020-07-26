#ifndef CONFIG_H
#define CONFIG_H

#include <Ethernet.h>

class Config
{
private:
public:
    Config(/* args */);

    void loop();
    void setup();

    bool getDHCP();
    IPAddress getIP();

    ~Config();
};

#endif // CONFIG_H
