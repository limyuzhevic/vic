# NLM Plugin Development Guide

## Overview
This comprehensive guide covers plugin development for the enhanced NLM (熙然) neural learning machine system. It provides detailed information about creating plugins for the NeuralActivityCalculator, SpikeDetector, EpisodeBuilder, and other utility classes.

## Plugin Architecture

The NLM plugin architecture is designed to be modular and extensible. Plugins can:

1. **Extend Core Functionality**: Add new neural activity calculation strategies
2. **Enhance Spike Detection**: Implement custom spike detection algorithms
3. **Customize Episode Management**: Create custom episode construction and management
4. **Extend Error Handling**: Add custom error handling and recovery mechanisms
5. **Add Neural Math Utilities**: Implement custom mathematical functions

### Plugin Interface

```cpp
// Base plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};
```

### Plugin Categories

#### 1. Neural Activity Calculator Plugins

Plugins that extend NeuralActivityCalculator functionality:

```cpp
class INeuralActivityCalculatorPlugin : public IPlugin {
public:
    virtual NeuralActivityCalculator::Strategy calculateStrategy() const = 0;
    virtual double calculate(const std::vector<double>& voltages) const = 0;
    virtual std::string getParameters() const = 0;
    virtual bool setParameters(const std::string& parameters) = 0;
};
```

#### 2. Spike Detection Plugins

Plugins that extend SpikeDetector functionality:

```cpp
class ISpikeDetectorPlugin : public IPlugin {
public:
    virtual bool detectSpike(double voltage, double timestep) = 0;
    virtual std::vector<double> getSpikeTimes() const = 0;
    virtual void reset() = 0;
    virtual std::string getDetectionParameters() const = 0;
    virtual bool setDetectionParameters(const std::string& parameters) = 0;
};
```

#### 3. Episode Management Plugins

Plugins that extend EpisodeBuilder functionality:

```cpp
class IEpisodeBuilderPlugin : public IPlugin {
public:
    virtual Episode buildEpisode(const std::vector<Event>& events) = 0;
    virtual Episode buildEpisode(const EpisodeData& data) = 0;
    virtual bool validateEpisode(const Episode& episode) const = 0;
    virtual std::string getBuilderParameters() const = 0;
    virtual bool setBuilderParameters(const std::string& parameters) = 0;
};
```

#### 4. Error Handling Plugins

Plugins that extend ErrorHandler functionality:

```cpp
class IErrorHandlerPlugin : public IPlugin {
public:
    virtual void handleError(const ErrorHandler::Error& error) = 0;
    virtual bool shouldHandleError(const ErrorHandler::Error& error) const = 0;
    virtual std::string getErrorHandlingParameters() const = 0;
    virtual bool setErrorHandlingParameters(const std::string& parameters) = 0;
};
```

#### 5. Neural Math Plugins

Plugins that extend NeuralMath functionality:

```cpp
class INeuralMathPlugin : public IPlugin {
public:
    virtual double sigmoid(double x) const = 0;
    virtual double tanh(double x) const = 0;
    virtual double relu(double x) const = 0;
    virtual double leakyRelu(double x, double alpha) const = 0;
    virtual double softplus(double x) const = 0;
    virtual double swish(double x) const = 0;
    virtual double mish(double x) const = 0;
    virtual double gelu(double x) const = 0;
    virtual double elu(double x, double alpha) const = 0;
    virtual double selu(double x, double alpha) const = 0;
};
```

## Plugin Development Examples

### Example 1: Neural Activity Calculator Plugin

```cpp
#include "core/utilities/NeuralActivityCalculator.hpp"
#include <string>
#include <vector>

class AdvancedNeuralActivityPlugin : public INeuralActivityCalculatorPlugin {
public:
    std::string getName() const override {
        return "AdvancedNeuralActivity";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Advanced neural activity calculator with multiple calculation strategies";
    }
    
    bool initialize() override {
        // Initialize plugin resources
        return true;
    }
    
    bool shutdown() override {
        // Cleanup plugin resources
        return true;
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void setEnabled(bool enabled) override {
        this->enabled = enabled;
    }
    
    NeuralActivityCalculator::Strategy calculateStrategy() const override {
        return NeuralActivityCalculator::Strategy::ADVANCED;
    }
    
    double calculate(const std::vector<double>& voltages) const override {
        if (!enabled) return 0.0;
        
        if (voltages.size() < 2) return 0.0;
        
        // Implement advanced neural activity calculation
        double sum = std::accumulate(voltages.begin(), voltages.end(), 0.0);
        double mean = sum / voltages.size();
        
        // Calculate weighted variance
        double variance = 0.0;
        for (double v : voltages) {
            variance += (v - mean) * (v - mean);
        }
        variance /= voltages.size();
        
        // Apply nonlinear transformation
        double activity = std::sqrt(variance);
        activity = sigmoid(activity);
        
        return activity;
    }
    
    std::string getParameters() const override {
        std::stringstream params;
        params << "sigmoid_alpha:" << sigmoidAlpha
               << ",weight_factor:" << weightFactor
               << ",normalization:" << (normalization ? "true" : "false");
        return params.str();
    }
    
    bool setParameters(const std::string& parameters) override {
        // Parse parameters
        std::istringstream params(parameters);
        std::string key, value;
        
        while (std::getline(params, key, ',')) {
            size_t delimiter = key.find(':');
            if (delimiter != std::string::npos) {
                std::string paramName = key.substr(0, delimiter);
                std::string paramValue = key.substr(delimiter + 1);
                
                if (paramName == "sigmoid_alpha") {
                    sigmoidAlpha = std::stod(paramValue);
                } else if (paramName == "weight_factor") {
                    weightFactor = std::stod(paramValue);
                } else if (paramName == "normalization") {
                    normalization = (paramValue == "true");
                }
            }
        }
        
        return true;
    }
    
    double sigmoid(double x) const {
        return 1.0 / (1.0 + std::exp(-x * sigmoidAlpha));
    }
    
private:
    bool enabled = true;
    double sigmoidAlpha = 1.0;
    double weightFactor = 1.0;
    bool normalization = true;
};
```

