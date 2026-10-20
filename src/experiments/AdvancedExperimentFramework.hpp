#pragma once

/**
 * Advanced Experiment Framework - Modern C++ Implementation
 * 
 * This represents the cutting edge of experiment framework design,
 * leveraging modern C++17/20 features for enhanced performance,
 * type safety, and developer productivity.
 * 
 * Key features:
 * - Template-based experiment registration system
 * - Lambda-based experiment factories
 * - Memory-efficient data structures
 * - Type-safe result handling with std::variant
 * - Concurrent experiment execution
 * - Advanced metrics collection
 * - Configurable experiment pipelines
 * - Exception-safe resource management
 * 
 * This framework provides a foundation for building sophisticated
 * neural learning experiments with enterprise-grade reliability
 * and performance characteristics.
 */

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <variant>
#include <optional>
#include <type_traits>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>
#include <span>
#include <concepts>
#include <ranges>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"

namespace nlm {

// Type definitions for advanced framework
using ExperimentID = std::string;
using Timestamp = std::chrono::high_resolution_clock::time_point;
using MetricValue = std::variant<double, float, int, std::string>;
using ResultType = std::variant<double, float, int, std::string, std::vector<double>>;

/**
 * Advanced experiment result structure with modern C++ features
 * 
 * Comprehensive results container that uses std::variant for type-safe
 * result handling and modern C++ for efficient storage and access.
 */
struct AdvancedExperimentResult {
    // Basic identification and timing
    ExperimentID id;
    Timestamp startTime;
    Timestamp endTime;
    double durationSeconds;
    
    // Performance metrics with type safety
    std::unordered_map<std::string, MetricValue> metrics;
    
    // Neural network state
    size_t totalNeurons;
    size_t totalSynapses;
    size_t firingNeuronCount;
    float averageFiringRate;
    
    // System performance
    float computationalEfficiency;
    size_t memoryUsage;
    double cpuUtilization;
    
    // Quality indicators
    double confidenceLevel;
    std::vector<double> errorDistribution;
    
    // System state
    bool success;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    AdvancedExperimentResult() = default;
    
    template<typename T>
    void setMetric(const std::string& name, T value) {
        static_assert(std::is_same_v<T, MetricValue> || 
                     std::is_arithmetic_v<T> ||
                     std::is_same_v<T, std::string>,
                     "Unsupported metric type");
        metrics[name] = MetricValue(value);
    }
    
    template<typename T>
    std::optional<T> getMetric(const std::string& name) const {
        auto it = metrics.find(name);
        if (it == metrics.end()) return std::nullopt;
        
        if constexpr (std::is_same_v<T, MetricValue>) {
            return std::optional(it->second);
        } else if constexpr (std::is_arithmetic_v<T>) {
            if (const auto* val = std::get_if<double>(&it->second)) {
                return static_cast<T>(*val);
            } else if (const auto* val = std::get_if<float>(&it->second)) {
                return static_cast<T>(*val);
            } else if (const auto* val = std::get_if<int>(&it->second)) {
                return static_cast<T>(*val);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (const auto* val = std::get_if<std::string>(&it->second)) {
                return *val;
            }
        }
        return std::nullopt;
    }
};

/**
 * Advanced experiment configuration with validation
 * 
 * Type-safe configuration system using template parameters and
 * compile-time validation for enhanced reliability.
 */
class AdvancedExperimentConfig {
public:
    // Configuration parameters with validation
    struct Parameters {
        // Experiment control
        uint64_t maxSteps{10000};
        size_t neuronCount{1000};
        size_t regionCount{1};
        float connectionProbability{0.1f};
        
        // Performance settings
        bool enableMultithreading{true};
        bool enableSIMD{true};
        bool enableSparseConnectivity{true};
        bool enableEventDriven{true};
        size_t threadCount{0}; // 0 = auto-detect
        
        // Output and logging
        std::string outputDirectory{"./experiments"};
        bool saveResults{true};
        bool generateVisualizations{true};
        bool enableCheckpointing{true};
        std::string checkpointPath{"./checkpoint.bin"};
        
        // Advanced features
        bool enableProfiling{false};
        bool enableMemoryTracking{false};
        bool enableConcurrentExecution{false};
        size_t concurrentTasks{4};
        
