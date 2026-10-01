// CLIAdvanced.hpp
// Advanced command-line argument parsing and validation for NLM

#ifndef NLM_CLIADVANCED_HPP
#define NLM_CLIADVANCED_HPP

#include <string>
#include <vector>

namespace nlm {

struct CLIAdvancedOptions {
    std::string mode = "phase6";
    std::string configFile = "configs/default.cfg";
    size_t steps = 1000;
    size_t neurons = 1000;
    std::string outputFile;
    bool verbose = false;
    std::string logLevel = "info";
    bool checkpointEnabled = true;
    bool replayEnabled = true;
    bool developmentEnabled = true;
    bool curiosityEnabled = true;
    bool rewardModulationEnabled = true;
    bool structuralPlasticityEnabled = true;
    bool help = false;
    bool showVersion = false;
};

class CLIAdvanced {
public:
    static CLIAdvancedOptions parse(int argc, char** argv);
    static void printHelp();
    static void printVersion();
    
private:
    static void parseOption(CLIAdvancedOptions& options, const std::string& key, const std::string& value);
    static void parseFlagOption(CLIAdvancedOptions& options, const std::string& key);
    static void parseShortOption(CLIAdvancedOptions& options, char shortOpt, const char* value);
    static void validateAndSetDefaults(CLIAdvancedOptions& options);
    
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
};

} // namespace nlm

#endif // NLM_CLIADVANCED_HPP