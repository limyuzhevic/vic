// ProfileCommandOptions implementation

#include "ProfileCommandOptions.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <ctime>

namespace nlm {
namespace cli {

// ProfileCommandOptions implementation

bool ProfileCommandOptions::validate() const {
    if (profile && benchmark && analyze) {
        return true;
    }
    
    if (samplingInterval <= 0) return false;
    if (maxThreads <= 0) return false;
    if (timeLimit < 0) return false;
    if (memoryLimit < 0) return false;
    
    if (outputFormat != "text" && outputFormat != "json" && outputFormat != "csv" && outputFormat != "html") {
        return false;
    }
    
    if (dashboardFormat != "text" && dashboardFormat != "json" && dashboardFormat != "html") {
        return false;
    }
    
    if (regressionThreshold <= 0 || regressionThreshold > 1) return false;
    
    return true;
}

void ProfileCommandOptions::printOptions() const {
    std::cout << "Profile Command Options Summary:" << std::endl;
    
    if (profile) {
        std::cout << "  Profiling: ENABLED" << std::endl;
        if (realTime) std::cout << "    Real-time profiling enabled" << std::endl;
        if (cpuProfiler) std::cout << "    CPU profiler enabled" << std::endl;
        if (memoryProfiler) std::cout << "    Memory profiler enabled" << std::endl;
        if (threadProfiler) std::cout << "    Thread profiler enabled" << std::endl;
        if (simulationProfiler) std::cout << "    Simulation profiler enabled" << std::endl;
        std::cout << "    Sampling interval: " << samplingInterval << "s" << std::endl;
    }
    
    if (benchmark) {
        std::cout << "  Benchmarking: ENABLED" << std::endl;
        if (enableBenchmarks) std::cout << "    All benchmarks enabled" << std::endl;
        if (!benchmarkSuite.empty()) std::cout << "    Benchmark suite: " << benchmarkSuite << std::endl;
        if (runScalability) std::cout << "    Scalability tests enabled" << std::endl;
        if (runStressTest) std::cout << "    Stress tests enabled" << std::endl;
        if (runLoadTest) std::cout << "    Load tests enabled" << std::endl;
        std::cout << "    Max threads: " << maxThreads << std::endl;
    }
    
    if (analyze) {
        std::cout << "  Analysis: ENABLED" << std::endl;
        if (detectRegressions) std::cout << "    Regression detection enabled (threshold: " << (regressionThreshold * 100) << "%)" << std::endl;
        if (generateReports) std::cout << "    Report generation enabled" << std::endl;
        std::cout << "    Analysis type: " << analysisType << std::endl;
    }
    
    if (outputFile.empty()) {
        std::cout << "  Output file: (none - output to console)" << std::endl;
    } else {
        std::cout << "  Output file: " << outputFile << std::endl;
    }
    std::cout << "  Output format: " << outputFormat << std::endl;
    
    if (showDashboard) {
        std::cout << "  Dashboard: ENABLED (format: " << dashboardFormat << ")" << std::endl;
    }
    
    if (!configFile.empty()) {
        std::cout << "  Config file: " << configFile << std::endl;
    }
}

std::string ProfileCommandOptions::toString() const {
    std::stringstream ss;
    ss << "ProfileCommandOptions{" << std::endl;
    ss << "  profile: " << (profile ? "true" : "false") << "," << std::endl;
    ss << "  benchmark: " << (benchmark ? "true" : "false") << "," << std::endl;
    ss << "  analyze: " << (analyze ? "true" : "false") << "," << std::endl;
    ss << "  outputFile: \"" << outputFile << "\"," << std::endl;
    ss << "  outputFormat: \"" << outputFormat << "\"," << std::endl;
    ss << "  realTime: " << (realTime ? "true" : "false") << "," << std::endl;
    ss << "  samplingInterval: " << samplingInterval << "," << std::endl;
    ss << "  cpuProfiler: " << (cpuProfiler ? "true" : "false") << "," << std::endl;
    ss << "  memoryProfiler: " << (memoryProfiler ? "true" : "false") << "," << std::endl;
    ss << "  threadProfiler: " << (threadProfiler ? "true" : "false") << "," << std::endl;
    ss << "  simulationProfiler: " << (simulationProfiler ? "true" : "false") << "," << std::endl;
    ss << "  enableBenchmarks: " << (enableBenchmarks ? "true" : "false") << "," << std::endl;
    ss << "  benchmarkSuite: \"" << benchmarkSuite << "\"," << std::endl;
    ss << "  runScalability: " << (runScalability ? "true" : "false") << "," << std::endl;
    ss << "  runStressTest: " << (runStressTest ? "true" : "false") << "," << std::endl;
    ss << "  runLoadTest: " << (runLoadTest ? "true" : "false") << "," << std::endl;
    ss << "  maxThreads: " << maxThreads << "," << std::endl;
    ss << "  detectRegressions: " << (detectRegressions ? "true" : "false") << "," << std::endl;
    ss << "  regressionThreshold: " << regressionThreshold << "," << std::endl;
    ss << "  generateReports: " << (generateReports ? "true" : "false") << "," << std::endl;
    ss << "  showDashboard: " << (showDashboard ? "true" : "false") << "," << std::endl;
    ss << "  exportMetrics: " << (exportMetrics ? "true" : "false") << "," << std::endl;
    ss << "  dashboardFormat: \"" << dashboardFormat << "\"," << std::endl;
    ss << "  configFile: \"" << configFile << "\"," << std::endl;
    ss << "  validateConfig: " << (validateConfig ? "true" : "false") << "," << std::endl;
    ss << "  enableHardwareProfiling: " << (enableHardwareProfiling ? "true" : "false") << "," << std::endl;
    ss << "  enableIoprofiling: " << (enableIoprofiling ? "true" : "false") << "," << std::endl;
    ss << "  timeLimit: " << timeLimit << "," << std::endl;
    ss << "  memoryLimit: " << memoryLimit << "," << std::endl;
    ss << "}";
    return ss.str();
}

void ProfileCommandOptions::parse(const std::vector<std::string>& args) {
    // Simple parsing implementation - can be expanded based on actual command line parsing needs
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--profile" || args[i] == "-p") {
            profile = true;
        } else if (args[i] == "--benchmark" || args[i] == "-b") {
            benchmark = true;
        } else if (args[i] == "--analyze" || args[i] == "-a") {
            analyze = true;
        } else if (args[i] == "--output-file") {
            if (i + 1 < args.size()) outputFile = args[++i];
        } else if (args[i] == "--output-format") {
            if (i + 1 < args.size()) outputFormat = args[++i];
        } else if (args[i] == "--real-time") {
            realTime = true;
        } else if (args[i] == "--sampling-interval") {
            if (i + 1 < args.size()) samplingInterval = std::stod(args[++i]);
        } else if (args[i] == "--cpu-profiler") {
            cpuProfiler = true;
        } else if (args[i] == "--memory-profiler") {
            memoryProfiler = true;
        } else if (args[i] == "--thread-profiler") {
            threadProfiler = true;
        } else if (args[i] == "--simulation-profiler") {
            simulationProfiler = true;
        } else if (args[i] == "--enable-benchmarks") {
            enableBenchmarks = true;
        } else if (args[i] == "--benchmark-suite") {
            if (i + 1 < args.size()) benchmarkSuite = args[++i];
        } else if (args[i] == "--run-scalability") {
            runScalability = true;
        } else if (args[i] == "--run-stress-test") {
            runStressTest = true;
        } else if (args[i] == "--run-load-test") {
            runLoadTest = true;
        } else if (args[i] == "--max-threads") {
            if (i + 1 < args.size()) maxThreads = std::stoi(args[++i]);
        } else if (args[i] == "--detect-regressions") {
            detectRegressions = true;
        } else if (args[i] == "--regression-threshold") {
            if (i + 1 < args.size()) regressionThreshold = std::stod(args[++i]);
        } else if (args[i] == "--generate-reports") {
            generateReports = true;
        } else if (args[i] == "--show-dashboard") {
            showDashboard = true;
        } else if (args[i] == "--export-metrics") {
            exportMetrics = true;
        } else if (args[i] == "--dashboard-format") {
            if (i + 1 < args.size()) dashboardFormat = args[++i];
        } else if (args[i] == "--config-file") {
            if (i + 1 < args.size()) configFile = args[++i];
        } else if (args[i] == "--validate-config") {
            validateConfig = true;
        } else if (args[i] == "--enable-hardware-profiling") {
            enableHardwareProfiling = true;
        } else if (args[i] == "--enable-io-profiling") {
            enableIoprofiling = true;
        } else if (args[i] == "--time-limit") {
            if (i + 1 < args.size()) timeLimit = std::stod(args[++i]);
        } else if (args[i] == "--memory-limit") {
            if (i + 1 < args.size()) memoryLimit = std::stoi(args[++i]);
        }
    }
}

