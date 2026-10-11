// SimulationController implementation
#include "SimulationController.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <filesystem>

SimulationController::SimulationController() : isRunning_(false), currentStep_(0), 
                           totalSteps_(0), autoRun_(false), autoRunDuration_(0.0), 
                           autoRunSteps_(0), configDirty_(false) {
    logger = &Logger::getGlobal();
}

SimulationController::~SimulationController() {
    stopSimulation();
}

bool SimulationController::initialize(std::shared_ptr<Config> config) {
    if (!config) {
        logger->error("Invalid configuration provided");
        return false;
    }

    this->config = config;
    
    // Validate configuration
    validateConfiguration();
    
    logger->info("SimulationController initialized successfully");
    return true;
}

bool SimulationController::startSimulation(const std::vector<std::string>& args) {
    if (isRunning_) {
        logger->warning("Simulation is already running");
        return true;
    }

    // Parse start arguments
    autoRun_ = false;
    autoRunDuration_ = 0.0;
    autoRunSteps_ = 0;
    
    if (args.size() >= 1) {
        // Try to parse as duration (e.g., "1s", "5m", "2h")
        if (parseDuration(args[0], autoRunDuration_)) {
            autoRun_ = true;
            logger->info("Starting simulation for duration: " + args[0]);
        } else if (std::all_of(args[0].begin(), args[0].end(), ::isdigit)) {
            autoRunSteps_ = std::stoul(args[0]);
            autoRun_ = true;
            logger->info("Starting simulation for " + args[0] + " steps");
        } else {
            logger->warning("Invalid start argument, using default auto-mode");
        }
    }

    // Create and initialize brain
    brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        logger->error("Failed to initialize brain");
        return false;
    }

    isRunning_ = true;
    currentStep_ = 0;
    startTime_ = std::chrono::steady_clock::now();
    
    // Schedule start event
    events.push_back({currentStep_, "simulation_started", {}});
    
    logger->info("Simulation started successfully");
    return true;
}

bool SimulationController::stopSimulation() {
    if (!isRunning_) {
        return true;  // Already stopped
    }

    // Save current state as checkpoint
    saveCheckpoint("last_state");
    
    isRunning_ = false;
    autoRun_ = false;
    
    // Schedule stop event
    events.push_back({currentStep_, "simulation_stopped", {}});
    
    logger->info("Simulation stopped successfully");
    return true;
}

bool SimulationController::stepSimulation(const std::vector<std::string>& args) {
    if (!isRunning_) {
        logger->error("Cannot step simulation: not running");
        return false;
    }

    // Parse number of steps
    size_t stepsToRun = 1;
    if (!args.empty() && std::all_of(args[0].begin(), args[0].end(), ::isdigit)) {
        stepsToRun = std::stoul(args[0]);
    }

    for (size_t i = 0; i < stepsToRun; ++i) {
        if (!isRunning_) break;  // Check for stop signal
        
        currentStep_++;
        auto currentTime = calculateElapsedTime();
        
        // Execute one simulation step
        brain->step(currentStep_, currentTime);
        
        // Update event queue
        events.push_back({currentStep_, "step_completed", {}});
        
        // Check for auto-run completion
        if (autoRun_) {
            if (autoRunSteps_ > 0 && currentStep_ >= autoRunSteps_) {
                stopSimulation();
                break;
            } else if (autoRunDuration_ > 0 && currentTime >= autoRunDuration_) {
                stopSimulation();
                break;
            }
        }
    }

    logger->info("Executed " + std::to_string(stepsToRun) + " simulation steps");
    return true;
}

bool SimulationController::runAutoSimulation(const std::vector<std::string>& args) {
    if (isRunning_) {
        logger->warning("Simulation is already running");
        return true;
    }

    // Parse duration and steps
    double duration = 0.0;
    size_t steps = 0;
    
    if (args.size() >= 1) {
        if (parseDuration(args[0], duration)) {
            logger->info("Running simulation for duration: " + args[0]);
        } else if (std::all_of(args[0].begin(), args[0].end(), ::isdigit)) {
            steps = std::stoul(args[0]);
            logger->info("Running simulation for " + args[0] + " steps");
        } else {
            logger->error("Invalid duration or step count argument");
            return false;
        }
    }

    if (args.size() >= 2 && std::all_of(args[1].begin(), args[1].end(), ::isdigit)) {
        steps = std::stoul(args[1]);
    }

    // Set auto-run parameters
    autoRun_ = true;
    autoRunDuration_ = duration;
    autoRunSteps_ = steps;
    
    // Start simulation
    return startSimulation({});
}

