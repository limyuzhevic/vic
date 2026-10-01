// CLIParser.hpp
// Command-line argument parser for NLM

#ifndef NLM_CLIPARSER_HPP
#define NLM_CLIPARSER_HPP

#include <string>
#include <vector>

namespace nlm {

class CLIParser {
public:
    struct Options {
        std::string mode = "phase6";
        std::string configFile = "configs/default.cfg";
        size_t steps = 1000;
        size_t neurons = 1000;
        std::string outputFile;
        bool verbose = false;
        bool help = false;
    };
    
    static Options parse(int argc, char** argv);
    static void printHelp();
};

} // namespace nlm

#endif // NLM_CLIPARSER_HPP