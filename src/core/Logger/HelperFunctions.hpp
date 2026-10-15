// Common logging and utility functions for NLM
// This file provides helper functions to reduce code duplication
// and improve maintainability across the NLM codebase

#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;
class Brain;

// =============================================================================
// LOGGING HELPER FUNCTIONS
// =============================================================================

// Enhanced logging functions with better formatting and consistency

void logConfigurationSummary(const Config& config) {
    NLM_LOG_INFO("Configuration Summary:");
    std::vector<std::string> keys = config.getKeys();
    for (const auto& key : keys) {
        auto value = config.get<std::string>(key);
        if (value) {
            NLM_LOG_INFO("  " + key + " = \"" + *value + "\"");
        } else {
            // Try to get numeric values
            auto intVal = config.get<int64_t>(key);
            if (intVal) {
                NLM_LOG_INFO("  " + key + " = " + std::to_string(*intVal));
            } else {
                auto floatVal = config.get<double>(key);
                if (floatVal) {
                    NLM_LOG_INFO("  " + key + " = " + std::to_string(*floatVal));
                } else {
                    auto boolVal = config.get<bool>(key);
                    if (boolVal) {
                        NLM_LOG_INFO("  " + key + " = " + std::string(*boolVal ? "true" : "false"));
                    }
                }
            }
        }
    }
}

