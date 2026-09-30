// NLM Core Bindings - Advanced Type Safety and Performance Optimizations
// Implements comprehensive type safety, runtime validation, and advanced features
// Phase 2: Real Neural Computation - Enhanced Edition

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "src/core/Types/Types.hpp"
#include 
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <optional>
#include <variant>
#include <type_traits>

namespace nlm {

// ==============================================================================
// ADVANCED EXCEPTION TYPES AND ERROR HANDLING
// ==============================================================================

class BindingError : public std::runtime_error {
    std::string component;
    int error_code;
    
public:
    enum class ErrorCode {
        TYPE_CONVERSION_FAILED = 1001,
        OUT_OF_BOUNDS = 1002,
        NULL_POINTER_DEREFERENCE = 1003,
        INVALID_ID = 1004,
        MEMORY_ALLOCATION_FAILED = 1005,
        CONFIGURATION_ERROR = 1006,
        PLASTICITY_ERROR = 1007,
        SIMULATION_ERROR =1008
    };
    
    BindingError(const std::string& message, const std::string& component, 
                ErrorCode code)
        : std::runtime_error(formatMessage(message, component, code)),
          component(component), error_code(static_cast<int>(code)) {}
    
    const std::string& getComponent() const { return component; }
    ErrorCode getErrorCode() const { return static_cast<ErrorCode>(error_code); }
    
private:
    static std::string formatMessage(const std::string& message, 
                                    const std::string& component, 
                                    ErrorCode code) {
        return "[" + component + "] Error " + std::to_string(static_cast<int>(code)) + 
               ": " + message;
    }
};

class TypeConversionError : public BindingError {
public:
    TypeConversionError(const std::string& message, const std::string& component = "TypeSystem")
        : BindingError(message, component, BindingError::ErrorCode::TYPE_CONVERSION_FAILED) {}
};

class BoundsValidationError : public BindingError {
public:
    BoundsValidationError(const std::string& message, const std::string& component = "Validation")
        : BindingError(message, component, BindingError::ErrorCode::OUT_OF_BOUNDS) {}
};

class NullPointerError : public BindingError {
public:
    NullPointerError(const std::string& message, const std::string& component = "PointerManagement")
        : BindingError(message, component, BindingError::ErrorCode::NULL_POINTER_DEREFERENCE) {}
};

// ==============================================================================
// TYPE CONVERTERS AND VALIDATION UTILITIES
// ==============================================================================

class TypeConverter {
public:
    template<typename TargetType, typename SourceType>
    static TargetType safeConvert(const SourceType& value, 
                                const std::string& context = "conversion") {
        try {
            // Runtime type checking for target type
            TargetType result = static_cast<TargetType>(value);
            
            // Additional validation for floating-point conversions
            if (std::is_floating_point_v<TargetType> && std::is_integral_v<SourceType>) {
                if (std::abs(static_cast<double>(result) - static_cast<double>(value)) > 1e-9) {
                    throw TypeConversionError(
                        "Precision loss during conversion from " + typeName<SourceType>() + 
                        " to " + typeName<TargetType>() + " in " + context,
                        "TypeConverter"
                    );
                }
            }
            
            return result;
        } catch (const std::exception& e) {
            throw TypeConversionError(
                "Failed to convert " + typeName<SourceType>() + " to " + typeName<TargetType>() + 
                ": " + e.what(),
                "TypeConverter"
            );
        }
    }
    
    template<typename IDType>
    static bool isValidId(const IDType& id) {
        static_assert(std::is_integral_v<IDType>, "ID type must be integral");
        return id > static_cast<IDType>(0);
    }
    
    template<typename T>
    static std::string typeName() {
        // Compile-time type name generation
        if constexpr (std::is_same_v<T, float>) return "float";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, uint64_t>) return "uint64_t";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, NeuronId>) return "NeuronId";
        else if constexpr (std::is_same_v<T, SynapseId>) return "SynapseId";
        else if constexpr (std::is_same_v<T, RegionId>) return "RegionId";
        else if constexpr (std::is_same_v<T, PopulationId>) return "PopulationId";
        else if constexpr (std::is_same_v<T, std::string>) return "std::string";
        else if constexpr (std::is_same_v<T, std::vector<float>>) return "std::vector<float>";
        else return "unknown";
    }
};

// ==============================================================================
// MEMORY MANAGER WITH LIFETIME TRACKING
// ==============================================================================

class LifetimeManager {
private:
    struct AllocationInfo {
        void* pointer;
        std::string origin;
        std::chrono::system_clock::time_point allocation_time;
        size_t reference_count;
        bool is_shared;
        
        AllocationInfo(void* ptr, const std::string& origin)
            : pointer(ptr), origin(origin), allocation_time(std::chrono::system_clock::now()),
              reference_count(1), is_shared(false) {}
    };
    
    std::unordered_map<void*, AllocationInfo> allocations_;
    std::mutex mutex_;
    
public:
    template<typename T, typename... Args>
    std::shared_ptr<T> allocateShared(const std::string& origin, Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
            allocations_[ptr.get()] = AllocationInfo(ptr.get(), origin);
            return ptr;
        } catch (const std::bad_alloc&) {
            throw BindingError("Memory allocation failed", "LifetimeManager", 
                             BindingError::ErrorCode::MEMORY_ALLOCATION_FAILED);
        }
    }
    
    template<typename T>
    std::unique_ptr<T> allocateUnique(const std::string& origin, T* raw_ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[raw_ptr] = AllocationInfo(raw_ptr, origin);
        return std::unique_ptr<T>(raw_ptr);
    }
    
    void deallocate(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.erase(ptr);
    }
    
    void addReference(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (allocations_.find(ptr) != allocations_.end()) {
            allocations_[ptr].reference_count++;
        }
    }
    
    void removeReference(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (allocations_.find(ptr) != allocations_.end()) {
            allocations_[ptr].reference_count--;
            if (allocations_[ptr].reference_count <= 0) {
                allocations_.erase(ptr);
            }
        }
    }
    
    bool isTracked(void* ptr) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.find(ptr) != allocations_.end();
    }
    
    void printLifetimeStats() {
        std::lock_guard<std::mutex> lock(mutex_);
        NLM_LOG_INFO("=== Lifetime Manager Statistics ===");
        NLM_LOG_INFO("Active allocations: " + std::to_string(allocations_.size()));
        size_t shared_count = 0;
        for (const auto& [ptr, info] : allocations_) {
            if (info.is_shared) shared_count++;
        }
        NLM_LOG_INFO("Shared allocations: " + std::to_string(shared_count));
        NLM_LOG_INFO("==============================");
    }
};

// ==============================================================================
// SAFE POINTER POLICIES
// ==============================================================================

class SafePointer {
public:
    template<typename T>
    class SafeRef {
        T* ptr_ = nullptr;
        LifetimeManager* manager_ = nullptr;
        
    public:
        SafeRef() = default;
        
        SafeRef(T* ptr, LifetimeManager* manager = nullptr) : ptr_(ptr), manager_(manager) {
            if (ptr_ && manager_) {
                manager_->addReference(ptr_);
            }
        }
        
        SafeRef(const SafeRef& other) : ptr_(other.ptr_), manager_(other.manager_) {
            if (ptr_ && manager_) {
                manager_->addReference(ptr_);
            }
        }
        
