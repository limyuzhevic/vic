// Utilities for NLM modular demo system
// Common helper functions and utilities used across different demo components

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <functional>

namespace nlm {

// Utility functions for console output and formatting
class ConsoleUtils {
public:
    static void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    static void printSection(const std::string& title) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    static void printSubSection(const std::string& title) {
        std::cout << "\n--- " << title << " ---" << std::endl;
    }
    
    static void printResult(const std::string& name, float value, float max = 1.0f) {
        std::cout << "  " << std::left << std::setw(30) << name << ": ";
        std::cout << std::fixed << std::setprecision(3) << value;
        if (max > 0) {
            std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
        }
        std::cout << std::endl;
    }
    
    static void printStatus(const std::string& message, bool success = true) {
        if (success) {
            std::cout << "  ✓ " << message << std::endl;
        } else {
            std::cout << "  ✗ " << message << std::endl;
        }
    }
    
    static void printProgress(size_t current, size_t total, const std::string& description = "") {
        std::cout << "  Progress: " << current << "/" << total << " ";
        if (!description.empty()) {
            std::cout << "- " << description;
        }
        std::cout << "\r" << std::flush;
    }
    
    static void printCompleted() {
        std::cout << "\r  Done!                    " << std::endl;
    }
    
    static std::string formatTime(double seconds) {
        if (seconds < 60.0) {
            return std::to_string(static_cast<int>(seconds)) + "s";
        } else if (seconds < 3600.0) {
            int minutes = static_cast<int>(seconds / 60.0);
            double remaining = seconds - minutes * 60.0;
            return std::to_string(minutes) + "m " + std::to_string(static_cast<int>(remaining)) + "s";
        } else {
            int hours = static_cast<int>(seconds / 3600.0);
            double remaining = seconds - hours * 3600.0;
            int minutes = static_cast<int>(remaining / 60.0);
            remaining -= minutes * 60.0;
            return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(static_cast<int>(remaining)) + "s";
        }
    }
    
    static std::string formatNumber(size_t number) {
        std::stringstream ss;
        ss << number;
        return ss.str();
    }
    
    static std::string formatFloat(float value, int precision = 3) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }
};

// Configuration utilities for demo system
class DemoConfiguration {
public:
    struct DemoSettings {
        int numEpisodes = 5;
        int stepsPerEpisode = 1000;
        double simulationTimestep = 0.001;
        int64_t neuronCount = 500;
        int64_t regionCount = 1;
        float connectionProbability = 0.15f;
        int64_t randomSeed = 42;
        float stdpLtpWeight = 0.02f;
        float stdpLtdWeight = 0.015f;
        float stdpTau = 20.0f;
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        bool enableRewardModulation = false;
        bool enableStructuralPlasticity = false;
        bool enableDevelopment = false;
        bool enableCuriosity = false;
        bool verboseLogging = false;
        std::string outputFile;
        
        // Parse command line arguments
        bool parseArgs(int argc, char** argv) {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                if (arg == "--help" || arg == "-h") {
                    showHelp();
                    exit(0);
                } else if (arg == "--verbose" || arg == "-v") {
                    verboseLogging = true;
                } else if (arg == "--output" && i + 1 < argc) {
                    outputFile = argv[++i];
                } else if (arg.find("--episodes=") == 0) {
                    numEpisodes = std::stoi(arg.substr(11));
                } else if (arg.find("--steps=") == 0) {
                    stepsPerEpisode = std::stoi(arg.substr(8));
                } else if (arg.find("--neurons=") == 0) {
                    neuronCount = std::stoll(arg.substr(9));
                } else if (arg.find("--seed=") == 0) {
                    randomSeed = std::stoll(arg.substr(7));
                } else if (arg.find("--timestep=") == 0) {
                    simulationTimestep = std::stod(arg.substr(11));
                } else if (arg.find("--connection=") == 0) {
                    connectionProbability = std::stof(arg.substr(12));
                } else if (arg == "--no-plasticity") {
                    enableStructuralPlasticity = false;
                } else if (arg == "--no-rewards") {
                    enableRewardModulation = false;
                } else if (arg == "--no-development") {
                    enableDevelopment = false;
                } else if (arg == "--no-curiosity") {
                    enableCuriosity = false;
                }
            }
            return true;
        }
        
