// NLM Simulate Command - Run specific simulation scenarios
// Handles running predefined simulation scenarios

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>

namespace nlm {

class SimulateCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool runScenario(NLMCLI& cli, const std::string& scenario, const std::map<std::string, std::string>& args);
    static bool runBasicScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runPlasticityScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runStdpScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runLearningScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runConnectivityScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runExperimentScenario(std::shared_ptr<Brain> brain, const std::string& scenario, const std::map<std::string, std::string>& args);
    static std::string getAvailableScenarios();
};
