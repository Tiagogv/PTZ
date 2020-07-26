
#include "Config.hpp"

#include <Ethernet.h>
#include <aWOT.h>
#include "StaticFiles.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

EthernetServer server(80);
Application app;

IPAddress _ip(EEPROM.read(0), EEPROM.read(1), EEPROM.read(2), EEPROM.read(3));
const size_t capacity = JSON_OBJECT_SIZE(2) + 24;

void writeIP()
{
    for (int i = 0; i < 4; i++)
    {
        EEPROM.update(i, _ip[i]);
    }
}

bool readDHCP()
{
    return EEPROM.read(5) == 1;
}

void writeDHCP(bool dchp)
{
    EEPROM.update(5, dchp);
}

bool _dhcp = readDHCP();

void _setIP(char *ip, IPAddress *ipArr)
{
    String ipStr = String(ip);
    ipStr.trim();

    int idx = 0;
    int dotIdx = 0;

    for (int i = 0; i < 4; i++)
    {
        dotIdx = ipStr.indexOf(".", idx);

        if (i < 3)
        {
            (*ipArr)[i] = ipStr.substring(idx, dotIdx).toInt();
        }
        else
        {
            (*ipArr)[i] = ipStr.substring(idx).toInt();
        }

        idx = dotIdx + 1;
    }
}

void setIP(char *ip)
{
    _setIP(ip, &_ip);
    writeIP();
}

void setDHCP(bool value)
{
    _dhcp = value;
    writeDHCP(_dhcp);
}

void getConfig(Request &req, Response &res)
{
    StaticJsonDocument<capacity> doc;
    String ipString = String(_ip[0]) + "." + _ip[1] + "." + _ip[2] + "." + _ip[3];

    doc["ip"] = ipString;
    doc["dhcp"] = _dhcp;

    int length = measureJson(doc);

    char lengthStr[4];
    sprintf(lengthStr, "%d", length);

    res.set("Content-Type", "application/json");
    res.set("Content-Length", lengthStr);
    res.set("Cache-Control", "no-cache");
    char output[200];
    serializeJson(doc, output);
    res.write(output, length);
}

void postConfig(Request &req, Response &res)
{
    StaticJsonDocument<capacity> doc;
    String body = req.readString();

    deserializeJson(doc, body);

    setDHCP(doc["dhcp"]);
    setIP(doc["ip"]);

    res.write("OK");
}

void Config::loop()
{
    EthernetClient client = server.available();
    if (client.connected())
    {
        app.process(&client);
        client.stop();    
    }
}

void Config::setup()
{
    app.use(staticFiles());

    app.post("/config", &postConfig);
    app.get("/config", &getConfig);
    
    server.begin();
}

IPAddress Config::getIP()
{
    return _ip;
}

bool Config::getDHCP()
{
    return _dhcp;
}

Config::Config()
{
}

Config::~Config()
{
}
