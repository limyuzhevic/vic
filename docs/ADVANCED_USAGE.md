# Advanced NLM Usage

## Overview
This guide covers advanced usage of the enhanced NLM system, including complex neural activity manipulation, plugin development, performance analysis, and integration techniques.

## Advanced Neural Activity Manipulation

### Custom Neural Activity Strategies

The NeuralActivityCalculator supports custom strategies beyond the built-in ones. Here's how to implement and use custom strategies:

```cpp
#include "core/utilities/NeuralActivityCalculator.hpp"

class CustomActivityStrategy : public NeuralActivityCalculator::Strategy {
public:
    double calculate(const std::vector<double>& voltages) const override {
        if (voltages.empty()) return 0.0;
        
        // Implement custom activity calculation
        double sum = std::accumulate(voltages.begin(), voltages.end(), 0.0);
        double mean = sum / voltages.size();
        
        // Calculate modified variance
        double variance = 0.0;
        for (double v : voltages) {
            variance += (v - mean) * (v - mean) * (v - mean);
        }
        variance /= voltages.size();
        
        return std::sqrt(variance);
    }
};

// Use custom strategy
NeuralActivityCalculator calculator;
calculator.setStrategy(std::make_unique<CustomActivityStrategy>());
```

### Multi-Region Activity Analysis

```cpp
#include "core/utilities/NeuralActivityCalculator.hpp"
#include "brain/Brain.hpp"

void analyzeMultiRegionActivity(Brain& brain) {
    // Create activity calculators for each region
    std::map<std::string, NeuralActivityCalculator> regionCalculators;
    
    const auto& regions = brain.getRegions();
    for (const auto& region : regions) {
        std::string regionName = region.getName();
        
        // Configure calculator for this region
        regionCalculators[regionName] = NeuralActivityCalculator(
            NeuralActivityCalculator::Strategy::WEIGHTED
        );
        regionCalculators[regionName].setWeightFactor(1.0 / region.getNeuronCount());
    }
    
    // Analyze activity across all regions
    while (simulationRunning) {
        for (const auto& region : regions) {
            std::string regionName = region.getName();
            
            // Get region voltages
            std::vector<double> voltages = region.getNeuronVoltages();
            
            // Calculate activity
            double activity = regionCalculators[regionName].calculate(voltages);
            
            // Update region metrics
            region.setCurrentActivity(activity);
            
            // Log if activity exceeds threshold
            if (activity > region.getActivityThreshold()) {
                logHighActivity(regionName, activity);
            }
        }
    }
}
```

### Adaptive Neural Activity Calculation

```cpp
#include "core/utilities/NeuralActivityCalculator.hpp"

class AdaptiveActivityCalculator : public NeuralActivityCalculator {
public:
    AdaptiveActivityCalculator() : NeuralActivityCalculator(Strategy::DEVIATION) {
        adaptationRate = 0.01;
        maxThreshold = 1.0;
        minThreshold = 0.01;
    }
    
    void adaptThreshold(double targetActivity, double currentActivity) {
        // Adapt threshold based on target vs current activity
        double error = targetActivity - currentActivity;
        double adjustment = adaptationRate * error;
        
        currentThreshold += adjustment;
        currentThreshold = std::clamp(currentThreshold, minThreshold, maxThreshold);
        
        setThreshold(currentThreshold);
    }
    
    double calculate(const std::vector<double>& voltages) override {
        double activity = NeuralActivityCalculator::calculate(voltages);
        
        // Adapt threshold based on activity level
        if (adaptationEnabled) {
            adaptThreshold(targetActivity, activity);
        }
        
        return activity;
    }
    
private:
    double adaptationRate;
    double maxThreshold;
    double minThreshold;
    double currentThreshold;
    double targetActivity;
    bool adaptationEnabled;
};
```

## Advanced Spike Detection

### Multi-Threshold Spike Detection

```cpp
#include "core/utilities/SpikeDetector.hpp"

class MultiThresholdDetector : public SpikeDetector {
public:
    MultiThresholdDetector() {
        addThreshold(20.0, "primary");      // Primary threshold
        addThreshold(25.0, "secondary");    // Secondary threshold
        addThreshold(30.0, "tertiary");     // Tertiary threshold
    }
    
    bool detectSpike(double voltage) override {
        bool primarySpike = SpikeDetector::detectSpike(voltage);
        if (primarySpike) {
            return true;
        }
        
        // Check secondary thresholds
        for (const auto& threshold : secondaryThresholds) {
            if (voltage > threshold) {
                onSecondarySpike(threshold);
            }
        }
        
        return false;
    }
    
    void addThreshold(double threshold, const std::string& type) {
        if (type == "primary") {
            primaryThreshold = threshold;
        } else if (type == "secondary") {
            secondaryThresholds.push_back(threshold);
        } else if (type == "tertiary") {
            tertiaryThreshold = threshold;
        }
    }
    
private:
    std::vector<double> secondaryThresholds;
};
```

