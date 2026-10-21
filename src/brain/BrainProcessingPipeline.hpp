// Processing pipeline for brain simulation
// This class implements the command pattern for brain processing steps
// Each step is a separate method that can be executed independently

#pragma once

#include "Brain.hpp"
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace nlm {

// Processing step result
struct ProcessingStepResult {
    std::string stepName;
    std::chrono::microseconds duration;
    bool success;
    std::string message;
};

// Processing step function type
using ProcessingStepFunction = std::function<ProcessingStepResult(SimulationStep, Timestamp, Brain*)>;

class BrainProcessingPipeline {
public:
    BrainProcessingPipeline() {
        initializeProcessingSteps();
    }
    
    // Execute the complete processing pipeline
    std::vector<ProcessingStepResult> executePipeline(
        SimulationStep currentStep, 
        Timestamp currentTime, 
        Brain* brain
    ) {
        std::vector<ProcessingStepResult> results;
        
        for (const auto& stepName : getProcessingStepOrder()) {
            auto it = stepFunctions_.find(stepName);
            if (it != stepFunctions_.end()) {
                auto startTime = std::chrono::high_resolution_clock::now();
                auto result = it->second(currentStep, currentTime, brain);
                auto endTime = std::chrono::high_resolution_clock::now();
                result.duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                results.push_back(result);
            }
        }
        
        return results;
    }
    
    // Get the order of processing steps
    const std::vector<std::string>& getProcessingStepOrder() const {
        return processingStepOrder_;
    }
    
    // Add a processing step
    void addProcessingStep(const std::string& name, ProcessingStepFunction stepFunction) {
        stepFunctions_[name] = std::move(stepFunction);
    }
    
    // Get a processing step
    ProcessingStepFunction getProcessingStep(const std::string& name) {
        auto it = stepFunctions_.find(name);
        if (it != stepFunctions_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Execute a specific processing step
    ProcessingStepResult executeStep(
        const std::string& name,
        SimulationStep currentStep,
        Timestamp currentTime,
        Brain* brain
    ) {
        auto it = stepFunctions_.find(name);
        if (it != stepFunctions_.end()) {
            auto startTime = std::chrono::high_resolution_clock::now();
            auto result = it->second(currentStep, currentTime, brain);
            auto endTime = std::chrono::high_resolution_clock::now();
            result.duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            return result;
        }
        
        return {name, std::chrono::microseconds(0), false, "Step not found"};
    }
    
    // Get statistics about processing steps
    std::string getPipelineStatistics() const {
        std::stringstream ss;
        ss << "Processing Pipeline Statistics:" << std::endl;
        ss << "  Total steps: " << stepFunctions_.size() << std::endl;
        ss << "  Step order: ";
        for (size_t i = 0; i < processingStepOrder_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << processingStepOrder_[i];
        }
        ss << std::endl;
        return ss.str();
    }
    
    // Execute a subset of processing steps
    std::vector<ProcessingStepResult> executeStepSubset(
        const std::vector<std::string>& stepNames,
        SimulationStep currentStep,
        Timestamp currentTime,
        Brain* brain
    ) {
        std::vector<ProcessingStepResult> results;
        
        for (const auto& stepName : stepNames) {
            auto it = stepFunctions_.find(stepName);
            if (it != stepFunctions_.end()) {
                auto startTime = std::chrono::high_resolution_clock::now();
                auto result = it->second(currentStep, currentTime, brain);
                auto endTime = std::chrono::high_resolution_clock::now();
                result.duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                results.push_back(result);
            }
        }
        
        return results;
    }
    
    // Reorder processing steps
    void reorderProcessingSteps(const std::vector<std::string>& newOrder) {
        processingStepOrder_ = newOrder;
    }
    
    // Get the number of processing steps
    size_t getStepCount() const {
        return stepFunctions_.size();
    }
    
    // Check if a step exists
    bool hasStep(const std::string& name) const {
        return stepFunctions_.find(name) != stepFunctions_.end();
    }
    
    // Get all step names
    std::vector<std::string> getStepNames() const {
        std::vector<std::string> names;
        for (const auto& pair : stepFunctions_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
protected:
    virtual void initializeProcessingSteps() = 0;
    
private:
    std::map<std::string, ProcessingStepFunction> stepFunctions_;
    std::vector<std::string> processingStepOrder_;
};

// Brain processing pipeline implementation
class DefaultBrainProcessingPipeline : public BrainProcessingPipeline {
protected:
    void initializeProcessingSteps() override {
        // Define the standard processing pipeline order
        processingStepOrder_ = {
            "ProcessDelayedSpikeEvents",
            "UpdateNeurons",
            "DetectSpikes",
            "UpdateWorkingMemory",
            "ApplyNeuromodulation",
            "ApplyPlasticity",
            "UpdateEpisodicMemory",
            "UpdatePredictionSystem",
            "UpdateAttention",
            "UpdateConceptFormation",
            "ApplyStructuralPlasticity",
            "ReplayMemories",
            "ApplyDevelopment",
            "CollectStatistics"
        };
        
        // Register processing step functions
        stepFunctions_["ProcessDelayedSpikeEvents"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->processDelayedSpikeEvents(currentStep, currentTime);
            };
        
        stepFunctions_["UpdateNeurons"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->updateNeurons(currentStep, currentTime);
            };
        
        stepFunctions_["DetectSpikes"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->detectSpikes(currentStep, currentTime);
            };
        
        stepFunctions_["UpdateWorkingMemory"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->updateWorkingMemory(currentStep, currentTime);
            };
        
        stepFunctions_["ApplyNeuromodulation"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->applyNeuromodulation(currentStep, currentTime);
            };
        
        stepFunctions_["ApplyPlasticity"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->applyPlasticity(currentStep, currentTime);
            };
        
        stepFunctions_["UpdateEpisodicMemory"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->updateEpisodicMemory(currentStep, currentTime);
            };
        
        stepFunctions_["UpdatePredictionSystem"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->updatePredictionSystem(currentStep, currentTime);
            };
        
        stepFunctions_["UpdateAttention"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->updateAttention(currentStep, currentTime);
            };
        
        stepFunctions_["UpdateConceptFormation"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->updateConceptFormation(currentStep, currentTime);
            };
        
        stepFunctions_["ApplyStructuralPlasticity"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->applyStructuralPlasticity(currentStep, currentTime);
            };
        
        stepFunctions_["ReplayMemories"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->replayMemories(currentStep, currentTime);
            };
        
        stepFunctions_["ApplyDevelopment"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->applyDevelopment(currentStep, currentTime);
            };
        
        stepFunctions_["CollectStatistics"] = 
            [](SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
                return brain->collectStatistics(currentStep, currentTime);
            };
    }
};

}} // namespace nlm