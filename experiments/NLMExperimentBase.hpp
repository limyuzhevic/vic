// NLM Base Experiment Class
// Common functionality for all NLM experiments across phases
// Provides reusable experiment infrastructure and shared functionality

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Experiment result base class
struct BaseExperimentResult {
    std::string experimentName;
    size_t numTrials;
    size_t numSteps;
    
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    
    size_t totalSpikes;
    size_t maxFiringNeuronCount;
    
    time_t startTime;
    time_t endTime;
    double wallClockTime;
    
    BaseExperimentResult() : numTrials(0), numSteps(0),
                             totalReward(0.0f), avgFiringRate(0.0f), avgSynapticWeight(0.0f),
                             totalSpikes(0), maxFiringNeuronCount(0),
                             startTime(0), endTime(0), wallClockTime(0.0) {}
    
    virtual ~BaseExperimentResult() {}
    
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "Experiment: " << experimentName << std::endl;
        oss << "  Trials: " << numTrials << ", Steps: " << numSteps << std::endl;
        oss << "  Total Reward: " << totalReward << std::endl;
        oss << "  Avg Firing Rate: " << avgFiringRate << std::endl;
        oss << "  Wall Clock Time: " << wallClockTime << "s" << std::endl;
        return oss.str();
    }
    
    virtual std::string toCSV() const {
        std::ostringstream oss;
        oss << experimentName << ","
            << numTrials << ","
            << numSteps << ","
            << totalReward << ","
            << avgFiringRate << ","
            << totalSpikes << ","
            << maxFiringNeuronCount << ","
            << wallClockTime;
        return oss.str();
    }
};

// Base experiment class
class BaseExperiment {
public:
    BaseExperiment(const std::string& name) : name_(name) {
        // Initialize logger for experiment
        logger_ = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger_->addLogger(consoleLogger);
        Logger::setGlobal(logger_);
        
        LOG_INFO("Initializing experiment: " + name);
    }
    
    virtual ~BaseExperiment() {
        LOG_INFO("Experiment completed: " + name_);
    }
    
    // Run experiment with specified brain and configuration
    BaseExperimentResult run(Brain* brain, size_t numTrials = 100, size_t stepsPerTrial = 200) {
        // Validate inputs
        if (!brain) {
            throw std::invalid_argument("Brain pointer cannot be null");
        }
        if (numTrials == 0) {
            LOG_WARNING("Number of trials is zero, using default 1");
            numTrials = 1;
        }
        if (stepsPerTrial == 0) {
            LOG_WARNING("Steps per trial is zero, using default 1");
            stepsPerTrial = 1;
        }
        
        // Record start time
        startTime_ = std::chrono::system_clock::now();
        
        // Run experiments
        result_ = onRun(brain, numTrials, stepsPerTrial);
        
        // Record end time
        endTime_ = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime_ - startTime_);
        result_.wallClockTime = duration.count();
        
        return result_;
    }
    
    // Get experiment name
    const std::string& getName() const { return name_; }
    
    // Get experiment result
    const BaseExperimentResult& getResult() const { return result_; }
    
    // Get logger
    std::shared_ptr<Logger> getLogger() const { return logger_; }
    
    // Reset experiment state
    virtual void reset() {
        result_ = BaseExperimentResult();
        result_.experimentName = name_;
    }
    
