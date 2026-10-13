#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations for all system classes
class NeuralCore;
class MemorySystem;
class PredictionSystem;
class CognitiveSystem;
class NeuromodulationSystem;
class DevelopmentSystem;
class PlasticitySystem;
class CheckpointSystem;
class ConfigurationManager;

// Integration: System interconnection and coordination
// Manages inter-system communication and dependency resolution
class Integration {
public:
    Integration(std::shared_ptr<ConfigurationManager> configManager);
    ~Integration();
    
    // System initialization
    bool initializeAllSystems();
    void shutdownAllSystems();
    
    // System access
    NeuralCore* getNeuralCore() { return neuralCore_.get(); }
    MemorySystem* getMemorySystem() { return memorySystem_.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    CognitiveSystem* getCognitiveSystem() { return cognitiveSystem_.get(); }
    NeuromodulationSystem* getNeuromodulationSystem() { return neuromodulationSystem_.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    PlasticitySystem* getPlasticitySystem() { return plasticitySystem_.get(); }
    CheckpointSystem* getCheckpointSystem() { return checkpointSystem_.get(); }
    
    // System statistics
    size_t getSystemCount() const { return systemCount_; }
    std::vector<std::string> getSystemNames() const;
    
    // Integration point management
    void registerIntegrationPoint(const std::string& sourceSystem, 
                                  const std::string& targetSystem,
                                  const std::string& signalType);
    void sendSignal(const std::string& sourceSystem,
                    const std::string& targetSystem,
                    const std::string& signalType,
                    const std::vector<float>& data);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // System instances
    std::unique_ptr<NeuralCore> neuralCore_;
    std::unique_ptr<MemorySystem> memorySystem_;
    std::unique_ptr<PredictionSystem> predictionSystem_;
    std::unique_ptr<CognitiveSystem> cognitiveSystem_;
    std::unique_ptr<NeuromodulationSystem> neuromodulationSystem_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    std::unique_ptr<PlasticitySystem> plasticitySystem_;
    std::unique_ptr<CheckpointSystem> checkpointSystem_;
    std::unique_ptr<ConfigurationManager> configManager_;
    
    // Integration statistics
    size_t systemCount_;
    size_t integrationPointCount_;
};

} // namespace nlm