### Example 2: Spike Detection Plugin

```cpp
#include "core/utilities/SpikeDetector.hpp"
#include <string>
#include <vector>
#include <deque>

class AdaptiveSpikeDetectorPlugin : public ISpikeDetectorPlugin {
public:
    std::string getName() const override {
        return "AdaptiveSpikeDetector";
    }
    
    std::string getVersion() const override {
        return "2.0.0";
    }
    
    std::string getDescription() const override {
        return "Adaptive spike detector with dynamic threshold adjustment";
    }
    
    bool initialize() override {
        // Initialize spike history and state variables
        spikeHistory.clear();
        lastSpikeTime = 0.0;
        adaptiveThreshold = 20.0;
        thresholdAdaptationRate = 0.01;
        return true;
    }
    
    bool shutdown() override {
        // Cleanup spike history
        spikeHistory.clear();
        return true;
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void setEnabled(bool enabled) override {
        this->enabled = enabled;
    }
    
    bool detectSpike(double voltage, double timestep) override {
        if (!enabled) return false;
        
        double currentTime = getCurrentTime();
        
        // Update adaptive threshold based on recent activity
        updateAdaptiveThreshold(currentTime);
        
        // Detect spike
        bool spikeDetected = (voltage > adaptiveThreshold) && 
                           (currentTime - lastSpikeTime) > refractoryPeriod;
        
        if (spikeDetected) {
            spikeHistory.push_back(currentTime);
            lastSpikeTime = currentTime;
            onSpikeDetected(currentTime);
        }
        
        return spikeDetected;
    }
    
    std::vector<double> getSpikeTimes() const override {
        return spikeHistory;
    }
    
    void reset() override {
        spikeHistory.clear();
        lastSpikeTime = 0.0;
        adaptiveThreshold = 20.0;
    }
    
    std::string getDetectionParameters() const override {
        std::stringstream params;
        params << "base_threshold:" << baseThreshold
               << ",refractory_period:" << refractoryPeriod
               << ",adaptation_rate:" << thresholdAdaptationRate
               << ",min_spike_interval:" << minSpikeInterval;
        return params.str();
    }
    
    bool setDetectionParameters(const std::string& parameters) override {
        // Parse parameters
        std::istringstream params(parameters);
        std::string key, value;
        
        while (std::getline(params, key, ',')) {
            size_t delimiter = key.find(':');
            if (delimiter != std::string::npos) {
                std::string paramName = key.substr(0, delimiter);
                std::string paramValue = key.substr(delimiter + 1);
                
                if (paramName == "base_threshold") {
                    baseThreshold = std::stod(paramValue);
                    adaptiveThreshold = baseThreshold;
                } else if (paramName == "refractory_period") {
                    refractoryPeriod = std::stod(paramValue);
                } else if (paramName == "adaptation_rate") {
                    thresholdAdaptationRate = std::stod(paramValue);
                } else if (paramName == "min_spike_interval") {
                    minSpikeInterval = std::stod(paramValue);
                }
            }
        }
        
        return true;
    }
    
    void updateAdaptiveThreshold(double currentTime) {
        // Adapt threshold based on recent activity
        double activityLevel = calculateRecentActivityLevel();
        
        double thresholdAdjustment = thresholdAdaptationRate * activityLevel;
        adaptiveThreshold += thresholdAdjustment;
        
        // Clamp threshold to reasonable range
        adaptiveThreshold = std::max(adaptiveThreshold, 10.0);
        adaptiveThreshold = std::min(adaptiveThreshold, 50.0);
    }
    
    double calculateRecentActivityLevel() {
        if (spikeHistory.size() < 2) return 0.0;
        
        double totalActivity = 0.0;
        for (size_t i = 1; i < spikeHistory.size(); ++i) {
            double interval = spikeHistory[i] - spikeHistory[i-1];
            if (interval > 0.0) {
                totalActivity += 1.0 / interval;
            }
        }
        
        return totalActivity / (spikeHistory.size() - 1);
    }
    
    void onSpikeDetected(double spikeTime) {
        // Callback when spike is detected
        // This can be overridden by the plugin user
    }
    
    double getCurrentTime() const {
        // Get current simulation time
        return currentSimulationTime;
    }
    
private:
    bool enabled = true;
    double baseThreshold = 20.0;
    double adaptiveThreshold = 20.0;
    double refractoryPeriod = 5.0;
    double thresholdAdaptationRate = 0.01;
    double minSpikeInterval = 1.0;
    std::vector<double> spikeHistory;
    double lastSpikeTime = 0.0;
    double currentSimulationTime = 0.0;
};
```

### Example 3: Episode Management Plugin

