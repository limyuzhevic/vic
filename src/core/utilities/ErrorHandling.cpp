#include "ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include <cstdarg>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>

namespace nlm {

// StringUtils implementation
std::string StringUtils::format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // First pass to determine buffer size
    int length = std::vsnprintf(nullptr, 0, format, args);
    va_end(args);
    
    if (length <= 0) {
        return "";
    }
    
    // Allocate buffer and format string
    std::string result;
    result.resize(length + 1); // +1 for null terminator
    
    va_start(args, format);
    std::vsnprintf(&result[0], result.size(), format, args);
    va_end(args);
    
    return result;
}

std::string StringUtils::toString(double value, int precision) {
    std::ostringstream oss;
    oss.precision(precision);
    oss << std::fixed << value;
    return oss.str();
}

std::string StringUtils::toString(float value, int precision) {
    return toString(static_cast<double>(value), precision);
}

std::string StringUtils::toString(int64_t value) {
    return std::to_string(value);
}

std::string StringUtils::toString(size_t value) {
    return std::to_string(value);
}

std::string StringUtils::toString(bool value) {
    return value ? "true" : "false";
}

// PerformanceTimer implementation
PerformanceTimer::PerformanceTimer() {
    start_ = std::chrono::high_resolution_clock::now();
}

double PerformanceTimer::elapsed() const {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
    return duration.count() / 1000000.0; // Convert to seconds
}

void PerformanceTimer::reset() {
    start_ = std::chrono::high_resolution_clock::now();
}

// MemoryPool implementation (template specializations)
template<typename T>
MemoryPool<T>::MemoryPool(size_t capacity) : capacity_(capacity), used_(0), head_(0) {
    if (capacity_ == 0) {
        NLM_LOG_WARNING("MemoryPool: Capacity must be positive, using default 1000");
        capacity_ = 1000;
    }
    pool_ = static_cast<T*>(std::malloc(capacity_ * sizeof(T)));
    if (!pool_) {
        throw MemoryError("MemoryPool: Failed to allocate memory pool");
    }
}

template<typename T>
T* MemoryPool<T>::allocate() {
    if (used_ >= capacity_) {
        throw MemoryError("MemoryPool: Out of memory");
    }
    T* ptr = &pool_[head_];
    head_++;
    used_++;
    return ptr;
}

template<typename T>
void MemoryPool<T>::deallocate(T* ptr) {
    if (!ptr || ptr < pool_ || ptr >= pool_ + capacity_) {
        NLM_LOG_WARNING("MemoryPool: Attempt to deallocate invalid pointer");
        return;
    }
    // Simple approach: just mark as unused (in real implementation, would track free list)
    used_--;
}

// ArrayUtils implementation
template<typename T>
void ArrayUtils::safeCopy(const T* src, T* dst, size_t count, const T& defaultValue) {
    if (!src || !dst) {
        throw NLMError("ArrayUtils: Null pointer provided for safeCopy");
    }
    for (size_t i = 0; i < count; ++i) {
        dst[i] = src[i];
    }
}

template<typename T>
void ArrayUtils::safeFill(T* array, T value, size_t count) {
    if (!array) {
        throw NLMError("ArrayUtils: Null pointer provided for safeFill");
    }
    for (size_t i = 0; i < count; ++i) {
        array[i] = value;
    }
}

template<typename T>
T ArrayUtils::clamp(T value, T min, T max) {
    return std::max(min, std::min(value, max));
}

template<typename T>
bool ArrayUtils::isNaN(const T& value) {
    return std::isnan(static_cast<double>(value));
}

template<typename T>
bool ArrayUtils::isInfinity(const T& value) {
    return std::isinf(static_cast<double>(value));
}

template<typename T>
T ArrayUtils::mean(const T* array, size_t count) {
    if (!array || count == 0) {
        throw NLMError("ArrayUtils: Invalid parameters for mean calculation");
    }
    T sum = 0;
    for (size_t i = 0; i < count; ++i) {
        sum += array[i];
    }
    return sum / static_cast<T>(count);
}

