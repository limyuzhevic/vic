#pragma once

#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <random>

namespace nlm {

// Advanced network topology types
enum class AdvancedTopologyType {
    Convolutional,
    Residual,
    SkipConnection,
    AutoEncoder,
    GAN,
    AttentionNetwork,
    GraphNeuralNetwork,
    HybridTopology
};

// Network layer types
enum class LayerType {
    Input,
    Hidden,
    Output,
    Recurrent,
    Convolutional,
    Pooling,
    Activation,
    Normalization,
    Custom
};

// Connection pattern for advanced topologies
struct ConnectionPattern {
    LayerType sourceLayer;
    LayerType targetLayer;
    float connectionStrength;
    size_t delay;
    bool plastic;
    bool recurrent;
    
    ConnectionPattern() : connectionStrength(1.0f), delay(1), plastic(false), recurrent(false) {}
    ConnectionPattern(LayerType src, LayerType tgt, float strength = 1.0f) 
        : sourceLayer(src), targetLayer(tgt), connectionStrength(strength), delay(1), plastic(false), recurrent(false) {}
};

// Advanced network configuration
struct AdvancedTopologyConfig {
    AdvancedTopologyType topologyType;
    std::vector<size_t> layerSizes;
    std::vector<LayerType> layerTypes;
    std::vector<std::string> layerNames;
    std::vector<std::vector<ConnectionPattern>> connectionMatrix;
    
    // Activation functions
    enum class ActivationFunction {
        ReLU,
        Sigmoid,
        Tanh,
        Softmax,
        LeakyReLU,
        ELU,
        Swish,
        Custom
    };
    
    ActivationFunction activationFunction;
    float learningRate;
    float dropoutRate;
    bool batchNormalization;
    bool residualConnections;
    
    // Network parameters
    size_t maxDepth;
    size_t branchingFactor;
    float sparsity;
    
    // Validation
    bool isValid() const {
        return !layerSizes.empty() && layerSizes.size() == layerTypes.size() &&
               layerSizes.size() == layerNames.size() &&
               layerSizes.size() <= maxDepth;
    }
};

// Advanced neural network topology manager
class AdvancedTopologyManager {
public:
    AdvancedTopologyManager() = default;
    ~AdvancedTopologyManager() = default;
    
    // Create advanced brain from topology configuration
    static std::shared_ptr<Brain> createFromTopology(const AdvancedTopologyConfig& config);
    
    // Create convolutional network topology
    static AdvancedTopologyConfig createConvolutionalTopology(const std::vector<size_t>& channels,
                                                             size_t kernelSize = 3,
                                                             size_t stride = 1);
    
    // Create residual network topology
    static AdvancedTopologyConfig createResidualTopology(const std::vector<size_t>& layerSizes,
                                                       size_t numResidualBlocks = 3);
    
    // Create skip connection topology
    static AdvancedTopologyConfig createSkipConnectionTopology(const std::vector<size_t>& layerSizes,
                                                             const std::vector<size_t>& skipLayers);
    
    // Create autoencoder topology
    static AdvancedTopologyConfig createAutoencoderTopology(size_t inputSize,
                                                         size_t latentSize,
                                                         size_t encodingLayers = 2);
    
    // Create GAN topology
    static AdvancedTopologyConfig createGANTopology(size_t generatorSize,
                                                   size_t discriminatorSize,
                                                   size_t sharedSize = 100);
    
    // Create attention network topology
    static AdvancedTopologyConfig createAttentionTopology(size_t seqLength,
                                                        size_t embeddingSize,
                                                        size_t attentionHeads = 8);
    
    // Create graph neural network topology
    static AdvancedTopologyConfig createGraphNeuralNetworkTopology(size_t numNodes,
                                                                 size_t nodeFeatureDim,
                                                                 size_t edgeFeatureDim = 0);
    
    // Create hybrid topology
    static AdvancedTopologyConfig createHybridTopology(const std::vector<AdvancedTopologyConfig>& subTopologies);
    
    // Get topology type name as string
    static std::string getTopologyTypeName(AdvancedTopologyType type);
    
    // Get layer type name as string
    static std::string getLayerTypeName(LayerType type);
    
    // Get activation function name as string
    static std::string getActivationFunctionName(AdvancedTopologyConfig::ActivationFunction func);
    
    // Generate random topology configuration
    static AdvancedTopologyConfig generateRandomTopology(AdvancedTopologyType type,
                                                       size_t minLayers = 2,
                                                       size_t maxLayers = 6);
    
    // Validate topology configuration
    static std::string validateTopologyConfig(const AdvancedTopologyConfig& config);
    
    // Create connectivity matrix for topology
    static std::vector<std::vector<ConnectionPattern>> createConnectivityMatrix(
        const AdvancedTopologyConfig& config,
        float connectionDensity = 0.5f,
        bool sparseConnections = true);
    
    // Apply topology to brain
    static void applyTopologyToBrain(std::shared_ptr<Brain> brain,
                                    const AdvancedTopologyConfig& config,
                                    const std::vector<std::vector<ConnectionPattern>>& connectivity);
    
    // Get available topology types
    static std::vector<AdvancedTopologyType> getAvailableTopologyTypes();
    
    // Get available layer types
    static std::vector<LayerType> getAvailableLayerTypes();
    
    // Get available activation functions
    static std::vector<AdvancedTopologyConfig::ActivationFunction> getAvailableActivationFunctions();
    
private:
    // Internal creation methods
    static std::shared_ptr<Brain> createConvolutionalBrain(const AdvancedTopologyConfig& config);
    static std::shared_ptr<Brain> createResidualBrain(const AdvancedTopologyConfig& config);
    static std::shared_ptr<Brain> createSkipConnectionBrain(const AdvancedTopologyConfig& config);
    static std::shared_ptr<Brain> createAutoencoderBrain(const AdvancedTopologyConfig& config);
    static std::shared_ptr<Brain> createGABrain(const AdvancedTopologyConfig& config);
    static std::shared_ptr<Brain> createAttentionBrain(const AdvancedTopologyConfig& config);
    static std::shared_ptr<Brain> createGraphNeuralNetworkBrain(const AdvancedTopologyConfig& config);
    static std::shared_ptr<Brain> createHybridBrain(const AdvancedTopologyConfig& config);
    
    // Helper methods for layer creation
    static std::shared_ptr<NeuralRegion> createLayerRegion(std::shared_ptr<Brain> brain,
                                                        size_t neuronCount,
                                                        LayerType layerType,
                                                        const std::string& layerName);
    
    // Helper methods for connection creation
    static void createConnectionsBetweenRegions(std::shared_ptr<NeuralRegion> source,
                                              std::shared_ptr<NeuralRegion> target,
                                              const ConnectionPattern& pattern);
    
    // Helper methods for activation functions
    static void applyActivationFunction(std::shared_ptr<NeuralRegion> region,
                                      AdvancedTopologyConfig::ActivationFunction func);
    
    // Helper methods for pooling layers
    static void createPoolingLayer(std::shared_ptr<NeuralRegion> source,
                                 std::shared_ptr<NeuralRegion> target,
                                 size_t poolSize = 2);
    
    // Helper methods for normalization layers
    static void createNormalizationLayer(std::shared_ptr<NeuralRegion> region);
};

} // namespace nlm