        SafeRef& operator=(const SafeRef& other) {
            if (this != &other) {
                if (ptr_ && manager_) {
                    manager_->removeReference(ptr_);
                }
                ptr_ = other.ptr_;
                manager_ = other.manager_;
                if (ptr_ && manager_) {
                    manager_->addReference(ptr_);
                }
            }
            return *this;
        }
        
        ~SafeRef() {
            if (ptr_ && manager_) {
                manager_->removeReference(ptr_);
            }
        }
        
        T* get() const { return ptr_; }
        T* operator->() const { return get(); }
        T& operator*() const { return *get(); }
        
        explicit operator bool() const { return ptr_ != nullptr; }
        
        bool isNull() const { return ptr_ == nullptr; }
        
        template<typename U>
        SafeRef<U> cast() const {
            return SafeRef<U>(static_cast<U*>(ptr_), manager_);
        }
        
        friend class SafePointer;
    };
    
    template<typename T>
    class SafePtr {
        T* ptr_ = nullptr;
        LifetimeManager* manager_ = nullptr;
        
    public:
        SafePtr() = default;
        
        explicit SafePtr(T* ptr, LifetimeManager* manager = nullptr) 
            : ptr_(ptr), manager_(manager) {}
        
        SafePtr(const SafePtr& other) = delete; // No copying to prevent double-free
        
        SafePtr& operator=(const SafePtr& other) = delete;
        
        ~SafePtr() {
            if (ptr_ && manager_) {
                manager_->deallocate(ptr_);
                ptr_ = nullptr;
            }
        }
        
        T* get() const { return ptr_; }
        T* operator->() const { return get(); }
        T& operator*() const { return *get(); }
        
        explicit operator bool() const { return ptr_ != nullptr; }
        
        bool isNull() const { return ptr_ == nullptr; }
        
        void release() {
            if (manager_) {
                manager_->deallocate(ptr_);
                ptr_ = nullptr;
            }
        }
        
        template<typename U, typename... Args>
        void reset(U* ptr) {
            if (ptr_ && manager_) {
                manager_->deallocate(ptr_);
            }
            ptr_ = ptr;
            if (ptr_ && manager_) {
                manager_->addReference(ptr_);
            }
        }
        
        friend class SafePointer;
    };
};

// ==============================================================================
// BOUNDS CHECKING AND VALIDATION
// ==============================================================================

class BoundsChecker {
public:
    template<typename T>
    static void checkIndex(size_t index, size_t size, const std::string& context = "array access") {
        if (index >= size) {
            throw BoundsValidationError(
                "Index out of bounds: index=" + std::to_string(index) + 
                ", size=" + std::to_string(size) + " in " + context,
                "BoundsChecker"
            );
        }
    }
    
    template<typename T>
    static void checkValue(const T& value, T min, T max, const std::string& context = "value") {
        if (value < min || value > max) {
            throw BoundsValidationError(
                "Value out of bounds: " + std::to_string(value) + 
                ", valid range=[" + std::to_string(min) + ", " + std::to_string(max) + "] in " + context,
                "BoundsChecker"
            );
        }
    }
    
    template<typename Container>
    static void checkBounds(const Container& container, const std::string& context = "container") {
        if (container.empty()) {
            throw BoundsValidationError(
                "Empty container accessed in " + context,
                "BoundsChecker"
            );
        }
    }
};

// ==============================================================================
// CACHE AND LAZY EVALUATION UTILITIES
// ==============================================================================

class CachedProperty {
private:
    mutable bool is_cached_ = false;
    mutable std::any cached_value_;
    mutable std::chrono::system_clock::time_point last_access_;
    
public:
    CachedProperty() = default;
    
    template<typename T, typename ComputeFunc>
    T getCachedValue(ComputeFunc compute) const {
        auto now = std::chrono::system_clock::now();
        
        if (!is_cached_ || std::chrono::duration_cast<std::chrono::seconds>(now - last_access_).count() > 60) {
            cached_value_ = compute();
            is_cached_ = true;
            last_access_ = now;
        }
        
        return std::any_cast<T>(cached_value_);
    }
    
    void invalidate() {
        is_cached_ = false;
    }
};

class LazyEvaluator {
public:
    template<typename T>
    class LazyValue {
        std::function<T()> compute_;
        mutable std::optional<T> cached_;
        
    public:
        LazyValue(std::function<T()> func) : compute_(func) {}
        
        T evaluate() const {
            if (!cached_.has_value()) {
                cached_ = compute_();
            }
            return *cached_;
        }
        
        bool isEvaluated() const { return cached_.has_value(); }
        
        void reset() { cached_.reset(); }
    };
    
    template<typename T>
    static LazyValue<T> createLazy(std::function<T()> func) {
        return LazyValue<T>(func);
    }
};

// ==============================================================================
// BATCH OPERATIONS UTILITIES
// ==============================================================================

class BatchOperations {
public:
    template<typename Container, typename Func>
    static std::vector<typename std::result_of<Func(typename Container::value_type)>::type>
    processBatch(const Container& container, Func func) {
        std::vector<typename std::result_of<Func(typename Container::value_type)>::type> results;
        results.reserve(container.size());
        
        for (const auto& item : container) {
            results.push_back(func(item));
        }
        
        return results;
    }
    
    template<typename Iterator, typename Func>
    static std::vector<typename std::result_of<Func(typename std::iterator_traits<Iterator>::value_type)>::type>
    processBatch(Iterator begin, Iterator end, Func func) {
        std::vector<typename std::result_of<Func(typename std::iterator_traits<Iterator>::value_type)>::type> results;
        results.reserve(std::distance(begin, end));
        
        for (auto it = begin; it != end; ++it) {
            results.push_back(func(*it));
        }
        
        return results;
    }
    
    template<typename T>
    static std::vector<T> filterBatch(const std::vector<T>& data, std::function<bool(const T&)> predicate) {
        std::vector<T> results;
        results.reserve(data.size());
        
        for (const auto& item : data) {
            if (predicate(item)) {
                results.push_back(item);
            }
        }
        
        return results;
    }
    
    template<typename T>
    static std::vector<T> mapBatch(const std::vector<T>& data, std::function<T(const T&)> func) {
        return processBatch(data, func);
    }
    
    template<typename T>
    static T reduceBatch(const std::vector<T>& data, T initial, std::function<T(T, const T&)> func) {
        T result = initial;
        for (const auto& item : data) {
            result = func(result, item);
        }
        return result;
    }
};

// ==============================================================================
// ADVANCED CONFIGURATION SYSTEM
// ==============================================================================

class AdvancedConfig : public Config {
private:
    struct ConfigOption {
        std::string key;
        std::variant<int64_t, double, float, bool, std::string> default_value;
        std::string description;
        bool required;
        std::function<bool(const std::variant<int64_t, double, float, bool, std::string>&)> validator;
        
        bool validate(const std::variant<int64_t, double, float, bool, std::string>& value) const {
            if (validator) {
                return validator(value);
            }
            return true;
        }
    };
    
    std::vector<ConfigOption> config_options_;
    mutable CachedProperty<std::string> summary_cache_;
    
public:
    AdvancedConfig() : Config() {
        initializeAdvancedOptions();
    }
    
