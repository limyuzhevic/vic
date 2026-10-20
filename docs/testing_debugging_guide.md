# NLM Testing and Debugging Guide

## Overview

This guide provides comprehensive instructions for testing and debugging NLM simulations. It covers testing strategies, debugging techniques, troubleshooting procedures, and tools for identifying and resolving issues in NLM applications. The guide is designed to help developers, researchers, and users ensure the reliability and correctness of their NLM simulations.

## Testing Framework Overview

### 1. Unit Testing

#### Core Unit Tests
```cpp
// Test neural dynamics
TEST(NeuronTest, LIFDynamics) {
    Neuron neuron(NeuronId(1));
    neuron.setType(NeuronType::Excitatory);
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-50.0f);
    
    // Test resting state
    EXPECT_TRUE(neuron.getState().firingState == FiringState::Resting);
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -65.0f);
    
    // Test spike generation
    bool spike = neuron.stepLIF(10.0f); // 10ms timestep
    // Verify spike conditions
    // ...
}

// Test plasticity rules
TEST(STDPTest, LTP_LTD) {
    STDP stdp;
    
    // Setup pre- and post-synaptic spike times
    std::vector<Timestamp> preSpikes{0.0f, 50.0f};
    std::vector<Timestamp> postSpikes{10.0f, 60.0f};
    
    // Test LTP (post after pre)
    float ltpWeight = stdp.calculateWeightChange(preSpikes, postSpikes, 1.0f);
    EXPECT_GT(ltpWeight, 0.0f); // Long-term potentiation
    
    // Test LTD (post before pre)
    std::vector<Timestamp> ltdPreSpikes{0.0f, 50.0f};
    std::vector<Timestamp> ltdPostSpikes{-10.0f, -60.0f};
    
    float ltdWeight = stdp.calculateWeightChange(ltdPreSpikes, ltdPostSpikes, 1.0f);
    EXPECT_LT(ltpWeight, 0.0f); // Long-term depression
}

// Test memory systems
TEST(WorkingMemoryTest, PersistentActivity) {
    NeuralWorkingMemory memory;
    
    // Test memory encoding
    memory.encode(NeuronId(1), 0.5f);
    EXPECT_FLOAT_EQ(memory.getActivation(NeuronId(1)), 0.5f);
    
    // Test memory maintenance
    memory.update(10.0f); // 10ms timestep
    EXPECT_GT(memory.getActivation(NeuronId(1)), 0.5f); // Persistent activity
    
    // Test memory decay
    memory.decay(100.0f); // 100ms decay
    EXPECT_LT(memory.getActivation(NeuronId(1)), 0.5f);
}
```

#### Integration Tests
```cpp
// Test brain initialization
TEST(BrainIntegrationTest, FullSystem) {
    auto config = Config::createDefaultConfig();
    config.set<int>"brain.neuron_count", 100);
    
    Brain brain(config);
    EXPECT_TRUE(brain.initialize());
    
    // Test all subsystems
    EXPECT_TRUE(brain.getSTDP() != nullptr);
    EXPECT_TRUE(brain.getHebbian() != nullptr);
    EXPECT_TRUE(brain.getWorkingMemory() != nullptr);
    EXPECT_TRUE(brain.getPredictionSystem() != nullptr);
    
    // Test simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Verify expected behavior
        EXPECT_GE(brain.getTotalSpikeCount(), 0);
        EXPECT_LE(brain.getFiringNeuronCount(), 100);
    }
}

// Test neuromodulation integration
TEST(NeuromodulationTest, RewardModulatedLearning) {
    auto config = Config::createDefaultConfig();
    config.set<bool>"neuromod.dopamine.scale", true);
    config.set<bool>"neuromod.curiosity.enable", true);
    
    Brain brain(config);
    EXPECT_TRUE(brain.initialize());
    
    // Test reward processing
    Dopamine* dopamine = brain.getDopamine();
    EXPECT_TRUE(dopamine != nullptr);
    
    // Test curiosity system
    Curiosity* curiosity = brain.getCuriosity();
    EXPECT_TRUE(curiosity != nullptr);
    
    // Test novely system
    Novelty* novelty = brain.getNovelty();
    EXPECT_TRUE(novelty != nullptr);
}
```

### 2. System-Level Testing

#### Performance Testing
```cpp
// Load testing for large networks
class NLMLoadTest {
public:
    void runLoadTest(const Config& config, int numNeurons) {
        std::cout << "Running load test with " << numNeurons << " neurons...\n";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Create brain
        Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        // Run simulation
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
            
            // Monitor performance
            if (step % 100 == 0) {
                auto current = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);
                
                double stepsPerSecond = step / (duration.count() / 1000.0);
                std::cout << "Step " << step << ": " << stepsPerSecond << " steps/second\n";
                
                // Check for performance degradation
                if (stepsPerSecond < 100.0) {
                    std::cout << "Warning: Performance degradation detected\n";
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        
        double stepsPerSecond = 1000.0 / totalDuration.count();
        std::cout << "Load test completed: " << stepsPerSecond << " steps/second\n";
        
        // Verify performance targets
        EXPECT_GT(stepsPerSecond, 50.0); // Minimum acceptable performance
    }
};
```

