#include "NeuromodulationMonitor.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>

namespace nlm {

class NeuromodulationMonitor::Impl {
public:
    Impl() : isMonitoring_(false) {
        startTime_ = std::chrono::high_resolution_clock::now();
        initializeDefaultState();
    }
    
    ~Impl() = default;
    
    void startMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!isMonitoring_) {
            isMonitoring_ = true;
            startTime_ = std::chrono::high_resolution_clock::now();
            signalHistory_.clear();
            initializeDefaultState();
        }
    }
    
    void stopMonitoring() {
        std::lock_guard<std::mutex> lock(mutex_);
        isMonitoring_ = false;
    }
    
    bool isMonitoring() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isMonitoring_;
    }
    
    void recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp, Timestamp duration, const std::string& source) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        NeuromodulatorSignal signal;
        signal.name = name;
        signal.level = level;
        signal.timestamp = timestamp;
        signal.duration = duration;
        signal.source = source;
        
        // Calculate effect based on signal level and type
        signal.effect = calculateSignalEffect(name, level, duration);
        
        signalHistory_[name].push_back(signal);
        updateStateFromSignal(signal);
    }
    
    void recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp) {
        recordNeuromodulatorSignal(name, level, timestamp, 1.0, "unknown");
    }
    
    void recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp, Timestamp duration) {
        recordNeuromodulatorSignal(name, level, timestamp, duration, "unknown");
    }
    
    void updateState(const NeuromodulationState& state) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentState_ = state;
        currentState_.lastUpdate = getCurrentTimestamp();
    }
    
    NeuromodulationState getCurrentState() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_;
    }
    
    double getDopamineLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_.dopamine;
    }
    
    double getCuriosityLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_.curiosity;
    }
    
    double getNoveltyLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_.novelty;
    }
    
    double getPredictionError() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_.predictionError;
    }
    
    double getAcetylcholineLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_.acetylcholine;
    }
    
    double getNorepinephrineLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_.norepinephrine;
    }
    
    double getSerotoninLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentState_.serotonin;
    }
    
    std::vector<NeuromodulatorSignal> getSignalHistory(const std::string& name, size_t limit) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<NeuromodulatorSignal> result;
        
        auto it = signalHistory_.find(name);
        if (it != signalHistory_.end()) {
            size_t startIndex = std::max(0ULL, static_cast<unsigned long long>(it->second.size() - limit));
            for (size_t i = startIndex; i < it->second.size(); ++i) {
                result.push_back(it->second[i]);
            }
        }
        
        return result;
    }
    
    std::vector<NeuromodulatorSignal> getSignalHistoryByTimeRange(const std::string& name, 
                                                                   Timestamp startTime, 
                                                                   Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<NeuromodulatorSignal> result;
        
        auto it = signalHistory_.find(name);
        if (it != signalHistory_.end()) {
            for (const auto& signal : it->second) {
                if (signal.timestamp >= startTime && signal.timestamp <= endTime) {
                    result.push_back(signal);
                }
            }
        }
        
        return result;
    }
    
    double analyzeSignalPatterns(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = signalHistory_.find(name);
        if (it == signalHistory_.end() || it->second.empty()) {
            return 0.0;
        }
        
        // Calculate pattern score based on signal consistency
        double totalLevel = 0.0;
        double levelVariance = 0.0;
        
        for (const auto& signal : it->second) {
            totalLevel += signal.level;
        }
        
        double meanLevel = totalLevel / it->second.size();
        
        for (const auto& signal : it->second) {
            levelVariance += std::pow(signal.level - meanLevel, 2);
        }
        
        double variance = levelVariance / it->second.size();
        double stdDev = std::sqrt(variance);
        
        // Pattern quality is inverse of coefficient of variation
        double coefficientOfVariation = (meanLevel > 0) ? (stdDev / meanLevel) : 0.0;
        return std::max(0.0, 1.0 - coefficientOfVariation);
    }
    
    double calculateSignalMean(const std::string& name, Timestamp startTime, Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto signals = getSignalHistoryByTimeRange(name, startTime, endTime);
        if (signals.empty()) {
            return 0.0;
        }
        
        double totalLevel = 0.0;
        for (const auto& signal : signals) {
            totalLevel += signal.level;
        }
        
        return totalLevel / signals.size();
    }
    
    double calculateSignalVariance(const std::string& name, Timestamp startTime, Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto signals = getSignalHistoryByTimeRange(name, startTime, endTime);
        if (signals.size() < 2) {
            return 0.0;
        }
        
        double meanLevel = calculateSignalMean(name, startTime, endTime);
        double variance = 0.0;
        
        for (const auto& signal : signals) {
            variance += std::pow(signal.level - meanLevel, 2);
        }
        
        return variance / signals.size();
    }
    
    double calculateSignalPeak(const std::string& name, Timestamp startTime, Timestamp endTime) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto signals = getSignalHistoryByTimeRange(name, startTime, endTime);
        if (signals.empty()) {
            return 0.0;
        }
        
        double peakLevel = 0.0;
        for (const auto& signal : signals) {
            peakLevel = std::max(peakLevel, signal.level);
        }
        
        return peakLevel;
    }
    
    double analyzeSignalEffect(const std::string& name, const std::string& target) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Simplified effect analysis
        // In a real implementation, this would analyze the actual effect on neural targets
        double totalEffect = 0.0;
        int signalCount = 0;
        
        auto it = signalHistory_.find(name);
        if (it != signalHistory_.end()) {
            for (const auto& signal : it->second) {
                // Placeholder effect calculation
                double effect = signal.level * calculateTargetModulation(target);
                totalEffect += effect;
                signalCount++;
            }
        }
        
        return signalCount > 0 ? totalEffect / signalCount : 0.0;
    }
    
    std::map<std::string, double> analyzeAllEffects() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, double> results;
        
        // Analyze effects on different neural targets
        std::vector<std::string> targets = {"excitability", "inhibition", "plasticity", 
                                           "learning", "attention", "memory"};
        
        for (const auto& target : targets) {
            results[target] = analyzeSignalEffect("dopamine", target);
            results[target + "_acetylcholine"] = analyzeSignalEffect("acetylcholine", target);
            results[target + "_norepinephrine"] = analyzeSignalEffect("norepinephrine", target);
        }
        
        return results;
    }
    
    double getSignalToNoiseRatio(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = signalHistory_.find(name);
        if (it == signalHistory_.end() || it->second.size() < 2) {
            return 0.0;
        }
        
        double meanLevel = calculateSignalMean(name, 0.0, getCurrentTimestamp());
        double variance = calculateSignalVariance(name, 0.0, getCurrentTimestamp());
        
        if (variance <= 0.0) return 0.0;
        
        double stdDev = std::sqrt(variance);
        return (meanLevel > 0.0) ? (meanLevel / stdDev) : 0.0;
    }
    
    double getSignalTimingConsistency(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = signalHistory_.find(name);
        if (it == signalHistory_.end() || it->second.size() < 2) {
            return 0.0;
        }
        
        // Calculate timing consistency (lower variance in signal timing)
        double totalTime = 0.0;
        double timeVariance = 0.0;
        
        for (size_t i = 1; i < it->second.size(); ++i) {
            double timeDiff = it->second[i].timestamp - it->second[i-1].timestamp;
            totalTime += timeDiff;
            timeVariance += std::pow(timeDiff - (totalTime / i), 2);
        }
        
        double variance = timeVariance / std::max(1.0, it->second.size() - 1);
        double stdDev = std::sqrt(variance);
        
        // Consistency is inverse of timing variance
        return std::max(0.0, 1.0 - (stdDev * 10.0)); // Scale factor
    }
    
    double getSignalDuration(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = signalHistory_.find(name);
        if (it == signalHistory_.end() || it->second.empty()) {
            return 0.0;
        }
        
        double totalDuration = 0.0;
        for (const auto& signal : it->second) {
            totalDuration += signal.duration;
        }
        
        return totalDuration / it->second.size();
    }
    
    bool isSignalValid(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = signalHistory_.find(name);
        if (it == signalHistory_.end() || it->second.empty()) {
            return false;
        }
        
        // Check if all signals are within valid range
        for (const auto& signal : it->second) {
            if (!isSignalWithinRange(name, signal.level)) {
                return false;
            }
        }
        
        return true;
    }
    
    std::vector<std::string> getValidSignals() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> validSignals;
        
        for (const auto& pair : signalHistory_) {
            if (isSignalValid(pair.first)) {
                validSignals.push_back(pair.first);
            }
        }
        
        return validSignals;
    }
    
    bool isSignalWithinRange(const std::string& name, double level) const {
        // Simplified validation - in real implementation would be more sophisticated
        return -1.0 <= level && level <= 1.0;
    }
    
    void clearSignalHistory() {
        std::lock_guard<std::mutex> lock(mutex_);
        signalHistory_.clear();
        initializeDefaultState();
    }
    
    void setSignalRetentionTime(Timestamp retentionTime) {
        std::lock_guard<std::mutex> lock(mutex_);
        signalRetentionTime_ = retentionTime;
        cleanOldSignals();
    }
    
