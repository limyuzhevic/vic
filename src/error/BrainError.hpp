// Brain Error Codes for Better Error Handling
enum class BrainError {
    SUCCESS = 0,
    NULL_POINTER_DEREFERENCE = 1,
    MEMORY_ALLOCATION_FAILED = 2,
    INVALID_CONFIGURATION = 3,
    INITIALIZATION_FAILED = 4,
    STEP_EXECUTION_FAILED = 5,
    NULL_REGION_ACCESS = 6,
    NULL_POPULATION_ACCESS = 7,
    NULL_NEURON_ACCESS = 8,
    NULL_SYNAPSE_ACCESS = 9,
    FILE_ACCESS_FAILED = 10,
    CHECKPOINT_SAVE_FAILED = 11,
    CHECKPOINT_LOAD_FAILED = 12
};

// Exception class for Brain errors
class BrainException {
public:
    BrainError errorCode;
    std::string message;
    std::string file;
    int line;
    
    BrainException(BrainError code, const std::string& msg, 
                   const std::string& fileName = __FILE__, 
                   int lineNum = __LINE__)
        : errorCode(code), message(msg), file(fileName), line(lineNum) {}
    
    std::string what() const {
        return "Brain Error " + std::to_string(static_cast<int>(errorCode)) + ": " + message +
               " (" + file + ":" + std::to_string(line) + ")";
    }
};

// Validation helper functions
inline bool validateBrain(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain validation failed: null brain pointer");
        return false;
    }
    return true;
}

inline bool validateRegion(NeuralRegion* region) {
    if (!region) {
        NLM_LOG_ERROR("Region validation failed: null region pointer");
        return false;
    }
    return true;
}

inline bool validatePopulation(NeuralPopulation* population) {
    if (!population) {
        NLM_LOG_ERROR("Population validation failed: null population pointer");
        return false;
    }
    return true;
}

inline bool validateNeuron(Neuron* neuron) {
    if (!neuron) {
        NLM_LOG_ERROR("Neuron validation failed: null neuron pointer");
        return false;
    }
    return true;
}

inline bool validateSynapse(Synapse* synapse) {
    if (!synapse) {
        NLM_LOG_ERROR("Synapse validation failed: null synapse pointer");
        return false;
    }
    return true;
}

// Memory safety wrapper for neural operations
template<typename T>
class NeuralSafeWrapper {
private:
    T* ptr_;
    const char* sourceFile_;
    int sourceLine_;
    const char* operation_;
    
public:
    NeuralSafeWrapper(T* ptr, const char* file, int line, const char* op)
        : ptr_(ptr), sourceFile_(file), sourceLine_(line), operation_(op) {
        if (!ptr_) {
            throw BrainException(BrainError::NULL_POINTER_DEREFERENCE,
                                std::string("Null pointer dereference in " ) + operation_ +
                                " at " + std::string(file) + ":" + std::to_string(line));
        }
    }
    
    ~NeuralSafeWrapper() {}
    
    T* get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }
};

// Initialize error logging for Brain
inline void initializeBrainErrorLogging() {
    NLM_LOG_INFO("Brain error logging initialized");
}

// Clear Brain error log
inline void clearBrainErrorLog() {
    NLM_LOG_INFO("Brain error log cleared");
}

// Log Brain error with context
inline void logBrainError(BrainError error, const std::string& context) {
    NLM_LOG_ERROR(std::string("Brain error: ") + context);
}