std::string ProfileCommandOptions::getProfileTypes() const {
    std::vector<std::string> types;
    if (cpuProfiler) types.push_back("CPU");
    if (memoryProfiler) types.push_back("Memory");
    if (threadProfiler) types.push_back("Thread");
    if (simulationProfiler) types.push_back("Simulation");
    if (enableHardwareProfiling) types.push_back("Hardware");
    if (enableIoprofiling) types.push_back("I/O");
    
    std::string result;
    for (size_t i = 0; i < types.size(); ++i) {
        if (i > 0) result += ", ";
        result += types[i];
    }
    return result.empty() ? "all" : result;
}

ProfileCommandOptions ProfileCommandOptions::fromEnvironment() {
    ProfileCommandOptions options;
    
    // Load from environment variables
    std::string profile = Environment::getVar("NLM_PROFILE", "0");
    std::string benchmark = Environment::getVar("NLM_BENCHMARK", "0");
    std::string analyze = Environment::getVar("NLM_ANALYZE", "0");
    std::string outputFile = Environment::getVar("NLM_OUTPUT_FILE", "");
    std::string outputFormat = Environment::getVar("NLM_OUTPUT_FORMAT", "text");
    std::string samplingInterval = Environment::getVar("NLM_SAMPLING_INTERVAL", "0.1");
    
    try {
        options.profile = (profile == "1" || profile == "true");
        options.benchmark = (benchmark == "1" || benchmark == "true");
        options.analyze = (analyze == "1" || analyze == "true");
        options.outputFile = outputFile;
        options.outputFormat = outputFormat;
        options.samplingInterval = std::stod(samplingInterval);
    } catch (...) {
        // Use defaults if parsing fails
    }
    
    return options;
}

ProfileCommandOptions ProfileCommandOptions::fromConfigFile(const std::string& filename) {
    ProfileCommandOptions options;
    options.loadFromFile(filename);
    return options;
}

} // namespace cli
} // namespace nlm