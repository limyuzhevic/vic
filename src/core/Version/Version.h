#pragma once

namespace nlm {

// Version information
const char* getVersion();
const char* getBuildDate();
const char* getGitCommit();
int getMajorVersion();
int getMinorVersion();
int getPatchVersion();

} // namespace nlm