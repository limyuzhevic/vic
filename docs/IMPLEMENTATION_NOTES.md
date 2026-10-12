# NLM Implementation Notes

## Overview

This document provides technical implementation details for the NLM (Neural Learning Machine) system. It covers architecture design, implementation choices, performance considerations, and technical specifications.

## Core Architecture

### Memory Management

NLM implements a comprehensive memory management system with the following components:

#### Neural Memory Hierarchy
```cpp
// Neural populations use smart pointers for automatic memory management
class NeuralPopulation {
    std::vector<Neuron*> neurons_;  // Raw pointers for performance
    // Neuron lifecycle managed by parent region
};

class NeuralRegion {
    std::vector<std::unique_ptr<NeuralPopulation>> populations_;
    // Automatic cleanup of populations
};

class Brain {
    std::vector<std::unique_ptr<NeuralRegion>> regions_;
    // Automatic cleanup of all regions
};
```

#### Event-Driven Memory System
```cpp
// Spike events use event pools for memory efficiency
class SpikeSystem {
    static constexpr size_t EVENT_POOL_SIZE = 65536;
    
    struct SpikeEvent {
        NeuronId neuron_id;
        Timestamp time;
        SimulationStep step;
    };
    
    SpikeEvent* event_pool_[EVENT_POOL_SIZE];
    size_t event_count_;
    
    // Pool-based allocation
    SpikeEvent* allocate_event() {
        return event_pool_[event_count_++];
    }
};
```

### Spike Processing Architecture

#### Event-Driven Design
```cpp
// High-performance spike processing pipeline
class SpikeSystem {
public:
    // Event queues for different spike types
    std::priority_queue<DelayedSpikeEvent> delayed_spikes_;
    std::queue<SpikeEvent> immediate_spikes_;
    
    // Handler registration
    using SpikeHandler = std::function<void(const SpikeEvent&)>;
    std::vector<SpikeHandler> spike_handlers_;
    
    // Processing pipeline
    void processSpikes(SimulationStep step) {
        while (!immediate_spikes_.empty()) {
            const SpikeEvent& event = immediate_spikes_.front();
            for (auto& handler : spike_handlers_) {
                handler(event);  // Call all handlers
            }
            immediate_spikes_.pop();
        }
    }
    
    void processDelayedSpikes(SimulationStep step, Timestamp time) {
        while (!delayed_spikes_.empty() && 
               delayed_spikes_.top().delivery_step <= step) {
            const DelayedSpikeEvent& event = delayed_spikes_.top();
            // Deliver synaptic input
            deliverSynapticInput(event);
            delayed_spikes_.pop();
        }
    }
};
```

### Plasticity Implementation

#### STDP Algorithm
```cpp
class STDP {
public:
    struct STDPConfig {
        float ltp_weight;    // LTP strength
        float ltd_weight;    // LTD strength
        float tau;           // Time constant
        float learning_rate; // Overall learning rate
    };
    
    void configure(float ltp_weight, float ltd_weight, float tau) {
        config_.ltp_weight = ltp_weight;
        config_.ltd_weight = ltd_weight;
        config_.tau = tau;
        config_.learning_rate = 1.0f;
    }
    
    void update(Synapse* synapse, 
               const SpikeHistory& pre_spikes,
               const SpikeHistory& post_spikes,
               float timestep) {
        
        float weight_change = 0.0f;
        
        // Calculate STDP contribution
        if (!pre_spikes.empty() && !post_spikes.empty()) {
            float latest_pre = pre_spikes.back().time;
            float latest_post = post_spikes.back().time;
            
            if (latest_post > latest_pre) {
                // LTP: pre before post
                float dt = latest_post - latest_pre;
                weight_change += config_.ltp_weight * 
                               std::exp(-dt / config_.tau);
            } else {
                // LTD: post before pre
                float dt = latest_pre - latest_post;
                weight_change -= config_.ltd_weight * 
                               std::exp(-dt / config_.tau);
            }
        }
        
        // Apply neuromodulation
        weight_change *= neuromodulation_factor_;
        
        // Update synapse weight
        float new_weight = synapse->getWeight() + weight_change;
        synapse->setWeight(new_weight);
    }
};
```

### Cognitive Architecture