private:
    Timestamp getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        return duration;
    }
    
    void initializeDefaultState() {
        currentState_.dopamine = 0.0;
        currentState_.acetylcholine = 0.0;
        currentState_.norepinephrine = 0.0;
        currentState_.serotonin = 0.0;
        currentState_.curiosity = 0.0;
        currentState_.novelty = 0.0;
        currentState_.predictionError = 0.0;
        currentState_.lastUpdate = getCurrentTimestamp();
    }
    
    double calculateSignalEffect(const std::string& name, double level, double duration) const {
        // Simplified effect calculation
        double baseEffect = level * duration;
        
        if (name == "dopamine") {
            return baseEffect * 0.8; // Strong effect on learning and motivation
        } else if (name == "acetylcholine") {
            return baseEffect * 0.7; // Effect on attention and memory
        } else if (name == "norepinephrine") {
            return baseEffect * 0.6; // Effect on arousal and vigilance
        } else if (name == "serotonin") {
            return baseEffect * 0.5; // Effect on mood and impulsivity
        } else if (name == "curiosity") {
            return baseEffect * 0.9; // Strong effect on exploration
        } else if (name == "novelty") {
            return baseEffect * 0.85; // Effect on novelty detection
        } else {
            return baseEffect * 0.5; // Default effect
        }
    }
    
    double calculateTargetModulation(const std::string& target) const {
        // Simplified target modulation calculation
        if (target == "excitability") return 0.8;
        else if (target == "inhibition") return 0.6;
        else if (target == "plasticity") return 0.9;
        else if (target == "learning") return 0.85;
        else if (target == "attention") return 0.7;
        else if (target == "memory") return 0.75;
        else return 0.5;
    }
    
    void updateStateFromSignal(const NeuromodulatorSignal& signal) {
        // Update current state based on the signal
        if (signal.name == "dopamine") {
            currentState_.dopamine = signal.effect;
        } else if (signal.name == "acetylcholine") {
            currentState_.acetylcholine = signal.effect;
        } else if (signal.name == "norepinephrine") {
            currentState_.norepinephrine = signal.effect;
        } else if (signal.name == "serotonin") {
            currentState_.serotonin = signal.effect;
        } else if (signal.name == "curiosity") {
            currentState_.curiosity = signal.effect;
        } else if (signal.name == "novelty") {
            currentState_.novelty = signal.effect;
        } else if (signal.name == "prediction_error") {
            currentState_.predictionError = signal.effect;
        }
        
        currentState_.lastUpdate = signal.timestamp;
    }
    
    void cleanOldSignals() {
        double currentTime = getCurrentTimestamp();
        
        for (auto& pair : signalHistory_) {
            auto& signals = pair.second;
            auto it = std::remove_if(signals.begin(), signals.end(),
                                     [this, currentTime](const NeuromodulatorSignal& signal) {
                                         return (currentTime - signal.timestamp) > signalRetentionTime_;
                                     });
            signals.erase(it, signals.end());
        }
    }
    
    mutable std::mutex mutex_;
    bool isMonitoring_;
    std::chrono::high_resolution_clock::time_point startTime_;
    Timestamp signalRetentionTime_ = 3600.0; // 1 hour default
    
    NeuromodulationState currentState_;
    
    std::map<std::string, std::vector<NeuromodulatorSignal>> signalHistory_;
};

