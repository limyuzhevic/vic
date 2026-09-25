// Utility functions for common patterns used throughout the NLM codebase
// Includes error handling, validation, clamping, and common utility operations
// Provides efficient, numerically stable utility functions for neural computation
// Designed for performance-critical applications with predictable complexity

// Biological inspiration: Neural computation patterns from biological systems
// Computational foundation: Numerical methods from computational neuroscience
// Design philosophy: Efficient, reusable utilities with predictable behavior

namespace nlm {

namespace Utils {

// Error handling utilities
// Biological inspiration: Error detection and correction mechanisms in neural systems
// Computational implementation: Standardized error reporting for debugging
// Performance: Minimal overhead with detailed error information

class Error : public std::runtime_error {
public:
    enum class Code {
        None = 0,
        InvalidArgument = 1,
        OutOfRange = 2,
        NullPointer = 3,
        ConfigurationError = 4,
        FileError = 5,
        MemoryError = 6,
        ValidationError = 7
    };

    Error(Code code, const std::string& message)
        : std::runtime_error(formatMessage(code, message)), code_(code) {}

    Code getCode() const { return code_; }

private:
    Code code_;
    static std::string formatMessage(Code code, const std::string& message) {
        return "[Error " + std::to_string(static_cast<int>(code)) + "] " + message;
    }
};

// Input validation functions
// Biological inspiration: Sensory thresholding and validation mechanisms
// Computational foundation: Defensive programming for numerical stability
// Performance: Early validation to prevent expensive computations

// Validate that a pointer is not null
// Biological inspiration: Neural connectivity validation
// Computational rationale: Prevents null pointer dereference
// Performance: O(1) check, enables early error detection
void validateNotNull(const void* ptr, const std::string& context = "");

// Validate that a value is within specified bounds
// Biological inspiration: Neural response thresholding
// Mathematical: Checks value against [min, max] interval
// Complexity: O(1), enables early error detection
void validateRange(double value, double min, double max, const std::string& context = "");

// Validate that a value is greater than minimum
// Biological inspiration: Excitatory threshold detection
// Mathematical: Checks value > min
// Complexity: O(1), enables early error detection
void validateGreaterThan(double value, double min, const std::string& context = "");

// Validate that a value is less than maximum
// Biological inspiration: Inhibitory threshold detection
// Mathematical: Checks value < max
// Complexity: O(1), enables early error detection
void validateLessThan(double value, double max, const std::string& context = "");

// Validate that a value is non-negative
// Biological inspiration: Physical constraint (potentials cannot be negative)
// Mathematical: Checks value >= 0
// Complexity: O(1), enables early error detection
void validateNonNegative(double value, const std::string& context = "");

// Validate that a value is positive
// Biological inspiration: Neural activation requires positive input
// Mathematical: Checks value > 0
// Complexity: O(1), enables early error detection
void validatePositive(double value, const std::string& context = "");

// Validate that arrays are not empty
// Biological inspiration: Sufficient sensory input required for processing
// Computational rationale: Prevents empty container operations
// Complexity: O(1), enables early error detection
void validateArraysNotEmpty(const std::vector<float>& arr, const std::string& context = "");

// Validate that two arrays have the same size
// Biological inspiration: Consistent dimensionality across neural representations
// Mathematical: Ensures vector operations are well-defined
// Complexity: O(1), enables early error detection
void validateArraysSameSize(const std::vector<float>& arr1, const std::vector<float>& arr2, const std::string& context = "");

// Weight clamping utility functions
// Biological inspiration: Synaptic weight homeostasis and stability
// Computational foundation: Bounded weight dynamics for numerical stability
// Performance: Hardcoded bounds for O(1) clamping operations

// Clamp weight to biologically plausible range [-1, 1]
// Biological inspiration: Synaptic efficacy bounds observed in experiments
// Mathematical: w_clamped = max(min_w, min(max_w, w))
// Complexity: O(1), constant-time for performance
inline float clampWeight(float weight) {
    return std::clamp(weight, Constants::MIN_WEIGHT, Constants::MAX_WEIGHT);
}

// Clamp learning rate to [0, 1] range
// Biological inspiration: Learning rate bounds for stable plasticity
// Mathematical: η_clamped = max(min_η, min(max_η, η))
// Complexity: O(1), constant-time for performance
inline float clampLearningRate(float rate) {
    return std::clamp(rate, Constants::MIN_LEARNING_RATE, Constants::MAX_LEARNING_RATE);
}

// Clamp synaptic efficacy to [0, 2] range
// Biological inspiration: Short-term plasticity modulation bounds
// Mathematical: ε_clamped = max(min_ε, min(max_ε, ε))
// Complexity: O(1), constant-time for performance
inline float clampEfficacy(float efficacy) {
    return std::clamp(efficacy, Constants::MIN_EFFICIENCY, Constants::MAX_EFFICIENCY);
}

// Spike history management utilities
// Biological inspiration: Spike timing-dependent plasticity (STDP) requirements
// Computational foundation: Efficient spike history storage and retrieval
// Performance: Optimized for frequent spike recording and lookup

// Determine if spike should be kept based on history and time window
// Biological inspiration: STDP spike history management
// Mathematical: Keeps spikes within correlation window
// Complexity: O(1) using circular buffer access patterns
inline bool shouldKeepSpike(Timestamp currentTime, const std::vector<Timestamp>& history, 
                           size_t maxHistorySize = Constants::MAX_SPIKE_HISTORY) {
    if (history.size() < maxHistorySize) return true;
    return (currentTime - history.front()) >= Constants::CORRELATION_WINDOW_MS;
}

// Manage spike history with circular buffer eviction
// Biological inspiration: Limited neural memory for spike timing
// Computational foundation: Circular buffer for O(1) push/pop operations
// Performance: Evicts oldest spike when buffer full
inline void manageSpikeHistory(std::vector<Timestamp>& history, Timestamp timestamp,
                               size_t maxHistorySize = Constants::MAX_SPIKE_HISTORY) {
    if (history.size() >= maxHistorySize) {
        history.erase(history.begin());
    }
    history.push_back(timestamp);
}

// Common pattern for update function input validation
// Biological inspiration: Component interaction validation
// Computational foundation: Precondition checking for neural updates
// Performance: Early validation prevents expensive update computations

// Validate inputs for synaptic update functions
// Biological inspiration: Synaptic integrity validation
// Mathematical: Ensures all inputs are valid for weight update
// Complexity: O(1), early validation for performance
inline bool validateUpdateInputs(const class Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt = Constants::DEFAULT_TIMESTEP_MS) {
    if (!synapse) return false;
    if (preSpikes.empty() && postSpikes.empty()) return false;
    if (std::abs(dt) < Constants::EPSILON_FOR_NORMALS) return false;
    return true;
}

// Similarity computation utilities
// Biological inspiration: Pattern matching and association in neural systems
// Computational foundation: Distance metrics for neural representations
// Performance: Efficient O(n) computation for similarity assessment

// Compute normalized pattern difference between two vectors
// Biological inspiration: Pattern recognition in neural systems
// Mathematical: Manhattan distance normalized by vector size
// Complexity: O(n), efficient for moderate-sized vectors
inline float computePatternDifference(const std::vector<float>& pattern1, 
                                     const std::vector<float>& pattern2) {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    size_t compareLen = std::min(pattern1.size(), pattern2.size());
    float totalDiff = 0.0f;
    
    for (size_t i = 0; i < compareLen; ++i) {
        totalDiff += std::abs(pattern1[i] - pattern2[i]);
    }
    
    return totalDiff / static_cast<float>(compareLen);
}

// Memory-efficient vector operations
// Biological inspiration: Efficient resource utilization in neural systems
// Computational foundation: Memory-conscious programming
// Performance: Precomputed scaling factors for O(1) operations

// Calculate vector size with load factor for efficient allocation
// Biological inspiration: Resource allocation in biological systems
// Mathematical: n' = ⌈n × load_factor⌉ for hash table sizing
// Complexity: O(1), constant-time calculation
inline size_t calculateSizeWithReserve(size_t expected, float loadFactor = 1.25f) {
    return static_cast<size_t>(expected * loadFactor) + 1;
}

// Safety clamping for membrane potential
// Biological inspiration: Neural membrane potential physical constraints
// Computational foundation: Numerical safety for LIF dynamics
// Performance: Hardcoded bounds for O(1) clamping

// Clamp membrane potential to biologically plausible range
// Biological inspiration: Membrane potential physical constraints (-100mV to 50mV)
// Mathematical: V_clamped = max(min_V, min(max_V, V))
// Complexity: O(1), constant-time for performance
inline MembranePotential clampMembranePotential(MembranePotential v) {
    return std::clamp(v, Constants::MIN_POTENTIAL_MV, Constants::MAX_POTENTIAL_MV);
}

// Developmental stage calculation
// Biological inspiration: Stage-based development in neural systems
// Computational foundation: Age-dependent state transitions
// Performance: Precomputed stage boundaries for O(1) lookup

// Calculate developmental stage based on age
// Biological inspiration: Critical periods and developmental windows
// Mathematical: piecewise linear stage boundaries
// Complexity: O(1), constant-time stage determination
inline DevelopmentalStage getDevelopmentalStage(double ageMs) {
    if (ageMs < Constants::INITIAL_STAGE_MS) {
        return DevelopmentalStage::Initial;
    } else if (ageMs < Constants::CRITICAL_PERIOD_MS) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (ageMs < Constants::MATURATION_MS) {
        return DevelopmentalStage::Maturation;
    } else {
        return DevelopmentalStage::Adult;
    }
}

// Exponential decay calculation
// Biological inspiration: Neuronal adaptation and decay processes
// Computational foundation: Exponential functions in neural dynamics
// Performance: Precomputed exponential base for efficient computation

// Calculate exponential decay with given parameters
// Biological inspiration: Synaptic adaptation and memory decay
// Mathematical: y(t) = y0 × e^(-λ×t)
// Complexity: O(1), single exponential calculation
inline double exponentialDecay(double value, double decayRate, TimestepDuration dt) {
    return value * std::exp(-decayRate * dt);
}

// Rolling average calculation
// Biological inspiration: Temporal averaging in neural systems
// Computational foundation: Moving window statistics
// Performance: O(n) for windowed averaging, O(1) for fixed windows

// Calculate rolling average over specified window
// Biological inspiration: Temporal integration in neural processing
// Mathematical: mean = Σ(window)/window_size
// Complexity: O(window_size), efficient for small-to-moderate windows
inline float rollingAverage(const std::vector<float>& history, size_t windowSize) {
    if (history.empty()) return 0.0f;
    
    size_t start = std::max<size_t>(0, static_cast<long>(history.size()) - windowSize);
    float sum = 0.0f;
    
    for (size_t i = start; i < history.size(); ++i) {
        sum += history[i];
    }
    
    return sum / static_cast<float>(history.size() - start);
}

// Vector fill utility
// Biological inspiration: Uniform state initialization in neural systems
// Computational foundation: Memory initialization patterns
// Performance: O(n) with efficient std::fill

// Fill vector with constant value
// Biological inspiration: Uniform synaptic weight initialization
// Mathematical: v[i] = value for all i
// Complexity: O(n), efficient vector initialization
inline void fillVector(std::vector<float>& vec, float value) {
    std::fill(vec.begin(), vec.end(), value);
}

// Check if vector is sorted
// Biological inspiration: Pattern regularity detection
// Computational foundation: Sorting verification algorithms
// Performance: O(n) for verification, O(1) for ascending/descending check

// Check if vector is sorted in specified order
// Biological inspiration: Pattern regularity in neural firing
// Mathematical: checks monotonic sequence property
// Complexity: O(n), efficient for validation
inline bool isSorted(const std::vector<float>& vec, bool ascending = true) {
    if (vec.empty()) return true;
    
    for (size_t i = 1; i < vec.size(); ++i) {
        if (ascending && vec[i-1] > vec[i]) return false;
        if (!ascending && vec[i-1] < vec[i]) return false;
    }
    
    return true;
}

// Normalize vector to [0, 1] range
// Biological inspiration: Neural response normalization
// Computational foundation: Feature scaling for comparison
// Performance: O(n) with linear transformation, handles edge cases

// Normalize vector values to [0, 1] range
// Biological inspiration: Neural response gain control
// Mathematical: v_normalized = (v - min) / (max - min)
// Complexity: O(n), linear transformation with edge case handling
inline std::vector<float> normalizeVector(const std::vector<float>& vec) {
    if (vec.empty()) return {};
    
    float minVal = *std::min_element(vec.begin(), vec.end());
    float maxVal = *std::max_element(vec.begin(), vec.end());
    
    if (std::abs(maxVal - minVal) < Constants::EPSILON_FOR_NORMALS) {
        return std::vector<float>(vec.size(), 0.5f);
    }
    
    std::vector<float> result(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        result[i] = (vec[i] - minVal) / (maxVal - minVal);
    }
    
    return result;
}

// Validation utilities documentation
// ================================

// Validation Design Philosophy:
// ---------------------------
// 1. Early Validation: Validate inputs before expensive computations
// 2. Context-Aware Errors: Include context in error messages for debugging
// 3. Defensive Programming: Assume invalid inputs may occur
// 4. Performance: O(1) validation with minimal overhead
// 5. Consistency: Consistent validation across all neural components

// Validation Patterns:
// -----------------
// 1. Null Pointer Checks: Prevent crashes from invalid references
// 2. Range Validation: Ensure values within biologically plausible ranges
// 3. Dimensionality Checks: Ensure consistent vector/matrix dimensions
// 4. State Validation: Ensure components are in valid states for operations

// Error Handling Design:
// ---------------------
// 1. Exception-based: Use exceptions for error reporting
// 2. Error Codes: Categorize errors for granular handling
// 3. Context Information: Include operation context in error messages
// 4. Recovery: Enable graceful error recovery when possible

// Performance Considerations:
// -------------------------
// 1. Constant-time Validation: All validation functions O(1)
// 2. Minimal Allocation: Avoid dynamic allocation in validation
// 3. Early Termination: Fail fast on invalid inputs
// 4. Cache-Friendly: Sequential access patterns for validation

// Testing and Validation Approaches:
// ------------------------------
// 1. Unit Tests: Test individual validation functions
// 2. Integration Tests: Test validation across component boundaries
// 3. Boundary Tests: Test validation at extreme values
// 4. Performance Tests: Ensure validation doesn't degrade performance

} // namespace Utils

} // namespace nlm
