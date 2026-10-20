// Enhanced C++ code quality improvements with better error handling

// This file contains improvements to the C++ codebase including:
// 1. Better error handling in brain initialization and core operations
// 2. Enhanced validation and safety checks
// 3. Improved performance with optimized data structures
// 4. Better logging and debugging support
// 5. Memory safety improvements

#ifndef ENHANCED_CPP_CODE_H
#define ENHANCED_CPP_CODE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <random>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <cmath>
#include <limits>
#include <type_traits>
#include <optional>
#include <variant>
#include <functional>
#include <exception>
#include <stdexcept>

namespace nlm {

// Enhanced exception hierarchy
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message), timestamp(std::chrono::system_clock::now()) {}
    
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    
private:
    std::chrono::system_clock::time_point timestamp;
};

class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException(message) {}
};

class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) : NLMException(message) {}
};

class NeuronException : public NLMException {
public:
    explicit NeuronException(const std::string& message) : NLMException(message) {}
};

// Enhanced configuration with validation
class EnhancedConfig {
public:
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    EnhancedConfig() : initialized(false), validationEnabled(false) {}
    
    bool loadFromFile(const std::string& filepath) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            logError("Failed to open config file: " + filepath);
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        size_t validLines = 0;
        
        while (std::getline(file, line)) {
            ++lineNumber;
            line = trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Enhanced parsing with validation
            auto parsed = parseLine(line, lineNumber);
            if (parsed) {
                set(parsed->key, parsed->value, parsed->source);
                ++validLines;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        logInfo("Loaded configuration from " + filepath + 
               " (" + std::to_string(validLines) + " valid lines, " +
               std::to_string(duration.count()) + " ms)");
        
        initialized = true;
        return true;
    }
    
    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        if (!validationEnabled) {
            return errors;
        }
        
        for (const auto& rule : validationRules) {
            if (has(rule.key)) {
                auto value = get(rule.key);
                if (!value.has_value()) {
                    errors.push_back("Key " + rule.key + " is present but cannot be parsed");
                    continue;
                }
                
                if (!rule.validator(*value)) {
                    errors.push_back(rule.errorMessage);
                }
            } else {
                // Check if key is required
                auto it = std::find_if(requiredKeys.begin(), requiredKeys.end(),
                                       [&rule](const std::string& reqKey) { return reqKey == rule.key; });
                if (it != requiredKeys.end()) {
                    errors.push_back("Required configuration key missing: " + rule.key);
                }
            }
        }
        
        return errors;
    }
    
    void addValidationRule(const ValidationRule& rule) {
        validationRules.push_back(rule);
    }
    
    void setRequiredKey(const std::string& key) {
        requiredKeys.insert(key);
    }
    
    void enableValidation(bool enable) {
        validationEnabled = enable;
    }
    
    std::vector<std::string> getValidationErrors() const {
        return validate();
    }
    
    bool isValid() const {
        return validate().empty();
    }
    
private:
    struct ParsedEntry {
        std::string key;
        ConfigValue value;
        ConfigSource source;
    };
    
    ParsedEntry parseLine(const std::string& line, size_t lineNumber) {
        ParsedEntry entry;
        
        // Parse key=value format
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            logWarning("Invalid config line " + std::to_string(lineNumber) + 
                      ": no '=' found: " + line);
            return entry; // Return empty entry
        }
        
        entry.key = trim(line.substr(0, pos));
        std::string rawValue = trim(line.substr(pos + 1));
        
        if (entry.key.empty()) {
            logWarning("Empty key in config line " + std::to_string(lineNumber));
            return entry;
        }
        
        // Parse value with type detection
        entry.value = parseValue(rawValue);
        entry.source = ConfigSource::File;
        
        return entry;
    }
    
    ConfigValue parseValue(const std::string& str) {
        // Try different types in order of preference
        try {
            // Check if it's a boolean
            if (str == "true" || str == "false") {
                return str == "true";
            }
            
            // Check if it's a number
            size_t pos;
            int intVal = std::stoi(str, &pos);
            if (pos == str.size()) {
                return intVal;
            }
            
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                return doubleVal;
            }
            
            // It's a string
            return str;
        } catch (const std::exception&) {
            return str; // Treat as string if parsing fails
        }
    }
    
    std::string trim(const std::string& str) const {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    void logError(const std::string& message) {
        std::cerr << "[ERROR] " << message << std::endl;
    }
    
    void logWarning(const std::string& message) {
        std::cerr << "[WARNING] " << message << std::endl;
    }
    
    void logInfo(const std::string& message) {
        std::cout << "[INFO] " << message << std::endl;
    }
    
    // Data members
    bool initialized;
    bool validationEnabled;
    std::vector<ValidationRule> validationRules;
    std::set<std::string> requiredKeys;
    // Other config members would be here...
};