NeuromodulationMonitor::NeuromodulationMonitor() : pImpl(new Impl()) {}

NeuromodulationMonitor::~NeuromodulationMonitor() {
    delete pImpl;
}

void NeuromodulationMonitor::startMonitoring() {
    pImpl->startMonitoring();
}

void NeuromodulationMonitor::stopMonitoring() {
    pImpl->stopMonitoring();
}

bool NeuromodulationMonitor::isMonitoring() const {
    return pImpl->isMonitoring();
}

void NeuromodulationMonitor::recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp, Timestamp duration, const std::string& source) {
    pImpl->recordNeuromodulatorSignal(name, level, timestamp, duration, source);
}

void NeuromodulationMonitor::recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp) {
    pImpl->recordNeuromodulatorSignal(name, level, timestamp);
}

void NeuromodulationMonitor::recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp, Timestamp duration) {
    pImpl->recordNeuromodulatorSignal(name, level, timestamp, duration);
}

void NeuromodulationMonitor::updateState(const NeuromodulationState& state) {
    pImpl->updateState(state);
}

NeuromodulationState NeuromodulationMonitor::getCurrentState() const {
    return pImpl->getCurrentState();
}

double NeuromodulationMonitor::getDopamineLevel() const {
    return pImpl->getDopamineLevel();
}

