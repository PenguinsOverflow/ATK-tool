#include "atk-help.h"
#include "../utils/inc/globals.h"

int main() {
    printf("%s", getHelp().c_str());
    return 0;
}

string getHelp() {
    string help = "ATK - Attack toolkit\n\n";
    help += "Usage:\n";
    help += "  atk [command] [options]\n\n";

    help += "\nOptions:\n";
    help += "  -h, --help       Show this help message\n";
    help += "  -v, --version    Show version information\n";
    
    help += "Available Commands:\n";
    vector<string> commands = getAvailableCommands(COMMANDS_DIR);
    for (string cmd : commands) {
        help += "  " + cmd + "\n";
    }

    return help;
}