// Enhanced neuron with better safety and performance
class EnhancedNeuron {
public:
    EnhancedNeuron(NeuronId id) : id(id), initialized(false) {
        initializeDefaults();
    }
    
    // Safe initialization with validation
    bool initialize(double vRest, double threshold, double tauMem, double tauRef) {
        if (initialized) {
            logWarning("Neuron " + std::to_string(id.value) + " already initialized");
            return false;
        }
        
        // Validate parameters
        if (!validateParameters(vRest, threshold, tauMem, tauRef)) {
            logError("Invalid neuron parameters for neuron " + std::to_string(id.value));
            return false;
        }
        
        // Initialize neuron state
        membranePotential = vRest;
        restingPotential = vRest;
        this->threshold = threshold;
        this->tauMem = tauMem;
        this->tauRef = tauRef;
        
        // Reset state
        firingState = FiringState::Resting;
        refractoryRemaining = 0;
        synapticInput = 0.0;
        totalCurrent = 0.0;
        
        spikeHistory.clear();
        adaptationVariable = 0.0;
        
        initialized = true;
        return true;
    }
    
    // Enhanced LIF integration with better numerical stability
    bool stepLIF(Timestamp currentTime, TimestepDuration dt) {
        if (!initialized) {
            logError("Cannot step uninitialized neuron " + std::to_string(id.value));
            return false;
        }
        
        // Update membrane potential with Euler integration
        double dV = (synapticInput - membranePotential + restingPotential) / tauMem * dt;
        membranePotential += dV;
        
        // Check for spike
        bool fired = false;
        if (membranePotential >= threshold && firingState != FiringState::Refractory) {
            fire(currentTime);
            fired = true;
            
            // Reset membrane potential
            membranePotential = resetPotential;
            
            // Start refractory period
            refractoryRemaining = static_cast<size_t>(tauRef / dt);
            firingState = FiringState::Refractory;
        }
        
        // Update refractory period
        if (refractoryRemaining > 0) {
            --refractoryRemaining;
            if (refractoryRemaining == 0) {
                firingState = FiringState::Resting;
            }
        }
        
        // Clear synaptic input for next time step
        synapticInput = 0.0;
        
        return fired;
    }
    
    // Safe weight management with bounds checking
    void setWeight(SynapticWeight weight) {
        // Apply biological constraints
        weight = std::clamp(weight, -1.0f, 1.0f);
        
        // Ensure weight doesn't cause instability
        if (std::abs(weight) > 0.5f) {
            logWarning("Large synaptic weight " + std::to_string(weight) + 
                      " for neuron " + std::to_string(id.value));
        }
        
        synapticWeight = weight;
    }
    
    // Enhanced synaptic input with saturation
    void receiveInput(MembranePotential input, bool isExcitatory = true) {
        double weightedInput = input * (isExcitatory ? 1.0 : -1.0);
        
        // Apply input saturation to prevent numerical instability
        const double maxInput = 50.0;
        weightedInput = std::clamp(weightedInput, -maxInput, maxInput);
        
        synapticInput += weightedInput;
        
        // Update adaptation variable for neuromodulation
        adaptationVariable += weightedInput * 0.01;
    }
    
    // Check if neuron is ready for firing
    bool canFire() const {
        return !isRefractory() && membranePotential >= threshold;
    }
    
    // Get neuron state summary
    std::map<std::string, double> getStateSummary() const {
        std::map<std::string, double> summary;
        
        summary["id"] = id.value;
        summary["membrane_potential"] = membranePotential;
        summary["threshold"] = threshold;
        summary["resting_potential"] = restingPotential;
        summary["refractory_remaining"] = refractoryRemaining;
        summary["is_firing"] = isFiring() ? 1.0 : 0.0;
        summary["is_refractory"] = isRefractory() ? 1.0 : 0.0;
        summary["synaptic_input"] = synapticInput;
        
        return summary;
    }
    
private:
    bool validateParameters(double vRest, double threshold, double tauMem, double tauRef) {
        if (tauMem <= 0) return false;
        if (tauRef <= 0) return false;
        if (threshold <= vRest) return false; // Threshold should be above resting potential
        if (std::abs(vRest) > 100) return false; // Unphysiological resting potential
        if (std::abs(threshold) > 100) return false; // Unphysiological threshold
        
        return true;
    }
    
    void fire(Timestamp timestamp) {
        firingState = FiringState::Active;
        firingStartTime = timestamp;
        
        // Record spike
        spikeHistory.push_back(timestamp);
        if (spikeHistory.size() > MAX_SPIKE_HISTORY) {
            spikeHistory.erase(spikeHistory.begin());
        }
        
        // Update statistics
        spikeCount++;
    }
    