#### Stress Testing
```cpp
// Stress testing for system stability
class NLMLoadStabilityTest {
public:
    void runStressTest(const Config& config, int durationSeconds) {
        std::cout << "Running stress test for " << durationSeconds << " seconds...\n";
        
        auto start = std::chrono::high_resolution_clock::now();
        int stepCount = 0;
        int errorCount = 0;
        
        try {
            // Create brain
            Brain brain(config);
            EXPECT_TRUE(brain.initialize());
            
            // Run simulation with random inputs
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int> stepDist(0, 100);
            
            while (std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - start).count() < durationSeconds) {
                
                // Random sensory input
                SensoryInput input;
                input.vision = std::vector<float>(config.get<int>"environment.world.resolution", 0.0f);
                for (float& pixel : input.vision) {
                    pixel = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
                }
                
                try {
                    brain.receiveSensoryInput(input);
                    brain.step(stepCount++);
                } catch (const std::exception& e) {
                    errorCount++;
                    std::cout << "Error at step " << stepCount << ": " << e.what() << "\n";
                    
                    // Check for system instability
                    if (errorCount > durationSeconds * 10) {
                        FAIL() << "Too many errors - system unstable";
                    }
                }
            }
            
        } catch (const std::exception& e) {
            std::cout << "Stress test failed: " << e.what() << "\n";
            throw;
        }
        
        double errorRate = static_cast<double>(errorCount) / stepCount;
        std::cout << "Stress test completed: " << stepCount << " steps, " 
                  << errorCount << " errors, " << errorRate * 100 << "% error rate\n";
        
        // Acceptable error rate
        EXPECT_LT(errorRate, 0.01); // Less than 1% error rate
    }
};
```

### 3. Behavioral Testing

#### Learning Testing
```cpp
// Test learning capabilities
class NMLBehaviorTest {
public:
    void testPlasticityLearning() {
        std::cout << "Testing plasticity learning capabilities...\n";
        
        auto config = Config::createDefaultConfig();
        config.set<bool>"plasticity.stdp.enable", true);
        config.set<double>"plasticity.stdp.learning_rate", 0.01);
        
        Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        // Record initial synaptic weights
        float initialWeight = brain.getSTDP()->getAverageWeight();
        
        // Run correlated spike patterns (should induce LTP)
        for (int step = 0; step < 1000; ++step) {
            // Create correlated activity
            if (step % 5 == 0) {
                // Pre-synaptic spike
                SensoryInput input;
                brain.receiveSensoryInput(input);
            }
            
            if (step % 7 == 0) {
                // Post-synaptic spike (short delay after pre)
                brain.step(step);
            } else {
                brain.step(step);
            }
            
            // Check for weight changes
            if (step % 100 == 0) {
                float currentWeight = brain.getSTDP()->getAverageWeight();
                if (currentWeight > initialWeight * 1.1) {
                    std::cout << "LTP detected: weight increased from " 
                              << initialWeight << " to " << currentWeight << "\n";
                    return;
                }
            }
        }
        
        EXPECT_TRUE(false) << "No LTP detected in plasticity test";
    }
    
    void testMemoryLearning() {
        std::cout << "Testing memory learning capabilities...\n";
        
        auto config = Config::createDefaultConfig();
        config.set<bool>"memory.working.enable", true);
        config.set<double>"memory.working.capacity", 1000.0);
        
        Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
        EXPECT_TRUE(workingMemory != nullptr);
        
        // Test memory encoding and retrieval
        NeuronId testNeuron(1);
        float testPattern = 0.7f;
        
        workingMemory->encode(testNeuron, testPattern);
        float retrievedPattern = workingMemory->getActivation(testNeuron);
        
        EXPECT_NEAR(retrievedPattern, testPattern, 0.01f);
        
        // Test memory persistence
        workingMemory->update(10.0f); // 10ms timestep
        float persistentPattern = workingMemory->getActivation(testNeuron);
        
        EXPECT_GT(persistentPattern, testPattern * 0.9f); // Should persist
        
        std::cout << "Memory learning test passed.\n";
    }
    
    void testCuriosityLearning() {
        std::cout << "Testing curiosity-driven learning...\n";
        
        auto config = Config::createDefaultConfig();
        config.set<bool>"neuromod.curiosity.enable", true);
        config.set<double>"neuromod.curiosity.exploration_rate", 0.2);
        
        Brain brain(config);
        EXPECT_TRUE(brain.initialize());
        
        Curiosity* curiosity = brain.getCuriosity();
        EXPECT_TRUE(curiosity != nullptr);
        
        // Test curiosity response to novel stimuli
        float initialCuriosity = curiosity->getCuriosityLevel();
        
        // Present novel input
        SensoryInput novelInput;
        brain.receiveSensoryInput(novelInput);
        brain.step(0);
        
        float newCuriosity = curiosity->getCuriosityLevel();
        
        // Curiosity should increase in response to novel input
        if (newCuriosity > initialCuriosity) {
            std::cout << "Curiosity response detected: " 
                      << initialCuriosity << " -> " << newCuriosity << "\n";
        } else {
            std::cout << "Warning: No curiosity response detected\n";
        }
        
        std::cout << "Curiosity learning test completed.\n";
    }
};
```

## Debugging Techniques

### 1. Debugging Framework

