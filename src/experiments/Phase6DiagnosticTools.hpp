#pragma once

/**
 * Phase 6 Diagnostic Tools
 * 
 * Comprehensive diagnostic and testing utilities for Phase 6 integration.
 * Includes stress testing, comparative analysis, benchmarking, and export functionality.
 */

#include "experiments/Phase6ConfigManager.hpp"
#include "experiments/ExperimentRunnerImpl.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/ExperimentInterfaces.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <numeric>
#include <map>
#include <functional>

namespace nlm {

class Phase6DiagnosticTools {
public:
    static void runStressTest(const Phase6Config& baseConfig);
    static void runComparativeAnalysis();
    static void runDiagnosticBenchmark();
    static void exportDiagnostics(const std::string& filename);
    
private:
    static std::string getCurrentTimestamp();
    static std::string getBuildDate();
    static std::string getConfigName(const Phase6Config& config);
};

} // namespace nlm