```cpp
#include "core/utilities/EpisodeBuilder.hpp"
#include <string>
#include <vector>

class MultiModalEpisodeBuilderPlugin : public IEpisodeBuilderPlugin {
public:
    std::string getName() const override {
        return "MultiModalEpisodeBuilder";
    }
    
    std::string getVersion() const override {
        return "1.5.0";
    }
    
    std::string getDescription() const override {
        return "Multi-modal episode builder supporting neural, reward, and environmental data";
    }
    
    bool initialize() override {
        // Initialize episode components
        enableNeuralFeatures = true;
        enableRewardFeatures = true;
        enableEnvironmentalFeatures = true;
        enableTemporalFeatures = true;
        return true;
    }
    
    bool shutdown() override {
        // Cleanup episode components
        return true;
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void setEnabled(bool enabled) override {
        this->enabled = enabled;
    }
    
    Episode buildEpisode(const std::vector<Event>& events) override {
        Episode episode;
        
        if (!enabled) {
            return episode;
        }
        
        // Extract data from events
        std::vector<double> neuralData;
        std::vector<double> rewardData;
        std::vector<double> environmentalData;
        
        for (const auto& event : events) {
            neuralData.push_back(event.neuralData);
            rewardData.push_back(event.rewardData);
            environmentalData.push_back(event.envData);
        }
        
        // Build neural component
        if (enableNeuralFeatures) {
            NeuralData neuralComp;
            neuralComp.setNeuralsActivity(neuralData);
            neuralComp.setSpikeTimes(detectSpikes(neuralData));
            neuralComp.setPatternSignature(calculatePatternSignature(neuralData));
            episode.setNeuralData(neuralComp);
        }
        
        // Build reward component
        if (enableRewardFeatures) {
            RewardData rewardComp;
            rewardComp.setRewards(rewardData);
            rewardComp.setCumulativeReward(computeCumulativeReward(rewardData));
            rewardComp.setPolicyGradient(calculatePolicyGradient(rewardData));
            episode.setRewardData(rewardComp);
        }
        
        // Build environmental component
        if (enableEnvironmentalFeatures) {
            EnvironmentalData envComp;
            envComp.setEnvironmentalData(environmentalData);
            envComp.setTemperature(environmentalData[0]);
            envComp.setHumidity(environmentalData[1]);
            envComp.setPressure(environmentalData[2]);
            envComp.setLightLevel(environmentalData[3]);
            episode.setEnvironmentalData(envComp);
        }
        
        // Build temporal component
        if (enableTemporalFeatures) {
            TemporalData temporalComp;
            temporalComp.setDuration(calculateDuration(events));
            temporalComp.setStartTime(events.front().timestamp);
            temporalComp.setEndTime(events.back().timestamp);
            temporalComp.setQualityScore(calculateEpisodeQuality(episode));
            episode.setTemporalData(temporalComp);
        }
        
        // Validate episode integrity
        if (!validateEpisodeIntegrity(episode)) {
            throw std::runtime_error("Episode integrity validation failed");
        }
        
        // Store episode
        storeEpisode(episode);
        
        return episode;
    }
    
    Episode buildEpisode(const EpisodeData& data) override {
        Episode episode;
        
        if (!enabled) {
            return episode;
        }
        
        // Build episode from EpisodeData structure
        // This method allows for custom episode construction
        
        // Implementation details...
        
        return episode;
    }
    
    bool validateEpisode(const Episode& episode) const override {
        // Validate episode integrity
        if (enableNeuralFeatures && !episode.hasNeuralData()) {
            return false;
        }
        
        if (enableRewardFeatures && !episode.hasRewardData()) {
            return false;
        }
        
        if (enableEnvironmentalFeatures && !episode.hasEnvironmentalData()) {
            return false;
        }
        
        if (enableTemporalFeatures && !episode.hasTemporalData()) {
            return false;
        }
        
        // Validate data consistency
        if (!validateDataConsistency(episode)) {
            return false;
        }
        
        // Validate episode quality
        if (!validateEpisodeQuality(episode)) {
            return false;
        }
        
        return true;
    }
    
    std::string getBuilderParameters() const override {
        std::stringstream params;
        params << "neural_features:" << (enableNeuralFeatures ? "true" : "false")
               << ",reward_features:" << (enableRewardFeatures ? "true" : "false")
               << ",environmental_features:" << (enableEnvironmentalFeatures ? "true" : "false")
               << ",temporal_features:" << (enableTemporalFeatures ? "true" : "false")
               << ",quality_threshold:" << qualityThreshold;
        return params.str();
    }
    
    bool setBuilderParameters(const std::string& parameters) override {
        // Parse parameters
        std::istringstream params(parameters);
        std::string key, value;
        
        while (std::getline(params, key, ',')) {
            size_t delimiter = key.find(':');
            if (delimiter != std::string::npos) {
                std::string paramName = key.substr(0, delimiter);
                std::string paramValue = key.substr(delimiter + 1);
                
                if (paramName == "neural_features") {
                    enableNeuralFeatures = (paramValue == "true");
                } else if (paramName == "reward_features") {
                    enableRewardFeatures = (paramValue == "true");
                } else if (paramName == "environmental_features") {
                    enableEnvironmentalFeatures = (paramValue == "true");
                } else if (paramName == "temporal_features") {
                    enableTemporalFeatures = (paramValue == "true");
                } else if (paramName == "quality_threshold") {
                    qualityThreshold = std::stod(paramValue);
                }
            }
        }
        
        return true;
    }
    
private:
    bool enabled = true;
    bool enableNeuralFeatures = true;
    bool enableRewardFeatures = true;
    bool enableEnvironmentalFeatures = true;
    bool enableTemporalFeatures = true;
    double qualityThreshold = 0.8;
    
    std::vector<double> detectSpikes(const std::vector<double>& voltages) {
        // Implement spike detection logic
        std::vector<double> spikeTimes;
        for (size_t i = 1; i < voltages.size(); ++i) {
            if (voltages[i] > spikeThreshold && voltages[i-1] < spikeThreshold) {
                spikeTimes.push_back(i * timestep);
            }
        }
        return spikeTimes;
    }
    
    PatternSignature calculatePatternSignature(const std::vector<double>& voltages) {
        PatternSignature signature;
        
        // Calculate pattern characteristics
        double mean = calculateMean(voltages);
        double variance = calculateVariance(voltages, mean);
        double skewness = calculateSkewness(voltages, mean, variance);
        double kurtosis = calculateKurtosis(voltages, mean, variance);
        
        signature.mean = mean;
        signature.variance = variance;
        signature.skewness = skewness;
        signature.kurtosis = kurtosis;
        
        return signature;
    }
    
    double computeCumulativeReward(const std::vector<double>& rewards) {
        double total = 0.0;
        for (double reward : rewards) total += reward;
        return total;
    }
    
    Vector calculatePolicyGradient(const std::vector<double>& rewards) {
        // Calculate policy gradient from rewards
        Vector gradient;
        
        // Simple policy gradient calculation
        for (size_t i = 0; i < rewards.size(); ++i) {
            gradient[i] = rewards[i] - baselineReward;
        }
        
        return gradient;
    }
    
    double calculateDuration(const std::vector<Event>& events) {
        if (events.empty()) return 0.0;
        return events.back().timestamp - events.front().timestamp;
    }
    
    double calculateEpisodeQuality(const Episode& episode) {
        // Calculate episode quality based on multiple factors
        double quality = 0.0;
        
        // Neural data quality
        if (enableNeuralFeatures) {
            quality += episode.getNeuralData().getPatternSignature().similarity;
        }
        
        // Reward data quality
        if (enableRewardFeatures) {
            quality += episode.getRewardData().getPolicyGradient().norm() * 0.3;
        }
        
        // Temporal data quality
        if (enableTemporalFeatures) {
            quality += episode.getTemporalData().getQualityScore() * 0.5;
        }
        
        return std::min(quality, 1.0);
    }
    
    bool validateDataConsistency(const Episode& episode) {
        // Validate data consistency across components
        // Implementation details...
        return true;
    }
    
    bool validateEpisodeQuality(const Episode& episode) {
        // Validate episode quality
        if (enableTemporalFeatures) {
            double quality = episode.getTemporalData().getQualityScore();
            return quality >= qualityThreshold;
        }
        return true;
    }
    
    void storeEpisode(Episode& episode) {
        // Store episode in memory system
        // Implementation details...
    }
    
    static constexpr double spikeThreshold = 20.0;
    static constexpr double timestep = 0.001;
    static constexpr double baselineReward = 0.0;
    
    double calculateMean(const std::vector<double>& values) {
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
    
    double calculateVariance(const std::vector<double>& values, double mean) {
        double sum = 0.0;
        for (double v : values) {
            sum += (v - mean) * (v - mean);
        }
        return sum / values.size();
    }
    
    double calculateSkewness(const std::vector<double>& values, double mean, double variance) {
        double skewness = 0.0;
        for (double v : values) {
            skewness += std::pow(v - mean, 3);
        }
        skewness /= (values.size() * std::pow(variance, 1.5));
        return skewness;
    }
    
    double calculateKurtosis(const std::vector<double>& values, double mean, double variance) {
        double kurtosis = 0.0;
        for (double v : values) {
            kurtosis += std::pow(v - mean, 4);
        }
        kurtosis /= (values.size() * std::pow(variance, 2));
        return kurtosis;
    }
};
```

