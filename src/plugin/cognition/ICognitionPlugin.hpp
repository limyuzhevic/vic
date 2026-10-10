// Cognition plugin base class interface
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "../PluginBase.hpp"
#include "../cognition/NeuralPlanner.hpp"

namespace nlm {

class ICognitionPlugin : public IPlugin {
public:
    virtual ~ICognitionPlugin() = default;
    
    // Plugin category identification
    virtual PluginCategory getCategory() const override { return PluginCategory::COGNITION; }
    
    // Cognitive system creation and management
    virtual std::shared_ptr<NeuralPlanner> createCognitiveSystem() = 0;
    virtual bool configureCognitiveSystem(NeuralPlanner& planner, const std::map<std::string, std::string>& parameters) = 0;
    virtual std::vector<std::string> getCognitiveSystemTypes() const = 0;
    
    // Cognitive processes customization
    virtual bool initializeCognitiveProcesses(NeuralPlanner& planner) = 0;
    virtual void updateCognitiveState(NeuralPlanner& planner, Timestamp currentTime, TimestepDuration dt) = 0;
    virtual std::string getCognitiveDecision(NeuralPlanner& planner, const std::vector<std::string>& inputs) = 0;
    
    // Learning and adaptation
    virtual bool learnFromExperience(NeuralPlanner& planner, const std::map<std::string, double>& experience) = 0;
    virtual void updateKnowledgeBase(NeuralPlanner& planner, const std::string& newKnowledge) = 0;
    
    // Plugin-specific configuration
    virtual std::map<std::string, std::string> getDefaultParameters() const = 0;
    virtual std::vector<std::string> getSupportedParameterTypes() const = 0;
    
    // Cognitive architecture
    virtual bool setCognitiveArchitecture(NeuralPlanner& planner, const std::string& architectureType) = 0;
    virtual std::string getCognitiveArchitecture(NeuralPlanner& planner) const = 0;
};

} // namespace nlm