        void showHelp() {
            std::cout << "Demo Configuration Options:" << std::endl;
            std::cout << "  --help, -h                    Show this help message" << std::endl;
            std::cout << "  --verbose, -v                 Enable verbose logging" << std::endl;
            std::cout << "  --output <file>               Save output to file" << std::endl;
            std::cout << "  --episodes <n>                Number of episodes (default: 5)" << std::endl;
            std::cout << "  --steps <n>                   Steps per episode (default: 1000)" << std::endl;
            std::cout << "  --neurons <n>                 Number of neurons (default: 500)" << std::endl;
            std::cout << "  --seed <n>                    Random seed (default: 42)" << std::endl;
            std::cout << "  --timestep <s>                 Simulation timestep (default: 0.001)" << std::endl;
            std::cout << "  --connection <p>               Connection probability (default: 0.15)" << std::endl;
            std::cout << "  --no-plasticity               Disable structural plasticity" << std::endl;
            std::cout << "  --no-rewards                   Disable reward modulation" << std::endl;
            std::cout << "  --no-development               Disable development" << std::endl;
            std::cout << "  --no-curiosity                 Disable curiosity" << std::endl;
        }
        
        void applyToConfig(std::shared_ptr<Config> config) {
            if (config) {
                config->set("random_seed", randomSeed, ConfigSource::Runtime);
                config->set("simulation_timestep", simulationTimestep, ConfigSource::Runtime);
                config->set("neuron_count", neuronCount, ConfigSource::Runtime);
                config->set("region_count", regionCount, ConfigSource::Runtime);
                config->set("connection_probability", connectionProbability, ConfigSource::Runtime);
                config->set("stdp_ltp_weight", stdpLtpWeight, ConfigSource::Runtime);
                config->set("stdp_ltd_weight", stdpLtdWeight, ConfigSource::Runtime);
                config->set("stdp_tau", stdpTau, ConfigSource::Runtime);
                config->set("synaptogenesis_rate", synaptogenesisRate, ConfigSource::Runtime);
                config->set("pruning_rate", pruningRate, ConfigSource::Runtime);
            }
        }
    };
    
    static DemoSettings parseDemoSettings(int argc, char** argv) {
        DemoSettings settings;
        settings.parseArgs(argc, argv);
        return settings;
    }
};

// Result tracking and metrics collection
class DemoMetrics {
public:
    struct TestMetrics {
        std::string testName;
        double executionTime;
        bool success;
        std::string details;
        
        TestMetrics(const std::string& name) : testName(name), executionTime(0.0), success(false) {}
    };
    
    struct ExperimentMetrics {
        int episodes;
        int stepsPerEpisode;
        double totalReward;
        double averageFiringRate;
        size_t finalSynapseCount;
        size_t finalNeuronCount;
        double curiosityLevel;
        double noveltyLevel;
        double predictionError;
        std::vector<TestMetrics> testResults;
        
        void reset() {
            totalReward = 0.0;
            averageFiringRate = 0.0;
            finalSynapseCount = 0;
            finalNeuronCount = 0;
            curiosityLevel = 0.0;
            noveltyLevel = 0.0;
            predictionError = 0.0;
            testResults.clear();
        }
    };
    
    static void logMetrics(const ExperimentMetrics& metrics, std::ostream& output = std::cout) {
        output << "\n" << std::string(60, '=') << std::endl;
        output << "EXPERIMENT METRICS" << std::endl;
        output << std::string(60, '=') << std::endl;
        
        output << "Experiment Configuration:" << std::endl;
        output << "  Episodes: " << metrics.episodes << std::endl;
        output << "  Steps per episode: " << metrics.stepsPerEpisode << std::endl;
        
        output << "\nPerformance Metrics:" << std::endl;
        output << "  Total Reward: " << metrics.totalReward << std::endl;
        output << "  Average Firing Rate: " << metrics.averageFiringRate << " Hz" << std::endl;
        output << "  Final Synapse Count: " << metrics.finalSynapseCount << std::endl;
        output << "  Final Neuron Count: " << metrics.finalNeuronCount << std::endl;
        
        output << "\nNeuromodulation State:" << std::endl;
        output << "  Curiosity Level: " << metrics.curiosityLevel << std::endl;
        output << "  Novelty Level: " << metrics.noveltyLevel << std::endl;
        output << "  Prediction Error: " << metrics.predictionError << std::endl;
        
        output << "\nTest Results:" << std::endl;
        for (const auto& test : metrics.testResults) {
            output << "  " << test.testName << ": " << (test.success ? "PASSED" : "FAILED") << std::endl;
            if (!test.details.empty()) {
                output << "    " << test.details << std::endl;
            }
        }
        
        output << std::endl;
    }
    
