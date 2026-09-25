#ifndef NLM_ADVANCED_COMMANDS_HPP
#define NLM_ADVANCED_COMMANDS_HPP

#include <string>
#include <vector>
#include <iostream>

namespace nlm {

// Command-line argument parsing for advanced features
struct AdvancedCommandArgs {
    std::string batchFile;
    std::string exportModel;
    std::string importModel;
    std::string benchmarkMode;
    std::string compareConfigs;
    double profileDuration;
    int debugLevel;
    bool monitor;
    bool help;
    bool version;
    std::vector<std::string> customArgs;
};

class AdvancedCommands {
public:
    static AdvancedCommandArgs parseAdvancedArgs(int argc, char** argv);
    static void printAdvancedHelp();
    static void printVersionInfo();
    static bool handleAdvancedCommands(const AdvancedCommandArgs& args);
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

#endif // NLM_ADVANCED_COMMANDS_HPP