#### Debug Output Configuration
```cpp
class NLMDebugger {
public:
    enum DebugLevel {
        None = 0,
        Errors = 1,
        Warnings = 2,
        Info = 3,
        Detailed = 4,
        Trace = 5
    };
    
    struct DebugOptions {
        DebugLevel level = DebugLevel::Info;
        bool enableProfiling = false;
        bool enableMemoryTracking = false;
        bool enableTimingAnalysis = false;
        bool enableStateLogging = false;
        std::string logFile = "nlm_debug.log";
        std::chrono::milliseconds logInterval = std::chrono::milliseconds(1000);
    };
    
    NLMDebugger(const DebugOptions& options) : options_(options) {
        if (options_.enableStateLogging) {
            setupStateLogging();
        }
    }
    
    void logError(const std::string& message, const std::source_location& location = std::source_location::current()) {
        if (options_.level >= DebugLevel::Errors) {
            writeLogEntry(LogEntryType::Error, message, location);
        }
    }
    
    void logWarning(const std::string& message, const std::source_location& location = std::source_location::current()) {
        if (options_.level >= DebugLevel::Warnings) {
            writeLogEntry(LogEntryType::Warning, message, location);
        }
    }
    
    void logInfo(const std::string& message, const std::source_location& location = std::source_location::current()) {
        if (options_.level >= DebugLevel::Info) {
            writeLogEntry(LogEntryType::Info, message, location);
        }
    }
    
    void logDetailed(const std::string& message, const std::source_location& location = std::source_location::current()) {
        if (options_.level >= DebugLevel::Detailed) {
            writeLogEntry(LogEntryType::Detailed, message, location);
        }
    }
    
    void logTrace(const std::string& message, const std::source_location& location = std::source_location::current()) {
        if (options_.level >= DebugLevel::Trace) {
            writeLogEntry(LogEntryType::Trace, message, location);
        }
    }
    
    void startProfiling() {
        if (options_.enableProfiling) {
            profileStartTime_ = std::chrono::high_resolution_clock::now();
            setupProfiling();
        }
    }
    
    void stopProfiling() {
        if (options_.enableProfiling) {
            profileEndTime_ = std::chrono::high_resolution_clock::now();
            analyzeProfilingResults();
        }
    }
    
    void logNeuralState(const Brain& brain, const std::string& context = "") {
        if (options_.enableStateLogging) {
            writeNeuralState(brain, context);
        }
    }
    
    void logMemoryState() {
        if (options_.enableMemoryTracking) {
            writeMemoryState();
        }
    }
    
private:
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        LogEntryType type;
        std::string message;
        std::string file;
        int line;
        std::string function;
    };
    
    enum class LogEntryType {
        Error,
        Warning,
        Info,
        Detailed,
        Trace
    };
    
    void writeLogEntry(LogEntryType type, const std::string& message, 
                      const std::source_location& location) {
        LogEntry entry {
            .timestamp = std::chrono::system_clock::now(),
            .type = type,
            .message = message,
            .file = std::string(location.file_name()),
            .line = location.line(),
            .function = std::string(location.function_name())
        };
        
        // Write to log file
        std::lock_guard<std::mutex> lock(logMutex_);
        logFile_ << formatLogEntry(entry) << std::endl;
        
        // Also output to console for debugging
        std::cout << formatLogEntry(entry) << std::endl;
    }
    
    std::string formatLogEntry(const LogEntry& entry) const {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << " [" << logEntryTypeToString(entry.type) << "] " << entry.message;
        ss << " (" << entry.file << ":" << entry.line << " in " << entry.function << ")";
        return ss.str();
    }
    
    std::string logEntryTypeToString(LogEntryType type) const {
        switch (type) {
            case LogEntryType::Error: return "ERROR";
            case LogEntryType::Warning: return "WARNING";
            case LogEntryType::Info: return "INFO";
            case LogEntryType::Detailed: return "DETAIL";
            case LogEntryType::Trace: return "TRACE";
            default: return "UNKNOWN";
        }
    }
    
    void setupStateLogging() {
        // Setup state logging file
        logFile_.open(options_.logFile, std::ios::app);
        if (!logFile_.is_open()) {
            std::cerr << "Failed to open log file: " << options_.logFile << std::endl;
        }
    }
    
    void setupProfiling() {
        // Setup performance profiling
        // Initialize performance monitoring
    }
    
    void analyzeProfilingResults() {
        // Analyze profiling results
        // Generate performance report
    }
    
    void writeNeuralState(const Brain& brain, const std::string& context) {
        // Write neural state to log
        // Include neuron states, synaptic weights, etc.
    }
    
    void writeMemoryState() {
        // Write memory state to log
        // Include memory usage, allocation patterns
    }
    
    std::ofstream logFile_;
    std::mutex logMutex_;
    DebugOptions options_;
    std::chrono::high_resolution_clock::time_point profileStartTime_;
    std::chrono::high_resolution_clock::time_point profileEndTime_;
};
```

