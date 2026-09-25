// NLM Debug Command - Debugging tools and verbose output
// Handles debugging, tracing, and verbose output options

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <vector>

namespace nlm {

class DebugCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool enableTracing(const std::map<std::string, std::string>& args);
    static bool showPerformanceStats(const std::map<std::string, std::string>& args);
    static bool showBrainDiagnostics(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options);
    static bool analyzeMemoryUsage(const std::map<std::string, std::string>& options);
    static bool analyzePerformance(std::shared_ptr<Brain> brain, const std::chrono::high_resolution_clock::time_point& startTime);
    static void printDebugInfo(std::shared_ptr<Brain> brain);
    static void printPerformanceStats(double elapsedTime, std::shared_ptr<Brain> brain);
    static std::string formatMemorySize(size_t bytes);
    static std::string formatTime(double seconds);
};