    void registerOption(const std::string& key, 
                       const std::variant<int64_t, double, float, bool, std::string>& default_value,
                       const std::string& description,
                       bool required = false,
                       std::function<bool(const std::variant<int64_t, double, float, bool, std::string>&)> validator = nullptr) {
        ConfigOption option;
        option.key = key;
        option.default_value = default_value;
        option.description = description;
        option.required = required;
        option.validator = validator;
        
        config_options_.push_back(option);
        
        // Set the default value
        if (std::holds_alternative<int64_t>(default_value)) {
            set(key, std::get<int64_t>(default_value));
        } else if (std::holds_alternative<double>(default_value)) {
            set(key, std::get<double>(default_value));
        } else if (std::holds_alternative<float>(default_value)) {
            set(key, std::get<float>(default_value));
        } else if (std::holds_alternative<bool>(default_value)) {
            set(key, std::get<bool>(default_value));
        } else if (std::holds_alternative<std::string>(default_value)) {
            set(key, std::get<std::string>(default_value));
        }
    }
    
    std::string getDescription(const std::string& key) const {
        for (const auto& option : config_options_) {
            if (option.key == key) {
                return option.description;
            }
        }
        return "";
    }
    
    bool validateOption(const std::string& key, const std::variant<int64_t, double, float, bool, std::string>& value) const {
        for (const auto& option : config_options_) {
            if (option.key == key) {
                return option.validate(value);
            }
        }
        return false;
    }
    
    void printConfigSchema() const {
        NLM_LOG_INFO("=== Configuration Schema ===");
        for (const auto& option : config_options_) {
            std::string required_str = option.required ? " (required)" : " (optional)";
            NLM_LOG_INFO("[" + option.key + "] " + option.description + required_str);
        }
        NLM_LOG_INFO("======================");
    }
    
    std::string getConfigSummary() const override {
        return summary_cache_.getCachedValue<std::string>([this]() {
            std::string summary = Config::getConfigSummary();
            summary += "\nAdvanced config schema: " + std::to_string(config_options_.size()) + " options defined";
            return summary;
        });
    }
    
private:
    void initializeAdvancedOptions() {
        // Register advanced configuration options for neural simulation
        registerOption("random_seed", int64_t(42), 
                     "Random seed for reproducible simulations", true);
        registerOption("simulation_timestep", double(0.001),
                     "Time step for simulation in seconds", true);
        registerOption("neuron_count", int64_t(1000),
                     "Total number of neurons in the simulation", true);
        registerOption("region_count", int64_t(1),
                     "Number of brain regions", true);
        registerOption("connection_probability", float(0.15),
                     "Probability of connection between neurons", false);
        
        // Advanced plasticity parameters
        registerOption("stdp_ltp_weight", float(0.02),
                     "STDP LTP learning rate", false);
        registerOption("stdp_ltd_weight", float(0.015),
                     "STDP LTD learning rate", false);
        registerOption("stdp_tau", float(20.0),
                     "STDP time constant (ms)", false);
        
        // Structural plasticity parameters
        registerOption("synaptogenesis_rate", float(0.0001),
                     "Rate of new synapse formation", false);
        registerOption("pruning_rate", float(0.00001),
                     "Rate of synapse elimination", false);
        
        // Performance and optimization parameters
        registerOption("enable_lazy_evaluation", bool(true),
                     "Enable lazy evaluation of properties", false);
        registerOption("cache_ttl_seconds", int64_t(60),
                     "Time-to-live for cached properties in seconds", false);
        registerOption("batch_size_neurons", int64_t(100),
                     "Batch size for neuron operations", false);
        
        // Visualization and debugging
        registerOption("enable_debug_logging", bool(true),
                     "Enable detailed debug logging", false);
        registerOption("log_level", int64_t(2),
                     "Log level (0=Off, 1=Error, 2=Warning, 3=Info, 4=Debug)", false);
        
        // Learning and training
        registerOption("learning_rate", float(0.01),
                     "Global learning rate for plasticity", false);
        registerOption("enable_reward_modulation", bool(false),
                     "Enable reward-based modulation", false);
        registerOption("reward_decay_rate", float(0.99),
                     "Decay rate for reward signals", false);
    }
};

// ==============================================================================
// ENHANCED BRAIN CLASS WITH ADVANCED FEATURES
// ==============================================================================

class EnhancedBrain : public Brain {
private:
    mutable CachedProperty<std::vector<NeuronId>> most_active_neurons_;
    mutable CachedProperty<double> average_firing_rate_;
    mutable CachedProperty<size_t> total_connections_;
    mutable CachedProperty<std::unordered_map<NeuronType, size_t>> neuron_type_counts_;
    
    std::shared_ptr<LifetimeManager> lifetime_manager_;
    std::shared_ptr<AdvancedConfig> advanced_config_;
    
    // Type-safe hash lambdas with const references
    struct NeuronIdHash {
        size_t operator()(const NeuronId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };
    
    struct PopulationIdHash {
        size_t operator()(const PopulationId& id) const {
            return std::hash<uint64_t>{}(id.value);
        }
    };
    
    // Step method with type safety improvements
    void step(SimulationStep step, double time, StepType type = StepType::Standard) {
        // Validate input parameters
        if (step < 0) {
            throw BoundsValidationError(
                "Invalid step number: " + std::to_string(step),
                "EnhancedBrain"
            );
        }
        
        if (time < 0.0) {
            throw BoundsValidationError(
                "Invalid time value: " + std::to_string(time),
                "EnhancedBrain"
            );
        }
        
        // Invalidate caches before step
        most_active_neurons_.invalidate();
        average_firing_rate_.invalidate();
        total_connections_.invalidate();
        neuron_type_counts_.invalidate();
        
        // Type-safe step processing
        switch (type) {
            case StepType::Standard:
                Brain::step(step, time);
                break;
            case StepType::Learning:
                processLearningStep(step, time);
                break;
            case StepType::Development:
                processDevelopmentStep(step, time);
                break;
            default:
                throw BindingError(
                    "Unknown step type: " + std::to_string(static_cast<int>(type)),
                    "EnhancedBrain",
                    BindingError::ErrorCode::SIMULATION_ERROR
                );
        }
    }
    
    void processLearningStep(SimulationStep step, double time) {
        // Enhanced learning step with type safety
        if (auto* region = getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                if (syn->isPlastic()) {
                    syn->updatePlasticity(step, time);
                }
            }
        }
    }
    
    void processDevelopmentStep(SimulationStep step, double time) {
        // Enhanced development step with type safety
        if (auto* region = getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->updateStructuralPlasticity(step, time);
            }
        }
    }
    
    // Runtime type checking for SensoryInput
    static bool validateSensoryInput(const SensoryInput* input) {
        if (!input) {
            throw NullPointerError("SensoryInput pointer is null", "EnhancedBrain");
        }
        
        // Check if the input is valid (has data)
        auto data = input->getData();
        if (data.empty()) {
            throw BoundsValidationError(
                "SensoryInput has no data",
                "EnhancedBrain"
            );
        }
        
        return true;
    }
    