### Example 4: Error Handling Plugin

```cpp
#include "core/utilities/ErrorHandler.hpp"
#include <string>
#include <vector>

class ErrorRecoveryPlugin : public IErrorHandlerPlugin {
public:
    std::string getName() const override {
        return "ErrorRecovery";
    }
    
    std::string getVersion() const override {
        return "1.2.0";
    }
    
    std::string getDescription() const override {
        return "Error handling and recovery plugin with automatic error mitigation";
    }
    
    bool initialize() override {
        // Initialize error recovery mechanisms
        recoveryStrategies.clear();
        errorCounts.clear();
        recoveryAttempts.clear();
        return true;
    }
    
    bool shutdown() override {
        // Cleanup error recovery mechanisms
        recoveryStrategies.clear();
        return true;
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void setEnabled(bool enabled) override {
        this->enabled = enabled;
    }
    
    void handleError(const ErrorHandler::Error& error) override {
        if (!enabled) return;
        
        // Check if this error should be handled by this plugin
        if (!shouldHandleError(error)) {
            return;
        }
        
        // Get recovery strategy for this error type
        RecoveryStrategy strategy = getRecoveryStrategy(error.category);
        
        // Handle error based on recovery strategy
        handleErrorByStrategy(error, strategy);
    }
    
    bool shouldHandleError(const ErrorHandler::Error& error) const override {
        // Check if this error should be handled by this plugin
        return error.severity >= minSeverityToHandle &&
               error.category == targetCategory;
    }
    
    std::string getErrorHandlingParameters() const override {
        std::stringstream params;
        params << "min_severity:" << severityToString(minSeverityToHandle)
               << ",target_category:" << categoryToString(targetCategory)
               << ",max_recovery_attempts:" << maxRecoveryAttempts
               << ",auto_recover:" << (autoRecover ? "true" : "false");
        return params.str();
    }
    
    bool setErrorHandlingParameters(const std::string& parameters) override {
        // Parse parameters
        std::istringstream params(parameters);
        std::string key, value;
        
        while (std::getline(params, key, ',')) {
            size_t delimiter = key.find(':');
            if (delimiter != std::string::npos) {
                std::string paramName = key.substr(0, delimiter);
                std::string paramValue = key.substr(delimiter + 1);
                
                if (paramName == "min_severity") {
                    minSeverityToHandle = stringToSeverity(paramValue);
                } else if (paramName == "target_category") {
                    targetCategory = stringToCategory(paramValue);
                } else if (paramName == "max_recovery_attempts") {
                    maxRecoveryAttempts = std::stoi(paramValue);
                } else if (paramName == "auto_recover") {
                    autoRecover = (paramValue == "true");
                }
            }
        }
        
        return true;
    }
    
    void addRecoveryStrategy(ErrorHandler::Category category, 
                           RecoveryStrategy strategy) {
        recoveryStrategies[category] = strategy;
    }
    
    RecoveryStrategy getRecoveryStrategy(ErrorHandler::Category category) const {
        auto it = recoveryStrategies.find(category);
        if (it != recoveryStrategies.end()) {
            return it->second;
        }
        return RecoveryStrategy::NONE;
    }
    
    void handleErrorByStrategy(const ErrorHandler::Error& error, RecoveryStrategy strategy) {
        // Handle error based on recovery strategy
        switch (strategy) {
            case RecoveryStrategy::RETRY:
                handleErrorWithRetry(error);
                break;
            case RecoveryStrategy::ROLLBACK:
                handleErrorWithRollback(error);
                break;
            case RecoveryStrategy::FALLBACK:
                handleErrorWithFallback(error);
                break;
            case RecoveryStrategy::IGNORE:
                handleErrorByIgnore(error);
                break;
            case RecoveryStrategy::REPORT:
                handleErrorByReport(error);
                break;
            case RecoveryStrategy::TERMINATE:
                handleErrorByTerminate(error);
                break;
            default:
                handleErrorByDefault(error);
                break;
        }
        
        // Update error statistics
        errorCounts[error.category]++;
        recoveryAttempts[error.category]++;
    }
    
    void handleErrorWithRetry(const ErrorHandler::Error& error) {
        // Handle error with retry mechanism
        auto it = recoveryAttempts.find(error.category);
        if (it != recoveryAttempts.end() && it->second >= maxRecoveryAttempts) {
            // Too many recovery attempts, use fallback
            handleErrorWithFallback(error);
            return;
        }
        
        // Attempt recovery
        if (autoRecover) {
            // Implement recovery logic
            // This is where you would implement specific recovery for different error types
        }
        
        // Log recovery attempt
        // errorHandler.report(ErrorHandler::Severity::INFO, "Recovery attempt for error: " + error.message);
    }
    
    void handleErrorWithRollback(const ErrorHandler::Error& error) {
        // Handle error with rollback mechanism
        // Implement rollback logic for reversible operations
        // This is where you would implement specific rollback for different error types
    }
    
    void handleErrorWithFallback(const ErrorHandler::Error& error) {
        // Handle error with fallback mechanism
        // Implement fallback logic for non-critical errors
        // This is where you would implement specific fallback for different error types
    }
    
    void handleErrorByIgnore(const ErrorHandler::Error& error) {
        // Handle error by ignoring it
        // This is for non-critical errors that can be safely ignored
    }
    
    void handleErrorByReport(const ErrorHandler::Error& error) {
        // Handle error by reporting only
        // This is for errors that require reporting but no recovery
    }
    
    void handleErrorByTerminate(const ErrorHandler::Error& error) {
        // Handle error by terminating the operation
        // This is for critical errors that cannot be recovered from
        // errorHandler.report(ErrorHandler::Severity::CRITICAL, "Terminating operation due to critical error: " + error.message);
        // Implement termination logic
    }
    
    void handleErrorByDefault(const ErrorHandler::Error& error) {
        // Handle error by default mechanism
        // This is the default error handling behavior
    }
    
    std::string severityToString(ErrorHandler::Severity severity) const {
        switch (severity) {
            case ErrorHandler::Severity::DEBUG: return "DEBUG";
            case ErrorHandler::Severity::INFO: return "INFO";
            case ErrorHandler::Severity::WARNING: return "WARNING";
            case ErrorHandler::Severity::ERROR: return "ERROR";
            case ErrorHandler::Severity::CRITICAL: return "CRITICAL";
            case ErrorHandler::Severity::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    ErrorHandler::Severity stringToSeverity(const std::string& severityStr) const {
        if (severityStr == "DEBUG") return ErrorHandler::Severity::DEBUG;
        if (severityStr == "INFO") return ErrorHandler::Severity::INFO;
        if (severityStr == "WARNING") return ErrorHandler::Severity::WARNING;
        if (severityStr == "ERROR") return ErrorHandler::Severity::ERROR;
        if (severityStr == "CRITICAL") return ErrorHandler::Severity::CRITICAL;
        if (severityStr == "FATAL") return ErrorHandler::Severity::FATAL;
        return ErrorHandler::Severity::ERROR;
    }
    
    std::string categoryToString(ErrorHandler::Category category) const {
        switch (category) {
            case ErrorHandler::Category::CONFIG: return "CONFIG";
            case ErrorHandler::Category::MEMORY: return "MEMORY";
            case ErrorHandler::Category::NETWORK: return "NETWORK";
            case ErrorHandler::Category::FILE: return "FILE";
            case ErrorHandler::Category::NEURAL: return "NEURAL";
            case ErrorHandler::Category::PLASTICITY: return "PLASTICITY";
            case ErrorHandler::Category::PREDICTION: return "PREDICTION";
            case ErrorHandler::Category::MEMORY_SYSTEM: return "MEMORY_SYSTEM";
            case ErrorHandler::Category::NEUROMODULATION: return "NEUROMODULATION";
            case ErrorHandler::Category::DEVELOPMENT: return "DEVELOPMENT";
            case ErrorHandler::Category::ENVIRONMENT: return "ENVIRONMENT";
            case ErrorHandler::Category::SYSTEM: return "SYSTEM";
            case ErrorHandler::Category::UNKNOWN: return "UNKNOWN";
            default: return "UNKNOWN";
        }
    }
    
    ErrorHandler::Category stringToCategory(const std::string& categoryStr) const {
        if (categoryStr == "CONFIG") return ErrorHandler::Category::CONFIG;
        if (categoryStr == "MEMORY") return ErrorHandler::Category::MEMORY;
        if (categoryStr == "NETWORK") return ErrorHandler::Category::NETWORK;
        if (categoryStr == "FILE") return ErrorHandler::Category::FILE;
        if (categoryStr == "NEURAL") return ErrorHandler::Category::NEURAL;
        if (categoryStr == "PLASTICITY") return ErrorHandler::Category::PLASTICITY;
        if (categoryStr == "PREDICTION") return ErrorHandler::Category::PREDICTION;
        if (categoryStr == "MEMORY_SYSTEM") return ErrorHandler::Category::MEMORY_SYSTEM;
        if (categoryStr == "NEUROMODULATION") return ErrorHandler::Category::NEUROMODULATION;
        if (categoryStr == "DEVELOPMENT") return ErrorHandler::Category::DEVELOPMENT;
        if (categoryStr == "ENVIRONMENT") return ErrorHandler::Category::ENVIRONMENT;
        if (categoryStr == "SYSTEM") return ErrorHandler::Category::SYSTEM;
        return ErrorHandler::Category::UNKNOWN;
    }
    
private:
    enum class RecoveryStrategy {
        NONE,
        RETRY,
        ROLLBACK,
        FALLBACK,
        IGNORE,
        REPORT,
        TERMINATE
    };
    
    bool enabled = true;
    ErrorHandler::Severity minSeverityToHandle = ErrorHandler::Severity::ERROR;
    ErrorHandler::Category targetCategory = ErrorHandler::Category::UNKNOWN;
    int maxRecoveryAttempts = 3;
    bool autoRecover = true;
    
    std::map<ErrorHandler::Category, RecoveryStrategy> recoveryStrategies;
    std::map<ErrorHandler::Category, size_t> errorCounts;
    std::map<ErrorHandler::Category, size_t> recoveryAttempts;
};
```

