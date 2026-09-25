// NLM Config Command - Manage configuration
// Handles configuration management operations

#pragma once

#include "cli/nlm_cli.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

class ConfigCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool listConfig(const std::map<std::string, std::string>& options);
    static bool setConfig(const std::map<std::string, std::string>& args);
    static bool saveConfig(const std::map<std::string, std::string>& args);
    static bool loadConfig(const std::map<std::string, std::string>& args);
    static bool validateConfig(const std::map<std::string, std::string>& options);
    static void printConfig(const std::map<std::string, std::string>& config, bool showDefaults = false);
    static std::string getConfigTypeString(ConfigType type);
    static bool parseConfigLine(const std::string& line, std::string& key, std::string& value);
};