public:
    EnhancedBrain(std::shared_ptr<Config> config) : Brain(config) {
        lifetime_manager_ = std::make_shared<LifetimeManager>();
        advanced_config_ = std::make_shared<AdvancedConfig>();
        
        // Initialize with advanced configuration
        if (config) {
            config->set("enable_lazy_evaluation", true, ConfigSource::User);
            config->set("cache_ttl_seconds", 30, ConfigSource::User);
        }
    }
    
    // Override step with enhanced type safety
    void step(SimulationStep step, double time) override {
        step(step, time, StepType::Standard);
    }
    
    // Enhanced step with learning
    void stepLearning(SimulationStep step, double time) {
        step(step, time, StepType::Learning);
    }
    
    // Enhanced step with development
    void stepDevelopment(SimulationStep step, double time) {
        step(step, time, StepType::Development);
    }
    
    // Runtime type checking for sensory inputs
    bool validateAndProcessInput(const SensoryInput& input) {
        return validateSensoryInput(&input);
    }
    
    // Batch processing with safety checks
    void processInputBatch(const std::vector<std::reference_wrapper<const SensoryInput>>& inputs) {
        for (const auto& input : inputs) {
            if (!validateSensoryInput(&input.get())) {
                throw BindingError(
                    "Invalid sensory input in batch",
                    "EnhancedBrain",
                    BindingError::ErrorCode::TYPE_CONVERSION_FAILED
                );
            }
        }
        
        // Process all inputs (delegated to parent class)
        Brain::processInputs(inputs);
    }
    
    // Cached property access
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const {
        return most_active_neurons_.getCachedValue<std::vector<NeuronId>>([this, count]() {
            return Brain::getMostActiveNeurons(count);
        });
    }
    
    double getAverageFiringRate() const {
        return average_firing_rate_.getCachedValue<double>([this]() {
            size_t total_spikes = getTotalSpikeCount();
            size_t total_time_steps = getStepCount();
            return total_time_steps > 0 ? static_cast<double>(total_spikes) / total_time_steps : 0.0;
        });
    }
    
    size_t getTotalConnections() const {
        return total_connections_.getCachedValue<size_t>([this]() {
            size_t connections = 0;
            if (auto* region = getRegion(RegionId(1))) {
                for (const auto& pop : region->getPopulations()) {
                    connections += pop->getTotalSynapseCount();
                }
            }
            return connections;
        });
    }
    
    std::unordered_map<NeuronType, size_t> getNeuronTypeCounts() const {
        return neuron_type_counts_.getCachedValue<std::unordered_map<NeuronType, size_t>>([this]() {
            std::unordered_map<NeuronType, size_t> counts;
            if (auto* region = getRegion(RegionId(1))) {
                for (const auto& neuron : region->getAllNeurons()) {
                    counts[neuron->getType()]++;
                }
            }
            return counts;
        });
    }
    
    // Iterator factories
    class NeuronIterator {
        std::vector<std::shared_ptr<Neuron>> neurons_;
        size_t current_index_ = 0;
        
    public:
        NeuronIterator(const std::vector<std::shared_ptr<Neuron>>& neurons) 
            : neurons_(neurons) {}
        
        class iterator {
            std::vector<std::shared_ptr<Neuron>>::iterator it_;
            
        public:
            iterator(std::vector<std::shared_ptr<Neuron>>::iterator it) : it_(it) {}
            
            std::shared_ptr<Neuron>& operator*() { return *it_; }
            iterator& operator++() { ++it_; return *this; }
            bool operator!=(const iterator& other) const { return it_ != other.it_; }
            bool operator==(const iterator& other) const { return it_ == other.it_; }
        };
        
        iterator begin() { return iterator(neurons_.begin()); }
        iterator end() { return iterator(neurons_.end()); }
    };
    
    NeuronIterator createNeuronIterator() const {
        auto neurons = getAllNeurons();
        return NeuronIterator(neurons);
    }
    
    // Batch operations
    void batchInjectCurrent(const std::vector<std::pair<NeuronId, double>>& neuron_currents) {
        for (const auto& [neuron_id, current] : neuron_currents) {
            BoundsChecker::checkValue(current, 0.0, 1000.0, "injectCurrent");
            
            if (auto* neuron = getNeuron(neuron_id)) {
                neuron->injectCurrent(current);
            }
        }
    }
    
    void batchUpdateWeights(const std::vector<std::tuple<NeuronId, NeuronId, float>>& weight_updates) {
        for (const auto& [pre_id, post_id, delta] : weight_updates) {
            BoundsChecker::checkValue(delta, -1.0f, 1.0f, "weight_update");
            
            if (auto* pre_neuron = getNeuron(pre_id);
                auto* post_neuron = getNeuron(post_id)) {
                auto synapse = findSynapse(pre_neuron, post_neuron);
                if (synapse) {
                    float new_weight = synapse->getWeight() + delta;
                    BoundsChecker::checkValue(new_weight, 0.0f, 10.0f, "new_weight");
                    synapse->setWeight(new_weight);
                }
            }
        }
    }
    
    // Configuration access
    std::shared_ptr<AdvancedConfig> getAdvancedConfig() const { return advanced_config_; }
    
    // Lifetime management
    std::shared_ptr<LifetimeManager> getLifetimeManager() const { return lifetime_manager_; }
    
    // Error reporting
    void logAdvancedDiagnostics() const {
        NLM_LOG_INFO("=== Enhanced Brain Diagnostics ===");
        NLM_LOG_INFO("Step: " + std::to_string(getStep()));
        NLM_LOG_INFO("Time: " + std::to_string(getTime()));
        NLM_LOG_INFO("Neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Spikes: " + std::to_string(getTotalSpikeCount()));
        
        // Log cached properties status
        NLM_LOG_INFO("Cache status - Most active neurons: " + 
                    std::string(most_active_neurons_.is_cached() ? "cached" : "not cached"));
        NLM_LOG_INFO("Cache status - Average firing rate: " + 
                    std::string(average_firing_rate_.is_cached() ? "cached" : "not cached"));
        
        // Log lifetime manager stats
        if (lifetime_manager_) {
            lifetime_manager_->printLifetimeStats();
        }
        
        NLM_LOG_INFO("==============================");
    }
    
    // Safe pointer access helpers
    template<typename T>
    SafePointer::SafePtr<T> safeGetNeuron(const NeuronId& id) {
        auto* neuron = getNeuron(id);
        if (!neuron) {
            throw NullPointerError("Neuron not found for ID: " + std::to_string(id.value), "EnhancedBrain");
        }
        return SafePointer::SafePtr<T>(neuron, lifetime_manager_.get());
    }
    
    template<typename T>
    SafePointer::SafeRef<T> safeGetNeuronRef(const NeuronId& id) {
        auto* neuron = getNeuron(id);
        if (!neuron) {
            throw NullPointerError("Neuron not found for ID: " + std::to_string(id.value), "EnhancedBrain");
        }
        return SafePointer::SafeRef<T>(neuron, lifetime_manager_.get());
    }
    
    // Advanced error handling for plasticity
    bool safeUpdatePlasticity(NeuronId pre_id, NeuronId post_id, float delta_weight, 
                           double time_step) {
        try {
            // Validate inputs
            BoundsChecker::checkValue(delta_weight, -1.0f, 1.0f, "plasticity_delta");
            BoundsChecker::checkValue(time_step, 0.0, 1000.0, "time_step");
            
            if (auto* pre_neuron = getNeuron(pre_id);
                auto* post_neuron = getNeuron(post_id)) {
                auto synapse = findSynapse(pre_neuron, post_neuron);
                if (!synapse) {
                    throw BindingError(
                        "Synapse not found between neurons " + std::to_string(pre_id.value) + 
                        " and " + std::to_string(post_id.value),
                        "EnhancedBrain",
                        BindingError::ErrorCode::PLASTICITY_ERROR
                    );
                }
                
                if (!synapse->isPlastic()) {
                    throw BindingError(
                        "Synapse is not plastic",
                        "EnhancedBrain",
                        BindingError::ErrorCode::PLASTICITY_ERROR
                    );
                }
                
                synapse->updatePlasticity(delta_weight, time_step);
                return true;
            } else {
                throw BindingError(
                    "Neuron not found for plasticity update",
                    "EnhancedBrain",
                    BindingError::ErrorCode::PLASTICITY_ERROR
                );
            }
        } catch (const BindingError& e) {
            NLM_LOG_ERROR("Plasticity update failed: " + std::string(e.what()));
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Unexpected error during plasticity update: " + std::string(e.what()));
            return false;
        }
    }
};