    void initializeDefaults() {
        id = NeuronId();
        type = NeuronType::Internal;
        
        // Default LIF parameters
        restingPotential = -70.0;
        resetPotential = -75.0;
        threshold = -55.0;
        tauMem = 20.0;
        tauRef = 2.0;
        
        membranePotential = restingPotential;
        firingState = FiringState::Resting;
        refractoryRemaining = 0;
        
        synapticInput = 0.0;
        synapticWeight = 0.5f;
        
        adaptationVariable = 0.0;
        firingStartTime = 0.0;
        spikeCount = 0;
        
        initialized = false;
    }
    
    void logError(const std::string& message) {
        std::cerr << "[NEURON ERROR " << id.value << "] " << message << std::endl;
    }
    
    void logWarning(const std::string& message) {
        std::cerr << "[NEURON WARNING " << id.value << "] " << message << std::endl;
    }
    
    // Data members
    NeuronId id;
    NeuronType type;
    
    // LIF parameters
    double restingPotential;
    double resetPotential;
    double threshold;
    double tauMem;
    double tauRef;
    
    // State variables
    double membranePotential;
    FiringState firingState;
    size_t refractoryRemaining;
    double synapticInput;
    SynapticWeight synapticWeight;
    
    // History and adaptation
    std::vector<Timestamp> spikeHistory;
    double adaptationVariable;
    Timestamp firingStartTime;
    size_t spikeCount;
    
    bool initialized;
};

// Enhanced spike system with better event handling
class EnhancedSpikeSystem {
public:
    EnhancedSpikeSystem() : nextEventId(0), maxDelay(1000) {}
    
    // Register spike handler with type safety
    void registerHandler(std::function<void(const DetailedSpikeEvent&)>&& handler) {
        handlers.push_back(std::move(handler));
    }
    
    // Register delayed spike handler with validation
    void registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)>&& handler, 
                                uint32_t maxDelay = 1000) {
        delayedHandlers.push_back(std::make_pair(std::move(handler), maxDelay));
        this->maxDelay = std::max(this->maxDelay, maxDelay);
    }
    
    // Process incoming spikes with priority queue
    void processSpikes(const std::vector<std::shared_ptr<Neuron>>& neurons, 
                       Timestamp currentTime) {
        // Process immediate spikes
        for (auto& neuron : neurons) {
            if (neuron->shouldFire()) {
                DetailedSpikeEvent event;
                event.spike_id = nextEventId++;
                event.source_neuron = neuron->getId();
                event.timestamp = currentTime;
                event.is_excitatory = neuron->getType() == NeuronType::Excitatory;
                
                // Deliver to all connected neurons
                deliverSpike(event, neuron->getTargetNeurons());
            }
        }
    }
    
    // Schedule delayed spike
    void scheduleDelayedSpike(const DetailedSpikeEvent& event, 
                             const std::vector<std::pair<NeuronId, float>>& targets,
                             uint32_t delay) {
        if (delay > maxDelay) {
            logWarning("Delay " + std::to_string(delay) + " exceeds maximum " + 
                      std::to_string(maxDelay));
            delay = maxDelay;
        }
        
        DelayedSpikeEvent delayedEvent;
        delayedEvent.spike_id = event.spike_id;
        delayedEvent.source_neuron = event.source_neuron;
        delayedEvent.destination_neuron = event.destination_neuron;
        delayedEvent.weight = event.weight;
        delayedEvent.is_excitatory = event.is_excitatory;
        delayedEvent.delivery_time = event.timestamp + delay;
        
        delayedQueue.push(std::make_pair(delayedEvent.delivery_time, delayedEvent));
    }
    
    // Process delayed spike events
    void processDelayedEvents(Timestamp currentTime) {
        while (!delayedQueue.empty() && delayedQueue.top().first <= currentTime) {
            auto [deliveryTime, event] = delayedQueue.top();
            delayedQueue.pop();
            
            // Deliver delayed spike
            for (auto& handler : delayedHandlers) {
                try {
                    handler.first(event);
                } catch (const std::exception& e) {
                    logError("Error in delayed spike handler: " + std::string(e.what()));
                }
            }
        }
    }
    
    // Get next spike delivery time
    std::optional<Timestamp> getNextDeliveryTime() const {
        if (delayedQueue.empty()) {
            return std::nullopt;
        }
        return delayedQueue.top().first;
    }
    