#### Working Memory Implementation
```cpp
class NeuralWorkingMemory {
public:
    struct MemoryItem {
        NeuronId neuron_id;
        float activation;
        Timestamp timestamp;
        float decay_rate;
    };
    
    NeuralWorkingMemory(size_t capacity) : capacity_(capacity) {
        // Initialize with circular buffer for O(1) access
        memory_buffer_ = std::vector<MemoryItem>(capacity);
        head_ = 0;
        size_ = 0;
    }
    
    void storeToNeuron(NeuronId neuron_id, float activation) {
        MemoryItem item;
        item.neuron_id = neuron_id;
        item.activation = activation;
        item.timestamp = current_time_;
        item.decay_rate = 0.99f;
        
        // Insert into circular buffer
        size_t index = (head_ + size_) % capacity_;
        memory_buffer_[index] = item;
        
        if (size_ < capacity_) {
            size_++;
        } else {
            head_ = (head_ + 1) % capacity_;
        }
    }
    
    void update(float timestep) {
        current_time_ += timestep;
        
        // Decay all items
        for (size_t i = 0; i < size_; ++i) {
            size_t index = (head_ + i) % capacity_;
            memory_buffer_[index].activation *= memory_buffer_[index].decay_rate;
        }
        
        // Competition for attention
        apply_attention_competition();
    }
    
private:
    void apply_attention_competition() {
        if (size_ <= 1) return;
        
        // Find neuron with highest activation
        NeuronId winner = memory_buffer_[head_].neuron_id;
        float max_activation = memory_buffer_[head_].activation;
        
        for (size_t i = 1; i < size_; ++i) {
            size_t index = (head_ + i) % capacity_;
            if (memory_buffer_[index].activation > max_activation) {
                winner = memory_buffer_[index].neuron_id;
                max_activation = memory_buffer_[index].activation;
            }
        }
        
        // Apply winner-take-all
        for (size_t i = 0; i < size_; ++i) {
            size_t index = (head_ + i) % capacity_;
            if (memory_buffer_[index].neuron_id != winner) {
                memory_buffer_[index].activation *= 0.1f; // Suppress
            }
        }
    }
    
    size_t capacity_;
    std::vector<MemoryItem> memory_buffer_;
    size_t head_;
    size_t size_;
    Timestamp current_time_;
};
```

### Prediction System

#### Forward Model Implementation
```cpp
class NeuralPrediction {
public:
    struct Prediction {
        nlm::Action action;
        float expected_reward;
        std::vector<float> predicted_state;
        float confidence;
    };
    
    void configure(size_t horizon, float update_rate) {
        horizon_ = horizon;
        update_rate_ = update_rate;
        prediction_buffer_.resize(horizon, 0.0f);
    }
    
    float predict(const nlm::Action& action, 
                  const std::vector<float>& current_state) {
        // Use neural network to predict action outcome
        std::vector<float> predicted_state = 
            neural_network_->forward(concatenate_state_and_action(
                current_state, action_to_features(action)));
        
        // Calculate prediction error
        float error = calculate_prediction_error(
            predicted_state, get_expected_outcome(action));
        
        // Update prediction model
        update_prediction_model(error, action, current_state);
        
        return error;
    }
    
private:
    std::vector<float> concatenate_state_and_action(
        const std::vector<float>& state,
        const std::vector<float>& action) {
        std::vector<float> combined;
        combined.reserve(state.size() + action.size());
        combined.insert(combined.end(), state.begin(), state.end());
        combined.insert(combined.end(), action.begin(), action.end());
        return combined;
    }
    
    size_t horizon_;
    float update_rate_;
    std::vector<float> prediction_buffer_;
    std::unique_ptr<NeuralNetwork> neural_network_;
};
```

### Neuromodulation

