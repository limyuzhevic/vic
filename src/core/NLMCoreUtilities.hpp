// NLM Core Utilities
// Common functions and utilities used throughout the NLM codebase

#ifndef NLM_CORE_UTILITIES_HPP
#define NLM_CORE_UTILITIES_HPP

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <random>
#include <algorithm>

namespace nlm {

// Logging utilities
namespace LoggingUtils {
    // Format timing information for performance metrics
    std::string formatTiming(double seconds);
    
    // Create standardized log messages
    std::string logStep(size_t step, size_t totalSteps);
    std::string logProgress(size_t completed, size_t total, const std::string& message = "");
    
    // Format simulation statistics
    std::string formatStats(size_t neurons, size_t synapses, size_t spikes, double time);
}

// Brain initialization utilities
namespace InitUtils {
    // Common initialization sequence for neural components
    template<typename T>
    bool initializeComponent(std::shared_ptr<T> component, const std::string& componentName,
                            std::function<bool(std::shared_ptr<T>)> initFunc);
    
    // Safe brain initialization with error handling
    struct BrainInitResult {
        bool success;
        std::string errorMessage;
        std::shared_ptr<Brain> brain;
    };
    
    BrainInitResult safeBrainInitialize(std::shared_ptr<Config> config);
    
    // Component initialization checklist
    bool validateComponentDependencies(const std::vector<std::string>& componentNames);
}

// Current injection utilities
namespace InputUtils {
    // Common input patterns for neural stimulation
    struct InputPattern {
        std::string name;
        std::function<void(std::shared_ptr<Brain>)> apply;
    };
    
    // Predefined input patterns
    InputPattern randomSensoryInput(size_t neuronCount = 20, float current = 30.0f);
    InputPattern correlatedInput(size_t preNeuron, size_t postNeuron, 
                                float preCurrent = 60.0f, float postCurrent = 60.0f,
                                size_t delay = 2);
    InputPattern sustainedInput(float current = 50.0f, size_t duration = 100);
    
    // Generic input application
    void applyInputPattern(std::shared_ptr<Brain> brain, const InputPattern& pattern);
}

// Simulation utilities
namespace SimulationUtils {
    // Time management and step tracking
    struct SimulationInfo {
        size_t currentStep;
        double currentTime;
        size_t totalSteps;
        double timeLimit;
        
        double elapsedTime() const { return currentTime; }
        double remainingTime() const { return timeLimit - currentTime; }
        bool isComplete() const { return currentStep >= totalSteps || 
                                        (timeLimit > 0 && currentTime >= timeLimit); }
    };
    
    // Safe simulation stepping with error handling
    struct SimulationStepResult {
        bool success;
        size_t stepCompleted;
        std::string errorMessage;
        double timeAtStep;
    };
    
    SimulationStepResult safeSimulationStep(std::shared_ptr<Brain> brain, 
                                           size_t step, double time);
    
    // Performance monitoring
    struct PerformanceMetrics {
        double totalTime;
        size_t stepsCompleted;
        size_t spikesGenerated;
        size_t neuronsActive;
        
        double stepsPerSecond() const { 
            return stepsCompleted > 0 ? stepsCompleted / totalTime : 0.0; 
        }
        double spikesPerSecond() const {
            return totalTime > 0 ? spikesGenerated / totalTime : 0.0;
        }
    };
    
    PerformanceMetrics calculatePerformance(const SimulationInfo& info);
}

// Experiment utilities
namespace ExperimentUtils {
    // Experiment result tracking
    struct ExperimentResult {
        std::string experimentName;
        bool success;
        double executionTime;
        std::string output;
        std::vector<std::string> errors;
        
        bool isSuccessful() const { return success && errors.empty(); }
    };
    
    // Experiment execution wrapper
    ExperimentResult runExperiment(std::function<void()> experimentFunc,
                                   const std::string& name = "Experiment");
    
    // Metric collection and analysis
    struct Metric {
        std::string name;
        double value;
        std::string unit;
    };
    
    std::vector<Metric> extractMetrics(const std::string& data);
}

// Configuration utilities
namespace ConfigUtils {
    // Configuration validation helpers
    struct ValidationRule {
        std::string key;
        std::string type;  // "int", "double", "bool", "string"
        std::string rule;  // "min:value", "max:value", "in:list"
        std::string errorMessage;
    };
    
    bool validateConfig(const Config& config, const std::vector<ValidationRule>& rules);
    
    // Default configuration setter
    void setDefaultConfig(std::shared_ptr<Config> config);
    
    // Configuration file utilities
    std::string generateConfigFileName(const std::string& baseName = "config");
    bool isConfigFileValid(const std::string& filepath);
}

// Random number generation utilities
namespace RandomUtils {
    // Thread-safe random number generator
    class ThreadSafeRNG {
    private:
        mutable std::mt19937 generator;
        mutable std::mutex mutex;
    
    public:
        ThreadSafeRNG(uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count());
        
        int nextInt(int min, int max);
        double nextDouble(double min = 0.0, double max = 1.0);
        float nextFloat(float min = 0.0f, float max = 1.0f);
        bool nextBool(double probability = 0.5);
    };
    
    // Distribution utilities
    template<typename T>
    T randomFromDistribution(const std::vector<std::pair<T, double>>& distribution);
}

// Memory management utilities
namespace MemoryUtils {
    // Smart pointer utilities
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    
    // Object lifecycle tracking
    class ObjectTracker {
    private:
        std::vector<void*> objects;
        std::mutex mutex;
    
    public:
        void add(void* obj);
        void remove(void* obj);
        size_t count() const;
    };
    
    // Safe memory deallocation
    template<typename T>
    void safeDelete(T*& ptr);
}

// File I/O utilities
namespace FileUtils {
    // File operations with error handling
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    std::vector<std::string> listFiles(const std::string& directory, 
                                       const std::string& extension = "");
    
    // Atomic file operations
    bool atomicWrite(const std::string& filepath, const std::string& content);
    bool atomicRead(const std::string& filepath, std::string& content);
}

// String manipulation utilities
namespace StringUtils {
    // String formatting and manipulation
    std::string format(const std::string& format, ...);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    
    // Path utilities
    std::string getFileName(const std::string& path);
    std::string getFileExtension(const std::string& path);
    std::string removeFileExtension(const std::string& path);
    std::string joinPaths(const std::string& path1, const std::string& path2);
}

} // namespace nlm

#endif // NLM_CORE_UTILITIES_HPP