template<typename T>
T ArrayUtils::variance(const T* array, size_t count, T mean) {
    if (!array || count == 0) {
        throw NLMError("ArrayUtils: Invalid parameters for variance calculation");
    }
    T sum = 0;
    for (size_t i = 0; i < count; ++i) {
        T diff = array[i] - mean;
        sum += diff * diff;
    }
    return sum / static_cast<T>(count);
}

// ValidationUtils implementation
bool ValidationUtils::isValidNeuronType(NeuronType type) {
    return type >= NeuronType::Excitatory && type <= NeuronType::Internal;
}

bool ValidationUtils::isValidSynapseType(SynapseType type) {
    return type >= SynapseType::Excitatory && type <= SynapseType::GapJunction;
}

bool ValidationUtils::isValidActionType(ActionType type) {
    return type >= ActionType::MoveForward && type <= ActionType::Custom;
}

bool ValidationUtils::isValid DevelopmentalStage(DevelopmentalStage stage) {
    return stage >= DevelopmentalStage::Initial && stage <= DevelopmentalStage::Aging;
}

bool ValidationUtils::isValidLogLevel(LogLevel level) {
    return level >= LogLevel::Debug && level <= LogLevel::Critical;
}

bool ValidationUtils::isPositive(double value) {
    return value > 0.0;
}

bool ValidationUtils::isNonNegative(double value) {
    return value >= 0.0;
}

bool ValidationUtils::isInRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool ValidationUtils::isFinite(double value) {
    return std::isfinite(value);
}

bool ValidationUtils::isNormal(double value) {
    return std::isnormal(value);
}

template<typename T>
bool ValidationUtils::isValidIndex(T index, T size) {
    return index >= 0 && index < size;
}

std::string ValidationUtils::validateConfig(const Config& config) {
    std::string errors;
    
    // Check for required keys
    const std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.v_thresh",
        "brain.v_rest",
        "brain.tau_mem"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errors += "Missing required config key: " + key + "\n";
        }
    }
    
    // Validate values
    if (config.has("brain.neuron_count")) {
        int64_t neuronCount = config.getOr<int64_t>("brain.neuron_count", 0);
        if (neuronCount <= 0) {
            errors += "brain.neuron_count must be positive\n";
        }
    }
    
    return errors;
}

std::string ValidationUtils::validateBrainState(const Brain* brain) {
    std::string errors;
    
    if (!brain) {
        return "Brain pointer is null";
    }
    
    if (brain->getTotalNeuronCount() == 0) {
        errors += "Brain has no neurons\n";
    }
    
    if (brain->getTotalSynapseCount() == 0 && brain->getTotalNeuronCount() > 0) {
        errors += "Brain has neurons but no synapses\n";
    }
    
    return errors;
}

// CallbackManager implementation
CallbackManager::CallbackManager() {}

void CallbackManager::addCallback(const Callback& callback) {
    if (callback) {
        callbacks_.push_back(callback);
    }
}

void CallbackManager::removeCallback(const Callback& callback) {
    callbacks_.erase(std::remove(callbacks_.begin(), callbacks_.end(), callback), callbacks_.end());
}

void CallbackManager::executeAll() {
    for (const auto& callback : callbacks_) {
        try {
            callback();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Callback execution failed: " + std::string(e.what()));
        }
    }
}

void CallbackManager::clear() {
    callbacks_.clear();
}

// ThreadUtils implementation
void ThreadUtils::memoryBarrier() {
#ifdef __ATOMIC_SEQ_CST
    std::atomic_thread_fence(std::memory_order_seq_cst);
#else
    // Fallback for compilers without atomic thread fence
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}

void ThreadUtils::threadYield() {
    std::this_thread::yield();
}

size_t ThreadUtils::getCPUCount() {
    return std::thread::hardware_concurrency();
}

bool ThreadUtils::isMultiCore() {
    return getCPUCount() > 1;
}

// Platform getPlatformString() implementation
const char* Platform::getPlatformString() {
#ifdef _WIN32
    return "Windows";
#elif __linux__
    return "Linux";
#elif __APPLE__
    return "macOS";
#else
    return "Unknown";
#endif
}

} // namespace nlm