void logBrainStatus(const Brain& brain, const std::string& prefix = "") {
    NLM_LOG_INFO(prefix + "=== Brain Status ===");
    NLM_LOG_INFO(prefix + "Regions: " + std::to_string(brain.getRegionCount()));
    NLM_LOG_INFO(prefix + "Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO(prefix + "Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO(prefix + "Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    NLM_LOG_INFO(prefix + "Firing neurons (this step): " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_INFO(prefix + "Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
    NLM_LOG_INFO(prefix + "Average firing rate: " + std::to_string(brain.getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO(prefix + "Excitatory/Inhibitory ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));
}

void logTestHeader(const std::string& testName, const std::string& description = "") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== " + testName + " ===");
    if (!description.empty()) {
        NLM_LOG_INFO(description);
    }
}

void logTestResult(const std::string& testName, bool passed, const std::string& details = "") {
    NLM_LOG_INFO("");
    if (passed) {
        NLM_LOG_INFO("✓ " + testName + " PASSED");
    } else {
        NLM_LOG_INFO("✗ " + testName + " FAILED");
        if (!details.empty()) {
            NLM_LOG_INFO("  Details: " + details);
        }
    }
}

void logProgress(const std::string& message, int step, int total_steps) {
    NLM_LOG_INFO(std::to_string(step) + "/" + std::to_string(total_steps) + ": " + message);
    
    // Log progress with visual indicator every 10% of total steps
    if (total_steps > 0 && step % std::max(1, total_steps / 10) == 0) {
        int progress = (step * 100) / total_steps;
        std::string bar = "=";
        for (int i = 0; i < progress / 5; ++i) bar += "=";
        bar += ">";
        for (int i = progress / 5; i < 20; ++i) bar += "=";
        
        NLM_LOG_INFO(std::to_string(progress) + "% " + bar + " " + std::to_string(step) + "/" + std::to_string(total_steps));
    }
}

void logError(const std::string& component, const std::string& error, bool fatal = false) {
    if (fatal) {
        NLM_LOG_ERROR(std::string("FATAL ERROR in ") + component + ": " + error);
        throw std::runtime_error(std::string("Fatal error in ") + component + ": " + error);
    } else {
        NLM_LOG_WARNING(std::string("WARNING in ") + component + ": " + error);
    }
}

// =============================================================================
// VALIDATION HELPER FUNCTIONS
// =============================================================================

bool validateBrainConfiguration(const Config& config) {
    bool valid = true;
    
    // Check for required configuration parameters
    std::vector<std::string> required_keys = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep"
    };
    
    for (const auto& key : required_keys) {
        if (!config.has(key)) {
            NLM_LOG_WARNING("Missing required configuration parameter: " + key);
            valid = false;
        }
    }
    
    // Validate parameter ranges
    auto neuron_count = config.getOr<int64_t>("neuron_count", 0);
    if (neuron_count <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: must be positive");
        valid = false;
    } else if (neuron_count > 10000) {
        NLM_LOG_WARNING("Large neuron_count may impact performance: " + std::to_string(neuron_count));
    }
    
    auto connection_prob = config.getOr<double>("connection_probability", 0.0);
    if (connection_prob < 0.0 || connection_prob > 1.0) {
        NLM_LOG_ERROR("Invalid connection_probability: must be between 0 and 1");
        valid = false;
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: must be positive");
        valid = false;
    } else if (timestep > 0.01) {
        NLM_LOG_WARNING("Large timestep may impact stability: " + std::to_string(timestep));
    }
    
    return valid;
}

bool validateWorldConfiguration(const Config& config) {
    bool valid = true;
    
    // Validate world-related parameters if they exist
    auto world_width = config.getOr<int64_t>("world.width", 0);
    if (world_width > 0 && world_width < 10) {
        NLM_LOG_WARNING("World width may be too small for meaningful simulation: " + std::to_string(world_width));
    }
    
    auto world_height = config.getOr<int64_t>("world.height", 0);
    if (world_height > 0 && world_height < 10) {
        NLM_LOG_WARNING("World height may be too small for meaningful simulation: " + std::to_string(world_height));
    }
    
    return valid;
}

// =============================================================================
// SIMULATION HELPER FUNCTIONS
// =============================================================================

void safeBrainStep(Brain& brain, SimulationStep step, Timestamp time) {
    try {
        brain.step(step, time);
    } catch (const std::exception& e) {
        logError("Brain.step", std::string("Step ") + std::to_string(step) + ": " + e.what());
        // Try to recover by resetting and reinitializing
        brain.reset();
        // Note: Brain doesn't have a reinitialize method, so this is a simplified recovery
        throw;
    }
}

void safeWorldUpdate(class SimpleWorld& world, double timestep) {
    try {
        world.update(timestep);
    } catch (const std::exception& e) {
        logError("World.update", std::string("Timestep ") + std::to_string(timestep) + ": " + e.what());
        throw;
    }
}

void safeAgentProcess(class AgentBrain& agent, const class SensoryPercept& percept) {
    try {
        agent.processSensoryInput(percept);
    } catch (const std::exception& e) {
        logError("Agent.processSensoryInput", std::string("Processing percept: ") + e.what());
        throw;
    }
}

// =============================================================================
// STATISTICS AND METRICS HELPER FUNCTIONS
// =============================================================================

struct SimulationStatistics {
    SimulationStep steps_completed;
    Timestamp time_elapsed;
    size_t total_spikes;
    size_t firing_neurons;
    double average_firing_rate;
    double memory_usage;
    double cpu_usage;
    
    SimulationStatistics() : 
        steps_completed(0), time_elapsed(0.0), total_spikes(0), 
        firing_neurons(0), average_firing_rate(0.0), 
        memory_usage(0.0), cpu_usage(0.0) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "SimulationStatistics{";
        oss << "steps_completed=" << steps_completed << ",";
        oss << "time_elapsed=" << time_elapsed << ",";
        oss << "total_spikes=" << total_spikes << ",";
        oss << "firing_neurons=" << firing_neurons << ",";
        oss << "average_firing_rate=" << average_firing_rate << ",";
        oss << "memory_usage=" << memory_usage << ",";
        oss << "cpu_usage=" << cpu_usage;
        oss << "}";
        return oss.str();
    }
};

SimulationStatistics collectSimulationStatistics(const Brain& brain, const SimpleWorld& world, const AgentBrain& agent) {
    SimulationStatistics stats;
    
    // Basic brain statistics
    stats.total_spikes = brain.getTotalSpikeCount();
    stats.firing_neurons = brain.getFiringNeuronCount();
    stats.average_firing_rate = brain.getAverageFiringRate();
    
    // World statistics (if available)
    stats.time_elapsed = world.getSimulationTime();
    
    // Agent statistics (if available)
    stats.memory_usage = agent.getNeuromodulationLevel();  // Simple proxy
    
    // CPU usage would require system-specific code
    stats.cpu_usage = 0.0;
    
    return stats;
}

void printStatistics(const SimulationStatistics& stats) {
    NLM_LOG_INFO("=== Simulation Statistics ===");
    NLM_LOG_INFO("Steps completed: " + std::to_string(stats.steps_completed));
    NLM_LOG_INFO("Time elapsed: " + std::to_string(stats.time_elapsed) + " seconds");
    NLM_LOG_INFO("Total spikes: " + std::to_string(stats.total_spikes));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(stats.firing_neurons));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(stats.average_firing_rate) + " Hz");
    NLM_LOG_INFO("Memory usage: " + std::to_string(stats.memory_usage) + " units");
    NLM_LOG_INFO("CPU usage: " + std::to_string(stats.cpu_usage) + " %");
}

// =============================================================================
// EXPERIMENT CONTROL HELPER FUNCTIONS
// =============================================================================

class ExperimentController {
public:
    ExperimentController(Brain& brain, SimpleWorld& world, AgentBrain& agent)
        : brain_(brain), world_(world), agent_(agent), running_(false) {}
    
    ~ExperimentController() {
        stop();
    }
    
    void start(SimulationStep total_steps, double timestep = 0.1) {
        if (running_) {
            logError("ExperimentController", "Experiment already running");
            return;
        }
        
        running_ = true;
        total_steps_ = total_steps;
        timestep_ = timestep;
        current_step_ = 0;
        
        NLM_LOG_INFO("Starting experiment: " + std::to_string(total_steps) + " steps");
    }
    