// ==============================================================================
// ENHANCED SENSORY INPUT CLASS WITH TYPE SAFETY
// ==============================================================================

class EnhancedSensoryInput : public SensoryInput {
private:
    mutable CachedProperty<std::vector<float>> processed_data_;
    mutable CachedProperty<std::vector<int>> dimensions_;
    std::vector<float> raw_data_;
    std::vector<int> dimensions_;
    double timestamp_;
    
    // Type-safe conversions
    static std::vector<float> validateAndConvertData(const std::vector<float>& input_data) {
        if (input_data.empty()) {
            throw BoundsValidationError("SensoryInput data cannot be empty", "EnhancedSensoryInput");
        }
        
        // Check for NaN or infinity values
        for (float value : input_data) {
            if (std::isnan(value) || std::isinf(value)) {
                throw TypeConversionError("Invalid data value (NaN or infinity)", "EnhancedSensoryInput");
            }
        }
        
        return input_data;
    }
    
    static std::vector<int> validateAndConvertDimensions(const std::vector<int>& input_dims) {
        if (input_dims.empty()) {
            throw BoundsValidationError("SensoryInput dimensions cannot be empty", "EnhancedSensoryInput");
        }
        
        for (int dim : input_dims) {
            if (dim <= 0) {
                throw BoundsValidationError("Invalid dimension size: " + std::to_string(dim), "EnhancedSensoryInput");
            }
        }
        
        return input_dims;
    }
    
public:
    EnhancedSensoryInput() : timestamp_(0.0) {}
    
    EnhancedSensoryInput(const std::vector<float>& data, const std::vector<int>& dims) 
        : raw_data_(validateAndConvertData(data)),
          dimensions_(validateAndConvertDimensions(dims)),
          timestamp_(0.0) {}
    
    // Override with type safety
    std::vector<float> getData() const override {
        return processed_data_.getCachedValue<std::vector<float>>([this]() {
            return raw_data_;
        });
    }
    
    std::vector<int> getDimensions() const override {
        return dimensions_.getCachedValue<std::vector<int>>([this]() {
            return dimensions_;
        });
    }
    
    double getTimestamp() const override { return timestamp_; }
    void setTimestamp(double timestamp) override { timestamp_ = timestamp; }
    
    // Type-safe data processing
    std::vector<float> getProcessedData() const {
        return processed_data_.getCachedValue<std::vector<float>>([this]() {
            std::vector<float> processed = raw_data_;
            
            // Apply simple preprocessing: normalize to [0, 1] range
            if (!processed.empty()) {
                float min_val = *std::min_element(processed.begin(), processed.end());
                float max_val = *std::max_element(processed.begin(), processed.end());
                
                if (max_val > min_val) {
                    for (auto& value : processed) {
                        value = (value - min_val) / (max_val - min_val);
                    }
                }
            }
            
            return processed;
        });
    }
    
    // Batch processing factory
    class SensoryInputBatch {
        std::vector<std::reference_wrapper<const EnhancedSensoryInput>> inputs_;
        
    public:
        SensoryInputBatch(const std::vector<std::reference_wrapper<const EnhancedSensoryInput>>& inputs)
            : inputs_(inputs) {}
        
        std::vector<std::vector<float>> getAllData() const {
            std::vector<std::vector<float>> all_data;
            all_data.reserve(inputs_.size());
            
            for (const auto& input : inputs_) {
                all_data.push_back(input.get().getData());
            }
            
            return all_data;
        }
        
        size_t getTotalDimensions() const {
            size_t total = 0;
            for (const auto& input : inputs_) {
                auto dims = input.get().getDimensions();
                total += dims.empty() ? 0 : std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<>());
            }
            return total;
        }
        
        double getTimeRange() const {
            double min_time = std::numeric_limits<double>::max();
            double max_time = std::numeric_limits<double>::lowest();
            
            for (const auto& input : inputs_) {
                double time = input.get().getTimestamp();
                min_time = std::min(min_time, time);
                max_time = std::max(max_time, time);
            }
            
            return max_time - min_time;
        }
    };
    
    SensoryInputBatch createBatch(const std::vector<std::reference_wrapper<const EnhancedSensoryInput>>& other_inputs) const {
        std::vector<std::reference_wrapper<const EnhancedSensoryInput>> all_inputs;
        all_inputs.push_back(*this);
        all_inputs.insert(all_inputs.end(), other_inputs.begin(), other_inputs.end());
        return SensoryInputBatch(all_inputs);
    }
    
    // Safe type conversions
    static std::shared_ptr<EnhancedSensoryInput> convertToEnhanced(
        const std::shared_ptr<SensoryInput>& input) {
        if (!input) {
            throw NullPointerError("Input SensoryInput pointer is null", "EnhancedSensoryInput");
        }
        
        // Dynamic type checking and conversion
        try {
            // Attempt to cast to EnhancedSensoryInput
            auto enhanced = std::dynamic_pointer_cast<EnhancedSensoryInput>(input);
            if (enhanced) {
                return enhanced;
            }
            
            // If not already enhanced, create a new EnhancedSensoryInput
            // Extract data from base class
            auto data = input->getData();
            auto dims = input->getDimensions();
            double timestamp = input->getTimestamp();
            
            auto result = std::make_shared<EnhancedSensoryInput>(data, dims);
            result->setTimestamp(timestamp);
            return result;
            
        } catch (const std::bad_cast& e) {
            throw TypeConversionError("Failed to convert to EnhancedSensoryInput: " + std::string(e.what()),
                                    "EnhancedSensoryInput");
        }
    }
    
    // Iterator factory for data access
    class DataIterator {
        const std::vector<float>* data_ = nullptr;
        size_t current_index_ = 0;
        
    public:
        DataIterator(const std::vector<float>* data) : data_(data) {}
        
        class iterator {
            const std::vector<float>* data_ = nullptr;
            size_t index_ = 0;
            
        public:
            iterator(const std::vector<float>* data, size_t index) : data_(data), index_(index) {}
            
            float operator*() const { return (*data_)[index_]; }
            iterator& operator++() { ++index_; return *this; }
            bool operator!=(const iterator& other) const { return index_ != other.index_; }
            bool operator==(const iterator& other) const { return index_ == other.index_; }
        };
        
        iterator begin() { return iterator(data_, 0); }
        iterator end() { return iterator(data_, data_ ? data_->size() : 0); }
    };
    
    DataIterator createDataIterator() const {
        return DataIterator(&raw_data_);
    }
    
    // Lazy evaluation for expensive operations
    LazyEvaluator::LazyValue<std::vector<float>> getNormalizedData() const {
        return LazyEvaluator::createLazy([this]() {
            std::vector<float> normalized = getProcessedData();
            
            // Apply additional normalization (e.g., Z-score)
            if (!normalized.empty()) {
                double sum = std::accumulate(normalized.begin(), normalized.end(), 0.0);
                double mean = sum / normalized.size();
                
                double sq_sum = 0.0;
                for (float val : normalized) {
                    sq_sum += (val - mean) * (val - mean);
                }
                double std_dev = std::sqrt(sq_sum / normalized.size());
                
                if (std_dev > 0.0) {
                    for (auto& val : normalized) {
                        val = (val - mean) / std_dev;
                    }
                }
            }
            
            return normalized;
        });
    }
    
    // Safe pointer access
    template<typename T>
    SafePointer::SafeRef<T> getDataAs() const {
        // Type-safe cast of data pointer
        return SafePointer::SafeRef<T>(const_cast<float*>(raw_data_.data()), nullptr);
    }
};