### Adaptive Refractory Period

```cpp
#include "core/utilities/SpikeDetector.hpp"

class AdaptiveRefractoryDetector : public SpikeDetector {
public:
    AdaptiveRefractoryDetector() : SpikeDetector() {
        baseRefractoryPeriod = 5.0;
        adaptationFactor = 0.1;
        spikeHistory.clear();
    }
    
    void setRefractoryPeriod(double period) override {
        // Store current spike times for adaptation
        double minISI = calculateMinISI(spikeHistory);
        
        if (minISI < baseRefractoryPeriod * 0.5) {
            // Adjust refractory period based on minimum ISI
            double adaptivePeriod = baseRefractoryPeriod * (1.0 + adaptationFactor * (1.0 - minISI / baseRefractoryPeriod));
            SpikeDetector::setRefractoryPeriod(adaptivePeriod);
        } else {
            SpikeDetector::setRefractoryPeriod(period);
        }
    }
    
    bool detectSpike(double voltage) override {
        bool spike = SpikeDetector::detectSpike(voltage);
        if (spike) {
            spikeHistory.push_back(getCurrentTime());
            maintainSpikeHistory();
        }
        return spike;
    }
    
private:
    double baseRefractoryPeriod;
    double adaptationFactor;
    std::vector<double> spikeHistory;
    
    double calculateMinISI(const std::vector<double>& spikeTimes) {
        if (spikeTimes.size() < 2) return baseRefractoryPeriod;
        
        double minISI = spikeTimes[1] - spikeTimes[0];
        for (size_t i = 2; i < spikeTimes.size(); ++i) {
            double isi = spikeTimes[i] - spikeTimes[i-1];
            if (isi < minISI) minISI = isi;
        }
        
        return minISI;
    }
    
    void maintainSpikeHistory() {
        double currentTime = getCurrentTime();
        while (!spikeHistory.empty() && (currentTime - spikeHistory.front()) > 1000.0) {
            spikeHistory.erase(spikeHistory.begin());
        }
    }
};
```

### Spike Pattern Recognition

```cpp
#include "core/utilities/SpikeDetector.hpp"

class SpikePatternDetector : public SpikeDetector {
public:
    SpikePatternDetector() {
        burstThreshold = 50.0;  // Hz
        silenceThreshold = 10.0;  // Hz
        currentPattern = "baseline";
    }
    
    void analyzeSpikePattern(const std::vector<double>& spikeTimes) {
        if (spikeTimes.size() < 2) {
            currentPattern = "baseline";
            return;
        }
        
        // Calculate instantaneous frequency
        double totalDuration = spikeTimes.back() - spikeTimes.front();
        double instantaneousFreq = (spikeTimes.size() - 1) / totalDuration * 1000.0;  // Hz
        
        // Classify pattern
        if (instantaneousFreq > burstThreshold) {
            currentPattern = "burst";
            onBurstDetected();
        } else if (instantaneousFreq < silenceThreshold) {
            currentPattern = "silence";
            onSilenceDetected();
        } else {
            currentPattern = "regular";
        }
        
        // Log pattern change
        if (currentPattern != previousPattern) {
            onPatternChange(previousPattern, currentPattern);
            previousPattern = currentPattern;
        }
    }
    
    std::string getCurrentPattern() const { return currentPattern; }
    
private:
    double burstThreshold;
    double silenceThreshold;
    std::string currentPattern;
    std::string previousPattern;
};
```

## Advanced Episode Management

### Custom Episode Construction