    static std::string metricsToString(const ExperimentMetrics& metrics) {
        std::stringstream ss;
        ss << "Experiment completed in " << metrics.episodes << " episodes" << std::endl;
        ss << "Total reward: " << metrics.totalReward << std::endl;
        ss << "Final synapses: " << metrics.finalSynapseCount << std::endl;
        ss << "Average firing rate: " << metrics.averageFiringRate << " Hz" << std::endl;
        ss << "Curiosity: " << metrics.curiosityLevel << std::endl;
        
        return ss.str();
    }
};

// Progress tracking for long-running operations
class ProgressTracker {
public:
    ProgressTracker(size_t totalSteps, const std::string& description = "") 
        : totalSteps(totalSteps), completedSteps(0), description(description) {}
    
    void update(size_t steps = 1) {
        completedSteps += steps;
        if (completedSteps % 100 == 0 || completedSteps == totalSteps) {
            printProgress();
        }
    }
    
    void printProgress() {
        double percentage = (static_cast<double>(completedSteps) / totalSteps) * 100.0;
        std::cout << "\r" << std::left << std::setw(40) << (description.empty() ? "Processing" : description) 
                  << " " << std::setw(6) << std::fixed << std::setprecision(1) << percentage << "% "
                  << "[" << completedSteps << "/" << totalSteps << "]" << std::flush;
        
        if (completedSteps >= totalSteps) {
            std::cout << "\r" << std::left << std::setw(40) << (description.empty() ? "Processing" : description)
                      << " " << std::setw(6) << "100.0% "
                      << "[" << totalSteps << "/" << totalSteps << "]" << std::endl;
            std::cout << "Done!" << std::endl;
        }
    }
    
private:
    size_t totalSteps;
    size_t completedSteps;
    std::string description;
};

// Experiment configuration and validation
class DemoValidator {
public:
    static bool validateConfiguration(const DemoConfiguration::DemoSettings& settings) {
        bool valid = true;
        
        if (settings.simulationTimestep <= 0.0) {
            std::cerr << "Error: simulation_timestep must be positive" << std::endl;
            valid = false;
        }
        
        if (settings.neuronCount <= 0) {
            std::cerr << "Error: neuron_count must be positive" << std::endl;
            valid = false;
        }
        
        if (settings.regionCount <= 0) {
            std::cerr << "Error: region_count must be positive" << std::endl;
            valid = false;
        }
        
        if (settings.connectionProbability < 0.0f || settings.connectionProbability > 1.0f) {
            std::cerr << "Error: connection_probability must be between 0.0 and 1.0" << std::endl;
            valid = false;
        }
        
        if (settings.stepsPerEpisode <= 0) {
            std::cerr << "Error: stepsPerEpisode must be positive" << std::endl;
            valid = false;
        }
        
        if (settings.numEpisodes <= 0) {
            std::cerr << "Error: numEpisodes must be positive" << std::endl;
            valid = false;
        }
        
        return valid;
    }
    
    static bool validateEnvironment() {
        // Check if required system resources are available
        return true; // Placeholder - could check for available memory, etc.
    }
};

// File utilities for saving experiment results
class FileUtils {
public:
    static bool writeToFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return false;
        }
        
        file << content;
        file.close();
        return true;
    }
    
    static bool ensureDirectory(const std::string& path) {
        // Implementation would create directories if they don't exist
        return true; // Placeholder
    }
    
    static std::string generateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

} // namespace nlm

// Additional forward declarations for demo functions
bool runBasicConnectivityTest(std::shared_ptr<Brain> brain);
bool runPlasticityExperiment(std::shared_ptr<Brain> brain);
bool runStdpVerification(std::shared_ptr<Brain> brain);
bool runLearningExperiment(std::shared_ptr<Brain> brain);
bool runCompleteDemo(nlm::DemoConfiguration::DemoSettings& settings, nlm::DemoMetrics::ExperimentMetrics& metrics);
bool runPhase3Demo(nlm::DemoConfiguration::DemoSettings& settings, nlm::DemoMetrics::ExperimentMetrics& metrics);
bool runPhase4Demo(nlm::DemoConfiguration::DemoSettings& settings, nlm::DemoMetrics::ExperimentMetrics& metrics);