#### Debugging with Visual Tools
```cpp
// Integration with visualization tools
class NLMVisualizationDebugger {
public:
    void enableVisualization(const std::string& backend = "default") {
        if (backend == "default") {
            visualization_ = std::make_unique<DefaultVisualizer>();
        } else if (backend == "qt") {
            visualization_ = std::make_unique<QtVisualizer>();
        } else if (backend == "webgl") {
            visualization_ = std::make_unique<WebGLVisualizer>();
        }
        
        visualization_->setupScene();
        visualization_->startRendering();
    }
    
    void visualizeNeuralNetwork(const Brain& brain) {
        if (!visualization_) return;
        
        // Extract neural network data
        std::vector<NeuronInfo> neurons;
        std::vector<SynapseInfo> synapses;
        
        extractNeuralData(brain, neurons, synapses);
        
        // Update visualization
        visualization_->updateNeuralData(neurons, synapses);
    }
    
    void visualizeSpikeActivity(const Brain& brain, int timeWindow) {
        if (!visualization_) return;
        
        // Extract spike activity
        std::vector<SpikeRecord> spikes = extractSpikeActivity(brain, timeWindow);
        
        // Update visualization
        visualization_->updateSpikeActivity(spikes);
    }
    
    void visualizePlasticity(const Brain& brain) {
        if (!visualization_) return;
        
        // Extract plasticity data
        std::vector<PlasticityEvent> plasticityEvents = extractPlasticityData(brain);
        
        // Update visualization
        visualization_->updatePlasticityEvents(plasticityEvents);
    }
    
    void visualizeMemory(const Brain& brain) {
        if (!visualization_) return;
        
        // Extract memory data
        std::vector<MemoryRecord> memoryRecords = extractMemoryData(brain);
        
        // Update visualization
        visualization_->updateMemoryData(memoryRecords);
    }
    
private:
    struct NeuronInfo {
        int id;
        float x, y, z;           // 3D position
        float membranePotential;
        float threshold;
        float firingRate;
        NeuronType type;
        std::vector<int> connectedNeurons;
    };
    
    struct SynapseInfo {
        int sourceId;
        int targetId;
        float weight;
        float delay;
        SynapseType type;
        float efficacy;
    };
    
    struct SpikeRecord {
        int neuronId;
        double timestamp;
        float amplitude;
    };
    
    struct PlasticityEvent {
        int synapseId;
        double timestamp;
        float weightChange;
        PlasticityType type;
    };
    
    struct MemoryRecord {
        int neuronId;
        double timestamp;
        float activation;
        MemoryType type;
    };
    
    void extractNeuralData(const Brain& brain, std::vector<NeuronInfo>& neurons,
                          std::vector<SynapseInfo>& synapses) {
        // Extract neural data from brain
        // Implementation
    }
    
    std::vector<SpikeRecord> extractSpikeActivity(const Brain& brain, int timeWindow) {
        // Extract spike activity data
        // Implementation
        return {};
    }
    
    std::vector<PlasticityEvent> extractPlasticityData(const Brain& brain) {
        // Extract plasticity data
        // Implementation
        return {};
    }
    
    std::vector<MemoryRecord> extractMemoryData(const Brain& brain) {
        // Extract memory data
        // Implementation
        return {};
    }
    
    class VisualizerBase {
    public:
        virtual void setupScene() = 0;
        virtual void updateNeuralData(const std::vector<NeuronInfo>& neurons,
                                     const std::vector<SynapseInfo>& synapses) = 0;
        virtual void updateSpikeActivity(const std::vector<SpikeRecord>& spikes) = 0;
        virtual void updatePlasticityEvents(const std::vector<PlasticityEvent>& events) = 0;
        virtual void updateMemoryData(const std::vector<MemoryRecord>& records) = 0;
        virtual void startRendering() = 0;
        virtual void stopRendering() = 0;
    protected:
        std::mutex renderMutex_;
    };
    
    std::unique_ptr<VisualizerBase> visualization_;
};
```

### 2. Advanced Debugging Techniques

