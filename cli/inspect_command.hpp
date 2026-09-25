// NLM Inspect Command - Inspect brain state and configuration
// Handles inspecting brain state, configuration, and statistics

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>

namespace nlm {

class InspectCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool inspectBrainState(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options);
    static bool inspectConfiguration(std::shared_ptr<Config> config, const std::map<std::string, std::string>& options);
    static bool inspectStatistics(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options);
    static bool exportBrainData(const std::string& format, const std::string& filename, std::shared_ptr<Brain> brain);
    static void printBrainStats(std::shared_ptr<Brain> brain);
    static void printConfigurationStats(std::shared_ptr<Config> config);
    static std::string formatNumber(double value, int precision = 2);
};
