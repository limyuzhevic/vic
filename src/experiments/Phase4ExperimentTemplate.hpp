#ifndef NLM_PHASE4_EXPERIMENT_TEMPLATE_H
#define NLM_PHASE4_EXPERIMENT_TEMPLATE_H

#include "ExperimentBase.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Base class for all Phase 4 experiments
class Phase4ExperimentBase : public BaseExperiment {
public:
    Phase4ExperimentBase(const std::string& name, std::shared_ptr<Logger> logger = nullptr)
        : BaseExperiment(name, logger) {}
    
    virtual ~Phase4ExperimentBase() = default;
    
    // Run the experiment with a specific brain and number of trials
    Phase4Results run(Brain* brain, size_t numTrials) {
        Phase4Results results;
        history_.clear();
        
        if (!brain) {
            logProgress("ERROR: Brain is null, cannot run experiment");
            return results;
        }
        
        // Setup the specific system for this experiment
        setupSystem(brain);
        
        // Execute trials
        executeTrials(results, numTrials);
        
        // Finalize results
        finalizeResults(results);
        
        // Save results if configured
        if (logEnabled_) {
            saveResults("phase4_experiment_results.txt");
        }
        
        return results;
    }
    
    // Override if specific cleanup needed
    virtual void cleanup() override {
        logProgress("Cleaning up phase 4 experiment");
        history_.clear();
        BaseExperiment::cleanup();
    }
    
protected:
    virtual void setupSystem(Brain* brain) = 0;
    virtual void executeTrials(Phase4Results& results, size_t numTrials) = 0;
    virtual void finalizeResults(Phase4Results& results) = 0;
    
    // Common helper functions
    template<typename TSystem>
    void initializeAndRunSystem(TSystem& system, Brain* brain, size_t numTrials,
                                Phase4Results& results, std::vector<float>& history) {
        system.initialize(brain);
        
        for (size_t trial = 0; trial < numTrials; ++trial) {
            processTrial(system, trial, results, history);
        }
    }
    
    template<typename TSystem>
    void processTrial(TSystem& system, size_t trial, Phase4Results& results, std::vector<float>& history) {
        // Common trial processing - override in derived classes if needed
        // This is a template method that can be customized
        history.push_back(system.getCurrentValue());
    }
    
    // Common trial loop with metrics collection
    template<typename TSystem>
    void runTrialsWithMetrics(TSystem& system, Brain* brain, size_t numTrials,
                             Phase4Results& results, std::vector<float>& history) {
        for (size_t trial = 0; trial < numTrials; ++trial) {
            // Run system-specific trial logic
            system.runTrial(trial);
            
            // Collect system-specific metrics
            collectSystemMetrics(system, results, trial);
            
            // Common metrics collection
            if (trial < numTrials / 4) {
                results.earlyPerformance += system.getTrialMetric("performance");
            }
            if (trial >= numTrials * 3 / 4) {
                results.latePerformance += system.getTrialMetric("performance");
            }
            
            history.push_back(system.getTrialMetric("error"));
            
            // Log progress every 10% of trials
            if (trial % (numTrials / 10) == 0) {
                logProgress("Trial %zu/%zu completed, error: %.4f",
                           trial, numTrials, system.getTrialMetric("error"));
            }
        }
    }
    
    virtual void collectSystemMetrics(TSystem& system, Phase4Results& results, size_t trial) = 0;
    
    // Common result calculation
    void calculateCommonResults(Phase4Results& results, const std::vector<float>& history) {
        if (!history.empty()) {
            float sum = std::accumulate(history.begin(), history.end(), 0.0f);
            results.predictionErrorInitial = history.front();
            results.predictionErrorFinal = history.back();
            results.predictionAccuracyImprovement = (results.predictionErrorInitial - results.predictionErrorFinal);
            
            float minError = *std::min_element(history.begin(), history.end());
            float maxError = *std::max_element(history.begin(), history.end());
            results.errorRange = maxError - minError;
        }
        
        results.workingMemoryRetentionInitial = results.earlyPerformance / (history.size() / 4);
        results.workingMemoryRetentionFinal = results.latePerformance / (history.size() / 4);
    }
    
    // Access to history
    void addToHistory(float value) { history_.push_back(value); }
    const std::vector<float>& getHistory() const { return history_; }
    
private:
    std::vector<float> history_;
};

// Helper function to create experiment with specific system
template<typename TSystem>
Phase4Results runPhase4Experiment(Brain* brain, size_t numTrials) {
    // This would create an experiment with the specific system type
    // In practice, this would be implemented in each concrete experiment
    // For now, return empty results
    Phase4Results results;
    return results;
}

} // namespace nlm

#endif // NLM_PHASE4_EXPERIMENT_TEMPLATE_H
