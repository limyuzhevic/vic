#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "../core/Logger/Logger.hpp"
#include "AgentBrain.hpp"

namespace nlm
{

// Simplified forward declarations
class SimpleWorld;
class SensoryPercept;
enum class DevelopmentalStage;

// Enhanced error handling system for AgentBrain
class AgentBrainErrorHandler {
public:
    enum class ErrorType {
        None,
        InvalidState,
        ConfigurationError,
        NeuralNetworkError,
        WorldInteractionError,
        DevelopmentalError,
        MemoryError,
        ParameterValidationError
    };
    
    struct ErrorInfo {
        ErrorType type;
        std::string message;
        std::string sourceFile;
        int lineNumber;
        double timestamp;
        std::string context;
        std::vector<std::string> stackTrace;
        bool recoverable;
    };
    
    AgentBrainErrorHandler() : logLevel_(LogLevel::INFO), maxErrors_(10) {}
    
    // Report an error
    void reportError(ErrorType type, const std::string& message, 
                   const std::string& sourceFile = "", int lineNumber = 0,
                   const std::string& context = "", bool recoverable = true) {
        if (getErrorCount() >= maxErrors_) {
            NLM_LOG_ERROR("Error limit exceeded: " + message);
            return;
        }
        
        ErrorInfo error;
        error.type = type;
        error.message = message;
        error.sourceFile = sourceFile;
        error.lineNumber = lineNumber;
        error.timestamp = getCurrentTimestamp();
        error.context = context;
        error.recoverable = recoverable;
        
        errors_.push_back(error);
        
        // Log the error with appropriate level
        std::string logMessage = "AgentBrain Error: " + message;
        if (!error.context.empty()) {
            logMessage += " [Context: " + error.context + "]";
        }
        
        switch (type) {
            case ErrorType::ConfigurationError:
                NLM_LOG_ERROR(logMessage);
                break;
            case ErrorType::NeuralNetworkError:
                NLM_LOG_WARN(logMessage);
                break;
            case ErrorType::WorldInteractionError:
                NLM_LOG_WARN(logMessage);
                break;
            case ErrorType::DevelopmentalError:
                NLM_LOG_WARN(logMessage);
                break;
            case ErrorType::InvalidState:
                NLM_LOG_ERROR(logMessage);
                break;
            case ErrorType::MemoryError:
                NLM_LOG_ERROR(logMessage);
                break;
            case ErrorType::ParameterValidationError:
                NLM_LOG_WARN(logMessage);
                break;
            default:
                NLM_LOG_ERROR(logMessage);
                break;
        }
        
        // Trim old errors if we exceed the limit
        if (errors_.size() > maxErrors_) {
            errors_.erase(errors_.begin());
        }
    }
    
    // Get all errors
    const std::vector<ErrorInfo>& getErrors() const { return errors_; }
    
    // Get error count
    size_t getErrorCount() const { return errors_.size(); }
    
    // Check if there are any errors
    bool hasErrors() const { return !errors_.empty(); }
    
    // Clear all errors
    void clearErrors() { errors_.clear(); }
    
    // Get errors of specific type
    std::vector<ErrorInfo> getErrorsByType(ErrorType type) const {
        std::vector<ErrorInfo> filtered;
        for (const auto& error : errors_) {
            if (error.type == type) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    // Export errors to string for debugging
    std::string exportErrors() const {
        std::stringstream ss;
        ss << "AgentBrain Error Report (" << errors_.size() << " errors):\n";
        
        for (size_t i = 0; i < errors_.size(); ++i) {
            const auto& error = errors_[i];
            ss << "  Error " << i + 1 << ":\n";
            ss << "    Type: " << errorTypeToString(error.type) << "\n";
            ss << "    Message: " + error.message << "\n";
            ss << "    Source: " << error.sourceFile << ":" << error.lineNumber << "\n";
            ss << "    Time: " << error.timestamp << "\n";
            if (!error.context.empty()) {
                ss << "    Context: " << error.context << "\n";
            }
            ss << "    Recoverable: " << (error.recoverable ? "Yes" : "No") << "\n";
            if (i < errors_.size() - 1) {
                ss << "\n";
            }
        }
        
        return ss.str();
    }
    
    // Set log level
    void setLogLevel(LogLevel level) { logLevel_ = level; }
    
    // Set maximum errors before suppression
    void setMaxErrors(size_t max) { maxErrors_ = max; }
    
    // Get current timestamp
    double getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return static_cast<double>(millis) / 1000.0;
    }
    
private:
    std::string errorTypeToString(ErrorType type) const {
        switch (type) {
            case ErrorType::None: return "None";
            case ErrorType::InvalidState: return "InvalidState";
            case ErrorType::ConfigurationError: return "ConfigurationError";
            case ErrorType::NeuralNetworkError: return "NeuralNetworkError";
            case ErrorType::WorldInteractionError: return "WorldInteractionError";
            case ErrorType::DevelopmentalError: return "DevelopmentalError";
            case ErrorType::MemoryError: return "MemoryError";
            case ErrorType::ParameterValidationError: return "ParameterValidationError";
            default: return "Unknown";
        }
    }
    
    std::vector<ErrorInfo> errors_;
    LogLevel logLevel_;
    size_t maxErrors_;
};

// Performance monitoring system for AgentBrain
class AgentBrainProfiler {
public:
    struct PerformanceMetrics {
        double processSensoryTime = 0.0;
        double decodeMotorTime = 0.0;
        double rewardModulationTime = 0.0;
        double developmentTime = 0.0;
        double totalUpdateTime = 0.0;
        size_t sensoryProcessCount = 0;
        size_t motorDecodeCount = 0;
        size_t rewardModulationCount = 0;
        size_t developmentCount = 0;
        double memoryUsage = 0.0;
        double cpuUsage = 0.0;
    };
    
