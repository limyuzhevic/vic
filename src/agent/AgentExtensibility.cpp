#include "AgentExtensibility.hpp"
#include "AgentBrain.hpp"
#include "SensoryPercept.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "motor/MotorCommand.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>

namespace nlm {

// Initialize static members
std::unordered_map<std::string, BrainRegionRegistry::RegionFactory> BrainRegionRegistry::regionFactories_;
std::unordered_map<NeuronType, BrainRegionRegistry::NeuronFactory> BrainRegionRegistry::neuronFactories_;
std::unordered_map<std::string, BrainRegionRegistry::SensoryProcessor> BrainRegionRegistry::sensoryProcessors_;
std::unordered_map<MotorCommand, BrainRegionRegistry::MotorProcessor> BrainRegionRegistry::motorProcessors_;

std::unordered_map<std::string, NeuralNetworkCustomizer::WeightInitializer> NeuralNetworkCustomizer::weightInitializers_;
std::unordered_map<std::string, NeuralNetworkCustomizer::ActivationFunction> NeuralNetworkCustomizer::activationFunctions_;
std::unordered_map<std::string, NeuralNetworkCustomizer::PlasticityRule> NeuralNetworkCustomizer::plasticityRules_;

std::vector<std::pair<AgentEventSystem::EventType, AgentEventSystem::EventHandler>> AgentEventSystem::eventHandlers_;
std::vector<std::pair<AgentEventSystem::EventType, AgentEventSystem::EventFilter>> AgentEventSystem::eventFilters_;
std::vector<AgentEventSystem::Event> AgentEventSystem::eventHistory_;

std::vector<BatchProcessor::BatchCallback> BatchProcessor::batchProcessors_;

// Region factory implementations
void BrainRegionRegistry::registerRegionFactory(const std::string& regionType, const RegionFactory& factory) {
    regionFactories_[regionType] = factory;
}

void BrainRegionRegistry::registerNeuronFactory(NeuronType type, const NeuronFactory& factory) {
    neuronFactories_[type] = factory;
}

void BrainRegionRegistry::registerSensoryProcessor(const std::string& regionType, const SensoryProcessor& processor) {
    sensoryProcessors_[regionType] = processor;
}

void BrainRegionRegistry::registerMotorProcessor(MotorCommand outputType, const MotorProcessor& processor) {
    motorProcessors_[outputType] = processor;
}

std::unique_ptr<NeuralRegion> BrainRegionRegistry::createRegion(const std::string& regionType) {
    auto it = regionFactories_.find(regionType);
    if (it != regionFactories_.end()) {
        return it->second();
    }
    return nullptr;
}

std::unique_ptr<Neuron> BrainRegionRegistry::createNeuron(NeuronType type) {
    auto it = neuronFactories_.find(type);
    if (it != neuronFactories_.end()) {
        return it->second();
    }
    return nullptr;
}

BrainRegionRegistry::SensoryProcessor BrainRegionRegistry::getSensoryProcessor(const std::string& regionType) {
    auto it = sensoryProcessors_.find(regionType);
    if (it != sensoryProcessors_.end()) {
        return it->second;
    }
    return nullptr;
}

BrainRegionRegistry::MotorProcessor BrainRegionRegistry::getMotorProcessor(MotorCommand outputType) {
    auto it = motorProcessors_.find(outputType);
    if (it != motorProcessors_.end()) {
        return it->second;
    }
    return nullptr;
}

// Neural network customizer implementations
void NeuralNetworkCustomizer::registerWeightInitializer(const std::string& name, const WeightInitializer& initializer) {
    weightInitializers_[name] = initializer;
}

void NeuralNetworkCustomizer::registerActivationFunction(const std::string& name, const ActivationFunction& function) {
    activationFunctions_[name] = function;
}

void NeuralNetworkCustomizer::registerPlasticityRule(const std::string& name, const PlasticityRule& rule) {
    plasticityRules_[name] = rule;
}

NeuralNetworkCustomizer::WeightInitializer NeuralNetworkCustomizer::getWeightInitializer(const std::string& name) {
    auto it = weightInitializers_.find(name);
    if (it != weightInitializers_.end()) {
        return it->second;
    }
    // Return default initializer
    return [](float pre, float post, float learningRate) { return learningRate * (post - pre); };
}

NeuralNetworkCustomizer::ActivationFunction NeuralNetworkCustomizer::getActivationFunction(const std::string& name) {
    auto it = activationFunctions_.find(name);
    if (it != activationFunctions_.end()) {
        return it->second;
    }
    // Return default activation (sigmoid)
    return [](float x) { return 1.0f / (1.0f + std::exp(-x)); };
}

NeuralNetworkCustomizer::PlasticityRule NeuralNetworkCustomizer::getPlasticityRule(const std::string& name) {
    auto it = plasticityRules_.find(name);
    if (it != plasticityRules_.end()) {
        return it->second;
    }
    // Return default STDP rule
    return [](float& pre, float& post, float learningRate, float preTrace, float postTrace) {
        pre += -learningRate * postTrace;
        post += learningRate * preTrace;
    };
}

void NeuralNetworkCustomizer::applyCustomConfiguration(AgentBrain* agent, const std::string& configName) {
    // TODO: Implement custom configuration application
    // This would allow users to apply predefined neural network configurations
}

// Event system implementations
void AgentEventSystem::registerEventHandler(EventType type, const EventHandler& handler) {
    eventHandlers_.emplace_back(type, handler);
}

void AgentEventSystem::registerEventFilter(EventType type, const EventFilter& filter) {
    eventFilters_.emplace_back(type, filter);
}

void AgentEventSystem::dispatchEvent(const Event& event) {
    // Check filters
    for (const auto& filterPair : eventFilters_) {
        if (filterPair.first == event.type || filterPair.first == EventType::Custom) {
            if (!filterPair.second(event)) {
                return; // Event filtered out
            }
        }
    }
    
    // Dispatch to handlers
    for (const auto& handlerPair : eventHandlers_) {
        if (handlerPair.first == event.type || handlerPair.first == EventType::Custom) {
            handlerPair.second(event);
        }
    }
    
    // Add to history
    eventHistory_.push_back(event);
}

std::vector<AgentEventSystem::Event> AgentEventSystem::getEventHistory(EventType type, size_t maxEvents) {
    std::vector<Event> filteredHistory;
    
    if (type == EventType::Custom) {
        filteredHistory = eventHistory_;
    } else {
        for (const auto& event : eventHistory_) {
            if (event.type == type) {
                filteredHistory.push_back(event);
            }
        }
    }
    
    // Limit size
    if (filteredHistory.size() > maxEvents) {
        filteredHistory.erase(filteredHistory.end() - (filteredHistory.size() - maxEvents), filteredHistory.end());
    }
    
    return filteredHistory;
}

void AgentEventSystem::clearEventHistory(EventType type) {
    if (type == EventType::Custom) {
        eventHistory_.clear();
    } else {
        auto newEnd = std::remove_if(eventHistory_.begin(), eventHistory_.end(),
                                    [type](const Event& e) { return e.type == type; });
        eventHistory_.erase(newEnd, eventHistory_.end());
    }
}

// Advanced configuration implementations
const AdvancedConfig::NeuralParameters& AdvancedConfig::getNeuralConfig() {
    static NeuralParameters params;
    return params;
}

const AdvancedConfig::MemoryParameters& AdvancedConfig::getMemoryConfig() {
    static MemoryParameters params;
    return params;
}

const AdvancedConfig::DevelopmentParameters& AdvancedConfig::getDevelopmentConfig() {
    static DevelopmentParameters params;
    return params;
}

const AdvancedConfig::SensoryParameters& AdvancedConfig::getSensoryConfig() {
    static SensoryParameters params;
    return params;
}

void AdvancedConfig::setNeuralConfig(const NeuralParameters& params) {
    neuralConfig_ = params;
}

void AdvancedConfig::setMemoryConfig(const MemoryParameters& params) {
    memoryConfig_ = params;
}

void AdvancedConfig::setDevelopmentConfig(const DevelopmentParameters& params) {
    developmentConfig_ = params;
}

void AdvancedConfig::setSensoryConfig(const SensoryParameters& params) {
    sensoryConfig_ = params;
}

void AdvancedConfig::applyAdvancedConfig(AgentBrain* agent) {
    if (!agent) return;
    
    // TODO: Apply advanced configuration to agent
    // This would modify agent's internal parameters based on advanced config
}

// Batch processor implementations
void BatchProcessor::registerBatchProcessor(const BatchCallback& processor) {
    batchProcessors_.push_back(processor);
}

BatchProcessor::MotorBatch BatchProcessor::processBatch(const SensoryBatch& input) {
    MotorBatch output;
    output.commands.reserve(input.percepts.size());
    output.executionTimes.reserve(input.percepts.size());
    output.outcomes.reserve(input.percepts.size());
    
    for (size_t i = 0; i < input.percepts.size(); ++i) {
        // TODO: Implement batch processing
        // This would process multiple sensory inputs at once for improved performance
        output.commands.push_back(MotorCommand::Wait);
        output.executionTimes.push_back(0.0);
        output.outcomes.push_back(0.0f);
    }
    
    // Apply batch processors
    for (const auto& processor : batchProcessors_) {
        MotorBatch tempOutput;
        processor(input, tempOutput);
        
        // Merge results (last processor wins)
        if (!tempOutput.commands.empty()) {
            output = tempOutput;
        }
    }
    
    return output;
}

void BatchProcessor::clearBatchProcessors() {
    batchProcessors_.clear();
}

} // namespace nlm