```cpp
#include "core/utilities/EpisodeBuilder.hpp"
#include "memory/Memory.hpp"

class CustomEpisodeBuilder : public EpisodeBuilder {
public:
    CustomEpisodeBuilder(Memory& memory) : EpisodeBuilder(memory) {
        enableNeuralFeatures = true;
        enableRewardFeatures = true;
        enableEnvironmentalFeatures = true;
        enableTemporalFeatures = true;
    }
    
    Episode buildContinuousEpisode(const std::vector<double>& neuralData,
                                   const std::vector<double>& rewardData,
                                   const std::vector<double>& environmentalData,
                                   double duration) {
        Episode episode;
        
        // Build neural component
        if (enableNeuralFeatures) {
            NeuralData neuralComp;
            neuralComp.setNeuralActivity(neuralData);
            neuralComp.setSpikeTimes(detectSpikes(neuralData));
            episode.setNeuralData(neuralComp);
        }
        
        // Build reward component
        if (enableRewardFeatures) {
            RewardData rewardComp;
            rewardComp.setRewards(rewardData);
            rewardComp.setCumulativeReward(computeCumulativeReward(rewardData));
            episode.setRewardData(rewardComp);
        }
        
        // Build environmental component
        if (enableEnvironmentalFeatures) {
            EnvironmentalData envComp;
            envComp.setEnvironmentalData(environmentalData);
            envComp.setTemperature(environmentalData[0]);
            envComp.setHumidity(environmentalData[1]);
            episode.setEnvironmentalData(envComp);
        }
        
        // Build temporal component
        if (enableTemporalFeatures) {
            TemporalData temporalComp;
            temporalComp.setDuration(duration);
            temporalComp.setStartTime(getCurrentTime());
            temporalComp.setEndTime(getCurrentTime() + duration);
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
    
    Episode buildEventDrivenEpisode(const std::vector<Event>& events) {
        Episode episode;
        
        // Sort events by timestamp
        std::vector<Event> sortedEvents = events;
        std::sort(sortedEvents.begin(), sortedEvents.end(),
                 [](const Event& a, const Event& b) {
                     return a.timestamp < b.timestamp;
                 });
        
        // Extract data from events
        std::vector<double> neuralData;
        std::vector<double> rewardData;
        std::vector<double> environmentalData;
        
        for (const auto& event : sortedEvents) {
            neuralData.push_back(event.neuralData);
            rewardData.push_back(event.rewardData);
            environmentalData.push_back(event.envData);
        }
        
        // Build episode using continuous builder
        return buildContinuousEpisode(neuralData, rewardData, environmentalData, getEpisodeDuration(sortedEvents));
    }
    
private:
    bool enableNeuralFeatures;
    bool enableRewardFeatures;
    bool enableEnvironmentalFeatures;
    bool enableTemporalFeatures;
    
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
    
    double computeCumulativeReward(const std::vector<double>& rewards) {
        double total = 0.0;
        for (double reward : rewards) total += reward;
        return total;
    }
    
    double getEpisodeDuration(const std::vector<Event>& events) {
        if (events.empty()) return 0.0;
        return events.back().timestamp - events.front().timestamp;
    }
    
    bool validateEpisodeIntegrity(const Episode& episode) {
        // Check episode components
        if (enableNeuralFeatures && !episode.hasNeuralData()) return false;
        if (enableRewardFeatures && !episode.hasRewardData()) return false;
        if (enableEnvironmentalFeatures && !episode.hasEnvironmentalData()) return false;
        if (enableTemporalFeatures && !episode.hasTemporalData()) return false;
        
        return true;
    }
    
    void storeEpisode(Episode& episode) {
        // Store episode in memory system
        memory.storeEpisode(episode);
    }
    
    static constexpr double spikeThreshold = 20.0;
    static constexpr double timestep = 0.001;
};
```

### Episode Replay and Consolidation