#### Dopamine System
```cpp
class Dopamine {
public:
    struct DopamineSignal {
        float reward;
        float prediction_error;
        float uncertainty;
    };
    
    void configure(float baseline, float gain) {
        baseline_ = baseline;
        gain_ = gain;
        plasticity_factor_ = 1.0f;
    }
    
    void update(float timestep) {
        // Update dopamine level based on reward prediction error
        float reward_error = current_reward_ - expected_reward_;
        
        dopamine_level_ += (reward_error * gain_ - dopamine_level_) * timestep;
        dopamine_level_ = std::max(0.0f, std::min(1.0f, dopamine_level_));
        
        // Update plasticity factor
        plasticity_factor_ = 1.0f + (dopamine_level_ - baseline_) * 0.5f;
    }
    
    float getPlasticityFactor() const {
        return plasticity_factor_;
    }
    
    float getLevel() const {
        return dopamine_level_;
    }
    
private:
    float baseline_;
    float gain_;
    float dopamine_level_;
    float plasticity_factor_;
    float current_reward_;
    float expected_reward_;
};
```

### Development System

#### Synaptogenesis Implementation
```cpp
class Synaptogenesis {
public:
    struct SynapseFormation {
        NeuronId pre_neuron;
        NeuronId post_neuron;
        float initial_weight;
        Delay delay;
        PlasticityFlags flags;
    };
    
    void update(Brain* brain, RandomGenerator& rng, float development_time) {
        // Calculate synaptogenesis rate based on developmental stage
        float formation_rate = calculateFormationRate(developmental_stage_);
        
        // Form new synapses
        for (size_t i = 0; i < formation_attempts_; ++i) {
            if (rng.generateFloat() < formation_rate) {
                createRandomSynapse(brain, rng);
            }
        }
        
        // Update connection probabilities
        updateConnectionProbabilities(brain, development_time);
    }
    
private:
    void createRandomSynapse(Brain* brain, RandomGenerator& rng) {
        // Select pre and post neurons randomly
        // Create synapse with initial weight
        // Add to brain's synapse management system
    }
    
    DevelopmentalStage developmental_stage_;
    size_t formation_attempts_;
};
```

### Checkpoint System

#### Checkpoint Implementation
```cpp
class CheckpointManager {
public:
    bool configure(const std::string& directory, 
                  size_t max_checkpoints,
                  size_t interval,
                  bool auto_save) {
        directory_ = directory;
        max_checkpoints_ = max_checkpoints;
        interval_ = interval;
        auto_save_ = auto_save;
        
        // Create directory if it doesn't exist
        if (!fs::exists(directory_)) {
            fs::create_directories(directory_);
        }
        
        return true;
    }
    
    bool save(Brain* brain, SimulationStep step) {
        std::string filename = generateCheckpointFilename(step);
        return saveToFile(brain, filename);
    }
    
    bool load(Brain* brain, const std::string& filename) {
        return loadFromFile(brain, filename);
    }
    
    void cleanup() {
        // Remove old checkpoints
        std::vector<fs::path> checkpoints;
        for (const auto& entry : fs::directory_iterator(directory_)) {
            if (entry.path().extension() == ".nlm") {
                checkpoints.push_back(entry.path());
            }
        }
        
        if (checkpoints.size() > max_checkpoints_) {
            // Sort by modification time
            std::sort(checkpoints.begin(), checkpoints.end(),
                     [](const fs::path& a, const fs::path& b) {
                         return fs::last_write_time(a) < fs::last_write_time(b);
                     });
            
            // Remove oldest checkpoints
            for (size_t i = 0; i < checkpoints.size() - max_checkpoints_; ++i) {
                fs::remove(checkpoints[i]);
            }
        }
    }
    
private:
    std::string generateCheckpointFilename(SimulationStep step) {
        std::stringstream ss;
        ss << directory_ << "/checkpoint_" << step << ".nlm";
        return ss.str();
    }
    
    std::string directory_;
    size_t max_checkpoints_;
    size_t interval_;
    bool auto_save_;
};
```

## Performance Optimization

