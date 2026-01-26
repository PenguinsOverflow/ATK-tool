#pragma once

#include <string>

using namespace std;

bool checkValidIpv4(const string& ip);
bool checkValidMac(const string& mac);
unsigned char* macStringToBytes(const string& macStr);
string getMacFromIP(const string& ip);
bool sendPacket(const string& interface, const unsigned char* packet, size_t length);
bool sendARPResponse(const string& targetIP, const string& targetMac,
                     const string& senderIP, const string& senderMac,
                     const string& interface);
string getMacFromInterface(const string& interface);
bool interfaceExists(const string& interface);