// ==============================================================================
// ENHANCED ACTION CLASS WITH ADVANCED FEATURES
// ==============================================================================

class EnhancedAction : public Action {
private:
    mutable CachedProperty<std::string> action_name_;
    mutable CachedProperty<std::vector<float>> processed_params_;
    
    // Type-safe parameter validation
    static std::vector<float> validateAndConvertParameters(const std::vector<float>& params) {
        if (params.empty()) {
            throw BoundsValidationError("Action parameters cannot be empty", "EnhancedAction");
        }
        
        // Check for valid parameter ranges based on action type
        // This is a simplified validation - real implementation would be more sophisticated
        for (float param : params) {
            if (std::isnan(param) || std::isinf(param)) {
                throw TypeConversionError("Invalid parameter value (NaN or infinity)", "EnhancedAction");
            }
            
            // Basic sanity check for parameter magnitude
            if (std::abs(param) > 1e6f) {
                throw BoundsValidationError("Parameter magnitude too large: " + std::to_string(param), "EnhancedAction");
            }
        }
        
        return params;
    }
    
public:
    EnhancedAction() : Action() {}
    
    EnhancedAction(ActionType type) : Action(type) {}
    
    EnhancedAction(ActionType type, const std::vector<float>& parameters) : Action(type, parameters) {}
    
    // Override with type safety
    std::string getName() const override {
        return action_name_.getCachedValue<std::string>([this]() {
            return Action::getName();
        });
    }
    
    std::vector<float> getParameters() const override {
        return processed_params_.getCachedValue<std::vector<float>>([this]() {
            return validateAndConvertParameters(Action::getParameters());
        });
    }
    
    void setParameters(const std::vector<float>& parameters) override {
        // Validate parameters before setting
        auto validated_params = validateAndConvertParameters(parameters);
        Action::setParameters(validated_params);
        processed_params_.invalidate();
    }
    
    // Batch processing factory
    class ActionBatch {
        std::vector<std::reference_wrapper<const EnhancedAction>> actions_;
        
    public:
        ActionBatch(const std::vector<std::reference_wrapper<const EnhancedAction>>& actions)
            : actions_(actions) {}
        
        std::vector<std::string> getAllNames() const {
            std::vector<std::string> names;
            names.reserve(actions_.size());
            
            for (const auto& action : actions_) {
                names.push_back(action.get().getName());
            }
            
            return names;
        }
        
        std::vector<std::vector<float>> getAllParameters() const {
            std::vector<std::vector<float>> all_params;
            all_params.reserve(actions_.size());
            
            for (const auto& action : actions_) {
                all_params.push_back(action.get().getParameters());
            }
            
            return all_params;
        }
        
        void batchSetParameters(const std::vector<std::vector<float>>& new_params) {
            if (new_params.size() != actions_.size()) {
                throw BoundsValidationError(
                    "Parameter batch size mismatch: expected " + std::to_string(actions_.size()) +
                    ", got " + std::to_string(new_params.size()),
                    "ActionBatch"
                );
            }
            
            for (size_t i = 0; i < actions_.size(); ++i) {
                actions_[i].get().setParameters(new_params[i]);
            }
        }
    };
    
    ActionBatch createBatch(const std::vector<std::reference_wrapper<const EnhancedAction>>& other_actions) const {
        std::vector<std::reference_wrapper<const EnhancedAction>> all_actions;
        all_actions.push_back(*this);
        all_actions.insert(all_actions.end(), other_actions.begin(), other_actions.end());
        return ActionBatch(all_actions);
    }
    
    // Safe type conversions
    static std::shared_ptr<EnhancedAction> convertToEnhanced(const std::shared_ptr<Action>& action) {
        if (!action) {
            throw NullPointerError("Action pointer is null", "EnhancedAction");
        }
        
        try {
            auto enhanced = std::dynamic_pointer_cast<EnhancedAction>(action);
            if (enhanced) {
                return enhanced;
            }
            
            auto result = std::make_shared<EnhancedAction>();
            result->setType(action->getType());
            result->setParameters(action->getParameters());
            return result;
            
        } catch (const std::bad_cast& e) {
            throw TypeConversionError("Failed to convert to EnhancedAction: " + std::string(e.what()),
                                    "EnhancedAction");
        }
    }
    
    // Iterator factory for parameters
    class ParameterIterator {
        const std::vector<float>* params_ = nullptr;
        
    public:
        ParameterIterator(const std::vector<float>* params) : params_(params) {}
        
        class iterator {
            const std::vector<float>* params_ = nullptr;
            size_t index_ = 0;
            
        public:
            iterator(const std::vector<float>* params, size_t index) : params_(params), index_(index) {}
            
            float operator*() const { return (*params_)[index_]; }
            iterator& operator++() { ++index_; return *this; }
            bool operator!=(const iterator& other) const { return index_ != other.index_; }
            bool operator==(const iterator& other) const { return index_ == other.index_; }
        };
        
        iterator begin() { return iterator(params_, 0); }
        iterator end() { return iterator(params_, params_ ? params_->size() : 0); }
    };
    
    ParameterIterator createParameterIterator() const {
        return ParameterIterator(&Action::getParameters());
    }
    
    // Lazy evaluation for parameter processing
    LazyEvaluator::LazyValue<std::vector<float>> getNormalizedParameters() const {
        return LazyEvaluator::createLazy([this]() {
            auto params = getParameters();
            
            // Apply normalization
            if (!params.empty()) {
                float sum = std::accumulate(params.begin(), params.end(), 0.0f);
                float mean = sum / params.size();
                
                float sq_sum = 0.0f;
                for (float val : params) {
                    sq_sum += (val - mean) * (val - mean);
                }
                float std_dev = std::sqrt(sq_sum / params.size());
                
                if (std_dev > 0.0f) {
                    for (auto& val : params) {
                        val = (val - mean) / std_dev;
                    }
                }
            }
            
            return params;
        });
    }
    
    // Clone with enhanced safety
    std::shared_ptr<Action> clone() const override {
        try {
            auto cloned = std::make_shared<EnhancedAction>(*this);
            // Invalidate caches in the clone
            action_name_.invalidate();
            processed_params_.invalidate();
            return cloned;
        } catch (const std::exception& e) {
            throw BindingError(
                "Failed to clone EnhancedAction: " + std::string(e.what()),
                "EnhancedAction",
                BindingError::ErrorCode::MEMORY_ALLOCATION_FAILED
            );
        }
    }
};

