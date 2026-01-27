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

bool checkValidMac(const string& mac) {
    static const regex mac_regex("^([0-9A-Fa-f]{1,2}[:-]){5}([0-9A-Fa-f]{1,2})$");
    return regex_match(mac, mac_regex);
}

unsigned char* macStringToBytes(const string& macStr) {
    unsigned char* mac = new unsigned char[6];
    sscanf(macStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &mac[0], &mac[1], &mac[2],
           &mac[3], &mac[4], &mac[5]);
    return mac;
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

bool sendPacket(const string& interface, const unsigned char* packet, size_t length) {
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
    
    ssize_t sent = write(bpf, packet, length);
    
    close(bpf);
    
    return sent == 42;
}

bool sendARPResponse(const string& targetIP, const string& targetMac, const string& senderIP, const string& senderMac, const string& interface) {
    
    if (!checkValidMac(senderMac) || !checkValidMac(targetMac)) {
        return false;
    }
    unsigned char* srcMac = macStringToBytes(senderMac);
    unsigned char* dstMac = macStringToBytes(targetMac);
    
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
    return sendPacket(interface, buffer, sizeof(buffer));
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

bool enableIPForwarding() {
    int result = system("sysctl -w net.inet.ip.forwarding=1 > /dev/null 2>&1");
    return result == 0;
}

bool disableIPForwarding() {
    int result = system("sysctl -w net.inet.ip.forwarding=0 > /dev/null 2>&1");
    return result == 0;
}