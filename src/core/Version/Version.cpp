#include "core/Version/Version.h"
#include <string>

namespace nlm {

// Version information
const char* getVersion() {
    return "0.1.0";
}

const char* getBuildDate() {
    return "2026-10-04";
}

const char* getGitCommit() {
    return "unknown";
}

int getMajorVersion() {
    return 0;
}

int getMinorVersion() {
    return 1;
}

int getPatchVersion() {
    return 0;
}

} // namespace nlm