### Example 5: Neural Math Plugin

```cpp
#include "core/utilities/NeuralMath.hpp"
#include <string>
#include <vector>

class AdvancedNeuralMathPlugin : public INeuralMathPlugin {
public:
    std::string getName() const override {
        return "AdvancedNeuralMath";
    }
    
    std::string getVersion() const override {
        return "1.3.0";
    }
    
    std::string getDescription() const override {
        return "Advanced neural math plugin with custom activation functions and optimizations";
    }
    
    bool initialize() override {
        // Initialize plugin resources
        customFunctions.clear();
        return true;
    }
    
    bool shutdown() override {
        // Cleanup plugin resources
        customFunctions.clear();
        return true;
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void setEnabled(bool enabled) override {
        this->enabled = enabled;
    }
    
    // INeuralMathPlugin interface
    double sigmoid(double x) const override {
        if (!enabled) return 0.5;
        
        // Use custom sigmoid implementation
        return 1.0 / (1.0 + std::exp(-x * customSigmoidAlpha));
    }
    
    double tanh(double x) const override {
        if (!enabled) return 0.0;
        
        // Use custom tanh implementation
        return std::tanh(x * customTanhBeta);
    }
    
    double relu(double x) const override {
        if (!enabled) return std::max(0.0, x);
        
        // Use custom ReLU implementation
        double result = std::max(0.0, x * customReluSlope);
        if (customReluNegativeSlope != 0.0 && x < 0.0) {
            result = customReluNegativeSlope * x;
        }
        return result;
    }
    
    double leakyRelu(double x, double alpha) const override {
        if (!enabled) return std::max(x, alpha * x);
        
        // Use custom leaky ReLU implementation
        double result = std::max(x, alpha * x * customReluAlpha);
        return result;
    }
    
    double softplus(double x) const override {
        if (!enabled) return std::log(1.0 + std::exp(x));
        
        // Use custom softplus implementation
        double result = (1.0 / customSoftplusBeta) * std::log(1.0 + std::exp(customSoftplusBeta * x));
        return result;
    }
    
    double swish(double x) const override {
        if (!enabled) return x / (1.0 + std::exp(-x));
        
        // Use custom Swish implementation
        double sigmoid = 1.0 / (1.0 + std::exp(-x * customSwishBeta));
        double result = x * sigmoid;
        return result;
    }
    
    double mish(double x) const override {
        if (!enabled) return x * std::tanh(std::log(1.0 + std::exp(x)));
        
        // Use custom Mish implementation
        double tanh = std::tanh(std::log(1.0 + std::exp(x * customMishBeta)));
        double result = x * tanh;
        return result;
    }
    
    double gelu(double x) const override {
        if (!enabled) return x * 0.5 * (1.0 + std::tanh(std::sqrt(2.0 / M_PI) * (x + 0.044715 * std::pow(x, 3))));
        
        // Use custom GELU implementation
        double erf = calculateErf(customGeluBeta * x);
        double result = x * 0.5 * (1.0 + erf);
        return result;
    }
    
    double elu(double x, double alpha) const override {
        if (!enabled) return (x > 0.0) ? x : alpha * (std::exp(x) - 1.0);
        
        // Use custom ELU implementation
        double result = (x > 0.0) ? x : alpha * (std::exp(x * customEluAlpha) - 1.0);
        return result;
    }
    
    double selu(double x, double alpha) const override {
        if (!enabled) {
            double scale = 1.64872127070012814684256376947308;
            return scale * ((x > 0.0) ? x : alpha * (std::exp(x) - 1.0));
        }
        
        // Use custom SELU implementation
        double scale = customSELUAlpha * 1.64872127070012814684256376947308;
        double result = scale * ((x > 0.0) ? x : alpha * (std::exp(x * customSELUBeta) - 1.0));
        return result;
    }
    
    std::string getMathParameters() const override {
        std::stringstream params;
        params << "sigmoid_alpha:" << customSigmoidAlpha
               << ",tanh_beta:" << customTanhBeta
               << ",relu_slope:" << customReluSlope
               << ",relu_negative_slope:" << customReluNegativeSlope
               << ",relu_alpha:" << customReluAlpha
               << ",softplus_beta:" << customSoftplusBeta
               << ",swish_beta:" << customSwishBeta
               << ",mish_beta:" << customMishBeta
               << ",gelu_beta:" << customGeluBeta
               << ",elu_alpha:" << customEluAlpha
               << ",selu_alpha:" << customSELUBeta
               << ",selu_beta:" << customSELUScale;
        return params.str();
    }
    
    bool setMathParameters(const std::string& parameters) override {
        // Parse parameters
        std::istringstream params(parameters);
        std::string key, value;
        
        while (std::getline(params, key, ',')) {
            size_t delimiter = key.find(':');
            if (delimiter != std::string::npos) {
                std::string paramName = key.substr(0, delimiter);
                std::string paramValue = key.substr(delimiter + 1);
                
                if (paramName == "sigmoid_alpha") {
                    customSigmoidAlpha = std::stod(paramValue);
                } else if (paramName == "tanh_beta") {
                    customTanhBeta = std::stod(paramValue);
                } else if (paramName == "relu_slope") {
                    customReluSlope = std::stod(paramValue);
                } else if (paramName == "relu_negative_slope") {
                    customReluNegativeSlope = std::stod(paramValue);
                } else if (paramName == "relu_alpha") {
                    customReluAlpha = std::stod(paramValue);
                } else if (paramName == "softplus_beta") {
                    customSoftplusBeta = std::stod(paramValue);
                } else if (paramName == "swish_beta") {
                    customSwishBeta = std::stod(paramValue);
                } else if (paramName == "mish_beta") {
                    customMishBeta = std::stod(paramValue);
                } else if (paramName == "gelu_beta") {
                    customGeluBeta = std::stod(paramValue);
                } else if (paramName == "elu_alpha") {
                    customEluAlpha = std::stod(paramValue);
                } else if (paramName == "selu_alpha") {
                    customSELUBeta = std::stod(paramValue);
                } else if (paramName == "selu_beta") {
                    customSELUScale = std::stod(paramValue);
                }
            }
        }
        
        return true;
    }
    
    void addCustomFunction(const std::string& name, std::function<double(double)> func) {
        customFunctions[name] = func;
    }
    
    double calculateErf(double x) const {
        // Calculate error function using approximation
        // This is a simple approximation, more sophisticated implementations exist
        double t = 1.0 / (1.0 + 0.3275911 * std::abs(x));
        double y = 1.0 - t * std::exp(-x * x - 1.11220406 * t);
        return (x >= 0.0) ? y : -y;
    }
    
private:
    bool enabled = true;
    double customSigmoidAlpha = 1.0;
    double customTanhBeta = 1.0;
    double customReluSlope = 1.0;
    double customReluNegativeSlope = 0.0;
    double customReluAlpha = 1.0;
    double customSoftplusBeta = 1.0;
    double customSwishBeta = 1.0;
    double customMishBeta = 1.0;
    double customGeluBeta = 1.0;
    double customEluAlpha = 1.0;
    double customSELUBeta = 1.6;
    double customSELUScale = 1.0507009873554802930193261179362;
    
    std::map<std::string, std::function<double(double)>> customFunctions;
};
```