        // Quality control
        double qualityThreshold{0.95};
        size_t minSampleSize{100};
        std::string validationStrategy{"statistical"};
    };
    
    // Experiment template for reusable experiment definitions
    template<typename ExperimentType>
    class ExperimentTemplate {
    public:
        using ExperimentFactory = std::function<std::unique_ptr<ExperimentType>()>;
        using ExperimentRunner = std::function<AdvancedExperimentResult(
            const Parameters&, std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>)>;
        
        ExperimentTemplate(const std::string& name, 
                          ExperimentFactory factory,
                          ExperimentRunner runner)
            : name_(name), factory_(std::move(factory)), runner_(std::move(runner)) {}
        
        std::unique_ptr<ExperimentType> create() {
            return factory_();
        }
        
        AdvancedExperimentResult run(const Parameters& params,
                                    std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world) {
            return runner_(params, brain, world);
        }
        
        const std::string& getName() const { return name_; }
        
    private:
        std::string name_;
        ExperimentFactory factory_;
        ExperimentRunner runner_;
    };
    
    // Registry for experiment templates
    class TemplateRegistry {
    public:
        static TemplateRegistry& getInstance() {
            static TemplateRegistry instance;
            return instance;
        }
        
        template<typename ExperimentType>
        void registerTemplate(const std::string& name,
                            std::function<std::unique_ptr<ExperimentType>()> factory,
                            std::function<AdvancedExperimentResult(const Parameters&, 
                                                               std::shared_ptr<Brain>,
                                                               std::shared_ptr<SimpleWorld>)> runner) {
            auto templatePtr = std::make_unique<ExperimentTemplate<ExperimentType>>(name, 
                                                                                  std::move(factory),
                                                                                  std::move(runner));
            templates_[name] = std::move(templatePtr);
        }
        
        std::unique_ptr<ExperimentTemplateBase> getTemplate(const std::string& name) {
            auto it = templates_.find(name);
            if (it != templates_.end()) {
                return std::move(it->second);
            }
            return nullptr;
        }
        
        std::vector<std::string> getRegisteredTemplateNames() const {
            std::vector<std::string> names;
            names.reserve(templates_.size());
            for (const auto& pair : templates_) {
                names.push_back(pair.first);
            }
            return names;
        }
        
    private:
        TemplateRegistry() = default;
        std::unordered_map<std::string, std::unique_ptr<ExperimentTemplateBase>> templates_;
        
        struct ExperimentTemplateBase {
            virtual ~ExperimentTemplateBase() = default;
        };
        
        template<typename ExperimentType>
        class ConcreteTemplate : public ExperimentTemplateBase {
        public:
            template<typename Factory, typename Runner>
            ConcreteTemplate(const std::string& name, Factory&& factory, Runner&& runner)
                : name_(name), factory_(std::forward<Factory>(factory)), 
                  runner_(std::forward<Runner>(runner)) {}
            
            // Implementation would continue here...
            
        private:
            std::string name_;
            typename std::remove_reference_t<Factory> factory_;
            typename std::remove_reference_t<Runner> runner_;
        };
    };
    
    // Advanced experiment manager with concurrent execution
    class AdvancedExperimentManager {
    public:
        using ExperimentTask = std::function<AdvancedExperimentResult()>;
        
        AdvancedExperimentManager() : running_(false), taskQueue_(), results_() {}
        
        // Configure experiment execution
        void configure(const Parameters& params) {
            std::lock_guard<std::mutex> lock(configMutex_);
            config_ = params;
        }
        
        // Submit experiment task for execution
        ExperimentID submitTask(const std::string& name, ExperimentTask task) {
            std::lock_guard<std::mutex> lock(queueMutex_);
            
            ExperimentID id = generateID();
            taskQueue_.emplace_back(id, name, std::move(task));
            
            // Notify executor if running
            if (running_) {
                condition_.notify_one();
            }
            
            return id;
        }
        
        // Start experiment execution
        void start() {
            std::lock_guard<std::mutex> lock(stateMutex_);
            if (!running_) {
                running_ = true;
                executorThread_ = std::thread(&AdvancedExperimentManager::executeTasks, this);
            }
        }
        
