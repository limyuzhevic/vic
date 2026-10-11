#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include "AdvancedCLI.hpp"
#include "CommandOutputFormatter.hpp"

namespace nlm {

class BrainCommands {
public:
    BrainCommands(std::shared_ptr<CommandOutputFormatter> formatter,
                  std::shared_ptr<Logger> logger);
    
    // Brain Configuration Management
    bool brainConfig(const std::string& configFile);
    bool brainSave(const std::string& saveFile);
    bool brainLoad(const std::string& loadFile);
    bool brainInfo();
    bool brainReset();
    bool brainDump(const std::string& dumpFile);
    bool brainDiff(const std::string& file1, const std::string& file2);
    bool brainStatus();
    bool brainValidate();
    
    // File format helpers
    static bool saveBrainToFile(const Brain* brain, const std::string& filename);
    static bool loadBrainFromFile(Brain* brain, const std::string& filename);
    static bool compareBrainFiles(const std::string& file1, const std::string& file2);
    
private:
    std::shared_ptr<CommandOutputFormatter> formatter;
    std::shared_ptr<Logger> logger;
    
    std::string formatBrainState(const Brain* brain);
    bool parseBrainConfig(const std::string& configFile, std::shared_ptr<Config> config);
};