// Modular demo runner that can run different types of demonstrations
class DemoRunner {
public:
    enum class DemoType {
        Phase2,
        Phase3,
        Phase4,
        Phase6,
        Complete
    };
    
    DemoRunner() {
        // Register available demo types
        demos[DemoType::Phase2] = {"Phase 2: Basic Neural Computation",
            "Demonstrates LIF neuron dynamics, STDP, and Hebbian plasticity",
            &runBasicConnectivityTest, &runPlasticityExperiment, &runStdpVerification};
        
        demos[DemoType::Complete] = {"Complete Demo",
            "Runs all available Phase 2 tests in sequence",
            &runCompleteDemo, nullptr, nullptr};
        
        demos[DemoType::Phase3] = {"Phase 3: World Interaction",
            "Demonstrates agent learning in a simulated environment",
            nullptr, &runPhase3Demo, nullptr};
        
        demos[DemoType::Phase4] = {"Phase 4: Emerging Cognition",
            "Demonstrates prediction, memory, and concept formation",
            nullptr, nullptr, &runPhase4Demo};
        
        demos[DemoType::Phase6] = {"Phase 6: Integration Demo",
            "Comprehensive integration test of all brain systems",
            nullptr, nullptr, nullptr}; // Would need implementation
    }
    
    bool runDemo(DemoType type, nlm::DemoConfiguration::DemoSettings& settings, 
                nlm::DemoMetrics::ExperimentMetrics& metrics) {
        auto it = demos.find(type);
        if (it == demos.end()) {
            std::cerr << "Error: Unknown demo type" << std::endl;
            return false;
        }
        
        const DemoInfo& demo = it->second;
        std::cout << "Running: " << demo.name << std::endl;
        std::cout << "Description: " << demo.description << std::endl;
        std::cout << std::endl;
        
        bool success = false;
        
        if (demo.phase2Runner) {
            success = demo.phase2Runner(settings, metrics);
        } else if (demo.phase3Runner) {
            success = demo.phase3Runner(settings, metrics);
        } else if (demo.phase4Runner) {
            success = demo.phase4Runner(settings, metrics);
        }
        
        return success;
    }
    
    void listAvailableDemos() {
        std::cout << "Available Demos:" << std::endl;
        std::cout << "  --demo=phase2        Phase 2: Basic Neural Computation" << std::endl;
        std::cout << "  --demo=complete      Complete Demo (all tests)" << std::endl;
        std::cout << "  --demo=phase3        Phase 3: World Interaction" << std::endl;
        std::cout << "  --demo=phase4        Phase 4: Emerging Cognition" << std::endl;
        std::cout << "  --demo=phase6        Phase 6: Integration (experimental)" << std::endl;
        std::cout << std::endl;
        std::cout << "Use --demo=<type> with --run to execute specific demos." << std::endl;
    }
    
    std::string getDemoDescription(DemoType type) {
        auto it = demos.find(type);
        if (it != demos.end()) {
            return it->second.description;
        }
        return "Unknown demo type";
    }
    
    struct DemoInfo {
        std::string name;
        std::string description;
        std::function<bool(nlm::DemoConfiguration::DemoSettings&, nlm::DemoMetrics::ExperimentMetrics&)> phase2Runner;
        std::function<bool(nlm::DemoConfiguration::DemoSettings&, nlm::DemoMetrics::ExperimentMetrics&)> phase3Runner;
        std::function<bool(nlm::DemoConfiguration::DemoSettings&, nlm::DemoMetrics::ExperimentMetrics&)> phase4Runner;
    };
    
private:
    std::unordered_map<DemoType, DemoInfo> demos;
};

// Helper functions to parse demo type from string
DemoRunner::DemoType parseDemoType(const std::string& typeStr) {
    if (typeStr == "phase2") return DemoRunner::DemoType::Phase2;
    if (typeStr == "complete") return DemoRunner::DemoType::Complete;
    if (typeStr == "phase3") return DemoRunner::DemoType::Phase3;
    if (typeStr == "phase4") return DemoRunner::DemoType::Phase4;
    if (typeStr == "phase6") return DemoRunner::DemoType::Phase6;
    return DemoRunner::DemoType::Phase2; // default
}

} // namespace nlm

// Global demo runner instance
nlm::DemoRunner globalDemoRunner;