## Plugin System Architecture

### Plugin Loading and Management

The NLM plugin system uses a plugin manager to load and manage plugins:

```cpp
class PluginManager {
public:
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    bool isPluginLoaded(const std::string& pluginName) const;
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getEnabledPlugins() const;
    IPlugin* getPlugin(const std::string& pluginName);
    bool initializeAllPlugins();
    bool shutdownAllPlugins();
};
```

### Plugin Configuration

Plugins can be configured using the NLM configuration system:

```json
{
    "plugins": {
        "enabled": ["AdvancedNeuralActivity", "AdaptiveSpikeDetector", "MultiModalEpisodeBuilder"],
        "disabled": ["DefaultErrorHandler"],
        "plugin_directories": ["plugins/", "extra_plugins/"],
        "auto_load": true,
        "validate_plugins": true,
        "plugin_cache_enabled": true,
        "plugin_cache_file": "plugin_cache.json"
    }
}
```

## Plugin Development Best Practices

### 1. Plugin Design

- **Single Responsibility**: Each plugin should have a single, well-defined responsibility
- **Extensibility**: Design plugins to be easily extended and modified
- **Reusability**: Make plugins reusable across different components
- **Testability**: Ensure plugins are easily testable

### 2. Plugin Implementation

- **Thread Safety**: Ensure thread-safe plugin operation
- **Error Handling**: Implement robust error handling
- **Resource Management**: Properly manage plugin resources
- **Memory Management**: Avoid memory leaks