#### Memory Debugging
```cpp
// Memory leak detection
class NLMMemoryDebugger {
public:
    void enableMemoryTracking(bool trackAllocation = true) {
        trackAllocation_ = trackAllocation;
        if (trackAllocation_) {
            setupMemoryTracking();
        }
    }
    
    void logAllocation(const void* pointer, size_t size, const std::string& context) {
        if (!trackAllocation_) return;
        
        std::lock_guard<std::mutex> lock(memoryLogMutex_);
        
        AllocationRecord record {
            .pointer = pointer,
            .size = size,
            .context = context,
            .timestamp = std::chrono::high_resolution_clock::now(),
            .threadId = std::this_thread::get_id()
        };
        
        allocationLog_.push_back(record);
        
        // Check for potential leaks
        checkForMemoryLeaks();
    }
    
    void logDeallocation(const void* pointer, const std::string& context) {
        if (!trackAllocation_) return;
        
        std::lock_guard<std::mutex> lock(memoryLogMutex_);
        
        // Find corresponding allocation
        auto it = std::find_if(allocationLog_.begin(), allocationLog_.end(),
                               [pointer](const AllocationRecord& record) {
                                   return record.pointer == pointer;
                               });n        
        if (it != allocationLog_.end()) {
            // Remove allocation record
            allocationLog_.erase(it);
            
            // Log deallocation
            std::cout << "Memory deallocated: " << context << std::endl;
        } else {
            std::cout << "Warning: Deallocation without corresponding allocation: " 
                      << context << std::endl;
        }
    }
    
    void checkForMemoryLeaks() {
        // Check for memory leaks
        auto now = std::chrono::high_resolution_clock::now();
        auto threshold = std::chrono::seconds(30); // 30 seconds threshold
        
        std::vector<AllocationRecord> potentialLeaks;
        for (const auto& record : allocationLog_) {
            if (now - record.timestamp > threshold) {
                potentialLeaks.push_back(record);
            }
        }
        
        if (!potentialLeaks.empty()) {
            std::cout << "Potential memory leaks detected:" << std::endl;
            for (const auto& leak : potentialLeaks) {
                std::cout << "  - " << leak.context << " (" << leak.size << " bytes)" << std::endl;
            }
            
            // Clear potential leaks after reporting
            for (const auto& leak : potentialLeaks) {
                allocationLog_.erase(std::remove_if(allocationLog_.begin(), allocationLog_.end(),
                                                    [&leak](const AllocationRecord& r) {
                                                        return r.pointer == leak.pointer;
                                                    }),
                                     allocationLog_.end());
            }
        }
    }
    
    void reportMemoryUsage() {
        // Report current memory usage
        size_t totalAllocated = 0;
        size_t activeAllocations = 0;
        
        for (const auto& record : allocationLog_) {
            totalAllocated += record.size;
            activeAllocations++;
        }
        
        std::cout << "=== Memory Usage Report ===" << std::endl;
        std::cout << "Total allocated: " << totalAllocated << " bytes" << std::endl;
        std::cout << "Active allocations: " << activeAllocations << std::endl;
        std::cout << "Average allocation size: " 
                  << (activeAllocations > 0 ? totalAllocated / activeAllocations : 0) 
                  << " bytes" << std::endl;
    }
    
private:
    struct AllocationRecord {
        const void* pointer;
        size_t size;
        std::string context;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::thread::id threadId;
    };
    
    std::vector<AllocationRecord> allocationLog_;
    std::mutex memoryLogMutex_;
    bool trackAllocation_{false};
};
```

