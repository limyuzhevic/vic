// NLM Replay Command - Replay saved brain states/episodes
// Handles replaying saved brain states and simulation episodes

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>

namespace nlm {

class ReplayCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool loadBrainState(const std::string& filename, std::shared_ptr<Brain>& brain);
    static bool replaySteps(std::shared_ptr<Brain> brain, size_t steps, double startTime, double speed);
    static void printReplayStats(double elapsedTime, size_t steps, std::shared_ptr<Brain> brain);
    static std::string formatReplayTime(double seconds);
};
