#pragma once

#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Config/Config.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <stdexcept>

namespace nlm {

// Brain component types for factory configuration
enum class BrainComponentType {
    DefaultBrain,
    EnhancedBrain,
    MinimalBrain,
    CustomBrain
};

// Neuron types for factory configuration
enum class NeuronTypeConfig {
    Standard,
    HighConductance,
    LowThreshold,
    Adaptive
};

// Network topology types
enum class NetworkTopology {
    Feedforward,
    Recurrent,
    Hierarchical,
    Modular
};

// Factory configuration for brain creation
struct BrainFactoryConfig {
    BrainComponentType brainType = BrainComponentType::DefaultBrain;
    NeuronTypeConfig neuronType = NeuronTypeConfig::Standard;
    NetworkTopology topology = NetworkTopology::Feedforward;
    size_t neuronCount = 1000;
    float connectionDensity = 0.1f;
    bool enablePlasticity = true;
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    bool enableMemory = true;
    bool enablePrediction = true;
    bool enableCognition = true;
    std::string configFile = "configs/default.cfg";
    std::map<std::string, float> customParameters;
    
    // Validation
    bool isValid() const {
        return neuronCount > 0 && 
               connectionDensity >= 0.0f && 
               connectionDensity <= 1.0f;
    }
};

// Brain component creation result
struct BrainCreationResult {
    bool success;
    std::string message;
    std::shared_ptr<Brain> brain;
    std::string errors;
    
    BrainCreationResult() : success(false), message("Creation failed") {}
    BrainCreationResult(std::shared_ptr<Brain> b, const std::string& msg = "Success")
        : success(true), message(msg), brain(b) {}
    
    bool isOk() const { return success; }
    bool isError() const { return !success; }
};

// Brain factory class - creates complex brain architectures
class BrainFactory {
public:
    BrainFactory() = default;
    ~BrainFactory() = default;
    
    // Create a brain from configuration
    static BrainCreationResult createBrain(const BrainFactoryConfig& config);
    
    // Create a brain from existing config file
    static BrainCreationResult createBrainFromFile(const std::string& configFile);
    
    // Create a brain with custom parameters
    static BrainCreationResult createBrain(const std::shared_ptr<Config>& config);
    
    // Create a minimal test brain (for quick testing)
    static BrainCreationResult createMinimalBrain(size_t neuronCount = 100);
    
    // Create an enhanced brain with all features enabled
    static BrainCreationResult createEnhancedBrain(size_t neuronCount = 5000);
    
    // Create a custom brain with specific architecture
    static BrainCreationResult createCustomBrain(const std::vector<size_t>& regionSizes,
                                                const std::vector<std::string>& regionNames);
    
    // Configure neuron types based on type config
    static void configureNeuronTypes(std::shared_ptr<Brain> brain, NeuronTypeConfig neuronType);
    
    // Configure network topology
    static void configureTopology(std::shared_ptr<Brain> brain, NetworkTopology topology);
    
    // Get default factory configuration
    static BrainFactoryConfig getDefaultConfig();
    
    // Get enhanced factory configuration
    static BrainFactoryConfig getEnhancedConfig();
    
    // Validate factory configuration
    static std::string validateConfig(const BrainFactoryConfig& config);
    
    // Get available brain component types as strings
    static std::vector<std::string> getAvailableBrainTypes();
    
    // Get available neuron types as strings
    static std::vector<std::string> getAvailableNeuronTypes();
    
    // Get available network topologies as strings
    static std::vector<std::string> getAvailableTopologies();
    
private:
    // Internal creation methods
    static BrainCreationResult createDefaultBrain(const BrainFactoryConfig& config);
    static BrainCreationResult createEnhancedBrainInternal(const BrainFactoryConfig& config);
    static BrainCreationResult createMinimalBrainInternal(const BrainFactoryConfig& config);
    
    // Helper methods for neuron configuration
    static void applyNeuronConfiguration(std::shared_ptr<Brain> brain, NeuronTypeConfig neuronType);
    static void applyTopologyConfiguration(std::shared_ptr<Brain> brain, NetworkTopology topology);
    static void applyPlasticityConfiguration(std::shared_ptr<Brain> brain, bool enable);
    static void applyDevelopmentConfiguration(std::shared_ptr<Brain> brain, bool enable);
    static void applyNeuromodulationConfiguration(std::shared_ptr<Brain> brain, bool enable);
    static void applyMemoryConfiguration(std::shared_ptr<Brain> brain, bool enable);
    static void applyPredictionConfiguration(std::shared_ptr<Brain> brain, bool enable);
    static void applyCognitionConfiguration(std::shared_ptr<Brain> brain, bool enable);
    
    // Neuron configuration helpers
    static void configureStandardNeurons(std::shared_ptr<Brain> brain);
    static void configureHighConductanceNeurons(std::shared_ptr<Brain> brain);
    static void configureLowThresholdNeurons(std::shared_ptr<Brain> brain);
    static void configureAdaptiveNeurons(std::shared_ptr<Brain> brain);
    
    // Topology configuration helpers
    static void configureFeedforwardTopology(std::shared_ptr<Brain> brain);
    static void configureRecurrentTopology(std::shared_ptr<Brain> brain);
    static void configureHierarchicalTopology(std::shared_ptr<Brain> brain);
    static void configureModularTopology(std::shared_ptr<Brain> brain);
};

} // namespace nlm