### 3. Plugin Testing

- **Unit Testing**: Test individual plugin functionality
- **Integration Testing**: Test plugin integration with NLM components
- **Performance Testing**: Test plugin performance impact
- **Compatibility Testing**: Test plugin compatibility with different NLM versions

### 4. Plugin Documentation

- **API Documentation**: Document plugin public interfaces
- **Usage Examples**: Provide usage examples for common plugin scenarios
- **Configuration Examples**: Provide configuration examples for plugins
- **Migration Guide**: Provide migration guide for plugin users

## Plugin Examples

### Complete Plugin Example

```cpp
// Complete example plugin implementation
#include "core/utilities/NeuralActivityCalculator.hpp"
#include "core/utilities/ErrorHandler.hpp"
#include <string>
#include <vector>
#include <memory>

class ExamplePlugin : public INeuralActivityCalculatorPlugin {
public:
    std::string getName() const override {
        return "ExamplePlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Example plugin demonstrating NLM plugin development";
    }
    
    bool initialize() override {
        // Initialize plugin resources
        return true;
    }
    
    bool shutdown() override {
        // Cleanup plugin resources
        return true;
    }
    
    bool isEnabled() const override {
        return enabled;
    }
    
    void setEnabled(bool enabled) override {
        this->enabled = enabled;
    }
    
    // INeuralActivityCalculatorPlugin interface
    NeuralActivityCalculator::Strategy calculateStrategy() const override {
        return NeuralActivityCalculator::Strategy::ADVANCED;
    }
    
    double calculate(const std::vector<double>& voltages) const override {
        if (!enabled) return 0.0;
        
        // Implement custom neural activity calculation
        if (voltages.size() < 2) return 0.0;
        
        // Calculate mean
        double sum = std::accumulate(voltages.begin(), voltages.end(), 0.0);
        double mean = sum / voltages.size();
        
        // Calculate variance
        double variance = 0.0;
        for (double v : voltages) {
            variance += (v - mean) * (v - mean);
        }
        variance /= voltages.size();
        
        // Calculate activity
        double activity = std::sqrt(variance);
        
        // Apply custom transformation
        activity = sigmoid(activity, sigmoidAlpha);
        activity *= weightFactor;
        
        return activity;
    }
    
    std::string getParameters() const override {
        std::stringstream params;
        params << "sigmoid_alpha:" << sigmoidAlpha
               << ",weight_factor:" << weightFactor
               << ",normalization:" << (normalization ? "true" : "false");
        return params.str();
    }
    
    bool setParameters(const std::string& parameters) override {
        // Parse parameters
        std::istringstream params(parameters);
        std::string key, value;
        
        while (std::getline(params, key, ',')) {
            size_t delimiter = key.find(':');
            if (delimiter != std::string::npos) {
                std::string paramName = key.substr(0, delimiter);
                std::string paramValue = key.substr(delimiter + 1);
                
                if (paramName == "sigmoid_alpha") {
                    sigmoidAlpha = std::stod(paramValue);
                } else if (paramName == "weight_factor") {
                    weightFactor = std::stod(paramValue);
                } else if (paramName == "normalization") {
                    normalization = (paramValue == "true");
                }
            }
        }
        
        return true;
    }
    
    double sigmoid(double x, double alpha) const {
        return 1.0 / (1.0 + std::exp(-alpha * x));
    }
    
    // Additional methods for plugin functionality
    void onNeuralActivityCalculated(double activity) {
        // Callback when neural activity is calculated
        // This can be used for logging, monitoring, or further processing
    }
    
    bool validateNeuralActivity(double activity) const {
        // Validate neural activity
        return !std::isnan(activity) && !std::isinf(activity) &&
               activity >= 0.0 && activity <= 1.0;
    }
    
    std::vector<double> batchCalculate(const std::vector<std::vector<double>>& voltageBatches) {
        std::vector<double> activities;
        
        for (const auto& voltages : voltageBatches) {
            double activity = calculate(voltages);
            activities.push_back(activity);
        }
        
        return activities;
    }
    
    double getAverageActivity(const std::vector<double>& activities) const {
        if (activities.empty()) return 0.0;
        
        double sum = std::accumulate(activities.begin(), activities.end(), 0.0);
        return sum / activities.size();
    }
    
    double getActivityStandardDeviation(const std::vector<double>& activities) const {
        if (activities.empty()) return 0.0;
        
        double mean = getAverageActivity(activities);
        double sum = 0.0;
        for (double activity : activities) {
            sum += (activity - mean) * (activity - mean);
        }
        
        return std::sqrt(sum / activities.size());
    }
    
    // Plugin metadata
    static std::string getPluginVersion() {
        return "1.0.0";
    }
    
    static std::string getPluginDescription() {
        return "Example plugin demonstrating NLM plugin development";
    }
    
    static std::vector<std::string> getPluginDependencies() {
        return {"CoreNeuralSystem", "BasicErrorHandler"};
    }
    
    static std::map<std::string, std::string> getPluginConfiguration() {
        return {
            {"sigmoid_alpha", "1.0"},
            {"weight_factor", "1.0"},
            {"normalization", "true"}
        };
    }
    
    static bool validatePluginConfiguration(const std::map<std::string, std::string>& config) {
        // Validate plugin configuration
        auto sigmoidAlphaIt = config.find("sigmoid_alpha");
        if (sigmoidAlphaIt != config.end()) {
            double alpha = std::stod(sigmoidAlphaIt->second);
            if (alpha < 0.0 || alpha > 10.0) {
                return false;
            }
        }
        
        auto weightFactorIt = config.find("weight_factor");
        if (weightFactorIt != config.end()) {
            double weight = std::stod(weightFactorIt->second);
            if (weight < 0.0 || weight > 10.0) {
                return false;
            }
        }
        
        auto normalizationIt = config.find("normalization");
        if (normalizationIt != config.end()) {
            std::string norm = normalizationIt->second;
            if (norm != "true" && norm != "false") {
                return false;
            }
        }
        
        return true;
    }
    
private:
    bool enabled = true;
    double sigmoidAlpha = 1.0;
    double weightFactor = 1.0;
    bool normalization = true;
};
```