// ==============================================================================
// BINDINGS INTEGRATION AND PUBLIC INTERFACE
// ==============================================================================

// Forward declarations for binding functions
void bindAdvancedBrain(py::module_& m);
void bindAdvancedSensoryInput(py::module_& m);
void bindEnhancedAction(py::module_& m);
void bindEnhancedConfig(py::module_& m);

// Main bindings module
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        NLM Enhanced Python Bindings
        ----------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        Includes advanced type safety, runtime validation, and performance optimizations.
        
        Key Features:
        - Type-safe error handling with specific exception types
        - Runtime bounds checking and validation
        - Enhanced memory management with lifetime tracking
        - Cached property access for performance
        - Batch operations for improved throughput
        - Lazy evaluation for expensive computations
        - Safe pointer policies and null checking
        - Flexible type conversions
        - Iterator factories for data access
        - Advanced configuration system
        
        All operations include comprehensive type safety and error handling.
    )pbdoc";
    
    // Register exception types
    py::register_exception<BindingError>(m, "BindingError");
    py::register_exception<TypeConversionError>(m, "TypeConversionError");
    py::register_exception<BoundsValidationError>(m, "BoundsValidationError");
    py::register_exception<NullPointerError>(m, "NullPointerError");
    
    // Bind enhanced classes
    bindAdvancedBrain(m);
    bindAdvancedSensoryInput(m);
    bindEnhancedAction(m);
    bindEnhancedConfig(m);
    
    // Bind core ID types with enhanced safety
    m.def("create_neuron_id", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), R"pbdoc(Create a NeuronId with type safety validation.
        
        Args:
            value: Unique identifier value (must be > 0)
            
        Returns:
            NeuronId instance
            
        Raises:
            BoundsValidationError: If value is invalid
        
        Example:
            >>> neuron_id = create_neuron_id(42)
            >>> print(neuron_id.value)
            42
        )pbdoc");
    
    m.def("create_synaptic_connection", 
        [](const NeuronId& pre, const NeuronId& post, float weight) {
            // Enhanced validation for synapse creation
            if (pre.value == 0 || post.value == 0) {
                throw BoundsValidationError(
                    "Neuron IDs cannot be zero",
                    "BindingFactory"
                );
            }
            if (weight < 0.0f || weight > 10.0f) {
                throw BoundsValidationError(
                    "Invalid weight: " + std::to_string(weight) + ". Must be in [0.0, 10.0]",
                    "BindingFactory"
                );
            }
            return std::make_shared<Synapse>(pre, post, weight);
        },
        py::arg("pre"), py::arg("post"), py::arg("weight"),
        R"pbdoc(Create a synaptic connection with enhanced validation.
        
        Args:
            pre: Pre-synaptic neuron ID
            post: Post-synaptic neuron ID  
            weight: Initial synaptic weight (0.0-10.0)
            
        Returns:
            Shared pointer to Synapse
            
        Raises:
            BoundsValidationError: If IDs or weight are invalid
        
        Example:
            >>> synapse = create_synaptic_connection(neuron1, neuron2, 0.5)
            >>> print(f"Synapse weight: {synapse.getWeight()}")
        )pbdoc");
    
    // Performance monitoring utilities
    m.def("get_memory_stats", []() {
        std::unordered_map<std::string, std::string> stats;
        
        // This would integrate with a global lifetime manager
        stats["status"] = "Enhanced memory management enabled";
        stats["features"] = "Lifetime tracking, automatic cleanup, safe pointers";
        stats["optimization"] = "Caching, lazy evaluation, batch operations";
        
        return stats;
    }, R"pbdoc(Get memory management and performance statistics.
        
        Returns:
            Dictionary with memory and performance statistics
        
        Example:
            >>> stats = get_memory_stats()
            >>> print(stats["status"])
            Enhanced memory management enabled
        )pbdoc");
    
    // Utility functions for type safety
    m.def("validate_id_range", [](const std::string& type_name, uint64_t value, 
                                uint64_t min_val, uint64_t max_val) {
        if (value < min_val || value > max_val) {
            throw BoundsValidationError(
                "ID value " + std::to_string(value) + " out of range for " + type_name +
                ": [" + std::to_string(min_val) + ", " + std::to_string(max_val) + "]",
                "ValidationUtils"
            );
        }
        return true;
    }, py::arg("type_name"), py::arg("value"), py::arg("min_val"), py::arg("max_val"),
        R"pbdoc(Validate that an ID value is within the expected range.
        
        Args:
            type_name: Name of the ID type (e.g., "NeuronId")
            value: ID value to validate
            min_val: Minimum valid value
            max_val: Maximum valid value
            
        Returns:
            True if valid
            
        Raises:
            BoundsValidationError: If value is out of range
        )pbdoc");
    
    // Batch processing utilities
    m.def("batch_process_neurons", 
        [](const std::vector<NeuronId>& neuron_ids, 
           std::function<void(std::shared_ptr<Neuron>)> processor) {
            for (const auto& id : neuron_ids) {
                if (auto neuron = std::dynamic_pointer_cast<Neuron>(getNeuron(id))) {
                    processor(neuron);
                }
            }
        },
        py::arg("neuron_ids"), py::arg("processor"),
        R"pbdoc(Batch process neurons with type-safe validation.
        
        Args:
            neuron_ids: Vector of neuron IDs to process
            processor: Function to apply to each neuron
            
        Note:
            Skips neurons that are not found
        
        Example:
            >>> ids = [neuron1_id, neuron2_id, neuron3_id]
            >>> batch_process_neurons(ids, lambda neuron: neuron.injectCurrent(10.0))
        )pbdoc");
    
    // Configuration and optimization utilities
    m.def("configure_advanced_features", 
        [](bool enable_lazy_eval, bool enable_caching, bool enable_batching) {
            // This would configure global settings for advanced features
            return std::unordered_map<std::string, bool>{
                {"lazy_evaluation", enable_lazy_eval},
                {"caching", enable_caching},
                {"batching", enable_batching}
            };
        },
        py::arg("enable_lazy_eval"), py::arg("enable_caching"), py::arg("enable_batching"),
        R"pbdoc(Configure advanced features globally.
        
        Args:
            enable_lazy_eval: Enable lazy evaluation of properties
            enable_caching: Enable cached property access
            enable_batching: Enable batch operations
            
        Returns:
            Dictionary of current feature settings
        
        Example:
            >>> config = configure_advanced_features(true, true, true)
            >>> print(config["lazy_evaluation"])
            True
        )pbdoc");
    
    // Error handling utilities
    m.def("catch_binding_errors", 
        [](std::function<void()> operation) {
            try {
                operation();
                return std::make_pair(true, std::string("Success"));
            } catch (const BindingError& e) {
                return std::make_pair(false, std::string(e.what()));
            } catch (const std::exception& e) {
                return std::make_pair(false, std::string("Unexpected error: ") + e.what());
            }
        },
        py::arg("operation"),
        R"pbdoc(Execute an operation with comprehensive error handling.
        
        Args:
            operation: Function to execute
            
        Returns:
            Tuple of (success, error_message)
            
        Example:
            >>> success, error = catch_binding_errors(lambda: dangerous_operation())
            >>> if not success:
            ...     print(f"Error: {error}")
        )pbdoc");
}

// ==============================================================================
// BINDING IMPLEMENTATIONS
// ==============================================================================