#### Performance Debugging
```cpp
// Performance bottleneck identification
class NLMPerformanceDebugger {
public:
    struct PerformanceProfile {
        double totalTime;                    // Total execution time
        double neuralDynamicsTime;           // Neural dynamics computation
        double plasticityTime;               // Plasticity computation
        double memoryManagementTime;         // Memory management
        double synchronizationTime;          // Synchronization overhead
        double iOSize;                       // I/O operations
        double configurationTime;            // Configuration processing
        
        std::vector<double> functionTimes;  // Individual function times
        std::vector<std::string> functionNames; // Function names
    };
    
    void profileFunction(const std::string& functionName, 
                        std::function<void()> function) {
        auto start = std::chrono::high_resolution_clock::now();
        
        function(); // Execute function
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Record profile
        std::lock_guard<std::mutex> lock(profileMutex_);
        lastProfile_.functionNames.push_back(functionName);
        lastProfile_.functionTimes.push_back(duration.count() / 1000.0); // Convert to milliseconds
        lastProfile_.totalTime += duration.count();
        
        // Update function-specific metrics
        if (functionName == "neuralDynamics") {
            lastProfile_.neuralDynamicsTime = duration.count();
        } else if (functionName == "plasticity") {
            lastProfile_.plasticityTime = duration.count();
        } else if (functionName == "memoryManagement") {
            lastProfile_.memoryManagementTime = duration.count();
        } else if (functionName == "synchronization") {
            lastProfile_.synchronizationTime = duration.count();
        }
    }
    
    PerformanceProfile getCurrentProfile() const {
        std::lock_guard<std::mutex> lock(profileMutex_);
        return lastProfile_;
    }
    
    void analyzePerformanceProfile(const PerformanceProfile& profile) {
        std::cout << "=== Performance Analysis ===" << std::endl;
        std::cout << "Total time: " << profile.totalTime << " microseconds" << std::endl;
        std::cout << "Neural dynamics: " << profile.neuralDynamicsTime << " microseconds ("
                  << (profile.totalTime > 0 ? profile.neuralDynamicsTime / profile.totalTime * 100 : 0)
                  << "%)" << std::endl;
        std::cout << "Plasticity: " << profile.plasticityTime << " microseconds ("
                  << (profile.totalTime > 0 ? profile.plasticityTime / profile.totalTime * 100 : 0)
                  << "%)" << std::endl;
        std::cout << "Memory management: " << profile.memoryManagementTime << " microseconds ("
                  << (profile.totalTime > 0 ? profile.memoryManagementTime / profile.totalTime * 100 : 0)
                  << "%)" << std::endl;
        std::cout << "Synchronization: " << profile.synchronizationTime << " microseconds ("
                  << (profile.totalTime > 0 ? profile.synchronizationTime / profile.totalTime * 100 : 0)
                  << "%)" << std::endl;
        
        // Identify bottlenecks
        identifyPerformanceBottlenecks(profile);
        
        // Generate recommendations
        generatePerformanceRecommendations(profile);
    }
    
    void identifyPerformanceBottlenecks(const PerformanceProfile& profile) {
        std::cout << "\n=== Performance Bottlenecks ===" << std::endl;
        
        double neuralDynamicsRatio = profile.totalTime > 0 ? 
            profile.neuralDynamicsTime / profile.totalTime * 100 : 0;
        double plasticityRatio = profile.totalTime > 0 ? 
            profile.plasticityTime / profile.totalTime * 100 : 0;
        double syncRatio = profile.totalTime > 0 ? 
            profile.synchronizationTime / profile.totalTime * 100 : 0;
        
        if (neuralDynamicsRatio > 50) {
            std::cout << "1. Neural dynamics computation is the main bottleneck (" 
                      << neuralDynamicsRatio << "%)" << std::endl;
            std::cout << "   Recommendations:" << std::endl;
            std::cout << "   - Optimize neural dynamics implementation" << std::endl;
            std::cout << "   - Use SIMD vectorization" << std::endl;
            std::cout << "   - Implement batch processing" << std::endl;
            std::cout << "   - Reduce time step size" << std::endl;
        }
        
        if (plasticityRatio > 30) {
            std::cout << "2. Plasticity computation is the main bottleneck (" 
                      << plasticityRatio << "%)" << std::endl;
            std::cout << "   Recommendations:" << std::endl;
            std::cout << "   - Optimize STDP implementation" << std::endl;
            std::cout << "   - Use sparse matrix operations" << std::endl;
            std::cout << "   - Implement parallel plasticity" << std::endl;
        }
        
        if (syncRatio > 20) {
            std::cout << "3. Synchronization overhead is high (" << syncRatio << "%)" << std::endl;
            std::cout << "   Recommendations:" << std::endl;
            std::cout << "   - Use lock-free algorithms" << std::endl;
            std::cout << "   - Reduce thread count" << std::endl;
            std::cout << "   - Implement work stealing" << std::endl;
        }
        
        // Check individual function performance
        for (size_t i = 0; i < profile.functionTimes.size(); ++i) {
            if (profile.functionTimes[i] > profile.totalTime * 0.3) {
                std::cout << (i + 4) << ". Function '" << profile.functionNames[i] << 
                          "' is slow (" << profile.functionTimes[i] << " microseconds)" << std::endl;
            }
        }
    }
    
    void generatePerformanceRecommendations(const PerformanceProfile& profile) {
        std::cout << "\n=== Performance Recommendations ===" << std::endl;
        
        double efficiency = calculateEfficiency(profile);
        if (efficiency < 50) {
            std::cout << "1. Overall efficiency is low (< 50%)" << std::endl;
            std::cout << "   - Identify and fix performance bottlenecks" << std::endl;
            std::cout << "   - Optimize memory access patterns" << std::endl;
            std::cout << "   - Consider hardware acceleration" << std::endl;
        } else if (efficiency < 80) {
            std::cout << "2. Efficiency is moderate (50-80%)" << std::endl;
            std::cout << "   - Minor optimizations possible" << std::endl;
            std::cout << "   - Focus on identified bottlenecks" << std::endl;
        } else {
            std::cout << "3. Efficiency is good (> 80%)" << std::endl;
            std::cout << "   - Optimize for specific use case" << std::endl;
            std::cout << "   - Consider specialized hardware" << std::endl;
        }
        
        // Check memory usage
        double memoryEfficiency = calculateMemoryEfficiency();
        if (memoryEfficiency < 70) {
            std::cout << "4. Memory efficiency is low (< 70%)" << std::endl;
            std::cout << "   - Optimize data layout" << std::endl;
            std::cout << "   - Use memory pools" << std::endl;
            std::cout << "   - Implement compression" << std::endl;
        }
        
        // Check scalability
        double scalability = calculateScalability();
        if (scalability < 0.8) {
            std::cout << "5. Scalability is limited (< 80%)" << std::endl;
            std::cout << "   - Identify scalability bottlenecks" << std::endl;
            std::cout << "   - Optimize for parallel processing" << std::endl;
        }
    }
    
    double calculateEfficiency(const PerformanceProfile& profile) const {
        // Calculate overall efficiency
        double idealTime = calculateIdealTime(profile);
        return (idealTime / profile.totalTime) * 100.0;
    }
    
    double calculateIdealTime(const PerformanceProfile& profile) const {
        // Calculate ideal execution time
        // This is a simplified calculation
        double baseTime = 1000.0; // baseline time in microseconds
        double overheadFactor = 1.5; // typical overhead factor
        
        return baseTime / overheadFactor;
    }
    
    double calculateMemoryEfficiency() const {
        // Calculate memory efficiency
        // Implementation
        return 0.0;
    }
    
    double calculateScalability() const {
        // Calculate scalability
        // Implementation
        return 0.0;
    }
    
    std::mutex profileMutex_;
    PerformanceProfile lastProfile_;
};
```

### 3. Advanced Debugging Scenarios

