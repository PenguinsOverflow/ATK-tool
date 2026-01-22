#include "../inc/ip.h"
#include "../inc/stringutils.h"
#include <regex>
#include <array>
#include <cstring>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/bpf.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <net/ethernet.h>
#include <sstream>
#include <iomanip>


bool checkValidIpv4(const string& ip) {
    static const regex ipv4(
        R"(^(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}$)"
    );
    return regex_match(ip, ipv4);
}

string getMacFromIP(const string& ip) {
    string mac = "00:00:00:00:00:00";
    string command = "arp " + ip + " | grep -Eo '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'";

    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        return mac;
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (!result.empty()) {
        mac = result;
        trim(mac);
    }

    return mac;
}

string getMacFromInterface(const string& interface) {
    struct ifaddrs *ifap, *ifa;
    string macAddress = "00:00:00:00:00:00";
    
    if (getifaddrs(&ifap) != 0) {
        return "00:00:00:00:00:00";
    }
    
    for (ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {

        // check is it a link layer address
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_LINK) {
            if (strcmp(ifa->ifa_name, interface.c_str()) == 0) {
                struct sockaddr_dl* sdl = (struct sockaddr_dl*)ifa->ifa_addr;
                unsigned char* mac = (unsigned char*)LLADDR(sdl);
                
                stringstream ss;
                ss << hex << setfill('0'); // Format as hexadecimal with leading zeros
                for (int i = 0; i < 6; i++) {
                    ss << setw(2) << (int)mac[i]; // Convert byte to hex with width 2
                    if (i < 5) ss << ":";
                }
                
                macAddress = ss.str();
                break;
            }
        }
    }
    
    freeifaddrs(ifap); // Free memory allocated by getifaddrs
    return macAddress;
}