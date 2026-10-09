#pragma once

#include <string>

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface();
    void run(int argc, char** argv);
};

void runCommandLineInterface(int argc, char** argv);

} // namespace nlm