#pragma once

#include <string>

using namespace std;

bool checkValidIpv4(const string& ip);
string getMacFromIP(const string& ip);
bool sendARPResponse(const string& targetIP, const string& targetMac,
                     const string& senderIP, const string& senderMac,
                     const string& interface);
string getMacFromInterface(const string& interface);
bool interfaceExists(const string& interface);