bool SimulationController::configureBrain(const std::vector<std::string>& args) {
    if (!brain) {
        logger->error("No brain simulation running");
        return false;
    }

    if (args.size() < 2) {
        logger->error("Configure requires key and value arguments");
        return false;
    }

    const std::string& key = args[0];
    const std::string& valueStr = args[1];
    
    // Try to parse value as different types
    try {
        // Try double first
        size_t pos;
        double doubleValue = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            brain->getConfig()->set(key, doubleValue, ConfigSource::Runtime);
            configDirty_ = true;
            logger->info("Set configuration parameter: " + key + " = " + valueStr);
            return true;
        }
        
        // Try int64_t
        int64_t intValue = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            brain->getConfig()->set(key, intValue, ConfigSource::Runtime);
            configDirty_ = true;
            logger->info("Set configuration parameter: " + key + " = " + valueStr);
            return true;
        }
        
        // Try bool
        if (valueStr == "true" || valueStr == "false") {
            bool boolValue = (valueStr == "true");
            brain->getConfig()->set(key, boolValue, ConfigSource::Runtime);
            configDirty_ = true;
            logger->info("Set configuration parameter: " + key + " = " + valueStr);
            return true;
        }
        
        // Try string (use as-is)
        brain->getConfig()->set(key, valueStr, ConfigSource::Runtime);
        configDirty_ = true;
        logger->info("Set configuration parameter: " + key + " = " + valueStr);
        return true;
        
    } catch (const std::exception& e) {
        logger->error("Failed to parse value \"" + valueStr + "\": " + std::string(e.what()));
        return false;
    }
}

bool SimulationController::inspectBrain(const std::vector<std::string>& args) {
    if (!brain) {
        logger->error("No brain simulation running");
        return false;
    }

    std::string component = "status"; // Default
    if (!args.empty()) {
        component = args[0];
        std::transform(component.begin(), component.end(), component.begin(), ::tolower);
    }

    displayBrainInfo(component);
    return true;
}

bool SimulationController::saveBrainState(const std::vector<std::string>& args) {
    if (args.empty()) {
        logger->error("Save requires filename argument");
        return false;
    }

    std::string filename = args[0];
    bool success = brain->save(filename);
    
    if (success) {
        logger->info("Brain state saved to: " + filename);
    } else {
        logger->error("Failed to save brain state to: " + filename);
    }
    
    return success;
}

bool SimulationController::loadBrainState(const std::vector<std::string>& args) {
    if (args.empty()) {
        logger->error("Load requires filename argument");
        return false;
    }

    std::string filename = args[0];
    bool success = brain->load(filename);
    
    if (success) {
        logger->info("Brain state loaded from: " + filename);
        // Reset simulation state after loading
        if (isRunning_) {
            stopSimulation();
            startSimulation({});
        }
    } else {
        logger->error("Failed to load brain state from: " + filename);
    }
    
    return success;
}

bool SimulationController::applyNeuromodulation(const std::vector<std::string>& args) {
    if (!brain) {
        logger->error("No brain simulation running");
        return false;
    }

    if (args.size() < 2) {
        logger->error("Neuromodulate requires type and intensity arguments");
        return false;
    }

    std::string type = args[0];
    std::string intensityStr = args[1];
    
    try {
        float intensity = std::stof(intensityStr);
        if (intensity < 0.0f || intensity > 1.0f) {
            logger->error("Neuromodulation intensity must be between 0.0 and 1.0");
            return false;
        }
        
        // Create neuromodulator signal based on type
        // This is a simplified implementation - real implementation would be more complex
        // For now, just log the request
        logger->info("Applying neuromodulation: type=" + type + ", intensity=" + std::to_string(intensity));
        
        // In real implementation, this would apply actual neuromodulation signals
        // For example: dopamine, curiosity, novelty, etc.
        return true;
        
    } catch (const std::exception& e) {
        logger->error("Failed to parse intensity \"" + intensityStr + "\": " + std::string(e.what()));
        return false;
    }
}

