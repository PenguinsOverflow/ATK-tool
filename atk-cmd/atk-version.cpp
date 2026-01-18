#include "atk-version.h"
#include "../globals.h"

int main() {
    printf("ATK Version: %s\n", getVersion().c_str());
    return 0;
}

string getVersion() {
    return VERSION;
}