double NeuromodulationMonitor::getCuriosityLevel() const {
    return pImpl->getCuriosityLevel();
}

double NeuromodulationMonitor::getNoveltyLevel() const {
    return pImpl->getNoveltyLevel();
}

double NeuromodulationMonitor::getPredictionError() const {
    return pImpl->getPredictionError();
}

double NeuromodulationMonitor::getAcetylcholineLevel() const {
    return pImpl->getAcetylcholineLevel();
}

double NeuromodulationMonitor::getNorepinephrineLevel() const {
    return pImpl->getNorepinephrineLevel();
}

double NeuromodulationMonitor::getSerotoninLevel() const {
    return pImpl->getSerotoninLevel();
}

std::vector<NeuromodulatorSignal> NeuromodulationMonitor::getSignalHistory(const std::string& name, size_t limit) const {
    return pImpl->getSignalHistory(name, limit);
}

std::vector<NeuromodulatorSignal> NeuromodulationMonitor::getSignalHistoryByTimeRange(const std::string& name, 
                                                                                     Timestamp startTime, 
                                                                                     Timestamp endTime) const {
    return pImpl->getSignalHistoryByTimeRange(name, startTime, endTime);
}

double NeuromodulationMonitor::analyzeSignalPatterns(const std::string& name) const {
    return pImpl->analyzeSignalPatterns(name);
}

double NeuromodulationMonitor::calculateSignalMean(const std::string& name, Timestamp startTime, Timestamp endTime) const {
    return pImpl->calculateSignalMean(name, startTime, endTime);
}

double NeuromodulationMonitor::calculateSignalVariance(const std::string& name, Timestamp startTime, Timestamp endTime) const {
    return pImpl->calculateSignalVariance(name, startTime, endTime);
}

double NeuromodulationMonitor::calculateSignalPeak(const std::string& name, Timestamp startTime, Timestamp endTime) const {
    return pImpl->calculateSignalPeak(name, startTime, endTime);
}

double NeuromodulationMonitor::analyzeSignalEffect(const std::string& name, const std::string& target) const {
    return pImpl->analyzeSignalEffect(name, target);
}

std::map<std::string, double> NeuromodulationMonitor::analyzeAllEffects() const {
    return pImpl->analyzeAllEffects();
}

double NeuromodulationMonitor::getSignalToNoiseRatio(const std::string& name) const {
    return pImpl->getSignalToNoiseRatio(name);
}

double NeuromodulationMonitor::getSignalTimingConsistency(const std::string& name) const {
    return pImpl->getSignalTimingConsistency(name);
}

double NeuromodulationMonitor::getSignalDuration(const std::string& name) const {
    return pImpl->getSignalDuration(name);
}

bool NeuromodulationMonitor::isSignalValid(const std::string& name) const {
    return pImpl->isSignalValid(name);
}

std::vector<std::string> NeuromodulationMonitor::getValidSignals() const {
    return pImpl->getValidSignals();
}

bool NeuromodulationMonitor::isSignalWithinRange(const std::string& name, double level) const {
    return pImpl->isSignalWithinRange(name, level);
}

void NeuromodulationMonitor::clearSignalHistory() {
    pImpl->clearSignalHistory();
}

void NeuromodulationMonitor::setSignalRetentionTime(Timestamp retentionTime) {
    pImpl->setSignalRetentionTime(retentionTime);
}

} // namespace nlm