bool SimulationController::manipulateWorkingMemory(const std::vector<std::string>& args) {
    if (!brain) {
        logger->error("No brain simulation running");
        return false;
    }

    if (args.empty()) {
        logger->error("Memory command requires action argument");
        return false;
    }

    std::string action = args[0];
    std::transform(action.begin(), action.end(), action.begin(), ::tolower);
    
    logger->info("Working memory operation requested: " + action);
    
    // In real implementation, this would manipulate the working memory systems
    // For example: store, recall, clear, list, capacity
    return true;
}

bool SimulationController::displayStatistics(const std::vector<std::string>& args) {
    if (!brain) {
        logger->error("No brain simulation running");
        return false;
    }

    double refreshInterval = 1.0; // Default 1 second
    if (!args.empty() && !args[0].empty()) {
        try {
            refreshInterval = std::stod(args[0]);
        } catch (...) {
            // Use default
        }
    }

    logger->info("Starting statistics display (refresh: " + std::to_string(refreshInterval) + "s)");
    
    // Display current statistics
    displayCurrentStatistics();
    
    return true;
}

bool SimulationController::profilePerformance(const std::vector<std::string>& args) {
    if (!brain) {
        logger->error("No brain simulation running");
        return false;
    }

    double duration = 0.0;
    std::string outputFile;
    
    if (args.size() >= 1) {
        if (parseDuration(args[0], duration)) {
            logger->info("Performance profiling for duration: " + args[0]);
        } else if (std::all_of(args[0].begin(), args[0].end(), ::isdigit)) {
            duration = std::stod(args[0]);
            logger->info("Performance profiling for " + args[0] + " seconds");
        }
    }

    if (args.size() >= 2) {
        outputFile = args[1];
    }

    // Run simulation while profiling
    auto startTime = std::chrono::steady_clock::now();
    size_t stepsRan = 0;
    
    double startRealTime = calculateElapsedTime();
    double endRealTime = startRealTime + duration;
    
    while (isRunning_ && calculateElapsedTime() < endRealTime) {
        stepSimulation({});
        stepsRan++;
        
        // Sleep briefly to avoid consuming too much CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto endTime = std::chrono::steady_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    logger->info("Performance profiling completed:");
    logger->info("  Total steps: " + std::to_string(stepsRan));
    logger->info("  Total time: " + std::to_string(totalTime) + "s");
    logger->info("  Steps per second: " + std::to_string(stepsRan / totalTime));
    
    if (!outputFile.empty()) {
        // Save profile data
        logger->info("Profile data would be saved to: " + outputFile);
    }

    return true;
}

bool SimulationController::monitorSystem(const std::vector<std::string>& args) {
    double interval = 1.0;
    if (!args.empty() && !args[0].empty()) {
        try {
            interval = std::stod(args[0]);
        } catch (...) {
            // Use default
        }
    }

    logger->info("Starting system monitoring (interval: " + std::to_string(interval) + "s)");
    
    // In real implementation, this would monitor memory usage, CPU, etc.
    // For now, just display current status
    displayCurrentStatistics();
    
    return true;
}

bool SimulationController::manageConfiguration(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        logger->error("Config command requires action and file arguments");
        return false;
    }

    std::string action = args[0];
    std::string filename = args[1];
    
    std::transform(action.begin(), action.end(), action.begin(), ::tolower);
    
    if (action == "save") {
        // Save configuration to file
        bool success = config->saveToFile(filename);
        if (success) {
            logger->info("Configuration saved to: " + filename);
        } else {
            logger->error("Failed to save configuration to: " + filename);
        }
        return success;
    } else if (action == "load") {
        // Load configuration from file
        bool success = config->loadFromFile(filename);
        if (success) {
            logger->info("Configuration loaded from: " + filename);
            configDirty_ = true;
        } else {
            logger->error("Failed to load configuration from: " + filename);
        }
        return success;
    } else if (action == "list") {
        // List available configuration files
        logger->info("Listing configuration files (not implemented)");
        return true;
    } else {
        logger->error("Unknown config action: " + action);
        return false;
    }
}

