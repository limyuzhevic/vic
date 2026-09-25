// NLM Run Command - Execute brain simulations
// Handles running various types of brain simulations

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>

namespace nlm {

class RunCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool runBasicSimulation(std::shared_ptr<Brain> brain, size_t steps);
    static bool runPlasticitySimulation(std::shared_ptr<Brain> brain, size_t steps);
    static bool runStdpSimulation(std::shared_ptr<Brain> brain, size_t steps);
    static bool runLearningSimulation(std::shared_ptr<Brain> brain, size_t iterations);
    static bool runConnectivitySimulation(std::shared_ptr<Brain> brain, size_t steps);
    static bool runExperimentScenario(std::shared_ptr<Brain> brain, const std::string& scenario, size_t steps);
};
