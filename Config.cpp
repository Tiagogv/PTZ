
#include "Config.hpp"

#include <SPI.h>
#include <Ethernet.h>
#include <aWOT.h>
#include "StaticFiles.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

EthernetServer server(80);
Application app;

IPAddress _ip;
byte *_mac;
int _vmixInput;
bool _dhcp;

IPAddress readIP()
{
    return IPAddress(EEPROM.read(0), EEPROM.read(1), EEPROM.read(2), EEPROM.read(3));
}

void writeIP(IPAddress ip)
{
    for (int i = 0; i < 4; i++)
    {
        EEPROM.update(i, ip[i]);
    }
}

byte *readMAC()
{
    byte mac[6];

    for (int i = 0; i < 6; i++)
    {
        mac[i] = EEPROM.read(i + 4);
    }

    return mac;
}

void writeMAC(byte *mac)
{
    for (int i = 0; i < 6; i++)
    {
        EEPROM.update(i + 4, mac[i]);
    }
}

int readVmixInput()
{
    return EEPROM.read(11);
}

void writeVmixInput(int input)
{
    EEPROM.update(11, input);
}

bool readDHCP()
{
    return EEPROM.read(10) == 1;
}

void writeDHCP(bool dchp)
{
    EEPROM.update(10, dchp);
}

void setIP(char *ip)
{
    String ipStr = String(ip);
    ipStr.trim();

    char ipNumber[3];
    int *ipNumbers[4];
    int idx = 0;
    int dotIdx = 0;

    for (int i = 0; i < 4; i++)
    {
        dotIdx = ipStr.indexOf(".", dotIdx + 1);

        if (i < 3)
        {
            ipNumbers[i] = ipStr.substring(idx, dotIdx - 1).toInt();
        }
        else
        {
            ipNumbers[i] = ipStr.substring(idx).toInt();
        }

        idx = dotIdx + 1;
    }

    _ip = IPAddress(ipNumbers[0], ipNumbers[1], ipNumbers[2], ipNumbers[3]);

    writeIP(_ip);
}

void setMAC(char *mac)
{

    String macStr = String(mac);

    macStr.toUpperCase();

    byte macArray[6];
    int byteIdx = 0;
    int byteShift = 4;
    int value = 0;
    int currentByte = 0;
    char ch;

    for (int i = 0; i < macStr.length(); i++)
    {
        ch = macStr.charAt(i);

        if (ch == ':')
        {
            byteIdx++;
            byteShift = 4;
            continue;
        }

        if (ch >= '0' && ch <= '9')
        {
            value = ch - '0';
        }
        else
        {
            value = ch - 'A' + 10;
        }

        currentByte += (value << byteShift);
        macArray[byteIdx] = currentByte;
        byteShift -= 4;
    }

    _mac = macArray;
    writeMAC(macArray);
}

void setDHCP(char *value)
{
    if (value == "false")
    {
        _dhcp = false;
    }
    else
    {
        _dhcp = true;
    }

    writeDHCP(_dhcp);
}

void setVmixInput(char *value)
{
    String input = String(value);

    _vmixInput = input.toInt();

    writeVmixInput(_vmixInput);
}

void getConfig(Request &req, Response &res)
{
    const size_t capacity = JSON_OBJECT_SIZE(4) + 56;
    StaticJsonDocument<capacity> doc;

    char ipString[15];
    sprintf(ipString, "%d.%d.%d.%d", _ip[0], _ip[1], _ip[2], _ip[3]);
    char macString[17];
    sprintf(macString, "%02X:%02X:%02X:%02X:%02X:%02X", _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);

    doc["ip"] = ipString;
    doc["mac"] = macString;
    doc["dhcp"] = _dhcp;
    doc["vmixInput"] = _vmixInput;

    int length = measureJson(doc);

    char lengthStr[4];
    sprintf(lengthStr, "%d", length);

    res.set("Content-Type", "application/json");
    res.set("Content-Length", lengthStr);
    char output[200];
    serializeJson(doc, output);
    res.write(output, length);
}

void postConfig(Request &req, Response &res)
{
    char name[15];
    char value[100];

    while (req.form(name, 15, value, 100))
    {
        if (name == "ip")
        {
            setIP(value);
        }

        if (name == "mac")
        {
            setMAC(value);
        }

        if (name == "dhcp")
        {
            setDHCP(value);
        }

        if (name == "vmixInput")
        {
            setVmixInput(value);
        }
    }

    res.set("Content-Length", "0");
    res.set("Location", "/");
    res.status(303);
}
void Config::loop()
{
    EthernetClient client = server.available();
    if (client.connected())
    {
        app.process(&client);
    }
}

void Config::setup()
{
    app.use(staticFiles());

    app.get("/config", &getConfig);
    app.post("/config", &postConfig);

    server.begin();
}

void Config::load()
{
    _ip = readIP();
    _mac = readMAC();
    _vmixInput = readVmixInput();
    _dhcp = readDHCP();
}

IPAddress Config::getIP()
{
    return _ip;
}

byte *Config::getMAC()
{
    return _mac;
}

bool Config::getDHCP()
{
    return _dhcp;
}

int Config::getVmixInput()
{
    return _vmixInput;
}

Config::Config()
{
    load();
}

Config::~Config()
{
}