    AgentBrainProfiler() : enabled_(false), sampleRate_(1.0), maxSamples_(1000) {}
    
    // Start timing a specific operation
    void startTimer(const std::string& operationName) {
        if (!enabled_) return;
        
        timers_[operationName] = std::chrono::high_resolution_clock::now();
    }
    
    // Stop timing and record the duration
    void stopTimer(const std::string& operationName) {
        if (!enabled_ || !timers_.count(operationName)) return;
        
        auto end = std::chrono::high_resolution_clock::now();
        auto start = timers_[operationName];
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        recordOperationTime(operationName, duration);
        timers_.erase(operationName);
    }
    
    // Record a performance sample
    void recordSample(const PerformanceMetrics& metrics) {
        if (!enabled_) return;
        
        samples_.push_back(metrics);
        if (samples_.size() > maxSamples_) {
            samples_.erase(samples_.begin());
        }
        
        // Log if we're at the sample rate
        if (sampleCount_ % static_cast<size_t>(sampleRate_ * 10)) == 0) {
            logSample(metrics);
        }
        
        sampleCount_++;
    }
    
    // Enable or disable profiling
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    // Set sampling rate (samples per second)
    void setSampleRate(double rate) { sampleRate_ = rate; }
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const {
        PerformanceMetrics metrics;
        
        // Sum up recorded times
        for (const auto& sample : samples_) {
            metrics.processSensoryTime += sample.processSensoryTime;
            metrics.decodeMotorTime += sample.decodeMotorTime;
            metrics.rewardModulationTime += sample.rewardModulationTime;
            metrics.developmentTime += sample.developmentTime;
            metrics.totalUpdateTime += sample.totalUpdateTime;
            metrics.sensoryProcessCount += sample.sensoryProcessCount;
            metrics.motorDecodeCount += sample.motorDecodeCount;
            metrics.rewardModulationCount += sample.rewardModulationCount;
            metrics.developmentCount += sample.developmentCount;
        }
        
        // Average the values
        size_t count = samples_.empty() ? 1 : samples_.size();
        metrics.processSensoryTime /= count;
        metrics.decodeMotorTime /= count;
        metrics.rewardModulationTime /= count;
        metrics.developmentTime /= count;
        metrics.totalUpdateTime /= count;
        
        return metrics;
    }
    
    // Get all samples
    const std::vector<PerformanceMetrics>& getSamples() const { return samples_; }
    
    // Get sample count
    size_t getSampleCount() const { return samples_.size(); }
    
    // Clear all samples
    void clearSamples() { samples_.clear(); }
    
