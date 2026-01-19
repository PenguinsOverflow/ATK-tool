#include <CLI/CLI.hpp>
#include <cstdio>
#include <regex>

using namespace std;

bool checkValidIpv4(const string ip);

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

    // startArpSpoofing(target_ip, gateway_ip, verbose);

    return 0;
}

bool checkValidIpv4(const string ip) {
    static const regex ipv4(
        R"(^(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}$)"
    );
    return regex_match(ip, ipv4);
}