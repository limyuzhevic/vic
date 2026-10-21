#!/usr/bin/env g++
// NLM System Information Tool
// Tool for displaying system information and capabilities

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <ctime>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"

using namespace nlm;

class NLMInfo {
private:
    std::string version;
    std::time_t buildTime;
    std::string compilerInfo;
    std::string platformInfo;
    
public:
    NLMInfo() {
        version = "0.1.0";
        buildTime = std::time(nullptr);
        compilerInfo = "GCC";
        platformInfo = "Linux";
        
        // Get compiler info
#if defined(__GNUC__) && !defined(__clang__)
        compilerInfo = "GCC " + std::string(__VERSION__);
#elif defined(__clang__)
        compilerInfo = "Clang " + std::string(__clang_version__);
#elif defined(_MSC_VER)
        compilerInfo = "MSVC " + std::to_string(_MSC_VER);
#endif
        
        // Get platform info
#ifdef __linux__
        platformInfo = "Linux";
#elif defined(__APPLE__) && defined(__MACH__)
        platformInfo = "macOS";
#elif defined(_WIN32) || defined(_WIN64)
        platformInfo = "Windows";
#endif
    }
    
    void printSystemInfo() {
        std::cout << "=== NLM System Information ===" << std::endl;
        std::cout << std::endl;
        
        printBasicInfo();
        std::cout << std::endl;
        
        printBuildInfo();
        std::cout << std::endl;
        
        printCompilationInfo();
        std::cout << std::endl;
        
        printEnvironmentInfo();
        std::cout << std::endl;
        
        printCapabilities();
        std::cout << std::endl;
    }
    
    void printBasicInfo() {
        std::cout << "Basic Information:" << std::endl;
        std::cout << "  Product:           NLM (熙然)" << std::endl;
        std::cout << "  Version:           " << version << std::endl;
        std::cout << "  Description:       Neural Learning Machine - Experimental Artificial Developmental Brain" << std::endl;
        std::cout << "  Build Date:        " << formatTime(buildTime) << std::endl;
    }
    
    void printBuildInfo() {
        std::cout << "Build Information:" << std::endl;
        std::cout << "  Platform:          " << platformInfo << std::endl;
        std::cout << "  Compiler:          " << compilerInfo << std::endl;
        std::cout << "  Architecture:      " << getArchitecture() << std::endl;
        std::cout << "  Build Type:        " << getBuildType() << std::endl;
        std::cout << "  C++ Standard:      C++20" << std::endl;
    }
    
    void printCompilationInfo() {
        std::cout << "Compilation Information:" << std::endl;
        
        // Memory info
        printMemoryInfo();
        
        // Performance counters
        printPerformanceCounters();
        
        // Feature detection
        printFeatureDetection();
    }
    
    void printEnvironmentInfo() {
        std::cout << "Environment Information:" << std::endl;
        
        // Environment variables
        printEnvironmentVariables();
        
        // File system info
        printFileSystemInfo();
    }
    
    void printCapabilities() {
        std::cout << "NLM Capabilities:" << std::endl;
        
        // Core systems
        printCoreSystems();
        
        // Neural systems
        printNeuralSystems();
        
        // Learning capabilities
        printLearningCapabilities();
        
        // Performance capabilities
        printPerformanceCapabilities();
    }
    
    void printMemoryInfo() {
        std::ifstream memInfo("/proc/meminfo");
        if (memInfo.is_open()) {
            std::string line;
            std::getline(memInfo, line); // Skip first line
            if (std::getline(memInfo, line)) {
                std::cout << "  Memory:            " << line.substr(line.find(':') + 1) << std::endl;
            }
            memInfo.close();
        } else {
            std::cout << "  Memory:            (unable to read)" << std::endl;
        }
    }
    
    void printPerformanceCounters() {
        std::cout << "  CPU Cores:         " << std::thread::hardware_concurrency() << std::endl;
        
        // Get CPU info
        std::ifstream cpuInfo("/proc/cpuinfo");
        if (cpuInfo.is_open()) {
            std::string line;
            while (std::getline(cpuInfo, line)) {
                if (line.find("model name") != std::string::npos) {
                    std::cout << "  CPU:               " << line.substr(line.find(':') + 1) << std::endl;
                    break;
                }
            }
            cpuInfo.close();
        }
    }
    
    void printFeatureDetection() {
        std::cout << "  Features:          " << getAvailableFeatures() << std::endl;
    }
    