// Main demo execution function for Phase 2
bool runPhase2Demo(nlm::DemoConfiguration::DemoSettings& settings, nlm::DemoMetrics::ExperimentMetrics& metrics) {
    std::cout << "Phase 2 Demo Execution" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << std::endl;
    
    metrics.reset();
    metrics.episodes = settings.numEpisodes;
    metrics.stepsPerEpisode = settings.stepsPerEpisode;
    
    auto config = std::make_shared<Config>();
    settings.applyToConfig(config);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Simulation timestep: " << settings.simulationTimestep << std::endl;
    std::cout << std::endl;
    
    // Run the three main tests
    nlm::ProgressTracker progress(settings.stepsPerEpisode * 3, "Running Phase 2 tests");
    
    std::cout << "Running Test 1: Basic Connectivity" << std::endl;
    auto testStart = std::chrono::high_resolution_clock::now();
    if (!runBasicConnectivityTest(brain)) {
        metrics.testResults.push_back({"Basic Connectivity", 0.0, false, "Test failed"});
        return false;
    }
    auto testEnd = std::chrono::high_resolution_clock::now();
    double testDuration = std::chrono::duration<double>(testEnd - testStart).count();
    metrics.testResults.push_back({"Basic Connectivity", testDuration, true, "Spikes propagated successfully"});
    progress.update(settings.stepsPerEpisode);
    
    brain->reset();
    brain->initialize();
    
    std::cout << "\nRunning Test 2: Plasticity Experiment" << std::endl;
    testStart = std::chrono::high_resolution_clock::now();
    if (!runPlasticityExperiment(brain)) {
        metrics.testResults.push_back({"Plasticity", 0.0, false, "Test failed"});
        return false;
    }
    testEnd = std::chrono::high_resolution_clock::now();
    testDuration = std::chrono::duration<double>(testEnd - testStart).count();
    metrics.testResults.push_back({"Plasticity", testDuration, true, "Learning detected"});
    progress.update(settings.stepsPerEpisode);
    
    brain->reset();
    brain->initialize();
    
    std::cout << "\nRunning Test 3: STDP Verification" << std::endl;
    testStart = std::chrono::high_resolution_clock::now();
    if (!runStdpVerification(brain)) {
        metrics.testResults.push_back({"STDP", 0.0, false, "Test failed"});
        return false;
    }
    testEnd = std::chrono::high_resolution_clock::now();
    testDuration = std::chrono::duration<double>(testEnd - testStart).count();
    metrics.testResults.push_back({"STDP", testDuration, true, "LTP detected"});
    progress.update(settings.stepsPerEpisode);
    
    // Collect final metrics
    metrics.totalSynapses = brain->getTotalSynapseCount();
    metrics.averageFiringRate = brain->getAverageFiringRate();
    metrics.finalEnergy = 100.0f; // Placeholder - would need to track energy
    metrics.finalNovelty = 0.5f; // Placeholder - would need neuromodulation data
    metrics.finalCuriosity = 0.3f; // Placeholder
    
    std::cout << "\nPhase 2 Demo completed successfully" << std::endl;
    return true;
}

// Complete demo that runs all available demonstrations
bool runCompleteDemo(nlm::DemoConfiguration::DemoSettings& settings, nlm::DemoMetrics::ExperimentMetrics& metrics) {
    std::cout << "Complete Demo - Running All Available Demonstrations" << std::endl;
    std::cout << "=====================================================" << std::endl;
    
    bool success = true;
    
    // Run Phase 2 demo
    if (!runPhase2Demo(settings, metrics)) {
        success = false;
    }
    
    // Could add other demos here
    // if (!runPhase3Demo(settings, metrics)) {
    //     success = false;
    // }
    
    return success;
}

// Phase 3 demo (would need to be implemented separately)
bool runPhase3Demo(nlm::DemoConfiguration::DemoSettings& settings, nlm::DemoMetrics::ExperimentMetrics& metrics) {
    std::cout << "Phase 3 Demo not yet implemented." << std::endl;
    return true; // Placeholder
}

// Phase 4 demo (would need to be implemented separately)
bool runPhase4Demo(nlm::DemoConfiguration::DemoSettings& settings, nlm::DemoMetrics::ExperimentMetrics& metrics) {
    std::cout << "Phase 4 Demo not yet implemented." << std::endl;
    return true; // Placeholder
}