### SIMD Optimization
```cpp
// SIMD-optimized neuron updates
class VectorizedNeuronProcessor {
public:
    static void stepLIF(float* membrane_potentials,
                        const float* inputs,
                        const float* weights,
                        size_t neuron_count,
                        float timestep,
                        float threshold,
                        float reset_potential,
                        float leak_conductance) {
        
        // Load constants into SIMD registers
        __m256 threshold_vec = _mm256_set1_ps(threshold);
        __m256 reset_vec = _mm256_set1_ps(reset_potential);
        __m256 leak_vec = _mm256_set1_ps(leak_conductance);
        
        // Process neurons in SIMD batches
        for (size_t i = 0; i < neuron_count; i += 8) {
            size_t batch_size = std::min(size_t(8), neuron_count - i);
            
            // Load neuron states
            __m256 membrane_vec = _mm256_loadu_ps(&membrane_potentials[i]);
            __m256 input_vec = _mm256_loadu_ps(&inputs[i * SYNAPSES_PER_NEURON]);
            
            // Calculate synaptic input
            __m256 synaptic_input = _mm256_mul_ps(input_vec, weights_);
            
            // Integrate LIF dynamics
            __m256 integration = _mm256_add_ps(membrane_vec, synaptic_input);
            integration = _mm256_sub_ps(integration, membrane_vec);
            integration = _mm256_mul_ps(integration, leak_vec);
            integration = _mm256_add_ps(integration, _mm256_set1_ps(R * input_weight_));
            
            // Update membrane potential
            membrane_vec = _mm256_add_ps(membrane_vec, integration);
            
            // Check for spikes
            __m256 spike_mask = _mm256_cmpgt_ps(membrane_vec, threshold_vec);
            
            // Reset spiked neurons
            membrane_vec = _mm256_andnot_ps(spike_mask, membrane_vec);
            membrane_vec = _mm256_or_ps(_mm256_and_ps(spike_mask, reset_vec), membrane_vec);
            
            // Store results
            _mm256_storeu_ps(&membrane_potentials[i], membrane_vec);
            
            // Record spike events
            record_spikes(spike_mask, i);
        }
    }
    
private:
    static constexpr float R = 100.0f;        // Membrane resistance
    static constexpr float input_weight_ = 10.0f; // Input weight scaling
};
```

### Memory Pool Optimization
```cpp
// Memory pool for spike events
class SpikeEventPool {
public:
    static constexpr size_t POOL_SIZE = 1048576; // 1 million events
    
    SpikeEventPool() {
        // Pre-allocate memory pool
        events_ = new SpikeEvent[POOL_SIZE];
        head_ = 0;
        capacity_ = POOL_SIZE;
    }
    
    SpikeEvent* allocate() {
        if (head_ >= capacity_) {
            // Pool full - expand or error
            return nullptr;
        }
        return &events_[head_++];
    }
    
    void reset() {
        head_ = 0;
    }
    
    size_t size() const {
        return head_;
    }
    
    ~SpikeEventPool() {
        delete[] events_;
    }
    
private:
    SpikeEvent* events_;
    size_t head_;
    size_t capacity_;
};
```

### Event-Driven Processing
```cpp
// Event-driven spike processing system
class EventDrivenProcessor {
public:
    EventDrivenProcessor() {
        // Initialize event queues
        immediate_queue_.reserve(10000);
        delayed_queue_.reserve(10000);
    }
    
    void queueImmediateSpike(const SpikeEvent& event) {
        immediate_queue_.push_back(event);
    }
    
    void queueDelayedSpike(const DelayedSpikeEvent& event) {
        delayed_queue_.push_back(event);
        delayed_queue_.sort([](const DelayedSpikeEvent& a, 
                               const DelayedSpikeEvent& b) {
            return a.delivery_step < b.delivery_step;
        });
    }
    
    void processEvents(SimulationStep step) {
        // Process immediate spikes
        processImmediateSpikes(step);
        
        // Process delayed spikes
        processDelayedSpikes(step);
        
        // Deliver synaptic inputs
        deliverSynapticInputs();
    }
    
private:
    void processImmediateSpikes(SimulationStep step) {
        for (const SpikeEvent& event : immediate_queue_) {
            handleSpikeEvent(event);
        }
        immediate_queue_.clear();
    }
    
    void processDelayedSpikes(SimulationStep step) {
        for (auto it = delayed_queue_.begin(); 
             it != delayed_queue_.end(); ++it) {
            if (it->delivery_step == step) {
                handleDelayedSpike(*it);
                it = delayed_queue_.erase(it);
            } else {
                break; // Queue is sorted
            }
        }
    }
    
    std::vector<SpikeEvent> immediate_queue_;
    std::vector<DelayedSpikeEvent> delayed_queue_;
};
```

## Numerical Methods