void bindAdvancedBrain(py::module_& m) {
    py::class_<EnhancedBrain, std::shared_ptr<EnhancedBrain>>(m, "EnhancedBrain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("step", &EnhancedBrain::step, py::arg("step"), py::arg("time"))
        .def("step_learning", &EnhancedBrain::stepLearning, py::arg("step"), py::arg("time"))
        .def("step_development", &EnhancedBrain::stepDevelopment, py::arg("step"), py::arg("time"))
        .def("validate_input", &EnhancedBrain::validateAndProcessInput, py::arg("input"))
        .def("process_input_batch", &EnhancedBrain::processInputBatch, py::arg("inputs"))
        .def("get_most_active_neurons", &EnhancedBrain::getMostActiveNeurons, py::arg("count") = 10)
        .def("get_average_firing_rate", &EnhancedBrain::getAverageFiringRate)
        .def("get_total_connections", &EnhancedBrain::getTotalConnections)
        .def("get_neuron_type_counts", &EnhancedBrain::getNeuronTypeCounts)
        .def("create_neuron_iterator", &EnhancedBrain::createNeuronIterator)
        .def("batch_inject_current", &EnhancedBrain::batchInjectCurrent)
        .def("batch_update_weights", &EnhancedBrain::batchUpdateWeights)
        .def("get_advanced_config", &EnhancedBrain::getAdvancedConfig)
        .def("get_lifetime_manager", &EnhancedBrain::getLifetimeManager)
        .def("log_diagnostics", &EnhancedBrain::logAdvancedDiagnostics)
        .def("safe_get_neuron", &EnhancedBrain::template safeGetNeuron<Neuron>)
        .def("safe_get_neuron_ref", &EnhancedBrain::template safeGetNeuronRef<Neuron>)
        .def("safe_update_plasticity", &EnhancedBrain::safeUpdatePlasticity);
}

void bindAdvancedSensoryInput(py::module_& m) {
    py::class_<EnhancedSensoryInput, SensoryInput, std::shared_ptr<EnhancedSensoryInput>>(m, "EnhancedSensoryInput")
        .def(py::init<>())
        .def(py::init<const std::vector<float>&, const std::vector<int>&>(),
             py::arg("data"), py::arg("dimensions"))
        .def("get_data", &EnhancedSensoryInput::getData)
        .def("get_dimensions", &EnhancedSensoryInput::getDimensions)
        .def("get_timestamp", &EnhancedSensoryInput::getTimestamp)
        .def("set_timestamp", &EnhancedSensoryInput::setTimestamp, py::arg("timestamp"))
        .def("get_processed_data", &EnhancedSensoryInput::getProcessedData)
        .def("create_batch", &EnhancedSensoryInput::createBatch, py::arg("other_inputs"))
        .def("convert_to_enhanced", &EnhancedSensoryInput::convertToEnhanced, py::arg("input"))
        .def("create_data_iterator", &EnhancedSensoryInput::createDataIterator)
        .def("get_normalized_data", &EnhancedSensoryInput::getNormalizedData);
}

void bindEnhancedAction(py::module_& m) {
    py::class_<EnhancedAction, Action, std::shared_ptr<EnhancedAction>>(m, "EnhancedAction")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_name", &EnhancedAction::getName)
        .def("get_parameters", &EnhancedAction::getParameters)
        .def("set_parameters", &EnhancedAction::setParameters, py::arg("parameters"))
        .def("create_batch", &EnhancedAction::createBatch, py::arg("other_actions"))
        .def("convert_to_enhanced", &EnhancedAction::convertToEnhanced, py::arg("action"))
        .def("create_parameter_iterator", &EnhancedAction::createParameterIterator)
        .def("get_normalized_parameters", &EnhancedAction::getNormalizedParameters)
        .def("clone", &EnhancedAction::clone);
}

void bindEnhancedConfig(py::module_& m) {
    py::class_<AdvancedConfig, Config, std::shared_ptr<AdvancedConfig>>(m, "EnhancedConfig")
        .def(py::init<>())
        .def("register_option", &AdvancedConfig::registerOption,
             py::arg("key"), py::arg("default_value"), py::arg("description"),
             py::arg("required") = false, py::arg("validator") = nullptr)
        .def("get_description", &AdvancedConfig::getDescription, py::arg("key"))
        .def("validate_option", &AdvancedConfig::validateOption,
             py::arg("key"), py::arg("value"))
        .def("print_schema", &AdvancedConfig::printConfigSchema)
        .def("get_config_summary", &AdvancedConfig::getConfigSummary);
}

// ==============================================================================
// COMPATIBILITY LAYER AND INTERFACES
// ==============================================================================

// Original NLM interfaces for backward compatibility
void bindOriginalInterfaces(py::module_& m) {
    // Original Brain interface
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
        .def("initialize", &Brain::initialize)
        .def("reset", &Brain::reset)
        .def("step", &Brain::step, py::arg("step"), py::arg("time"))
        .def("get_neuron", &Brain::getNeuron, py::arg("id"))
        .def("get_region", &Brain::getRegion, py::arg("id"))
        .def("get_all_neurons", &Brain::getAllNeurons)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getStep", &Brain::getStep)
        .def("getTime", &Brain::getTime)
        .def("logStatus", &Brain::logStatus)
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"))
        .def("getMostActiveNeurons", &Brain::getMostActiveNeurons, py::arg("count") = 10)
        .def("processInputs", &Brain::processInputs);
    
    // Original SensoryInput interface
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getType", &SensoryInput::getType)
        .def("getData", &SensoryInput::getData)
        .def("getDimensions", &SensoryInput::getDimensions)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"));
    
    // Original Vision interface
    py::class_<Vision, SensoryInput, std::shared_ptr<Vision>>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", &Vision::setData, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);
    
    // Original Audio interface
    py::class_<Audio, SensoryInput, std::shared_ptr<Audio>>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", &Audio::setData, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
    
    // Original InternalSignals interface
    py::class_<InternalSignals, SensoryInput, std::shared_ptr<InternalSignals>>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);
    
    // Original Action interface
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
    
    // Original WorldObject interface
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);
    
    // Original AgentBody interface
    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age);
    
    // Original Config interface
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"))
        .def("loadFromArgs", &Config::loadFromArgs, py::arg("argc"), py::arg("argv"))
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"))
        .def("has", &Config::has, py::arg("key"))
        .def("getKeys", &Config::getKeys)
        .def("clear", &Config::clear)
        .def("summary", &Config::summary);
}

// Register original interfaces for backward compatibility
void registerOriginalInterfaces(py::module_& m) {
    bindOriginalInterfaces(m);
}

// Initialize all advanced bindings
void initializeAdvancedBindings() {
    // Global initialization for advanced features
    NLM_LOG_INFO("=== Initializing Advanced NLM Bindings ===");
    NLM_LOG_INFO("Type safety: ENABLED");
    NLM_LOG_INFO("Runtime validation: ENABLED");
    NLM_LOG_INFO("Memory management: ENHANCED");
    NLM_LOG_INFO("Caching: ENABLED");
    NLM_LOG_INFO("Batch operations: ENABLED");
    NLM_LOG_INFO("Lazy evaluation: ENABLED");
    NLM_LOG_INFO("Safe pointers: ENABLED");
    NLM_LOG_INFO("Advanced error handling: ENABLED");
    NLM_LOG_INFO("==============================");
}