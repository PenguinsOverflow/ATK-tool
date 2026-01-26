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

bool sendARPResponse(const string& targetIP, const string& targetMac, const string& senderIP, const string& senderMac, const string& interface) {
    
    // Convert MAC addresses from string to byte array
    unsigned char srcMac[6], dstMac[6];
    if (sscanf(senderMac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &srcMac[0], &srcMac[1], &srcMac[2], 
               &srcMac[3], &srcMac[4], &srcMac[5]) != 6) {
        return false;
    }
    if (sscanf(targetMac.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &dstMac[0], &dstMac[1], &dstMac[2], 
               &dstMac[3], &dstMac[4], &dstMac[5]) != 6) {
        return false;
    }
    
    // open BPF device
    int bpf = -1;
    char bpf_dev[32];
    
    for (int i = 0; i < 99; i++) {
        snprintf(bpf_dev, sizeof(bpf_dev), "/dev/bpf%d", i);
        bpf = open(bpf_dev, O_RDWR);
        if (bpf != -1) break;
    }
    
    if (bpf < 0) {
        return false;
    }
    
    // link BPF to interface
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    if (ioctl(bpf, BIOCSETIF, &ifr) < 0) {
        close(bpf);
        return false;
    }
    
    // Enable immediate mode (no buffering)
    unsigned int enable = 1;
    ioctl(bpf, BIOCIMMEDIATE, &enable);
    
    // Craft ARP reply packet
    unsigned char buffer[42];
    memset(buffer, 0, sizeof(buffer));
    
    // Ethernet header
    memcpy(buffer, dstMac, 6);        // Destination MAC
    memcpy(buffer + 6, srcMac, 6);    // Source MAC
    buffer[12] = 0x08;                // EtherType: ARP
    buffer[13] = 0x06;
    
    // ARP header
    buffer[14] = 0x00; buffer[15] = 0x01;  // Hardware type: Ethernet
    buffer[16] = 0x08; buffer[17] = 0x00;  // Protocol type: IPv4
    buffer[18] = 0x06;                      // Hardware size
    buffer[19] = 0x04;                      // Protocol size
    buffer[20] = 0x00; buffer[21] = 0x02;  // Opcode: ARP Reply (0x0002)
    
    // Sender MAC
    memcpy(buffer + 22, srcMac, 6);
    
    // Sender IP
    struct in_addr senderAddr;
    inet_aton(senderIP.c_str(), &senderAddr);
    memcpy(buffer + 28, &senderAddr, 4);
    
    // Target MAC
    memcpy(buffer + 32, dstMac, 6);
    
    // Target IP
    struct in_addr targetAddr;
    inet_aton(targetIP.c_str(), &targetAddr);
    memcpy(buffer + 38, &targetAddr, 4);
    
    // Send the packet
    ssize_t sent = write(bpf, buffer, 42);
    
    close(bpf);
    
    return sent == 42;
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

bool interfaceExists(const string& interface) {
    struct ifaddrs *ifap, *ifa;
    bool found = false;
    
    // Get the list of network interfaces
    if (getifaddrs(&ifap) != 0) {
        return false; 
    }
    
    // Search interface
    for (ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, interface.c_str()) == 0) {
            found = true;
            break;
        }
    }
    
    freeifaddrs(ifap);
    return found;
}