    // Generate performance report
    std::string generateReport() const {
        std::stringstream ss;
        ss << "AgentBrain Performance Report\n";
        ss << "=============================\n";
        ss << "Total Samples: " << samples_.size() << "\n\n";
        
        if (samples_.empty()) {
            ss << "No performance data available.\n";
            return ss.str();
        }
        
        // Calculate averages and statistics
        double avgProcessSensory = 0.0, avgDecodeMotor = 0.0, avgRewardModulation = 0.0,
               avgDevelopment = 0.0, avgTotalUpdate = 0.0;
        
        for (const auto& sample : samples_) {
            avgProcessSensory += sample.processSensoryTime;
            avgDecodeMotor += sample.decodeMotorTime;
            avgRewardModulation += sample.rewardModulationTime;
            avgDevelopment += sample.developmentTime;
            avgTotalUpdate += sample.totalUpdateTime;
        }
        
        size_t count = samples_.size();
        avgProcessSensory /= count;
        avgDecodeMotor /= count;
        avgRewardModulation /= count;
        avgDevelopment /= count;
        avgTotalUpdate /= count;
        
        ss << "Average Processing Times (ms):\n";
        ss << "  Sensory Processing: " << avgProcessSensory << "\n";
        ss << "  Motor Decoding: " << avgDecodeMotor << "\n";
        ss << "  Reward Modulation: " << avgRewardModulation << "\n";
        ss << "  Development Updates: " << avgDevelopment << "\n";
        ss << "  Total Update: " << avgTotalUpdate << "\n\n";
        
        ss << "Operation Counts:\n";
        ss << "  Sensory Processing: " << getTotalSensoryCount() << "\n";
        ss << "  Motor Decoding: " << getTotalMotorDecodeCount() << "\n";
        ss << "  Reward Modulation: " << getTotalRewardModulationCount() << "\n";
        ss << "  Development Updates: " << getTotalDevelopmentCount() << "\n\n";
        
        ss << "Derived Metrics:\n";
        ss << "  Processing Rate: " << (count * 1000.0 / avgTotalUpdate) << " updates/sec\n";
        ss << "  Sensory Per Update: " << (getTotalSensoryCount() / static_cast<double>(count)) << "\n";
        
        return ss.str();
    }
    
private:
    void recordOperationTime(const std::string& operationName, double duration) {
        PerformanceMetrics& metrics = currentMetrics_;
        
        if (operationName == "processSensory") {
            metrics.processSensoryTime += duration;
            metrics.sensoryProcessCount++;
        } else if (operationName == "decodeMotor") {
            metrics.decodeMotorTime += duration;
            metrics.motorDecodeCount++;
        } else if (operationName == "rewardModulation") {
            metrics.rewardModulationTime += duration;
            metrics.rewardModulationCount++;
        } else if (operationName == "development") {
            metrics.developmentTime += duration;
            metrics.developmentCount++;
        }
        
        metrics.totalUpdateTime += duration;
    }
    
    void logSample(const PerformanceMetrics& metrics) {
        NLM_LOG_DEBUG("Performance Sample: " +
                     std::to_string(metrics.processSensoryTime) + "ms sensory, " +
                     std::to_string(metrics.decodeMotorTime) + "ms motor decode, " +
                     std::to_string(metrics.rewardModulationTime) + "ms reward, " +
                     std::to_string(metrics.developmentTime) + "ms development");
    }
    
    double getTotalSensoryCount() const {
        double total = 0.0;
        for (const auto& sample : samples_) {
            total += sample.sensoryProcessCount;
        }
        return total;
    }
    
    double getTotalMotorDecodeCount() const {
        double total = 0.0;
        for (const auto& sample : samples_) {
            total += sample.motorDecodeCount;
        }
        return total;
    }
    
    double getTotalRewardModulationCount() const {
        double total = 0.0;
        for (const auto& sample : samples_) {
            total += sample.rewardModulationCount;
        }
        return total;
    }
    
    double getTotalDevelopmentCount() const {
        double total = 0.0;
        for (const auto& sample : samples_) {
            total += sample.developmentCount;
        }
        return total;
    }
    
    std::map<std::string, std::chrono::high_resolution_clock::time_point> timers_;
    std::vector<PerformanceMetrics> samples_;
    PerformanceMetrics currentMetrics_;
    bool enabled_;
    double sampleRate_;
    size_t maxSamples_;
    size_t sampleCount_;
};

// State change listener system for extensibility
class AgentBrainStateChangeListener {
public:
    virtual ~AgentBrainStateChangeListener() = default;
    
    virtual void onDevelopmentalStageChange(DevelopmentalStage oldStage, DevelopmentalStage newStage) = 0;
    virtual void onRewardModulationApplied(float reward, float predictionError) = 0;
    virtual void onCuriosityLevelChanged(float oldLevel, float newLevel) = 0;
    virtual void onNoveltyLevelChanged(float oldLevel, float newLevel) = 0;
    virtual void onPredictionErrorChanged(float oldError, float newError) = 0;
    virtual void onConfigurationChanged(const std::string& parameterName, float oldValue, float newValue) = 0;
    virtual void onErrorReported(const AgentBrainErrorHandler::ErrorInfo& error) = 0;
};

// Simplified enhanced wrapper that adds error handling, profiling, and listener support
class EnhancedAgentBrain {
public:
    EnhancedAgentBrain(std::shared_ptr<AgentBrain> brain) 
        : errorHandler_(), profiler_(), listenerCount_(0) {
        
        // Initialize the core agent brain
        brain_ = brain;
        
        // Set up default error handlers
        setupDefaultErrorHandlers();
    }
    
    // Initialize with world
    void initialize(const SimpleWorld& world) {
        brain_->initialize(world);
    }
    
    // Process sensory input with profiling
    void processSensoryInput(const SensoryPercept& percept) {
        profiler_.startTimer("processSensory");
        brain_->processSensoryInput(percept);
        profiler_.stopTimer("processSensory");
        
        // Check for errors
        if (errorHandler_.hasErrors()) {
            notifyErrors(errorHandler_.getErrors());
        }
    }
    