bool SimulationController::setRuntimeParameter(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        logger->error("Set command requires key and value arguments");
        return false;
    }

    const std::string& key = args[0];
    const std::string& value = args[1];
    
    // Use the same logic as configureBrain but for runtime
    try {
        // Try double first
        size_t pos;
        double doubleValue = std::stod(value, &pos);
        if (pos == value.size()) {
            config->set(key, doubleValue, ConfigSource::Runtime);
            logger->info("Set runtime parameter: " + key + " = " + value);
            return true;
        }
        
        // Try int64_t
        int64_t intValue = std::stoll(value, &pos);
        if (pos == value.size()) {
            config->set(key, intValue, ConfigSource::Runtime);
            logger->info("Set runtime parameter: " + key + " = " + value);
            return true;
        }
        
        // Try bool
        if (value == "true" || value == "false") {
            bool boolValue = (value == "true");
            config->set(key, boolValue, ConfigSource::Runtime);
            logger->info("Set runtime parameter: " + key + " = " + value);
            return true;
        }
        
        // Try string
        config->set(key, value, ConfigSource::Runtime);
        logger->info("Set runtime parameter: " + key + " = " + value);
        return true;
        
    } catch (const std::exception& e) {
        logger->error("Failed to parse value \"" + value + "\": " + std::string(e.what()));
        return false;
    }
}

bool SimulationController::runTests(const std::vector<std::string>& args) {
    logger->info("Running NLM unit tests (not implemented)");
    // In real implementation, this would run the actual test suite
    return true;
}

bool SimulationController::runBenchmark(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        logger->error("Benchmark requires neuron_count and steps arguments");
        return false;
    }

    size_t neuronCount = 0;
    size_t steps = 0;
    
    try {
        neuronCount = std::stoul(args[0]);
        steps = std::stoul(args[1]);
    } catch (...) {
        logger->error("Invalid neuron_count or steps argument");
        return false;
    }

    logger->info("Running benchmark: " + std::to_string(neuronCount) + " neurons, " + 
               std::to_string(steps) + " steps");
    
    // Use the existing benchmark infrastructure
    // In real implementation, this would call the actual benchmark code
    return true;
}

bool SimulationController::detectMemoryLeaks(const std::vector<std::string>& args) {
    if (!brain) {
        logger->error("No brain simulation running");
        return false;
    }

    double duration = 5.0; // Default 5 seconds
    if (!args.empty() && !args[0].empty()) {
        try {
            duration = std::stod(args[0]);
        } catch (...) {
            // Use default
        }
    }

    logger->info("Starting memory leak detection for " + std::to_string(duration) + " seconds");
    
    // In real implementation, this would monitor memory allocations
    // and detect leaks by comparing before/after states
    return true;
}

bool SimulationController::runIntegrationTests(const std::vector<std::string>& args) {
    if (args.empty()) {
        logger->error("Integration test requires test_type argument");
        return false;
    }

    std::string testType = args[0];
    std::transform(testType.begin(), testType.end(), testType.begin(), ::tolower);
    
    logger->info("Running integration test: " + testType + " (not implemented)");
    
    // In real implementation, this would run integration tests
    // for memory_systems, neuromodulation, prediction, all_systems
    return true;
}

bool SimulationController::showStatus() {
    if (!brain) {
        logger->info("No brain simulation running");
        return true;
    }

    displayCurrentStatistics();
    return true;
}

bool SimulationController::exitApplication() {
    logger->info("Exiting NLM CLI");
    // Save current state before exiting
    saveCheckpoint("last_state");
    return false; // Signal to exit
}

bool SimulationController::displayVersion() {
    std::cout << "NLM CLI Version: 0.1.0" << std::endl;
    std::cout << "Build Date: " << __DATE__ << std::endl;
    std::cout << "C++ Standard: C++17" << std::endl;
    std::cout << "Phase: 6 - Final Integration" << std::endl;
    return true;
}

bool SimulationController::clearScreen() {
    // Simple screen clear - works on most terminals
    std::cout << "\033[2J\033[1;1H";
    return true;
}

// Getters
bool SimulationController::isSimulationRunning() const { return isRunning_; }
size_t SimulationController::getCurrentStep() const { return currentStep_; }
std::shared_ptr<Brain> SimulationController::getBrain() const { return brain; }

// Event processing
void SimulationController::processEvents() {
    // Process any pending events
    for (const auto& event : events) {
        // Handle events (logging, callbacks, etc.)
    }
    events.clear();
}

