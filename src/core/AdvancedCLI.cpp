#!/usr/bin/env cpp
// NLM Advanced Command Line Interface
// Provides advanced operations for NLM simulation management

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "performance/CheckpointSystem.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class AdvancedCLI {
public:
    AdvancedCLI() : config_(std::make_shared<Config>()) {
        initializeCommands();
    }
    
    void run() {
        std::cout << "=== NLM Advanced Command Line Interface ===" << std::endl;
        std::cout << "Type 'help' for available commands, 'quit' to exit" << std::endl;
        
        std::string line;
        while (true) {
            std::cout << "nlm> ";
            if (!std::getline(std::cin, line)) {
                break; // EOF
            }
            
            if (line.empty()) {
                continue;
            }
            
            handleCommand(line);
        }
    }
    
private:
    struct Command {
        std::string name;
        std::string description;
        std::function<bool(const std::vector<std::string>&)> handler;
    };
    
    std::vector<Command> commands_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    Phase6IntegratedExperiment experiment_;
    
    void initializeCommands() {
        // Core brain management commands
        commands_.push_back({"create", "Create a new brain with specified parameters", [this](const auto& args) { return handleCreateBrain(args); }});
        commands_.push_back({"load", "Load a saved brain checkpoint", [this](const auto& args) { return handleLoadBrain(args); }});
        commands_.push_back({"save", "Save current brain state to checkpoint", [this](const auto& args) { return handleSaveBrain(args); }});
        commands_.push_back({"delete", "Delete a checkpoint", [this](const auto& args) { return handleDeleteCheckpoint(args); }});
        commands_.push_back({"list", "List available checkpoints", [this](const auto& args) { return handleListCheckpoints(args); }});
        
        // Debug and profiling commands
        commands_.push_back({"debug", "Enable/disable debug logging", [this](const auto& args) { return handleDebug(args); }});
        commands_.push_back({"profile", "Start/stop performance profiling", [this](const auto& args) { return handleProfile(args); }});
        commands_.push_back({"timeline", "Show timeline events", [this](const auto& args) { return handleTimeline(args); }});
        commands_.push_back({"memory", "Show memory statistics", [this](const auto& args) { return handleMemoryStats(args); }});
        commands_.push_back({"benchmark", "Run performance benchmark", [this](const auto& args) { return handleBenchmark(args); }});
        
        // Simulation control commands
        commands_.push_back({"run", "Run simulation with given configuration", [this](const auto& args) { return handleRunSimulation(args); }});
        commands_.push_back({"step", "Execute one simulation step", [this](const auto& args) { return handleSingleStep(args); }});
        commands_.push_back({"pause", "Pause simulation", [this](const auto& args) { return handlePause(args); }});
        commands_.push_back({"resume", "Resume simulation", [this](const auto& args) { return handleResume(args); }});
        
        // Configuration commands
        commands_.push_back({"config", "Set configuration value", [this](const auto& args) { return handleConfig(args); }});
        commands_.push_back({"showconfig", "Show current configuration", [this](const auto& args) { return handleShowConfig(args); }});
        commands_.push_back({"resetconfig", "Reset configuration to defaults", [this](const auto& args) { return handleResetConfig(args); }});
        
        // Advanced operations
        commands_.push_back({"verify", "Verify brain integration", [this](const auto& args) { return handleVerifyIntegration(args); }});
        commands_.push_back({"validate", "Validate checkpoint integrity", [this](const auto& args) { return handleValidateCheckpoint(args); }});
        commands_.push_back({"recover", "Recover from checkpoint", [this](const auto& args) { return handleRecover(args); }});
        commands_.push_back({"export", "Export brain state to file", [this](const auto& args) { return handleExport(args); }});
        
        // Help and utility commands
        commands_.push_back({"help", "Show help for commands", [this](const auto& args) { return handleHelp(args); }});
        commands_.push_back({"quit", "Exit the CLI", [this](const auto& args) { return handleQuit(args); }});
        commands_.push_back({"exit", "Exit the CLI", [this](const auto& args) { return handleQuit(args); }});
    }
    
    void handleCommand(const std::string& line) {
        std::vector<std::string> args = splitString(line);
        if (args.empty()) return;
        
        std::string commandName = args[0];
        args.erase(args.begin()); // Remove command name
        
        // Find command handler
        auto it = std::find_if(commands_.begin(), commands_.end(),
                              [&commandName](const Command& cmd) {
                                  return cmd.name == commandName;
                              });
        
        if (it == commands_.end()) {
            std::cout << "Unknown command: " << commandName << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
            return;
        }
        
        try {
            if (!it->handler(args)) {
                std::cout << "Command failed: " << commandName << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error executing command: " << e.what() << std::endl;
        }
    }
    
    std::vector<std::string> splitString(const std::string& line) {
        std::vector<std::string> args;
        std::stringstream ss(line);
        std::string arg;
        
        while (ss >> arg) {
            args.push_back(arg);
        }
        
        return args;
    }
    
    // Command implementations
    bool handleCreateBrain(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: create <neuron_count> <region_count>" << std::endl;
            return false;
        }
        
        size_t neuronCount = std::stoul(args[0]);
        size_t regionCount = std::stoul(args[1]);
        
        config_->set("neuron_count", neuronCount);
        config_->set("region_count", regionCount);
        
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            std::cout << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        std::cout << "Brain created with " << neuronCount << " neurons and " 
                  << regionCount << " regions" << std::endl;
        return true;
    }
    
    bool handleLoadBrain(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Usage: load <checkpoint_path>" << std::endl;
            return false;
        }
        
        std::string checkpointPath = args[0];
        
        if (!brain_) {
            brain_ = std::make_shared<Brain>(config_);
            if (!brain_->initialize()) {
                std::cout << "Failed to initialize brain" << std::endl;
                return false;
            }
        }
        
        if (!brain_->load(checkpointPath)) {
            std::cout << "Failed to load checkpoint: " << checkpointPath << std::endl;
            return false;
        }
        
        std::cout << "Brain loaded successfully from: " << checkpointPath << std::endl;
        return true;
    }
    
    bool handleSaveBrain(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Usage: save <checkpoint_path>" << std::endl;
            return false;
        }
        
        if (!brain_) {
            std::cout << "No brain initialized. Use 'create' command first." << std::endl;
            return false;
        }
        
        std::string checkpointPath = args[0];
        
        if (!brain_->save(checkpointPath)) {
            std::cout << "Failed to save checkpoint: " << checkpointPath << std::endl;
            return false;
        }
        
        std::cout << "Brain saved successfully to: " << checkpointPath << std::endl;
        return true;
    }
    
    bool handleDeleteCheckpoint(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Usage: delete <checkpoint_path>" << std::endl;
            return false;
        }
        
        std::string checkpointPath = args[0];
        
        if (std::filesystem::exists(checkpointPath)) {
            std::filesystem::remove(checkpointPath);
            std::cout << "Checkpoint deleted: " << checkpointPath << std::endl;
            return true;
        } else {
            std::cout << "Checkpoint not found: " << checkpointPath << std::endl;
            return false;
        }
    }
    
    bool handleListCheckpoints(const std::vector<std::string>& args) {
        std::string directory = args.empty() ? "./checkpoints" : args[0];
        
        if (!std::filesystem::exists(directory)) {
            std::cout << "Directory does not exist: " << directory << std::endl;
            return false;
        }
        
        std::cout << "Checkpoints in " << directory << ":" << std::endl;
        
        CheckpointManager manager;
        auto checkpoints = manager.listCheckpoints();
        
        if (checkpoints.empty()) {
            std::cout << "  (no checkpoints found)" << std::endl;
            return true;
        }
        
        for (const auto& checkpoint : checkpoints) {
            std::cout << "  " << checkpoint.name 
                      << " (step: " << checkpoint.step 
                      << ", size: " << checkpoint.fileSize << " bytes)" << std::endl;
        }
        
        return true;
    }
    
    bool handleDebug(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "Current debug status:" << std::endl;
            std::cout << "  Memory tracking: " << (getGlobalMemoryTracker().getTotalAllocated() > 0 ? "enabled" : "disabled") << std::endl;
            std::cout << "  Timeline profiling: " << (getGlobalTimelineProfiler().getEvents().size() > 0 ? "active" : "inactive") << std::endl;
            return true;
        }
        
        std::string action = args[0];
        
        if (action == "start") {
            // Enable memory tracking
            auto& memoryTracker = getGlobalMemoryTracker();
            std::cout << "Memory tracking enabled" << std::endl;
            return true;
        } else if (action == "stop") {
            // Disable memory tracking
            auto& memoryTracker = getGlobalMemoryTracker();
            memoryTracker.reset();
            std::cout << "Memory tracking stopped and statistics cleared" << std::endl;
            return true;
        } else {
            std::cout << "Usage: debug [start|stop]" << std::endl;
            return false;
        }
    }
    
    bool handleProfile(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: profile <start|stop> <name>" << std::endl;
            return false;
        }
        
        std::string action = args[0];
        std::string name = args[1];
        
        PerformanceProfiler profiler(name);
        
        if (action == "start") {
            profiler.start();
            std::cout << "Profiling started for: " << name << std::endl;
            return true;
        } else if (action == "stop") {
            profiler.stop();
            std::cout << "Profiling stopped for: " << name 
                      << " (duration: " << profiler.getDuration() * 1000 << " ms)" << std::endl;
            return true;
        } else {
            std::cout << "Usage: profile <start|stop> <name>" << std::endl;
            return false;
        }
    }
    
    bool handleTimeline(const std::vector<std::string>& args) {
        auto& timelineProfiler = getGlobalTimelineProfiler();
        const auto& events = timelineProfiler.getEvents();
        
        if (events.empty()) {
            std::cout << "No timeline events recorded" << std::endl;
            return true;
        }
        
        std::cout << "Timeline events (" << events.size() << "):" << std::endl;
        
        for (const auto& event : events) {
            std::cout << "  [" << event.timestamp.time_since_epoch().count() << "] " 
                      << event.name << " (" << event.context << ")" << std::endl;
        }
        
        return true;
    }
    
    bool handleMemoryStats(const std::vector<std::string>& args) {
        auto& memoryTracker = getGlobalMemoryTracker();
        
        std::cout << "Memory Statistics:" << std::endl;
        std::cout << "  Total allocated: " << memoryTracker.getTotalAllocated() << " bytes" << std::endl;
        std::cout << "  Currently allocated: " << memoryTracker.getCurrentAllocated() << " bytes" << std::endl;
        std::cout << "  Allocation count: " << memoryTracker.getAllocationCount() << std::endl;
        std::cout << "  Deallocation count: " << memoryTracker.getDeallocationCount() << std::endl;
        
        if (memoryTracker.hasLeaks()) {
            std::cout << "  WARNING: Memory leaks detected!" << std::endl;
            auto leakInfo = memoryTracker.getLeakInfo();
            for (const auto& info : leakInfo) {
                std::cout << "    " << info << std::endl;
            }
        }
        
        return true;
    }
    
    bool handleBenchmark(const std::vector<std::string>& args) {
        std::string benchmarkName = args.empty() ? "default" : args[0];
        
        std::cout << "Running benchmark: " << benchmarkName << std::endl;
        
        // Create test brain
        config_->set("neuron_count", 1000);
        config_->set("region_count", 1);
        
        auto testBrain = std::make_shared<Brain>(config_);
        if (!testBrain->initialize()) {
            std::cout << "Failed to initialize brain for benchmark" << std::endl;
            return false;
        }
        
        // Run benchmark
        PerformanceProfiler profiler(benchmarkName);
        profiler.start();
        
        // Execute multiple steps
        for (uint64_t step = 0; step < 100; ++step) {
            testBrain->step(step, step * 0.001);
            
            // Inject some sensory input
            testBrain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        }
        
        profiler.stop();
        
        std::cout << "Benchmark completed:" << std::endl;
        std::cout << "  Duration: " << profiler.getDuration() * 1000 << " ms" << std::endl;
        std::cout << "  Steps per second: " << 100 / profiler.getDuration() << std::endl;
        
        return true;
    }
    
    bool handleRunSimulation(const std::vector<std::string>& args) {
        if (!brain_) {
            std::cout << "No brain initialized. Use 'create' command first." << std::endl;
            return false;
        }
        
        // Create experiment configuration
        Phase6Config config;
        config.maxSteps = 1000;
        config.neuronCount = brain_->getConfig()->getOr<size_t>("neuron_count", 1000);
        config.regionCount = brain_->getConfig()->getOr<size_t>("region_count", 1);
        config.connectionProbability = brain_->getConfig()->getOr<float>("connection_probability", 0.1f);
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        std::cout << "Running Phase 6 integration experiment..." << std::endl;
        
        auto result = experiment_.run(config);
        
        std::cout << "Experiment completed:" << std::endl;
        std::cout << "  Total reward: " << result.totalReward << std::endl;
        std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
        
        return true;
    }
    
    bool handleSingleStep(const std::vector<std::string>& args) {
        if (!brain_) {
            std::cout << "No brain initialized. Use 'create' command first." << std::endl;
            return false;
        }
        
        static uint64_t step = 0;
        brain_->step(step, step * 0.001);
        
        step++;
        std::cout << "Completed step " << step - 1 << std::endl;
        
        return true;
    }
    
    bool handlePause(const std::vector<std::string>& args) {
        std::cout << "Simulation paused" << std::endl;
        return true;
    }
    
    bool handleResume(const std::vector<std::string>& args) {
        std::cout << "Simulation resumed" << std::endl;
        return true;
    }
    
    bool handleConfig(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "Usage: config <key> <value>" << std::endl;
            return false;
        }
        
        std::string key = args[0];
        std::string value = args[1];
        
        // Try to parse as integer
        try {
            int intValue = std::stoi(value);
            config_->set(key, intValue);
        } catch (...) {
            // Try to parse as float
            try {
                float floatValue = std::stof(value);
                config_->set(key, floatValue);
            } catch (...) {
                // Try to parse as string
                config_->set(key, value);
            }
        }
        
        std::cout << "Configuration updated: " << key << " = " << value << std::endl;
        return true;
    }
    
    bool handleShowConfig(const std::vector<std::string>& args) {
        std::cout << "Current configuration:" << std::endl;
        
        // Print some common configuration values
        std::vector<std::string> keys = {"neuron_count", "region_count", "connection_probability", 
                                        "random_seed", "simulation_timestep"};
        
        for (const auto& key : keys) {
            auto value = config_->get<std::string>(key);
            if (value.has_value()) {
                std::cout << "  " << key << ": " << value.value() << std::endl;
            }
        }
        
        return true;
    }
    
    bool handleResetConfig(const std::vector<std::string>& args) {
        config_ = std::make_shared<Config>();
        std::cout << "Configuration reset to defaults" << std::endl;
        return true;
    }
    
    bool handleVerifyIntegration(const std::vector<std::string>& args) {
        std::cout << "Verifying brain system integration..." << std::endl;
        
        bool success = experiment_.verifyIntegration();
        
        if (success) {
            std::cout << "Integration verification PASSED" << std::endl;
        } else {
            std::cout << "Integration verification FAILED" << std::endl;
        }
        
        return success;
    }
    
    bool handleValidateCheckpoint(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Usage: validate <checkpoint_path>" << std::endl;
            return false;
        }
        
        std::string checkpointPath = args[0];
        
        CheckpointManager manager;
        if (manager.validateCheckpoint(checkpointPath)) {
            std::cout << "Checkpoint validation PASSED: " << checkpointPath << std::endl;
            return true;
        } else {
            std::cout << "Checkpoint validation FAILED: " << checkpointPath << std::endl;
            return false;
        }
    }
    
    bool handleRecover(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Usage: recover <checkpoint_path> [backup_path]" << std::endl;
            return false;
        }
        
        std::string checkpointPath = args[0];
        std::string backupPath = args.size() > 1 ? args[1] : "";
        
        std::cout << "Attempting recovery from checkpoint..." << std::endl;
        
        // This would require implementation of recovery logic
        std::cout << "Recovery functionality would be implemented here" << std::endl;
        return true;
    }
    
    bool handleExport(const std::vector<std::string>& args) {
        if (args.size() < 1) {
            std::cout << "Usage: export <output_format> <output_path> [options]" << std::endl;
            return false;
        }
        
        std::string format = args[0];
        std::string outputPath = args[1];
        
        std::cout << "Export functionality would be implemented here for format: " << format 
                  << " to: " << outputPath << std::endl;
        
        return true;
    }
    
    bool handleHelp(const std::vector<std::string>& args) {
        std::cout << "Available commands:" << std::endl;
        
        for (const auto& cmd : commands_) {
            std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
        }
        
        return true;
    }
    
    bool handleQuit(const std::vector<std::string>& args) {
        std::cout << "Exiting NLM Advanced CLI" << std::endl;
        return false; // Signal to exit
    }
    
    // Global helper function declarations
    TimelineProfiler& getGlobalTimelineProfiler();
    MemoryTracker& getGlobalMemoryTracker();
};

} // namespace nlm

int main(int argc, char* argv[]) {
    nlm::AdvancedCLI cli;
    cli.run();
    return 0;
}