    void printEnvironmentVariables() {
        std::cout << "  NLM_HOME:          " << (std::getenv("NLM_HOME") ? std::getenv("NLM_HOME") : "(not set)") << std::endl;
        std::cout << "  LD_LIBRARY_PATH:   " << (std::getenv("LD_LIBRARY_PATH") ? std::getenv("LD_LIBRARY_PATH") : "(not set)") << std::endl;
        std::cout << "  PYTHONPATH:        " << (std::getenv("PYTHONPATH") ? std::getenv("PYTHONPATH") : "(not set)") << std::endl;
        std::cout << "  PYTHONPATH:        " << (std::getenv("PYTHONPATH") ? std::getenv("PYTHONPATH") : "(not set)") << std::endl;
    }
    
    void printFileSystemInfo() {
        // Check for config directory
        std::string configPath = "configs";
        bool hasConfigs = std::ifstream(configPath + "/default.cfg").good();
        std::cout << "  Config Directory:  " << (hasConfigs ? configPath + " (found)" : configPath + " (missing)") << std::endl;
        
        // Check for tools directory
        std::string toolsPath = "tools";
        bool hasTools = std::ifstream(toolsPath + "/basic/nlm-config.cpp").good();
        std::cout << "  Tools Directory:    " << (hasTools ? toolsPath + " (found)" : toolsPath + " (missing)") << std::endl;
    }
    
    void printCoreSystems() {
        std::cout << "  Core Systems:      [x] Configuration Management" << std::endl;
        std::cout << "                   [x] Random Number Generation" << std::endl;
        std::cout << "                   [x] Logging System" << std::endl;
        std::cout << "                   [x] Simulation Clock" << std::endl;
        std::cout << "                   [x] Core Types and Utilities" << std::endl;
    }
    
    void printNeuralSystems() {
        std::cout << "  Neural Systems:    [x] LIF Neuron Dynamics" << std::endl;
        std::cout << "                   [x] Spike Propagation" << std::endl;
        std::cout << "                   [x] Synaptic Plasticity (STDP, Hebbian)" << std::endl;
        std::cout << "                   [x] Structural Plasticity" << std::endl;
        std::cout << "                   [x] Neuromodulation (Dopamine, Curiosity)" << std::endl;
        std::cout << "                   [x] Neural Populations and Regions" << std::endl;
    }
    
    void printLearningCapabilities() {
        std::cout << "  Learning Systems:  [x] Supervised Learning" << std::endl;
        std::cout << "                   [x] Unsupervised Learning" << std::endl;
        std::cout << "                   [x] Reinforcement Learning" << std::endl;
        std::cout << "                   [x] Developmental Learning" << std::endl;
        std::cout << "                   [x] Continual Learning" << std::endl;
        std::cout << "                   [x] Transfer Learning" << std::endl;
    }
    
    void printPerformanceCapabilities() {
        std::cout << "  Performance:       [x] Event-driven Simulation" << std::endl;
        std::cout << "                   [x] Parallel Processing" << std::endl;
        std::cout << "                   [x] Memory Pools" << std::endl;
        std::cout << "                   [x] SIMD Optimization" << std::endl;
        std::cout << "                   [x] Checkpoint/Restore" << std::endl;
    }
    
    std::string getArchitecture() {
#ifdef __x86_64__
        return "x86_64";
#elif __i386__
        return "x86";
#elif __aarch64__
        return "ARM64";
#elif __arm__
        return "ARM";
#elif __aarch64__ || __arm64__
        return "ARM64";
#else
        return "Unknown";
#endif
    }
    
    std::string getBuildType() {
#ifdef NDEBUG
        return "Release";
#else
        return "Debug";
#endif
    }
    
    std::string getAvailableFeatures() {
        std::string features = "";
        
        // Check for various features
#ifdef NLM_FEATURE_PLOTTING
        features += "Plotting ";
#endif
        
#ifdef NLM_FEATURE_PARALLEL
        features += "Parallel ";
#endif
        
#ifdef NLM_FEATURE_PYTHON_BINDING
        features += "Python Bindings ";
#endif
        
#ifdef NLM_FEATURE_OPENMP
        features += "OpenMP ";
#endif
        
        if (features.empty()) {
            features = "Basic";
        }
        
        return features;
    }
    
