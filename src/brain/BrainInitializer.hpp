#ifndef NLM_BRAIN_INITIALIZER_HPP
#define NLM_BRAIN_INITIALIZER_HPP

#include "brain/Brain.hpp"
#include "brain/BrainConfig.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * Brain initialization manager
 * Handles all aspects of brain system initialization
 */
class BrainInitializer {
public:
    struct InitializationParameters {
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        float weightRange = 0.2f;
    };

    BrainInitializer() = default;
    ~BrainInitializer() = default;
    
    // Main initialization method
    bool initialize(Brain& brain);
    
    // Configuration-based initialization
    bool initializeFromConfig(Brain& brain, const BrainConfig& config);
    
    // Legacy initialization (maintains backward compatibility)
    bool initializeLegacy(Brain& brain, const InitializationParameters& params = {});
    
    // Region setup
    bool setupRegions(Brain& brain, const BrainConfig& config);
    bool setupRegionsLegacy(Brain& brain, size_t regionCount, 
                           size_t neuronCountPerRegion, float connectionProbability);
    
    // Neuron population setup
    bool setupNeuronPopulations(Brain& brain, const BrainConfig& config);
    bool setupNeuronPopulationsLegacy(Brain& brain, size_t neuronsPerRegion);
    
    // System integration setup
    bool setupSystemIntegration(Brain& brain);
    bool setupMemorySystems(Brain& brain);
    bool setupPredictionSystem(Brain& brain);
    bool setupCognitionSystems(Brain& brain);
    bool setupNeuromodulationSystems(Brain& brain);
    bool setupPlasticitySystems(Brain& brain);
    bool setupDevelopmentSystem(Brain& brain);
    
    // Checkpoint setup
    bool setupCheckpointSystem(Brain& brain, const std::string& checkpointDir);
    
    // Statistics collection
    void logInitializationStats(const Brain& brain) const;
    
    // Reset and cleanup
    void cleanup(Brain& brain);
    
    // Validation
    bool validateInitializationParameters(const InitializationParameters& params) const;
    bool validateConfiguration(const BrainConfig& config) const;

private:
    bool initializeSpikeSystem(Brain& brain);
    bool initializeConnectivity(Brain& brain, float connectionProbability, float weightRange);
    bool initializeMemoryNeurons(Brain& brain);
    bool initializeSensoryMotorNeurons(Brain& brain);
    bool registerSpikeHandlers(Brain& brain);
    bool configurePlasticity(Brain& brain, const BrainConfig& config);
    bool configureDevelopment(Brain& brain, const BrainConfig& config);
    bool configureNeuromodulation(Brain& brain, const BrainConfig& config);
};

} // namespace nlm

#endif // NLM_BRAIN_INITIALIZER_HPP