### Integration Scheme
```cpp
// Runge-Kutta 4th order integration for LIF neurons
class RK4Integrator {
public:
    static void integrateLIF(float& V, float I, float dt, float tau, float threshold) {
        // RK4 coefficients
        float k1 = (-V + R * I - V_reset) / tau;
        float k2 = (- (V + 0.5f * dt * k1) + R * I - V_reset) / tau;
        float k3 = (- (V + 0.5f * dt * k2) + R * I - V_reset) / tau;
        float k4 = (- (V + dt * k3) + R * I - V_reset) / tau;
        
        // Update membrane potential
        V += (dt / 6.0f) * (k1 + 2.0f * k2 + 2.0f * k3 + k4);
        
        // Check for spike
        if (V >= threshold) {
            V = V_reset; // Reset membrane potential
            // Record spike event
        }
    }
    
private:
    static constexpr float R = 100.0f;      // Membrane resistance
    static constexpr float V_reset = -70.0f; // Reset potential
};
```

### Numerical Stability
```cpp
// Adaptive time step control for numerical stability
class AdaptiveTimeStep {
public:
    AdaptiveTimeStep(float min_dt, float max_dt, float tolerance)
        : min_dt_(min_dt), max_dt_(max_dt), tolerance_(tolerance),
          current_dt_(max_dt) {}
    
    float getTimeStep() const {
        return current_dt_;
    }
    
    void update(float error, float current_dt) {
        // Calculate error estimate
        float error_estimate = calculate_error_estimate(error);
        
        // Adjust time step based on error
        if (error_estimate > tolerance_) {
            // Reduce time step
            current_dt_ = std::max(min_dt_, current_dt_ * 0.5f);
        } else {
            // Increase time step
            current_dt_ = std::min(max_dt_, current_dt_ * 1.1f);
        }
        
        current_dt_ = std::clamp(current_dt_, min_dt_, max_dt_);
    }
    
private:
    float calculate_error_estimate(float error) const {
        // Simple error estimation based on membrane potential changes
        // More sophisticated implementations would use higher-order methods
        return std::abs(error);
    }
    
    float min_dt_;
    float max_dt_;
    float tolerance_;
    float current_dt_;
};
```

## Error Handling

### Exception Hierarchy
```cpp
// Comprehensive exception hierarchy for NLM
namespace nlm {

enum class ErrorCode {
    Success = 0,
    ConfigError = 1,
    SimulationError = 2,
    MemoryError = 3,
    FileError = 4,
    ValidationError = 5,
    NotImplementedError = 6,
    OutOfRangeError = 7,
    NullPointerError = 8
};

class NLMException : public std::exception {
protected:
    ErrorCode code_;
    std::string message_;
    std::string file_;
    int line_;
    
public:
    NLMException(ErrorCode code, const std::string& message)
        : code_(code), message_(message), line_(0) {}
    
    NLMException(ErrorCode code, const std::string& message,
                 const std::string& file, int line)
        : code_(code), message_(message), file_(file), line_(line) {}
    
    virtual ~NLMException() noexcept = default;
    
    ErrorCode getCode() const noexcept {
        return code_;
    }
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    std::string getFullMessage() const {
        std::stringstream ss;
        ss << "[" << errorCodeToString(code_) << "] " << message_;
        if (!file_.empty()) {
            ss << " (" << file_ << ":" << line_ << ")";
        }
        return ss.str();
    }
    
    static const char* errorCodeToString(ErrorCode code) {
        switch (code) {
            case ErrorCode::Success: return "Success";
            case ErrorCode::ConfigError: return "Configuration Error";
            case ErrorCode::SimulationError: return "Simulation Error";
            case ErrorCode::MemoryError: return "Memory Error";
            case ErrorCode::FileError: return "File Error";
            case ErrorCode::ValidationError: return "Validation Error";
            case ErrorCode::NotImplementedError: return "Not Implemented";
            case ErrorCode::OutOfRangeError: return "Out of Range";
            case ErrorCode::NullPointerError: return "Null Pointer";
            default: return "Unknown Error";
        }
    }
};

// Specific exception types
class ConfigError : public NLMException {
public:
    ConfigError(const std::string& message) 
        : NLMException(ErrorCode::ConfigError, message) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message)
        : NLMException(ErrorCode::SimulationError, message) {}
};

class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message)
        : NLMException(ErrorCode::MemoryError, message) {}
};

class FileError : public NLMException {
public:
    FileError(const std::string& message)
        : NLMException(ErrorCode::FileError, message) {}
};

} // namespace nlm
```