```cpp
#include "core/utilities/EpisodeBuilder.hpp"
#include "memory/Memory.hpp"

class AdvancedEpisodeReplay {
public:
    AdvancedEpisodeReplay(Memory& memory) : memory(memory) {
        consolidationThreshold = 0.8;  // 80% confidence threshold
        replayWindow = 3600.0;         // 1 hour replay window
        maxReplaySpeed = 10.0;        // 10x speed replay
    }
    
    void replayAndConsolidate() {
        // Get episodes for replay
        auto episodes = memory.getRecentEpisodes(replayWindow);
        
        // Sort by importance
        std::sort(episodes.begin(), episodes.end(),
                 [](const Episode& a, const Episode& b) {
                     return a.getImportance() > b.getImportance();
                 });
        
        // Replay episodes
        for (const auto& episode : episodes) {
            replayEpisode(episode);
            
            // Consolidate if importance is high
            if (episode.getImportance() > consolidationThreshold) {
                consolidateEpisode(episode);
            }
        }
    }
    
    void replayEpisode(const Episode& episode, double speedFactor = 1.0) {
        // Ensure replay speed is within limits
        speedFactor = std::clamp(speedFactor, 1.0, maxReplaySpeed);
        
        // Replay episode data
        replayNeuralData(episode.getNeuralData(), speedFactor);
        replayRewardData(episode.getRewardData(), speedFactor);
        replayEnvironmentalData(episode.getEnvironmentalData(), speedFactor);
        
        // Trigger callbacks
        onEpisodeReplay(episode);
    }
    
    void consolidateEpisode(const Episode& episode) {
        // Create consolidated version
        Episode consolidated = extractKeyFeatures(episode);
        
        // Enhance with replay experience
        enhanceWithReplayExperience(consolidated, episode);
        
        // Store consolidated episode
        memory.storeConsolidatedEpisode(consolidated);
        
        // Update statistics
        updateConsolidationStats(episode, consolidated);
    }
    
    void setReplayParameters(double window, double maxSpeed, double threshold) {
        replayWindow = window;
        maxReplaySpeed = maxSpeed;
        consolidationThreshold = threshold;
    }
    
private:
    Memory& memory;
    double replayWindow;
    double maxReplaySpeed;
    double consolidationThreshold;
    
    void replayNeuralData(const NeuralData& neuralData, double speed) {
        // Replay neural data at specified speed
        auto voltages = neuralData.getNeuralsActivity();
        auto spikeTimes = neuralData.getSpikeTimes();
        
        for (size_t i = 0; i < voltages.size(); ++i) {
            double voltage = voltages[i];
            double time = i * timestep * speed;
            
            // Process voltage
            processReplayVoltage(voltage, time);
            
            // Check for spikes
            if (i < spikeTimes.size()) {
                double spikeTime = spikeTimes[i] * speed;
                onReplaySpike(spikeTime);
            }
            
            // Simulate time passage
            simulateTimeStep(timestep / speed);
        }
    }
    
    void replayRewardData(const RewardData& rewardData, double speed) {
        // Replay reward data
        auto rewards = rewardData.getRewards();
        for (double reward : rewards) {
            processReplayReward(reward);
        }
    }
    
    void replayEnvironmentalData(const EnvironmentalData& envData, double speed) {
        // Replay environmental data
        auto envVars = envData.getEnvironmentalVariables();
        for (const auto& var : envVars) {
            processReplayEnvironmental(var.first, var.second);
        }
    }
    
    Episode extractKeyFeatures(const Episode& episode) {
        Episode features;
        
        // Extract neural features
        NeuralData neuralFeatures;
        neuralFeatures.setNeuralsActivity(episode.getNeuralData().getNeuralsActivity());
        neuralFeatures.setPatternSignature(calculatePatternSignature(episode.getNeuralData()));
        features.setNeuralData(neuralFeatures);
        
        // Extract reward features
        RewardData rewardFeatures;
        rewardFeatures.setRewards(episode.getRewardData().getRewards());
        rewardFeatures.setPolicyGradient(calculatePolicyGradient(episode.getRewardData()));
        features.setRewardData(rewardFeatures);
        
        // Extract environmental features
        EnvironmentalData envFeatures;
        envFeatures.setEnvironmentalData(episode.getEnvironmentalData().getEnvironmentalVariables());
        features.setEnvironmentalData(envFeatures);
        
        // Set temporal features
        TemporalData temporalFeatures;
        temporalFeatures.setDuration(episode.getTemporalData().getDuration());
        temporalFeatures.setStartTime(episode.getTemporalData().getStartTime());
        features.setTemporalData(temporalFeatures);
        
        return features;
    }
    
    void enhanceWithReplayExperience(Episode& consolidated, const Episode& original) {
        // Enhance consolidated episode with replay experience
        auto neuralActivity = consolidated.getNeuralData().getNeuralsActivity();
        
        // Apply replay-specific enhancements
        for (double& voltage : neuralActivity) {
            voltage = applyReplayEnhancement(voltage);
        }
        
        // Update importance based on replay value
        double replayValue = calculateReplayValue(consolidated);
        consolidated.setImportance(replayValue);
    }
    
    double applyReplayEnhancement(double voltage) {
        // Apply enhancement based on replay patterns
        return voltage * 1.1 + 0.05 * sin(voltage * 10.0);
    }
    
    double calculateReplayValue(const Episode& episode) {
        double value = 0.0;
        
        // Value based on neural patterns
        value += episode.getNeuralData().getPatternSignature().similarity;
        
        // Value based on reward patterns
        value += episode.getRewardData().getPolicyGradient().norm() * 0.5;
        
        return value;
    }
    
    void updateConsolidationStats(const Episode& original, const Episode& consolidated) {
        // Update consolidation statistics
        consolidationCount++;
        totalOriginalSize += sizeof(original);
        totalConsolidatedSize += sizeof(consolidated);
    }
    
    static constexpr double timestep = 0.001;
};