    void stop() {
        if (running_) {
            running_ = false;
            NLM_LOG_INFO("Experiment stopped");
        }
    }
    
    bool isRunning() const {
        return running_;
    }
    
    void step() {
        if (!running_) {
            logError("ExperimentController", "Cannot step: experiment not running");
            return;
        }
        
        if (current_step_ >= total_steps_) {
            stop();
            NLM_LOG_INFO("Experiment completed");
            return;
        }
        
        // Execute one simulation step
        try {
            safeWorldUpdate(world_, timestep_);
            safeAgentProcess(agent_, world_.getSensoryPercept());
            safeBrainStep(brain_, current_step_, current_step_ * timestep_);
            agent_.decodeMotorCommand(); // This is a placeholder - actual implementation would need to apply actions
            
            current_step_++;
            
            NLM_LOG_INFO("Step " + std::to_string(current_step_) + "/" + std::to_string(total_steps_));
            
        } catch (const std::exception& e) {
            logError("ExperimentController.step", std::string("Error at step ") + std::to_string(current_step_) + ": " + e.what());
            stop();
            throw;
        }
    }
    
    double getProgress() const {
        if (total_steps_ == 0) return 0.0;
        return static_cast<double>(current_step_) / total_steps_;
    }
    
    SimulationStep getCurrentStep() const {
        return current_step_;
    }
    
private:
    Brain& brain_;
    SimpleWorld& world_;
    AgentBrain& agent_;
    bool running_;
    SimulationStep total_steps_;
    double timestep_;
    SimulationStep current_step_;
};

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

std::string formatTime(double seconds) {
    if (seconds < 60.0) {
        return std::to_string(seconds) + " seconds";
    } else if (seconds < 3600.0) {
        double minutes = seconds / 60.0;
        if (minutes < 10.0) {
            return std::to_string(static_cast<int>(minutes)) + " minute " + std::to_string(static_cast<int>(seconds % 60)) + " seconds";
        } else {
            return std::to_string(minutes) + " minutes";
        }
    } else {
        double hours = seconds / 3600.0;
        double minutes = std::fmod(seconds, 3600.0) / 60.0;
        return std::to_string(static_cast<int>(hours)) + " hours, " + std::to_string(static_cast<int>(minutes)) + " minutes";
    }
}

std::string formatNumber(size_t number) {
    std::string str = std::to_string(number);
    std::string result;
    int count = 0;
    
    for (int i = str.size() - 1; i >= 0; --i) {
        result = str[i] + result;
        count++;
        
        if (count % 3 == 0 && i > 0) {
            result = ',' + result;
        }
    }
    
    return result;
}

// =============================================================================
// TESTING HELPER FUNCTIONS
// =============================================================================

class SimpleTestSuite {
public:
    SimpleTestSuite(const std::string& name) : name_(name), passed_(0), failed_(0) {}
    
    void addTest(const std::string& test_name, std::function<bool()> test_func) {
        tests_.push_back({test_name, test_func});
    }
    
    void runAll() {
        NLM_LOG_INFO("Running test suite: " + name_);
        NLM_LOG_INFO("Total tests: " + std::to_string(tests_.size()));
        
        for (const auto& test : tests_) {
            logTestHeader(test.name);
            bool test_passed = false;
            std::string error_msg;
            
            try {
                test_passed = test.func();
            } catch (const std::exception& e) {
                error_msg = e.what();
                test_passed = false;
            }
            
            logTestResult(test.name, test_passed, error_msg);
            
            if (test_passed) {
                passed_++;
            } else {
                failed_++;
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test Suite Summary ===");
        NLM_LOG_INFO("Tests passed: " + std::to_string(passed_));
        NLM_LOG_INFO("Tests failed: " + std::to_string(failed_));
        NLM_LOG_INFO("Total tests: " + std::to_string(tests_.size()));
        
        if (failed_ > 0) {
            NLM_LOG_ERROR("Some tests failed");
            throw std::runtime_error("Test suite failed");
        }
    }
    
    int getPassedCount() const {
        return passed_;
    }
    
    int getFailedCount() const {
        return failed_;
    }
    
private:
    std::string name_;
    std::vector<std::pair<std::string, std::function<bool()>>> tests_;
    int passed_;
    int failed_;
};

// Convenience function for simple test
bool runSingleTest(const std::string& test_name, std::function<bool()> test_func) {
    logTestHeader(test_name);
    bool passed = false;
    std::string error_msg;
    
    try {
        passed = test_func();
    } catch (const std::exception& e) {
        error_msg = e.what();
        passed = false;
    }
    
    logTestResult(test_name, passed, error_msg);
    return passed;
}

} // namespace nlm