### Error Recovery
```cpp
// Error recovery mechanisms
class ErrorRecovery {
public:
    enum class RecoveryAction {
        RETRY,
        CONTINUE,
        FALLBACK,
        TERMINATE
    };
    
    static RecoveryAction handleError(const std::exception& error) {
        try {
            throw; // Re-throw to get exception type
        } catch (const ConfigError& e) {
            return handleConfigError(e);
        } catch (const SimulationError& e) {
            return handleSimulationError(e);
        } catch (const MemoryError& e) {
            return handleMemoryError(e);
        } catch (const FileError& e) {
            return handleFileError(e);
        } catch (const std::exception& e) {
            return handleGenericError(e);
        }
    }
    
private:
    static RecoveryAction handleConfigError(const ConfigError& error) {
        // Log error and attempt to recover
        // Common config errors: invalid parameters, missing files
        return RecoveryAction::RETRY;
    }
    
    static RecoveryAction handleSimulationError(const SimulationError& error) {
        // Check if error is recoverable
        // Common simulation errors: invalid neuron state, out of bounds
        return RecoveryAction::CONTINUE;
    }
    
    static RecoveryAction handleMemoryError(const MemoryError& error) {
        // Check memory availability
        // Try to free unused memory
        return RecoveryAction::FALLBACK;
    }
    
    static RecoveryAction handleFileError(const FileError& error) {
        // Check file permissions, disk space
        // Try alternative file paths
        return RecoveryAction::RETRY;
    }
    
    static RecoveryAction handleGenericError(const std::exception& error) {
        // Unknown error - terminate safely
        return RecoveryAction::TERMINATE;
    }
};
```

## Debugging Tools

### Debug Logging
```cpp
// Structured logging system
class DebugLogger {
public:
    enum class LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    struct LogEntry {
        Timestamp timestamp;
        LogLevel level;
        std::string category;
        std::string message;
        std::string file;
        int line;
        std::map<std::string, std::string> metadata;
    };
    
    static void log(LogLevel level, const std::string& category, 
                   const std::string& message, 
                   const std::string& file = "", int line = 0) {
        LogEntry entry;
        entry.timestamp = getCurrentTime();
        entry.level = level;
        entry.category = category;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        
        // Send to all registered sinks
        for (auto& sink : sinks_) {
            sink->write(entry);
        }
    }
    
    static void addSink(std::shared_ptr<LogSink> sink) {
        sinks_.push_back(sink);
    }
    
private:
    static std::vector<std::shared_ptr<LogSink>> sinks_;
};

// Common log categories
#define NLM_LOG_TRACE(msg)   DebugLogger::log(DebugLogger::LogLevel::TRACE, "NLM", msg, __FILE__, __LINE__)
#define NLM_LOG_DEBUG(msg)   DebugLogger::log(DebugLogger::LogLevel::DEBUG, "NLM", msg, __FILE__, __LINE__)
#define NLM_LOG_INFO(msg)    DebugLogger::log(DebugLogger::LogLevel::INFO, "NLM", msg, __FILE__, __LINE__)
#define NLM_LOG_WARNING(msg) DebugLogger::log(DebugLogger::LogLevel::WARNING, "NLM", msg, __FILE__, __LINE__)
#define NLM_LOG_ERROR(msg)   DebugLogger::log(DebugLogger::LogLevel::ERROR, "NLM", msg, __FILE__, __LINE__)
#define NLM_LOG_CRITICAL(msg) DebugLogger::log(DebugLogger::LogLevel::CRITICAL, "NLM", msg, __FILE__, __LINE__)
```

