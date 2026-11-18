#include "DemoFramework.hpp"
#include <algorithm>
#include <cstring>

namespace nlm {

DemoFramework::Arguments DemoFramework::parseArguments(int argc, char* argv[]) {
    Arguments args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg == "--version") {
            args.showVersion = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--profile") {
            args.profiling = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else {
            args.extraArgs.push_back(arg);
        }
    }
    
    return args;
}

} // namespace nlm