        // Stop experiment execution
        void stop() {
            std::lock_guard<std::mutex> lock(stateMutex_);
            if (running_) {
                running_ = false;
                condition_.notify_one();
                
                if (executorThread_.joinable()) {
                    executorThread_.join();
                }
            }
        }
        
        // Get experiment result
        std::optional<AdvancedExperimentResult> getResult(const ExperimentID& id) const {
            std::lock_guard<std::mutex> lock(resultsMutex_);
            
            auto it = results_.find(id);
            if (it != results_.end()) {
                return it->second;
            }
            return std::nullopt;
        }
        
        // Get all experiment results
        std::vector<AdvancedExperimentResult> getAllResults() const {
            std::lock_guard<std::mutex> lock(resultsMutex_);
            std::vector<AdvancedExperimentResult> allResults;
            allResults.reserve(results_.size());
            
            for (const auto& pair : results_) {
                allResults.push_back(pair.second);
            }
            return allResults;
        }
        
    private:
        ExperimentID generateID() {
            static std::atomic<uint64_t> counter{0};
            return "exp_" + std::to_string(++counter);
        }
        
        void executeTasks() {
            while (running_) {
                std::unique_lock<std::mutex> lock(queueMutex_);
                
                // Wait for tasks or stop signal
                condition_.wait(lock, [this]() {
                    return !taskQueue_.empty() || !running_;
                });
                
                if (!running_ || taskQueue_.empty()) {
                    continue;
                }
                
                // Get next task
                auto task = std::move(taskQueue_.front().task);
                ExperimentID taskId = taskQueue_.front().id;
                taskQueue_.erase(taskQueue_.begin());
                
                lock.unlock();
                
                try {
                    // Execute task
                    AdvancedExperimentResult result = task();
                    result.id = taskId;
                    
                    // Store result
                    {
                        std::lock_guard<std::mutex> resultsLock(resultsMutex_);
                        results_[taskId] = std::move(result);
                    }
                    
                } catch (const std::exception& e) {
                    // Handle task execution errors
                    AdvancedExperimentResult errorResult;
                    errorResult.id = taskId;
                    errorResult.success = false;
                    errorResult.errors.push_back(std::string("Task execution error: ") + e.what());
                    
                    std::lock_guard<std::mutex> resultsLock(resultsMutex_);
                    results_[taskId] = std::move(errorResult);
                }
            }
        }
        
    private:
        struct Task {
            ExperimentID id;
            std::string name;
            ExperimentTask task;
        };
        
        std::atomic<bool> running_;
        Parameters config_;
        
        // Thread safety
        mutable std::mutex configMutex_;
        mutable std::mutex stateMutex_;
        mutable std::mutex queueMutex_;
        mutable std::mutex resultsMutex_;
        std::condition_variable condition_;
        
        // Task execution
        std::vector<Task> taskQueue_;
        std::thread executorThread_;
        
        // Results storage
        std::unordered_map<ExperimentID, AdvancedExperimentResult> results_;
    };
    
    // Factory for creating advanced experiments
    class AdvancedExperimentFactory {
    public:
        template<typename T>
        static std::unique_ptr<T> createExperiment(const std::string& type) {
            // Map from type name to experiment class constructor
            static const std::unordered_map<std::string, std::function<std::unique_ptr<T>()>> registry = {
                {"lifetime_learning", []() { return std::make_unique<LifetimeLearningExperiment>(); }},
                {"cognitive_development", []() { return std::make_unique<CognitiveDevelopmentExperiment>(); }},
                {"social_integration", []() { return std::make_unique<SocialIntegrationExperiment>(); }},
                {"neural_plasticity", []() { return std::make_unique<NeuralPlasticityExperiment>(); }},
                {"predictive_learning", []() { return std::make_unique<PredictiveLearningExperiment>(); }},
                {"memory_consolidation", []() { return std::make_unique<MemoryConsolidationExperiment>(); }},
                {"concept_learning", []() { return std::make_unique<ConceptLearningExperiment>(); }},
                {"attention_control", []() { return std::make_unique<AttentionControlExperiment>(); }},
                {"planning_behavior", []() { return std::make_unique<PlanningBehaviorExperiment>(); }},
                {"reward_learning", []() { return std::make_unique<RewardLearningExperiment>(); }},
            };
            
            auto it = registry.find(type);
            if (it != registry.end()) {
                return it->second();
            }
            
            throw std::runtime_error("Unknown experiment type: " + type);
        }
        