private:
    void deliverSpike(const DetailedSpikeEvent& event, 
                      const std::vector<NeuronId>& targets) {
        for (const auto& target : targets) {
            DelayedSpikeEvent delayedEvent;
            delayedEvent.spike_id = event.spike_id;
            delayedEvent.source_neuron = event.source_neuron;
            delayedEvent.destination_neuron = target;
            delayedEvent.weight = event.weight;
            delayedEvent.is_excitatory = event.is_excitatory;
            delayedEvent.delivery_time = event.timestamp + event.delay;
            
            delayedQueue.push(std::make_pair(delayedEvent.delivery_time, delayedEvent));
        }
        
        // Notify immediate handlers
        for (auto& handler : handlers) {
            try {
                handler(event);
            } catch (const std::exception& e) {
                logError("Error in spike handler: " + std::string(e.what()));
            }
        }
    }
    
    void logError(const std::string& message) {
        std::cerr << "[SPIKE SYSTEM ERROR] " << message << std::endl;
    }
    
    void logWarning(const std::string& message) {
        std::cerr << "[SPIKE SYSTEM WARNING] " << message << std::endl;
    }
    
    // Data members
    uint64_t nextEventId;
    uint32_t maxDelay;
    std::priority_queue<std::pair<Timestamp, DelayedSpikeEvent>,
                       std::vector<std::pair<Timestamp, DelayedSpikeEvent>>,
                       std::greater<>> delayedQueue;
    std::vector<std::function<void(const DetailedSpikeEvent&)>> handlers;
    std::vector<std::pair<std::function<void(const DelayedSpikeEvent&)>, uint32_t>> delayedHandlers;
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage = 0.0;
        double memoryUsageMB = 0.0;
        double simulationTimePerStep = 0.0;
        size_t totalSpikes = 0;
        size_t activeNeurons = 0;
        double averageFiringRate = 0.0;
        size_t timestamp = 0;
    };
    
    PerformanceMonitor() : running(false), samplingInterval(1000) {
        startSampling();
    }
    
    ~PerformanceMonitor() {
        stopSampling();
    }
    
    void startSampling() {
        running = true;
        samplerThread = std::thread(&PerformanceMonitor::samplingWorker, this);
    }
    
    void stopSampling() {
        running = false;
        if (samplerThread.joinable()) {
            samplerThread.join();
        }
    }
    
    PerformanceMetrics getCurrentMetrics() const {
        std::lock_guard<std::mutex> lock(metricsMutex);
        return currentMetrics;
    }
    
    std::vector<PerformanceMetrics> getMetricsHistory(size_t count = 100) const {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        std::vector<PerformanceMetrics> history;
        auto it = metricsHistory.end();
        for (size_t i = 0; i < std::min(count, metricsHistory.size()); ++i) {
            if (it == metricsHistory.begin()) {
                it = metricsHistory.end();
            }
            --it;
            history.push_back(*it);
        }
        
        return history;
    }
    
    double calculateAverageCpuUsage(size_t sampleCount = 10) const {
        auto history = getMetricsHistory(sampleCount);
        if (history.empty()) return 0.0;
        
        double total = 0.0;
        for (const auto& metrics : history) {
            total += metrics.cpuUsage;
        }
        
        return total / history.size();
    }
    
private:
    void samplingWorker() {
        while (running) {
            PerformanceMetrics metrics = collectMetrics();
            
            {
                std::lock_guard<std::mutex> lock(metricsMutex);
                currentMetrics = metrics;
                metricsHistory.push_back(metrics);
                
                // Keep history size manageable
                if (metricsHistory.size() > 10000) {
                    metricsHistory.erase(metricsHistory.begin());
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(samplingInterval));
        }
    }
    
    PerformanceMetrics collectMetrics() {
        PerformanceMetrics metrics;
        metrics.timestamp = static_cast<size_t>(std::chrono::system_clock::now().time_since_epoch().count());
        
        // Collect system metrics (simplified implementation)
        metrics.cpuUsage = getCpuUsage();
        metrics.memoryUsageMB = getMemoryUsage();
        
        // These would require access to brain metrics
        // metrics.totalSpikes = brainMetrics.totalSpikes;
        // metrics.activeNeurons = brainMetrics.activeNeurons;
        // metrics.averageFiringRate = brainMetrics.averageFiringRate;
        
        return metrics;
    }
    
    double getCpuUsage() const {
        // Simplified CPU usage calculation
        // In a real implementation, this would read system statistics
        return 0.0;
    }
    
    double getMemoryUsage() const {
        // Simplified memory usage calculation
        // In a real implementation, this would read system memory info
        return 0.0;
    }
    
    // Data members
    bool running;
    size_t samplingInterval;
    std::thread samplerThread;
    mutable std::mutex metricsMutex;
    PerformanceMetrics currentMetrics;
    std::vector<PerformanceMetrics> metricsHistory;
};

} // namespace nlm

#endif // ENHANCED_CPP_CODE_H