// Configuration validation
void SimulationController::validateConfiguration() {
    // Validate that all required configuration parameters are set
    // and have valid values
    
    // Set defaults for missing parameters
    if (!config->has("simulation_timestep")) {
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    
    if (!config->has("neuron_count")) {
        config->set("neuron_count", 500, ConfigSource::Default);
    }
    
    if (!config->has("connection_probability")) {
        config->set("connection_probability", 0.15f, ConfigSource::Default);
    }
    
    if (!config->has("random_seed")) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
}

// Utility functions
bool SimulationController::parseDuration(const std::string& durationStr, double& result) {
    // Parse duration strings like "1s", "5m", "2h", "1.5s"
    if (durationStr.empty()) return false;
    
    result = 0.0;
    size_t i = 0;
    
    // Parse numeric part
    while (i < durationStr.size() && std::isdigit(durationStr[i])) {
        i++;
    }
    
    std::string numStr = durationStr.substr(0, i);
    if (numStr.empty()) return false;
    
    try {
        result = std::stod(numStr);
    } catch (...) {
        return false;
    }
    
    // Parse unit
    if (i >= durationStr.size()) {
        // No unit specified, assume seconds
        return true;
    }
    
    char unit = durationStr[i];
    switch (unit) {
        case 's': case 'S': // seconds
            return true;
        case 'm': case 'M': // minutes
            result *= 60.0;
            return true;
        case 'h': case 'H': // hours
            result *= 3600.0;
            return true;
        default:
            return false;
    }
}

double SimulationController::calculateElapsedTime() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime_);
    return elapsed.count();
}

void SimulationController::displayBrainInfo(const std::string& component) {
    if (!brain) {
        logger->error("No brain simulation running");
        return;
    }

    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                NLM Brain Status                                ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    if (component == "status" || component.empty()) {
        // Overall brain status
        std::cout << "║ Simulation: " << (isRunning_ ? "RUNNING" : "STOPPED") << std::endl;
        std::cout << "║ Step: " << currentStep_ << std::endl;
        std::cout << "║ Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "║ Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "║ Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "║ Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "║ Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "║ Pending Events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "║ Regions: " << brain->getRegionCount() << std::endl;
        std::cout << "║ Time: " << calculateElapsedTime() << "s" << std::endl;
    } else if (component == "neurons") {
        // Neuron information
        std::cout << "║ Neuron detailed information would be displayed here" << std::endl;
    } else if (component == "synapses") {
        // Synapse information
        std::cout << "║ Synapse detailed information would be displayed here" << std::endl;
    } else if (component == "memory") {
        // Memory system information
        std::cout << "║ Memory system information would be displayed here" << std::endl;
    } else if (component == "prediction") {
        // Prediction system information
        std::cout << "║ Prediction system information would be displayed here" << std::endl;
    } else {
        std::cout << "║ Unknown component: " << component << std::endl;
    }

    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void SimulationController::displayCurrentStatistics() {
    if (!brain) return;

    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                Current Statistics                              ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    std::cout << "║ Time: " << std::fixed << std::setprecision(3) << calculateElapsedTime() << "s" << std::endl;
    std::cout << "║ Step: " << currentStep_ << std::endl;
    std::cout << "║ Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "║ Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "║ Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "║ Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "║ Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "║ Pending Events: " << brain->getPendingSpikeEventCount() << std::endl;
    std::cout << "║ Regions: " << brain->getRegionCount() << std::endl;
    
    // Calculate derived statistics
    if (brain->getTotalNeuronCount() > 0) {
        std::cout << "║ Firing Rate: " << std::fixed << std::setprecision(2) 
                  << (100.0 * brain->getFiringNeuronCount() / brain->getTotalNeuronCount()) 
                  << "%" << std::endl;
    }
    
    std::cout << "║ Event Queue Size: " << events.size() << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void SimulationController::saveCheckpoint(const std::string& filename) {
    std::string fullPath = "checkpoints/" + filename + ".nlm";
    
    // Ensure directory exists
    std::filesystem::create_directories("checkpoints");
    
    if (brain->save(fullPath)) {
        logger->info("Checkpoint saved to: " + fullPath);
    } else {
        logger->error("Failed to save checkpoint to: " + fullPath);
    }
}