## Plugin Development Tools

### Plugin Development Kit

The NLM plugin development kit includes:

1. **Plugin Template**: Template for creating new plugins
2. **Plugin Validator**: Tool for validating plugin configuration
3. **Plugin Debugger**: Tool for debugging plugins
4. **Plugin Profiler**: Tool for profiling plugin performance
5. **Plugin Documentation Generator**: Tool for generating plugin documentation

### Plugin Development Workflow

```
Plugin Development Workflow:

1. Plugin Design:
   - Define plugin requirements and functionality
   - Design plugin architecture
   - Plan plugin implementation

2. Plugin Implementation:
   - Implement plugin base classes
   - Implement plugin functionality
   - Add error handling and recovery
   - Implement plugin configuration

3. Plugin Testing:
   - Unit test plugin functionality
   - Integration test plugin with NLM components
   - Performance test plugin
   - Compatibility test plugin

4. Plugin Documentation:
   - Document plugin API
   - Document plugin usage
   - Document plugin configuration
   - Document plugin migration

5. Plugin Validation:
   - Validate plugin configuration
   - Validate plugin dependencies
   - Validate plugin compatibility
   - Validate plugin performance

6. Plugin Packaging:
   - Package plugin for distribution
   - Create plugin documentation
   - Create plugin examples
   - Create plugin installation instructions

7. Plugin Deployment:
   - Deploy plugin to production
   - Monitor plugin performance
   - Maintain plugin
   - Update plugin
```

## Plugin Development Resources

### Plugin Development Resources

1. **Plugin Documentation**: Comprehensive documentation for plugin development
2. **Plugin Examples**: Examples of plugin implementation
3. **Plugin Templates**: Templates for creating new plugins
4. **Plugin Testing Framework**: Framework for testing plugins
5. **Plugin Debugging Tools**: Tools for debugging plugins
6. **Plugin Performance Monitoring**: Tools for monitoring plugin performance
7. **Plugin Compatibility Matrix**: Compatibility information for plugins
8. **Plugin Version Control**: Version control for plugins
9. **Plugin Security Guidelines**: Security guidelines for plugin development
10. **Plugin Best Practices**: Best practices for plugin development

### Plugin Development Community

1. **Plugin Development Forum**: Forum for plugin developers
2. **Plugin Development Mailing List**: Mailing list for plugin developers
3. **Plugin Development Chat**: Chat room for plugin developers
4. **Plugin Development Slack**: Slack channel for plugin developers
5. **Plugin Development Discord**: Discord server for plugin developers

## Conclusion

The NLM plugin system provides a powerful and flexible framework for plugin development. By following the guidelines and best practices in this guide, you can create high-quality plugins that enhance the functionality of the NLM system.

Key aspects of the plugin system:

1. **Modular Architecture**: Plugins are designed to be modular and extensible
2. **Interface Standardization**: Plugins implement standard interfaces for consistency
3. **Plugin Management**: The plugin manager handles plugin loading, unloading, and management
4. **Plugin Configuration**: Plugins can be configured using the NLM configuration system
5. **Plugin Testing**: Plugins can be easily tested using the NLM testing framework
6. **Plugin Documentation**: Plugins are well-documented with comprehensive documentation
7. **Plugin Examples**: Examples of plugin implementation are provided
8. **Plugin Development Tools**: Tools are provided for plugin development
9. **Plugin Development Workflow**: A clear workflow for plugin development is provided
10. **Plugin Development Resources**: Resources are provided for plugin development

For more detailed information, refer to the other documentation files, particularly the usage guide, advanced usage guide, and plugin development guide.