#### Corrupted State Detection
```cpp
// Detect corrupted neural state
class NLMStateValidator {
public:
    bool validateNeuralState(const Brain& brain) const {
        // Validate neuron states
        if (!validateNeuronStates(brain)) {
            return false;
        }
        
        // Validate synaptic states
        if (!validateSynapseStates(brain)) {
            return false;
        }
        
        // Validate memory states
        if (!validateMemoryStates(brain)) {
            return false;
        }
        
        // Validate developmental states
        if (!validateDevelopmentStates(brain)) {
            return false;
        }
        
        return true;
    }
    
    bool validateNeuronStates(const Brain& brain) const {
        const auto& regions = brain.getRegions();
        
        for (const auto& region : regions) {
            for (const auto& neuron : region->getNeurons()) {
                if (!validateNeuronState( neuron)) {
                    std::cout << "Invalid neuron state detected: neuron " 
                              << neuron->getId().value() << std::endl;
                    return false;
                }
            }
        }
        
        return true;
    }
    
    bool validateNeuronState(const std::unique_ptr<Neuron>& neuron) const {
        const NeuronState& state = neuron->getState();
        
        // Check for valid values
        if (std::isnan(state.membranePotential)) {
            return false;
        }
        
        if (state.membranePotential < -100.0f || state.membranePotential > 100.0f) {
            return false;
        }
        
        if (state.threshold < -100.0f || state.threshold > 100.0f) {
            return false;
        }
        
        if (state.refractoryRemaining > state.refractoryPeriod) {
            return false;
        }
        
        return true;
    }
    
    bool validateSynapseStates(const Brain& brain) const {
        // Validate synapse states
        // Implementation
        return true;
    }
    
    bool validateMemoryStates(const Brain& brain) const {
        // Validate memory states
        // Implementation
        return true;
    }
    
    bool validateDevelopmentStates(const Brain& brain) const {
        // Validate developmental states
        // Implementation
        return true;
    }
    
    void fixCorruptedState(Brain& brain) {
        // Attempt to fix corrupted state
        // Implementation
    }
};
```

#### Learning Verification
```cpp
// Verify learning behavior
class NLMLearningValidator {
public:
    bool validateLearning(const Brain& brain, double learningThreshold = 0.1) const {
        // Check if learning is occurring
        double initialLearningRate = getInitialLearningRate();
        double currentLearningRate = getCurrentLearningRate(brain);
        
        if (currentLearningRate <= initialLearningRate) {
            return false;
        }
        
        if (currentLearningRate < learningThreshold) {
            return false;
        }
        
        return true;
    }
    
    double getInitialLearningRate() const {
        // Get initial learning rate from configuration
        // Implementation
        return 0.01;
    }
    
    double getCurrentLearningRate(const Brain& brain) const {
        // Calculate current learning rate based on system state
        // Implementation
        return 0.0;
    }
    
    bool validatePlasticityLearning(const Brain& brain) const {
        // Validate that plasticity is working
        // Implementation
        return true;
    }
    
    bool validateMemoryLearning(const Brain& brain) const {
        // Validate that memory is learning
        // Implementation
        return true;
    }
    
    bool validateNeuromodulationLearning(const Brain& brain) const {
        // Validate that neuromodulation is learning
        // Implementation
        return true;
    }
};
```

## Debugging Workflow

### 1. Issue Identification

#### Step 1: Collect Symptoms
- Document all error messages
- Record timing information
- Capture system state
- Note environmental conditions

#### Step 2: Reproduce the Issue
- Create minimal reproduction case
- Isolate the problematic component
- Test with different configurations
- Vary input data

#### Step 3: Gather Information
- Collect system logs
- Capture debug output
- Record performance metrics
- Document configuration

### 2. Root Cause Analysis

#### Step 4: Systematic Debugging
- Use breakpoints at suspected locations
- Step through code execution
- Monitor variable values
- Check memory usage

#### Step 5: Isolation Testing
- Test individual components
- Validate input/output
- Check for edge cases
- Verify boundary conditions

### 3. Solution Implementation

#### Step 6: Apply Fix
- Implement corrective action
- Verify fix addresses root cause
- Test in isolation
- Validate in integrated system

#### Step 7: Regression Testing
- Re-run original test case
- Test edge cases
- Verify performance impact
- Document changes

## Debugging Tools and Techniques

### 1. Built-in Debugging Tools

#### Debug Output Levels
```cpp
// Configure debugging verbosity
NLMDebugger::DebugLevel debugLevel = NLMDebugger::DebugLevel::Detailed;

NLMDebugger debugger;
debugger.setLevel(debugLevel);
debugger.enableProfiling(true);
debugger.enableMemoryTracking(true);
debugger.enableStateLogging(true);
```

#### Real-time Monitoring
```cpp
// Start real-time monitoring
NLMDebugger debugger(options);
debugger.startMonitoring();

while (simulationRunning) {
    PerformanceMetrics metrics = debugger.getCurrentMetrics();
    printPerformanceMetrics(metrics);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

debugger.stopMonitoring();
debugger.generateDebugReport();
```

### 2. External Debugging Tools

#### GDB Integration
```bash
# Launch NLM with debugging
./nlm --debug-level=3 --log-file=debug.log --profile

# Attach GDB
./nlm_debug gdb nlm

# Set breakpoints
break brain.cpp:150
break neuron.cpp:200

# Step through execution
continue
step
next

# Inspect variables
print brain.getTotalNeuronCount()
print neuron.getMembranePotential()
```

#### Valgrind
```bash
# Run with memory debugging
valgrind --leak-check=full ./nlm --debug

# Check for memory errors
valgrind --tool=memcheck ./nlm

# Check for threading issues
valgrind --tool=helgrind ./nlm
```