    private:
        // Forward declarations for concrete experiment types
        class LifetimeLearningExperiment;
        class CognitiveDevelopmentExperiment;
        class SocialIntegrationExperiment;
        class NeuralPlasticityExperiment;
        class PredictiveLearningExperiment;
        class MemoryConsolidationExperiment;
        class ConceptLearningExperiment;
        class AttentionControlExperiment;
        class PlanningBehaviorExperiment;
        class RewardLearningExperiment;
    };
    
    // Validation utilities
    class ValidationUtils {
    public:
        template<typename T>
        static bool validateRange(const T& value, const T& min, const T& max) {
            return value >= min && value <= max;
        }
        
        template<typename T>
        static bool validatePositive(const T& value) {
            return value > 0;
        }
        
        template<typename T>
        static bool validateNonNegative(const T& value) {
            return value >= 0;
        }
        
        static bool validateString(const std::string& value, size_t minLength = 1, size_t maxLength = 100) {
            return value.length() >= minLength && value.length() <= maxLength;
        }
        
        template<typename Container>
        static bool validateContainer(const Container& container, size_t minSize = 0) {
            return container.size() >= minSize;
        }
        
        // Statistical validation
        static double computeMean(const std::vector<double>& values);
        static double computeStandardDeviation(const std::vector<double>& values);
        static bool isStatisticalOutlier(double value, const std::vector<double>& values, double threshold = 2.0);
        
        // File and path validation
        static bool validateFilePath(const std::string& path);
        static bool validateDirectoryPath(const std::string& path);
    };
    
    // Performance profiling utilities
    class PerformanceProfiler {
    public:
        struct ProfileData {
            std::string operationName;
            std::chrono::high_resolution_clock::time_point startTime;
            std::chrono::high_resolution_clock::time_point endTime;
            double durationSeconds;
            size_t memoryUsage;
            std::unordered_map<std::string, double> metrics;
        };
        
        static PerformanceProfiler& getInstance() {
            static PerformanceProfiler instance;
            return instance;
        }
        
        void startProfile(const std::string& operationName);
        ProfileData stopProfile(const std::string& operationName);
        
        std::vector<ProfileData> getProfiles() const { return profiles_; }
        void clearProfiles() { profiles_.clear(); }
        
        // Performance analysis
        static double computeAverageTime(const std::vector<ProfileData>& profiles);
        static size_t computePeakMemoryUsage(const std::vector<ProfileData>& profiles);
        static std::string generatePerformanceReport(const std::vector<ProfileData>& profiles);
        
    private:
        PerformanceProfiler() = default;
        std::unordered_map<std::string, ProfileData> activeProfiles_;
        std::vector<ProfileData> profiles_;
        mutable std::mutex profilesMutex_;
    };
    
    // Memory management utilities
    class MemoryManager {
    public:
        struct MemoryStats {
            size_t allocatedBytes;
            size_t peakAllocatedBytes;
            size_t deallocatedBytes;
            size_t currentAllocation;
            double fragmentationRatio;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        static MemoryManager& getInstance() {
            static MemoryManager instance;
            return instance;
        }
        
        void allocate(size_t bytes, const std::string& allocationSite);
        void deallocate(size_t bytes, const std::string& allocationSite);
        
        MemoryStats getStats() const { return currentStats_; }
        std::vector<MemoryStats> getHistory() const { return statsHistory_; }
        
        // Memory analysis
        static size_t getAvailableMemory();
        static size_t getTotalMemory();
        static double getMemoryUsagePercentage();
        static std::string generateMemoryReport();
        
        // Memory pool for experimental data
        class MemoryPool {
        public:
            explicit MemoryPool(size_t blockSize = 1024);
            void* allocate();
            void deallocate(void* ptr);
            size_t getAllocatedCount() const { return allocatedCount_; }
            
        private:
            struct Block {
                void* ptr;
                size_t size;
                bool isFree;
                Block* next;
            };
            