    std::string formatTime(std::time_t time) {
        std::tm* tm = std::localtime(&time);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --version                 Show version information" << std::endl;
        std::cout << "  --config <file>           Load and display configuration info" << std::endl;
        std::cout << "  --check <file>            Check configuration file" << std::endl;
        std::cout << "  --benchmark               Run quick benchmark" << std::endl;
        std::cout << "  --validate                Validate NLM installation" << std::endl;
        std::cout << "  --gui                    Launch graphical interface" << std::endl;
        std::cout << "  --list-tools              List available command-line tools" << std::endl;
        std::cout << "  --tools-dir               Show tools directory location" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-info";
    std::string configFile;
    bool versionOnly = false;
    bool validateOnly = false;
    bool benchmarkOnly = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMInfo info;
            info.printUsage(programName);
            return 0;
        } else if (arg == "--version") {
            versionOnly = true;
        } else if (arg == "--validate") {
            validateOnly = true;
        } else if (arg == "--benchmark") {
            benchmarkOnly = true;
        } else if (arg == "--tools-dir") {
            std::cout << "Tools directory: tools/" << std::endl;
            return 0;
        } else if (arg == "--list-tools") {
            std::cout << "Available NLM command-line tools:" << std::endl;
            std::cout << "  Basic Tools:" << std::endl;
            std::cout << "    nlm-config    - Configuration file creation and validation" << std::endl;
            std::cout << "    nlm-validate  - Validate configuration and test brain functionality" << std::endl;
            std::cout << "    nlm-info      - Display system information and capabilities" << std::endl;
            std::cout << "    nlm-benchmark - Performance benchmarking suite" << std::endl;
            std::cout << "    nlm-save      - Save simulation checkpoints" << std::endl;
            std::cout << "    nlm-load      - Load simulation checkpoints" << std::endl;
            std::cout << "  Advanced Tools:" << std::endl;
            std::cout << "    nlm-explore    - Interactive exploration of brain parameters" << std::endl;
            std::cout << "    nlm-plot      - Generate visualization plots" << std::endl;
            std::cout << "    nlm-compare   - Compare simulation configurations" << std::endl;
            std::cout << "    nlm-analyze   - Deep analysis of simulation results" << std::endl;
            std::cout << "    nlm-export    - Export brain state for external analysis" << std::endl;
            std::cout << "    nlm-import    - Import external data into NLM" << std::endl;
            std::cout << "  Development Tools:" << std::endl;
            std::cout << "    nlm-profiler  - Performance profiling and optimization" << std::endl;
            std::cout << "    nlm-monitor   - Real-time monitoring of simulation state" << std::endl;
            std::cout << "    nlm-test      - Run specific test suites" << std::endl;
            std::cout << "    nlm-setup     - Setup development environment" << std::endl;
            std::cout << "    nlm-help      - Comprehensive help documentation" << std::endl;
            std::cout << "  Scripting Tools:" << std::endl;
            std::cout << "    Python bindings for command-line integration" << std::endl;
            std::cout << "    JSON configuration file generators" << std::endl;
            std::cout << "    Simulation script templates" << std::endl;
            std::cout << "    Experiment macros and presets" << std::endl;
            return 0;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--check") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --check requires a filename argument" << std::endl;
                return 1;
            }
        }
    }
    
    NLMInfo info;
    
    if (versionOnly) {
        std::cout << "NLM " << info.version << std::endl;
        return 0;
    }
    
    if (validateOnly) {
        std::cout << "Validating NLM installation..." << std::endl;
        // Run validation checks
        bool configExists = std::ifstream("configs/default.cfg").good();
        bool toolsExist = std::ifstream("tools/basic/nlm-config.cpp").good();
        
        if (configExists && toolsExist) {
            std::cout << "✓ NLM installation appears to be valid." << std::endl;
        } else {
            std::cout << "✗ NLM installation validation failed." << std::endl;
            if (!configExists) std::cout << "  Missing: configs/default.cfg" << std::endl;
            if (!toolsExist) std::cout << "  Missing: tools/ directory" << std::endl;
            return 1;
        }
        return 0;
    }
    
    if (benchmarkOnly) {
        std::cout << "Running quick benchmark..." << std::endl;
        // Simple benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Do some basic operations
        NLMInfo testInfo;
        testInfo.getArchitecture();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        std::cout << "Benchmark completed in " << elapsed << "ms" << std::endl;
        return 0;
    }
    
    if (!configFile.empty()) {
        // Load and display configuration info
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            std::cout << "Configuration from " << configFile << ":" << std::endl;
            std::cout << config->summary() << std::endl;
        } else {
            std::cout << "Failed to load configuration from: " << configFile << std::endl;
            return 1;
        }
    }
    
    // Always show system info by default
    info.printSystemInfo();
    
    return 0;
}
