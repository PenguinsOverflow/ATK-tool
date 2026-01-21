#include "../inc/ip.h"
#include "../inc/stringutils.h"
#include <regex>
#include <array>

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