protected:
    virtual BaseExperimentResult onRun(Brain* brain, size_t numTrials, size_t stepsPerTrial) = 0;
    
    // Helper methods for common experiment operations
    void recordTrialStart() {
        trialStartTime_ = std::chrono::system_clock::now();
    }
    
    double recordTrialEnd() {
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - trialStartTime_);
        return duration.count();
    }
    
    // Configuration helper methods
    template<typename T>
    T getConfig(const std::shared_ptr<Config>& config, const std::string& key, const T& defaultValue) const {
        auto val = config->get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    // Statistical calculation helpers
    static float calculateMean(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    static float calculateStdDev(const std::vector<float>& values, float mean) {
        if (values.size() <= 1) return 0.0f;
        float sum = 0.0f;
        for (float val : values) {
            sum += (val - mean) * (val - mean);
        }
        return std::sqrt(sum / (values.size() - 1));
    }
    
    // String conversion helpers
    static std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Logging helpers
    void logTrial(int trial, int total, const std::string& message = "") {
        std::ostringstream oss;
        oss << "Trial " << trial << "/" << total << ": " << message;
        LOG_INFO(oss.str());
    }
    
    void logProgress(int step, int total, const std::string& message = "") {
        std::ostringstream oss;
        oss << "Step " << step << "/" << total;
        if (!message.empty()) oss << " - " << message;
        if (step % 100 == 0 || step == total) {
            LOG_INFO(oss.str());
        }
    }

private:
    std::string name_;
    std::shared_ptr<Logger> logger_;
    BaseExperimentResult result_;
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point trialStartTime_;
    std::chrono::system_clock::time_point endTime_;
};

// Specialized experiment result for Phase 4
struct Phase4ExperimentResult : public BaseExperimentResult {
    // Prediction metrics
    float predictionErrorInitial;
    float predictionErrorFinal;
    float predictionAccuracyImprovement;
    
    // Memory metrics
    float workingMemoryRetentionInitial;
    float workingMemoryRetentionFinal;
    float workingMemoryCapacity;
    
    // Episodic memory metrics
    float episodicRecallAccuracy;
    size_t episodesStored;
    float episodicInfluence;
    
    // Concept formation metrics
    size_t conceptsFormed;
    float conceptStability;
    float generalizationAbility;
    
    // Planning metrics
    float planningAccuracy;
    float planningConfidence;
    
    Phase4ExperimentResult() : BaseExperimentResult(),
                               predictionErrorInitial(0.0f), predictionErrorFinal(0.0f),
                               predictionAccuracyImprovement(0.0f),
                               workingMemoryRetentionInitial(0.0f), workingMemoryRetentionFinal(0.0f),
                               workingMemoryCapacity(0.0f),
                               episodicRecallAccuracy(0.0f), episodesStored(0),
                               episodicInfluence(0.0f),
                               conceptsFormed(0), conceptStability(0.0f),
                               generalizationAbility(0.0f),
                               planningAccuracy(0.0f), planningConfidence(0.0f) {}
    
    std::string toString() const override {
        std::ostringstream oss;
        oss << BaseExperimentResult::toString();
        oss << "  Prediction Error Initial: " << predictionErrorInitial << std::endl;
        oss << "  Prediction Error Final: " << predictionErrorFinal << std::endl;
        oss << "  Concepts Formed: " << conceptsFormed << std::endl;
        return oss.str();
    }
    
    std::string toCSV() const override {
        std::ostringstream oss;
        oss << BaseExperimentResult::toCSV() << ","
            << predictionErrorInitial << ","
            << predictionErrorFinal << ","
            << conceptsFormed << ","
            << planningAccuracy;
        return oss.str();
    }
};

// Phase 4 experiment base class
class Phase4ExperimentBase : public BaseExperiment {
public:
    Phase4ExperimentBase(const std::string& name) : BaseExperiment(name) {}
    
    virtual Phase4ExperimentResult run(Brain* brain, size_t numTrials = 50) = 0;
    
protected:
    virtual Phase4ExperimentResult onRun(Brain* brain, size_t numTrials, size_t stepsPerTrial) override {
        return run(brain, numTrials);
    }
};

// Generic experiment runner for common experiment patterns
class ExperimentRunner {
public:
    // Run a simple stimulus-response experiment
    static BaseExperimentResult runStimulusResponse(Brain* brain, size_t numTrials, 
                                                     const std::function<void(Brain*, size_t)>& stimulus,
                                                     const std::function<float(Brain*, size_t)>& response) {
        BaseExperimentResult result;
        result.experimentName = "Stimulus-Response Experiment";
        result.numTrials = numTrials;
        result.numSteps = numTrials * 10; // Assuming 10 steps per trial
        
        for (size_t trial = 0; trial < numTrials; ++trial) {
            stimulus(brain, trial);
            
            // Run simulation steps
            for (size_t step = 0; step < 10; ++step) {
                brain->step(trial * 10 + step, trial * 10 * 0.001);
                
                float resp = response(brain, trial);
                result.totalReward += resp;
                result.totalSpikes += brain->getTotalSpikeCount();
            }
            
            result.maxFiringNeuronCount = std::max(result.maxFiringNeuronCount, brain->getFiringNeuronCount());
        }
        
        // Calculate averages
        if (numTrials > 0) {
            result.totalReward /= numTrials;
            result.totalSpikes /= numTrials;
        }
        
        return result;
    }
    
    // Run a learning experiment with initial and final measurements
    static BaseExperimentResult runLearningExperiment(Brain* brain, size_t numTrials,
                                                        const std::function<void(Brain*)>& prepare,
                                                        const std::function<void(Brain*, size_t)>& step,
                                                        const std::function<float(Brain*)>& measure) {
        BaseExperimentResult result;
        result.experimentName = "Learning Experiment";
        result.numTrials = numTrials;
        result.numSteps = numTrials;
        
        // Prepare initial state
        prepare(brain);
        float initialValue = measure(brain);
        
        // Run learning trials
        for (size_t trial = 0; trial < numTrials; ++trial) {
            step(brain, trial);
            float currentValue = measure(brain);
            result.totalReward += currentValue;
        }
        
        // Calculate final state
        float finalValue = measure(brain);
        result.totalReward = finalValue - initialValue;
        
        return result;
    }
    
    // Run a temporal sequence experiment
    static BaseExperimentResult runTemporalExperiment(Brain* brain, size_t numTrials,
                                                       const std::vector<size_t>& sequence) {
        BaseExperimentResult result;
        result.experimentName = "Temporal Sequence Experiment";
        result.numTrials = numTrials;
        result.numSteps = numTrials * sequence.size();
        
        for (size_t trial = 0; trial < numTrials; ++trial) {
            for (size_t step : sequence) {
                brain->step(trial * sequence.size() + step, step * 0.001);
                
                // Record spike statistics
                result.totalSpikes += brain->getTotalSpikeCount();
                result.maxFiringNeuronCount = std::max(result.maxFiringNeuronCount, brain->getFiringNeuronCount());
            }
        }
        
        // Calculate averages
        size_t totalEvents = numTrials * sequence.size();
        if (totalEvents > 0) {
            result.totalSpikes /= totalEvents;
        }
        
        return result;
    }
};

} // namespace nlm