    // Decode motor command with profiling
    MotorCommand decodeMotorCommand() {
        profiler_.startTimer("decodeMotor");
        MotorCommand cmd = brain_->decodeMotorCommand();
        profiler_.stopTimer("decodeMotor");
        return cmd;
    }
    
    // Apply reward modulation with profiling
    void applyRewardModulation(float reward, float predictedReward) {
        profiler_.startTimer("rewardModulation");
        brain_->applyRewardModulation(reward, predictedReward);
        profiler_.stopTimer("rewardModulation");
        
        // Notify listeners
        notifyRewardModulation(reward, predictedReward);
        
        // Check for errors
        if (errorHandler_.hasErrors()) {
            notifyErrors(errorHandler_.getErrors());
        }
    }
    
    // Update development with profiling
    void updateDevelopment(double timestep) {
        profiler_.startTimer("development");
        brain_->updateDevelopment(timestep);
        profiler_.stopTimer("development");
        
        // Check for errors
        if (errorHandler_.hasErrors()) {
            notifyErrors(errorHandler_.getErrors());
        }
    }
    
    // Add a state change listener
    void addStateChangeListener(std::shared_ptr<AgentBrainStateChangeListener> listener) {
        listeners_.push_back(listener);
        listenerCount_++;
    }
    
    // Remove a state change listener
    void removeStateChangeListener(std::shared_ptr<AgentBrainStateChangeListener> listener) {
        auto it = std::find(listeners_.begin(), listeners_.end(), listener);
        if (it != listeners_.end()) {
            listeners_.erase(it);
            listenerCount_--;
        }
    }
    
    // Get error handler reference
    AgentBrainErrorHandler& getErrorHandler() { return errorHandler_; }
    
    // Get profiler reference
    AgentBrainProfiler& getProfiler() { return profiler_; }
    
    // Enable or disable profiling
    void setProfilingEnabled(bool enabled) { profiler_.setEnabled(enabled); }
    
    // Enable or disable error reporting
    void setErrorReportingEnabled(bool enabled) {
        if (!enabled) {
            errorHandler_.clearErrors();
        }
    }
    
    // Record performance sample
    void recordPerformanceSample() {
        profiler_.recordSample(profiler_.getCurrentMetrics());
    }
    
    // Get core brain reference
    AgentBrain& getCoreBrain() { return *brain_; }
    
    // Export debugging information
    std::string exportDebugInfo() const {
        std::stringstream ss;
        ss << "EnhancedAgentBrain Debug Information\n";
        ss << "====================================\n";
        ss << "Core Brain: " << (brain_ ? "Initialized" : "Not Initialized") << "\n";
        ss << "Error Count: " << errorHandler_.getErrorCount() << "\n";
        ss << "Listener Count: " << listenerCount_ << "\n";
        ss << "Profiling Enabled: " << (profiler_.getSamples().empty() ? "No" : "Yes") << "\n";
        ss << "\n";
        ss << "Errors:\n" << errorHandler_.exportErrors() << "\n";
        ss << "Performance:\n" << profiler_.generateReport() << "\n";
        return ss.str();
    }
    
    // Reset all systems
    void reset() {
        brain_->reset();
        errorHandler_.clearErrors();
        profiler_.clearSamples();
    }
    
protected:
    // Virtual method for subclasses to customize error handling
    virtual void handleError(const AgentBrainErrorHandler::ErrorInfo& error) {
        errorHandler_.reportError(error.type, error.message, error.sourceFile, 
                                 error.lineNumber, error.context, error.recoverable);
    }
    
    // Virtual method for subclasses to customize profiling
    virtual void handlePerformanceMetrics(const AgentBrainProfiler::PerformanceMetrics& metrics) {
        profiler_.recordSample(metrics);
    }
    
private:
    void setupDefaultErrorHandlers() {
        // Set up default error recovery and reporting
        errorHandler_.setMaxErrors(100);
        errorHandler_.setLogLevel(LogLevel::INFO);
    }
    
    void notifyRewardModulation(float reward, float predictionError) {
        for (const auto& listener : listeners_) {
            listener->onRewardModulationApplied(reward, predictionError);
        }
    }
    
    void notifyErrors(const std::vector<AgentBrainErrorHandler::ErrorInfo>& errors) {
        for (const auto& listener : listeners_) {
            for (const auto& error : errors) {
                listener->onErrorReported(error);
            }
        }
    }
    
    std::shared_ptr<AgentBrain> brain_;
    AgentBrainErrorHandler errorHandler_;
    AgentBrainProfiler profiler_;
    std::vector<std::shared_ptr<AgentBrainStateChangeListener>> listeners_;
    size_t listenerCount_;
};

} // namespace nlm