### 3. Visualization Debugging
```cpp
// Enable visualization debugging
NLMDebugger debugger;
debugger.enableVisualization("qt");

// Visualize neural network
std::thread visualizationThread([&]() {
    while (visualizationEnabled) {
        debugger.visualizeNeuralNetwork(brain);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
});

// Run simulation
runSimulation(brain);

// Stop visualization
visualizationEnabled = false;
if (visualizationThread.joinable()) {
    visualizationThread.join();
}
```

## Common Debugging Scenarios

### Scenario 1: No Learning Detected

#### Symptoms
- Synaptic weights remain constant
- Memory activation doesn't change
- Curiosity levels remain at baseline

#### Debugging Steps
1. Verify plasticity is enabled
2. Check for correlated spike patterns
3. Validate reward signals
4. Examine neuromodulation

#### Solution
```cpp
// Check if plasticity is working
bool stdpEnabled = config.get<bool>"plasticity.stdp.enable";
bool hebbianEnabled = config.get<bool>"plasticity.hebbian.enable";

if (!stdpEnabled && !hebbianEnabled) {
    std::cout << "Error: Plasticity not enabled" << std::endl;
    return false;
}

// Check for correlated activity
if (!hasCorrelatedSpikePatterns(brain)) {
    std::cout << "Warning: No correlated spike patterns detected" << std::endl;
    // Consider adjusting connection probability
}
```

### Scenario 2: System Instability

#### Symptoms
- System diverges to infinity
- Numerical overflow
- Excessive resource usage
- Frequent crashes

#### Debugging Steps
1. Check for numerical instability
2. Verify memory management
3. Monitor resource usage
4. Check for race conditions

#### Solution
```cpp
// Check numerical stability
bool isNumericallyStable(const Brain& brain) {
    const auto& regions = brain.getRegions();
    
    for (const auto& region : regions) {
        for (const auto& neuron : region->getNeurons()) {
            const NeuronState& state = neuron->getState();
            
            // Check for NaN or infinity
            if (std::isnan(state.membranePotential) || std::isinf(state.membranePotential)) {
                return false;
            }
            
            // Check for extreme values
            if (std::abs(state.membranePotential) > 1000.0f) {
                return false;
            }
        }
    }
    
    return true;
}
```

### Scenario 3: Performance Degradation

#### Symptoms
- Slow simulation speed
- Increasing resource usage
- Memory usage growth
- Synchronization overhead

#### Debugging Steps
1. Profile system performance
2. Identify bottlenecks
3. Analyze memory usage
4. Check for memory leaks

#### Solution
```cpp
// Profile performance
NLMDebugger debugger;
debugger.startProfiling();

// Run simulation
runSimulation(brain);

// Stop profiling
debugger.stopProfiling();

// Analyze results
PerformanceMetrics metrics = debugger.getCurrentMetrics();
debugger.analyzePerformanceProfile(metrics);
```

## Debugging Summary

### Key Debugging Principles

1. **Reproducibility**: Create consistent test cases
2. **Isolation**: Test components individually
3. **Verification**: Validate fixes thoroughly
4. **Documentation**: Document findings and solutions
5. **Prevention**: Implement measures to prevent future issues

### Debugging Tools Summary

| Tool | Purpose | Platform | Complexity |
|------|---------|----------|------------|
| NLMDebugger | Built-in debugging | Cross-platform | Low |
| GDB | Source-level debugging | Linux/macOS | Medium |
| Valgrind | Memory debugging | Linux | High |
| Visualizer | Neural visualization | Cross-platform | Medium |
| Performance Monitor | Performance analysis | Cross-platform | High |

### Testing Checklist

#### Unit Tests
- [ ] Test all neural dynamics functions
- [ ] Test plasticity rules
- [ ] Test memory systems
- [ ] Test neuromodulation
- [ ] Test development

#### Integration Tests
- [ ] Test brain initialization
- [ ] Test subsystem integration
- [ ] Test simulation stability
- [ ] Test performance requirements

#### System Tests
- [ ] Test learning capabilities
- [ ] Test memory retention
- [ ] Test behavior validation
- [ ] Test stress testing

#### Performance Tests
- [ ] Test load characteristics
- [ ] Test scalability
- [ ] Test resource efficiency
- [ ] Test real-time capabilities

## Conclusion

Comprehensive testing and debugging are essential for ensuring the reliability and correctness of NLM simulations. The testing framework provides:

1. **Structured Testing**: Unit tests, integration tests, and system tests
2. **Advanced Debugging**: Memory tracking, performance profiling, visualization
3. **Systematic Troubleshooting**: Step-by-step debugging process
4. **Validation Tools**: Performance analysis, learning verification, state validation
5. **Integration Support**: Visualization tools and external debugging integration

By implementing thorough testing and debugging practices, developers can ensure:

- **Correctness**: All components work as expected
- **Reliability**: Systems handle edge cases gracefully
- **Performance**: Optimal performance for target use cases
- **Maintainability**: Easy to debug and modify
- **Scalability**: Effective scaling with increasing complexity

The testing and debugging guide provides the foundation for creating robust, reliable, and high-performance NLM simulations that meet the demanding requirements of modern neuroscience research and real-world applications.