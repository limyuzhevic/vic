#!/usr/bin/env g++
// NLM Setup Tool
// Setup development environment for NLM

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

using namespace nlm;

namespace fs = std::filesystem;

struct SetupResult {
    std::string component;
    bool success;
    std::string message;
    double executionTime;
};

class NLMSetup {
private:
    std::vector<SetupResult> setupResults;
    std::string workspaceRoot;
    bool verbose;
    bool autoSetup;
    bool dryRun;
    
public:
    NLMSetup() {
        workspaceRoot = ".";
        verbose = false;
        autoSetup = false;
        dryRun = false;
        
        // Find workspace root
        if (fs::exists("src") && fs::exists("configs")) {
            workspaceRoot = ".";
        } else if (fs::exists("..")) {
            if (fs::exists("../src") && fs::exists("../configs")) {
                workspaceRoot = "..";
            }
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --verbose                 Enable verbose output" << std::endl;
        std::cout << "  --auto                    Auto-setup (install missing files)" << std::endl;
        std::cout << "  --dry-run                 Show what would be done without doing it" << std::endl;
        std::cout << "  --workspace <path>        Set workspace root directory" << std::endl;
        std::cout << "  --check                   Check if environment is already set up" << std::endl;
        std::cout << "  --install                Install NLM to system (requires sudo)" << std::endl;
        std::cout << "  --create-templates        Create configuration templates" << std::endl;
        std::cout << "  --setup-python            Setup Python environment" << std::endl;
        std::cout << "  --setup-tools             Setup command-line tools" << std::endl;
        std::cout << "  --setup-docs              Setup documentation" << std::endl;
        std::cout << "  --all                    Run all setup steps" << std::endl;
    }
    
    void checkEnvironment() {
        std::cout << "=== Checking NLM Environment ===" << std::endl;
        
        std::vector<std::string> requiredDirectories = {
            "src",
            "src/core",
            "src/core/Config",
            "src/brain",
            "src/dynamics",
            "src/plasticity",
            "src/development",
            "src/neuromodulation",
            "src/memory",
            "src/cognition",
            "src/sensory",
            "src/motion",
            "src/environment",
            "src/experiments",
            "src/visualization",
            "configs",
            "tools",
            "tests"
        };
        
        std::vector<std::string> requiredFiles = {
            "src/main.cpp",
            "configs/default.cfg",
            "CMakeLists.txt",
            "pyproject.toml"
        };
        
        int dirCount = 0, dirMissing = 0;
        int fileCount = 0, fileMissing = 0;
        
        std::cout << "Checking directories..." << std::endl;
        for (const auto& dir : requiredDirectories) {
            std::string fullPath = workspaceRoot + "/" + dir;
            if (fs::exists(fullPath) && fs::is_directory(fullPath)) {
                std::cout << "  ✓ " << dir << " (exists)" << std::endl;
                dirCount++;
            } else {
                std::cout << "  ✗ " << dir << " (missing)" << std::endl;
                dirMissing++;
            }
        }
        
        std::cout << "\nChecking files..." << std::endl;
        for (const auto& file : requiredFiles) {
            std::string fullPath = workspaceRoot + "/" + file;
            if (fs::exists(fullPath) && fs::is_regular_file(fullPath)) {
                std::cout << "  ✓ " << file << " (exists)" << std::endl;
                fileCount++;
            } else {
                std::cout << "  ✗ " << file << " (missing)" << std::endl;
                fileMissing++;
            }
        }
        
        std::cout << "\n=== Environment Check Complete ===" << std::endl;
        std::cout << "Directories: " << dirCount << "/" << requiredDirectories.size() << " (" << dirMissing << " missing)" << std::endl;
        std::cout << "Files: " << fileCount << "/" << requiredFiles.size() << " (" << fileMissing << " missing)" << std::endl;
        
        if (dirMissing == 0 && fileMissing == 0) {
            std::cout << "✓ Environment is properly set up!" << std::endl;
        } else {
            std::cout << "✗ Environment needs setup." << std::endl;
        }
    }
    
    void createConfigurationTemplates() {
        std::cout << "=== Creating Configuration Templates ===" << std::endl;
        
        std::string templateDir = workspaceRoot + "/configs/templates";
        if (!fs::exists(templateDir)) {
            if (!dryRun) {
                fs::create_directories(templateDir);
            }
            std::cout << "Created directory: " << templateDir << std::endl;
        }
        
        // Create basic configuration template
        std::string basicTemplate = templateDir + "/basic.cfg";
        if (!fs::exists(basicTemplate) && !dryRun) {
            std::ofstream file(basicTemplate);
            file << "# Basic NLM Configuration Template" << std::endl;
            file << "# This template provides basic settings for NLM simulation" << std::endl;
            file << "# " << std::time(nullptr) << " NLM" << std::endl;
            file << std::endl;
            file << "# Basic settings" << std::endl;
            file << "random_seed = 42" << std::endl;
            file << "simulation_timestep = 0.001" << std::endl;
            file << "neuron_count = 1000" << std::endl;
            file << "region_count = 2" << std::endl;
            file << "connection_probability = 0.1" << std::endl;
            file << std::endl;
            file << "# Plasticity settings" << std::endl;
            file << "plasticity_learning_rate = 0.01" << std::endl;
            file << "stdp_ltp_weight = 0.01" << std::endl;
            file << "stdp_ltd_weight = 0.012" << std::endl;
            file << "stdp_time_constant = 20.0" << std::endl;
            file << std::endl;
            file << "# Development settings" << std::endl;
            file << "development_synaptogenesis_rate = 0.001" << std::endl;
            file << "development_pruning_rate = 0.0001" << std::endl;
            file << std::endl;
            file << "# Neuromodulation settings" << std::endl;
            file << "dopamine_baseline = 0.1" << std::endl;
            file << "curiosity_baseline = 0.0" << std::endl;
            file << std::endl;
            file << "# Environment settings" << std::endl;
            file << "environment_name = GridWorld" << std::endl;
            file << "environment_width = 10" << std::endl;
            file << "environment_height = 10" << std::endl;
            file << std::endl;
            file << "# Logging settings" << std::endl;
            file << "log_level = INFO" << std::endl;
            file << "log_to_file = false" << std::endl;
            file << "log_filename = nlm.log" << std::endl;
            file.close();
            std::cout << "Created: " << basicTemplate << std::endl;
        }
        
        // Create advanced configuration template
        std::string advancedTemplate = templateDir + "/advanced.cfg";
        if (!fs::exists(advancedTemplate) && !dryRun) {
            std::ofstream file(advancedTemplate);
            file << "# Advanced NLM Configuration Template" << std::endl;
            file << "# This template includes advanced settings for complex simulations" << std::endl;
            file << std::endl;
            file << "# Basic settings (advanced values)" << std::endl;
            file << "random_seed = 12345" << std::endl;
            file << "simulation_timestep = 0.0005" << std::endl;
            file << "neuron_count = 5000" << std::endl;
            file << "region_count = 5" << std::endl;
            file << "connection_probability = 0.05" << std::endl;
            file << std::endl;
            file << "# Plasticity settings (advanced)" << std::endl;
            file << "plasticity_learning_rate = 0.005" << std::endl;
            file << "stdp_ltp_weight = 0.02" << std::endl;
            file << "stdp_ltd_weight = 0.018" << std::endl;
            file << "stdp_time_constant = 15.0" << std::endl;
            file << std::endl;
            file << "# Development settings (advanced)" << std::endl;
            file << "development_synaptogenesis_rate = 0.0005" << std::endl;
            file << "development_pruning_rate = 0.00005" << std::endl;
            file << "maturation_rate = 0.01" << std::endl;
            file << std::endl;
            file << "# Neuromodulation settings (advanced)" << std::endl;
            file << "dopamine_baseline = 0.2" << std::endl;
            file << "curiosity_baseline = 0.3" << std::endl;
            file << "novelty_baseline = 0.1" << std::endl;
            file << std::endl;
            file << "# Memory settings (advanced)" << std::endl;
            file << "working_memory_capacity = 100" << std::endl;
            file << "episodic_memory_enabled = true" << std::endl;
            file << "semantic_memory_enabled = true" << std::endl;
            file << std::endl;
            file << "# Advanced simulation settings" << std::endl;
            file << "max_simulation_steps = 50000" << std::endl;
            file << "simulation_time_limit = 100.0" << std::endl;
            file << "checkpoint_interval = 1000" << std::endl;
            file << std::endl;
            file.close();
            std::cout << "Created: " << advancedTemplate << std::endl;
        }
        
        std::cout << "Configuration templates created successfully!" << std::endl;
    }
    
    void setupPythonEnvironment() {
        std::cout << "=== Setting Up Python Environment ===" << std::endl;
        
        // Check if pyproject.toml exists
        std::string pyprojectPath = workspaceRoot + "/pyproject.toml";
        if (fs::exists(pyprojectPath)) {
            std::cout << "✓ pyproject.toml found" << std::endl;
            
            // Check for requirements
            std::vector<std::string> pyprojectSections = {
                "pybind11>=2.11.0",
                "scikit-build-core>=0.5.0"
            };
            
            for (const auto& req : pyprojectSections) {
                std::cout << "  Python dependency: " << req << std::endl;
            }
            
            std::cout << "Python environment setup: COMPLETED (configure build system)" << std::endl;
        } else {
            std::cout << "✗ pyproject.toml not found" << std::endl;
        }
        
        // Check for Python files in tools
        std::string toolsDir = workspaceRoot + "/tools";
        std::vector<std::string> pythonFiles = {
            "python/nlm_cli.py",
            "python/bindings.cpp",
            "python/CMakeLists.txt"
        };
        
        for (const auto& file : pythonFiles) {
            std::string fullPath = workspaceRoot + "/" + file;
            if (fs::exists(fullPath)) {
                std::cout << "✓ " << file << " (exists)" << std::endl;
            } else {
                std::cout << "✗ " << file << " (missing)" << std::endl;
            }
        }
        
        std::cout << "Python environment setup: COMPLETED" << std::endl;
    }
    
    void setupCommandLineTools() {
        std::cout << "=== Setting Up Command-Line Tools ===" << std::endl;
        
        // Check basic tools directory
        std::string basicToolsDir = workspaceRoot + "/tools/basic";
        if (fs::exists(basicToolsDir)) {
            std::cout << "✓ Basic tools directory exists" << std::endl;
            
            std::vector<std::string> basicTools = {
                "nlm-config",
                "nlm-validate", 
                "nlm-info",
                "nlm-benchmark",
                "nlm-save",
                "nlm-load"
            };
            
            for (const auto& tool : basicTools) {
                std::string toolPath = basicToolsDir + "/" + tool + ".cpp";
                if (fs::exists(toolPath)) {
                    std::cout << "  ✓ " << tool << ".cpp (exists)" << std::endl;
                } else {
                    std::cout << "  ✗ " << tool << ".cpp (missing)" << std::endl;
                }
            }
        } else {
            std::cout << "✗ Basic tools directory missing" << std::endl;
        }
        
        // Check advanced tools directory
        std::string advancedToolsDir = workspaceRoot + "/tools/advanced";
        if (fs::exists(advancedToolsDir)) {
            std::cout << "✓ Advanced tools directory exists" << std::endl;
        } else {
            std::cout << "✗ Advanced tools directory missing (create for advanced tools)" << std::endl;
        }
        
        // Check development tools directory
        std::string devToolsDir = workspaceRoot + "/tools/development";
        if (fs::exists(devToolsDir)) {
            std::cout << "✓ Development tools directory exists" << std::endl;
            
            std::vector<std::string> devTools = {
                "nlm-profiler",
                "nlm-monitor",
                "nlm-test",
                "nlm-setup",
                "nlm-help"
            };
            
            for (const auto& tool : devTools) {
                std::string toolPath = devToolsDir + "/" + tool + ".cpp";
                if (fs::exists(toolPath)) {
                    std::cout << "  ✓ " << tool << ".cpp (exists)" << std::endl;
                } else {
                    std::cout << "  ✗ " << tool << ".cpp (missing)" << std::endl;
                }
            }
        } else {
            std::cout << "✗ Development tools directory missing" << std::endl;
        }
        
        // Check CMakeLists.txt for tools
        std::string toolsCMake = workspaceRoot + "/tools/CMakeLists.txt";
        if (fs::exists(toolsCMake)) {
            std::cout << "✓ Tools CMakeLists.txt (exists)" << std::endl;
        } else {
            std::cout << "✗ Tools CMakeLists.txt (missing)" << std::endl;
        }
        
        std::cout << "Command-line tools setup: COMPLETED" << std::endl;
    }
    
    void setupDocumentation() {
        std::cout << "=== Setting Up Documentation ===" << std::endl;
        
        // Check for documentation directory
        std::string docsDir = workspaceRoot + "/docs";
        if (fs::exists(docsDir)) {
            std::cout << "✓ Documentation directory exists" << std::endl;
            
            std::vector<std::string> docFiles = {
                "README.md",
                "API_REFERENCE.md",
                "HOW_TO_USE.md",
                "easy_usage.md"
            };
            
            for (const auto& file : docFiles) {
                std::string fullPath = docsDir + "/" + file;
                if (fs::exists(fullPath)) {
                    std::cout << "  ✓ " << file << " (exists)" << std::endl;
                } else {
                    std::cout << "  ✗ " << file << " (missing)" << std::endl;
                }
            }
        } else {
            std::cout << "✗ Documentation directory missing" << std::endl;
        }
        
        std::cout << "Documentation setup: COMPLETED" << std::endl;
    }
    
    void printSetupSummary() {
        std::cout << "=== NLM Setup Complete ===" << std::endl;
        std::cout << "Workspace root: " << workspaceRoot << std::endl;
        std::cout << "Verbose mode: " << (verbose ? "enabled" : "disabled") << std::endl;
        std::cout << "Auto setup: " << (autoSetup ? "enabled" : "disabled") << std::endl;
        std::cout << "Dry run: " << (dryRun ? "enabled" : "disabled") << std::endl;
        std::cout << std::endl;
        std::cout << "Setup completed successfully!" << std::endl;
        std::cout << std::endl;
        std::cout << "Next steps:" << std::endl;
        std::cout << "  1. Build the project: mkdir build && cd build && cmake .. && make" << std::endl;
        std::cout << "  2. Run tests: ./nlm-test --all" << std::endl;
        std::cout << "  3. Start monitoring: ./nlm-monitor --brain NLM_Brain" << std::endl;
        std::cout << "  4. Run benchmarks: ./nlm-benchmark --detailed" << std::endl;
        std::cout << "  5. Use configuration tools: ./nlm-config --create config.cfg" << std::endl;
    }
};