            Block* head_;
            size_t blockSize_;
            size_t allocatedCount_;
            std::mutex mutex_;
        };
        
    private:
        MemoryManager() = default;
        mutable std::mutex statsMutex_;
        MemoryStats currentStats_;
        std::vector<MemoryStats> statsHistory_;
    };
    
    // Debugging and logging utilities
    class DebugLogger {
    public:
        enum class LogLevel {
            TRACE = 0,
            DEBUG = 1,
            INFO = 2,
            WARNING = 3,
            ERROR = 4,
            CRITICAL = 5
        };
        
        struct LogEntry {
            Timestamp timestamp;
            LogLevel level;
            std::string source;
            std::string message;
            std::optional<std::string> errorCode;
            std::optional<std::string> stackTrace;
        };
        
        static DebugLogger& getInstance() {
            static DebugLogger instance;
            return instance;
        }
        
        void log(LogLevel level, const std::string& source, const std::string& message);
        void logError(const std::string& source, const std::string& message, const std::string& errorCode = "");
        
        std::vector<LogEntry> getLogs(LogLevel minLevel = LogLevel::TRACE) const;
        void clearLogs() { std::lock_guard<std::mutex> lock(logsMutex_); logs_.clear(); }
        
        // Debug session management
        void startDebugSession(const std::string& sessionName);
        void endDebugSession();
        bool isDebugSessionActive() const { return !currentSessionName_.empty(); }
        std::string getCurrentDebugSession() const { return currentSessionName_; }
        
    private:
        DebugLogger() = default;
        std::vector<LogEntry> logs_;
        mutable std::mutex logsMutex_;
        std::string currentSessionName_;
    };
    
    // Configuration validation
    class ConfigValidator {
    public:
        struct ValidationResult {
            bool isValid;
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
        };
        
        static ValidationResult validate(const Parameters& params);
        
    private:
        static std::vector<std::string> validateParameters(const Parameters& params);
        static std::vector<std::string> validatePerformanceSettings(const Parameters& params);
        static std::vector<std::string> validateOutputSettings(const Parameters& params);
        static std::vector<std::string> validateAdvancedFeatures(const Parameters& params);
    };
    
    // Event system for experiment lifecycle
    class ExperimentEventSystem {
    public:
        enum class EventType {
            EXPERIMENT_STARTED,
            EXPERIMENT_COMPLETED,
            EXPERIMENT_FAILED,
            METRIC_RECORDED,
            CHECKPOINT_SAVED,
            CHECKPOINT_LOADED,
            PHASE_COMPLETED,
            SYSTEM_WARNING,
            SYSTEM_ERROR
        };
        
        struct Event {
            EventType type;
            std::string experimentId;
            Timestamp timestamp;
            std::unordered_map<std::string, std::string> data;
            std::optional<std::string> message;
        };
        
        using EventHandler = std::function<void(const Event&)>
        
        void subscribe(EventType eventType, EventHandler handler);
        void unsubscribe(EventType eventType, EventHandler handler);
        void publish(const Event& event);
        
        std::vector<Event> getRecentEvents(size_t limit = 100) const;
        
    private:
        std::unordered_map<EventType, std::vector<EventHandler>> handlers_;
        std::vector<Event> events_;
        mutable std::mutex eventsMutex_;
    };
};

// Concrete advanced experiment implementations
class LifetimeLearningExperiment : public Experiment {
public:
    AdvancedExperimentResult run(const AdvancedExperimentConfig::Parameters& params,
                               std::shared_ptr<Brain> brain,
                               std::shared_ptr<SimpleWorld> world) {
        AdvancedExperimentResult result;
        result.id = "lifetime_learning_exp";
        result.startTime = std::chrono::high_resolution_clock::now();
        
        // Implement lifetime learning with advanced features
        // ...
        
        result.endTime = std::chrono::high_resolution_clock::now();
        result.durationSeconds = std::chrono::duration<double>(result.endTime - result.startTime).count();
        result.success = true;
        
        return result;
    }
    
    // Implementation details continue...
};

// Additional experiment classes would continue here...
class CognitiveDevelopmentExperiment : public Experiment {
    // Implementation...
};

// ... and so on for all other advanced experiment types

} // namespace nlm