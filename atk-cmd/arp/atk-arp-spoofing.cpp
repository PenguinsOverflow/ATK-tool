#include "../../utils/inc/ip.h"
#include <CLI/CLI.hpp>
#include <cstdio>

using namespace std;

// **********************
void startArpSpoofing(const string target_ip, const string gateway_ip, bool verbose);
// **********************

int main(int argc, char* argv[]) {
    CLI::App app{"ARP spoofing command", "atk arp spoofing"};

    // ******** Flag variables ********
    bool verbose = false;
    string target_ip;
    string gateway_ip;
    // ********************************


    app.add_flag("-v,--verbose", verbose, "Verbose output");
    app.add_option("-t,--target", target_ip, "Target IPv4 address")->required();
    app.add_option("-g,--gateway", gateway_ip, "Gateway IPv4 address")->required();


    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    if (!checkValidIpv4(target_ip)) {
        printf("Invalid target IP address: %s\n", target_ip.c_str());
        return 1;
    }
    if (!checkValidIpv4(gateway_ip)) {
        printf("Invalid gateway IP address: %s\n", gateway_ip.c_str());
        return 1;
    }

    printf("%s\n", "Starting ARP spoofing...");
    printf("Target IP: %s\n", target_ip.c_str());
    printf("Gateway IP: %s\n", gateway_ip.c_str());
    if (verbose) {
        printf("%s\n", "Verbose mode enabled.");
    }

    printf("%s\n", getMacFromIP("192.168.1.254").c_str());

    // startArpSpoofing(target_ip, gateway_ip, verbose);

    return 0;
}

// void startArpSpoofing(const string target_ip, const string gateway_ip, bool verbose) {

// }