### Memory Debugging
```cpp
// Memory leak detection
class MemoryDebugger {
public:
    struct AllocationRecord {
        void* pointer;
        size_t size;
        const char* file;
        int line;
        std::string stack_trace;
        Timestamp timestamp;
    };
    
    static void* allocate(size_t size, const char* file, int line) {
        void* pointer = std::malloc(size);
        
        if (pointer) {
            AllocationRecord record;
            record.pointer = pointer;
            record.size = size;
            record.file = file;
            record.line = line;
            record.stack_trace = getStackTrace();
            record.timestamp = getCurrentTime();
            
            allocations_.push_back(record);
        }
        
        return pointer;
    }
    
    static void deallocate(void* pointer) {
        if (!pointer) return;
        
        // Find allocation
        auto it = std::find_if(allocations_.begin(), allocations_.end(),
                              [pointer](const AllocationRecord& record) {
                                  return record.pointer == pointer;
                              });
        
        if (it != allocations_.end()) {
            std::free(pointer);
            allocations_.erase(it);
        }
    }
    
    static void checkLeaks() {
        if (!allocations_.empty()) {
            std::cout << "Memory leaks detected:" << std::endl;
            for (const auto& record : allocations_) {
                std::cout << "  " << record.size << " bytes at " << record.pointer
                         << " (" << record.file << ":" << record.line << ")" << std::endl;
                std::cout << "    Stack trace:" << std::endl;
                for (const auto& line : record.stack_trace) {
                    std::cout << "      " << line << std::endl;
                }
            }
        }
    }
    
private:
    static std::string getStackTrace() {
        // Implementation would capture actual stack trace
        return "Stack trace not implemented";
    }
    
    static std::vector<AllocationRecord> allocations_;
};
```

## Configuration Validation

### Schema Validation
```cpp
// Configuration schema validation
class ConfigValidator {
public:
    struct ValidationRule {
        std::string parameter;
        std::function<bool(const Config&)> validator;
        std::string error_message;
    };
    
    static bool validate(const Config& config) {
        bool valid = true;
        
        for (const auto& rule : validation_rules_) {
            if (!validateParameter(config, rule)) {
                std::cerr << "Validation error: " << rule.error_message << std::endl;
                valid = false;
            }
        }
        
        return valid;
    }
    
private:
    static bool validateParameter(const Config& config, 
                                  const ValidationRule& rule) {
        if (!config.has(rule.parameter)) {
            return rule.validator(config); // Use default validator
        }
        
        return rule.validator(config);
    }
    
    static std::vector<ValidationRule> validation_rules_;
};

// Common validation rules
static const std::vector<ConfigValidator::ValidationRule> ConfigValidator::validation_rules_ = {
    // Neuron count validation
    {
        "neuron_count",
        [](const Config& config) {
            auto neuron_count = config.get<size_t>("neuron_count");
            return neuron_count.has_value() && neuron_count.value() > 0 && 
                   neuron_count.value() <= 1000000;
        },
        "neuron_count must be between 1 and 1,000,000"
    },
    
    // Connection probability validation
    {
        "connection_probability",
        [](const Config& config) {
            auto prob = config.get<float>("connection_probability");
            return prob.has_value() && prob.value() >= 0.0f && prob.value() <= 1.0f;
        },
        "connection_probability must be between 0.0 and 1.0"
    },
    
    // Region count validation
    {
        "region_count",
        [](const Config& config) {
            auto region_count = config.get<size_t>("region_count");
            return region_count.has_value() && region_count.value() > 0 && 
                   region_count.value() <= 100;
        },
        "region_count must be between 1 and 100"
    },
    
    // Simulation timestep validation
    {
        "simulation_timestep",
        [](const Config& config) {
            auto timestep = config.get<double>("simulation_timestep");
            return timestep.has_value() && timestep.value() > 0.0 && 
                   timestep.value() <= 0.1;
        },
        "simulation_timestep must be between 0.000001 and 0.1"
    },
    
    // Consistency validation
    {
        "consistency",
        [](const Config& config) {
            auto neuron_count = config.get<size_t>("neuron_count");
            auto region_count = config.get<size_t>("region_count");
            
            if (!neuron_count || !region_count) return true;
            
            // Check if neurons can be distributed evenly
            return neuron_count.value() % region_count.value() == 0;
        },
        "neuron_count must be evenly divisible by region_count"
    }
};
```

## See Also

- [API Reference](API_REFERENCE.md)
- [Configuration Reference](CONFIG_REFERENCE.md)
- [Tutorial Documentation](TUTORIALS/README.md)
- [Examples](EXAMPLES.md)
- [Cheat Sheet](CHEAT_SHEET.md)
- [FAQ](FAQ.md)
- [Coding Guidelines](CODING_GUIDELINES.md)
