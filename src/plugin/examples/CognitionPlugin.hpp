#pragma once

#include "CognitionPlugin.hpp"
#include <random>

namespace nlm {

class ExampleCognitionPlugin : public CognitionPlugin {
public:
    ExampleCognitionPlugin();
    ~ExampleCognitionPlugin() override;
    
    // Plugin identification
    std::string getName() const override { return "ExampleCognition"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Example cognition plugin with custom cognitive algorithms"; 
    }
    std::string getAuthor() const override { return "NLM Team"; }
    PluginType getType() const override { return PluginType::COGNITION; }
    
    // Plugin lifecycle
    bool initialize(const PluginConfig& config, void* manager) override;
    void update(double deltaTime) override;
    void shutdown() override;
    
    // Plugin state management
    void pause() override;
    void resume() override;
    PluginState getState() const override;
    
    // Plugin dependencies
    std::vector<std::string> getDependencies() const override;
    bool checkDependencies(const std::unordered_map<std::string, PluginInfo>& plugins) const override;
    
    // Plugin communication
    void receiveMessage(const std::string& sender, const PluginMessage& message) override;
    void sendMessage(const std::string& receiver, const PluginMessage& message) override;
    
    // Plugin configuration
    void configure(const PluginConfig& config) override;
    PluginConfig getConfig() const override;
    
    // Cognitive extension
    void updateCognition(double deltaTime) override;
    void makeDecision(const CognitiveState& state) override;
    void learnFromExperience(const Experience& experience) override;
    
    // Cognitive algorithms
    CognitiveAlgorithm getAlgorithm() const override { return CognitiveAlgorithm::CUSTOM; }
    double getLearningRate() const override { return learningRate; }
    int getPlanningDepth() const override { return planningDepth; }
    
private:
    struct Impl;
    Impl* pImpl;
    double learningRate;
    int planningDepth;
};

} // namespace nlm
