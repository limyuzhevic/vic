#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Advanced neural network features for deep learning

// Deep neural network with dendritic computation
class DendriticNeuralNetwork {
public:
    struct DendriticCompartment {
        std::vector<float> inputs;
        std::vector<float> weights;
        std::vector<float> biases;
        std::vector<float> outputs;
        std::vector<float> dendritic_potentials;
        
        // Dendritic computation parameters
        float dendritic_threshold;
        float synaptic_decay;
        float integration_time_constant;
        
        DendriticCompartment() : dendritic_threshold(0.5f), synaptic_decay(0.9f), 
                                 integration_time_constant(10.0f) {}
    };
    
    struct Layer {
        std::vector<DendriticCompartment> dendritic_units;
        std::vector<float> activation_output;
        std::vector<std::vector<size_t>> connectivity_pattern;  // Which neurons connect to which
        
        Layer() : activation_scale(1.0f), activation_shift(0.0f) {}
        
        void forwardPropagate(const std::vector<float>& inputs, float dt);
        void updateDendriticStates(float dt);
        void computeActivationOutputs();
        
    private:
        float activation_scale;
        float activation_shift;
    };
    
    DendriticNeuralNetwork() : learning_rate_(0.01f), depth_(3), 
                              width_(100), branching_factor_(2) {}
    
    // Build deep network architecture
    void buildNetwork(int depth, int width, int branching_factor);
    
    // Forward pass through the entire network
    std::vector<float> forwardPropagate(const std::vector<float>& inputs, float dt);
    
    // Deep learning training with backpropagation
    void train(const std::vector<std::vector<float>>& training_inputs,
               const std::vector<std::vector<float>>& targets, int epochs);
    
    // Update weights using deep learning rules
    void updateWeights(const std::vector<std::vector<float>>& inputs,
                      const std::vector<std::vector<float>>& errors, float dt);
    
    // Get network output
    std::vector<float> getOutput() const { return current_output_; }
    
    // Set parameters
    void setLearningRate(float rate) { learning_rate_ = rate; }
    void setDepth(int depth) { depth_ = depth; }
    void setWidth(int width) { width_ = width; }
    
private:
    float learning_rate_;
    int depth_;
    int width_;
    int branching_factor_;
    
    std::vector<Layer> layers_;
    std::vector<float> current_output_;
    std::vector<std::vector<float>> layer_inputs_;
    std::vector<std::vector<float>> layer_errors_;
};

// Advanced attention mechanisms and gating systems
class AdvancedAttention {
public:
    struct AttentionHead {
        std::vector<std::vector<float>> query_weights;
        std::vector<std::vector<float>> key_weights;
        std::vector<std::vector<float>> value_weights;
        std::vector<std::vector<float>> output_weights;
        
        // Attention parameters
        float temperature;
        float attention_dropout;
        float gating_strength;
        float context_sensitivity;
        
        AttentionHead() : temperature(1.0f), attention_dropout(0.1f), 
                         gating_strength(1.0f), context_sensitivity(0.5f) {}
    };
    
    struct MultiHeadAttention {
        std::vector<AttentionHead> heads;
        std::vector<std::vector<float>> combined_output;
        std::vector<std::vector<float>> attention_weights;
        std::vector<std::vector<float>> gating_signals;
        
        void forward(const std::vector<std::vector<float>>& inputs);
        void computeAttentionWeights(const std::vector<std::vector<float>>& queries,
                                    const std::vector<std::vector<float>>& keys,
                                    const std::vector<std::vector<float>>& values);
        void applyGatingSignals(const std::vector<float>& context_signals);
    };
    
    struct PositionalEncoding {
        std::vector<float> sinusoidal_encoding;
        std::vector<float> learned_encoding;
        std::vector<float> relative_position_encoding;
        
        void computeSinusoidalEncoding(int max_len, int d_model);
        void computeRelativeEncoding(int seq_len);
    };
    
    AdvancedAttention() : num_heads_(8), seq_length_(100), model_dim_(512),
                         attention_dropout_(0.1f), residual_connection_(true) {}
    
    // Compute multi-head attention
    std::vector<std::vector<float>> computeMultiHeadAttention(
        const std::vector<std::vector<float>>& queries,
        const std::vector<std::vector<float>>& keys,
        const std::vector<std::vector<float>>& values,
        const std::vector<float>& attention_mask = {});
    
    // Apply positional encoding
    std::vector<std::vector<float>> applyPositionalEncoding(
        const std::vector<std::vector<float>>& inputs,
        PositionalEncoding& encoding);
    
    // Compute attention scores with advanced mechanisms
    std::vector<std::vector<float>> computeAdvancedAttentionScores(
        const std::vector<std::vector<float>>& queries,
        const std::vector<std::vector<float>>& keys,
        const std::vector<float>& mask = {});
    
    // Set parameters
    void setNumHeads(int heads) { num_heads_ = heads; }
    void setSequenceLength(int seq_len) { seq_length_ = seq_len; }
    void setModelDimension(int dim) { model_dim_ = dim; }
    
private:
    int num_heads_;
    int seq_length_;
    int model_dim_;
    float attention_dropout_;
    bool residual_connection_;
};

// Skip connections and residual networks
class ResidualNetwork {
public:
    struct ResidualBlock {
        std::vector<std::vector<float>> conv_weights;  // Convolutional weights
        std::vector<float> batch_norm_gamma;
        std::vector<float> batch_norm_beta;
        std::vector<float> batch_norm_mean;
        std::vector<float> batch_norm_var;
        
        // Residual connection parameters
        float skip_connection_scale;
        float gradient_scaling;
        bool use_projection;
        
        ResidualBlock() : skip_connection_scale(1.0f), gradient_scaling(1.0f),
                         use_projection(false) {}
        
        std::vector<std::vector<float>> forward(const std::vector<std::vector<float>>& input,
                                               float skip_scale = 1.0f);
        void updateBatchNormStats(const std::vector<std::vector<float>>& input,
                                 float momentum = 0.9f);
    };
    
    struct DenseBlock {
        std::vector<std::vector<std::vector<float>>> layer_weights;  // Each layer has its own weights
        std::vector<std::vector<float>> growth_rates;
        float concatenation_factor;
        float transition_factor;
        
        DenseBlock() : concatenation_factor(0.5f), transition_factor(0.5f) {}
        
        std::vector<std::vector<float>> forward(const std::vector<std::vector<float>>& input);
    };
    
    ResidualNetwork() : num_residual_blocks_(5), use_batch_norm_(true),
                       use_skip_projection_(true), activation_function_("leaky_relu") {}
    
    // Build residual network architecture
    void buildResidualNetwork(int num_blocks, int input_dim, int hidden_dim, int output_dim);
    
    // Forward pass through residual network
    std::vector<float> forward(const std::vector<float>& input);
    
    // Deep residual learning with gradient flow optimization
    void trainDeep(const std::vector<std::vector<float>>& inputs,
                  const std::vector<std::vector<float>>& targets, int epochs);
    
    // Set parameters
    void setNumBlocks(int num_blocks) { num_residual_blocks_ = num_blocks; }
    void setUseBatchNorm(bool use) { use_batch_norm_ = use; }
    void setUseSkipProjection(bool use) { use_skip_projection_ = use; }
    void setActivationFunction(const std::string& func) { activation_function_ = func; }
    
private:
    int num_residual_blocks_;
    bool use_batch_norm_;
    bool use_skip_projection_;
    std::string activation_function_;
    
    std::vector<ResidualBlock> residual_blocks_;
    std::vector<DenseBlock> dense_blocks_;
    std::vector<std::vector<float>> current_features_;
};

// Feature extraction and hierarchical processing
class HierarchicalFeatureExtractor {
public:
    struct FeatureMap {
        std::vector<std::vector<float>> features;
        std::vector<std::vector<float>> gradients;
        std::vector<size_t> spatial_dimensions;
        std::vector<size_t> channel_dimensions;
        
        // Feature map properties
        float spatial_scale;
        float channel_depth;
        float extraction_efficiency;
        
        FeatureMap() : spatial_scale(1.0f), channel_depth(1.0f), extraction_efficiency(0.8f) {}
    };
    
    struct HierarchicalLayer {
        std::vector<FeatureMap> feature_maps;
        std::vector<std::vector<size_t>> pooling_strides;
        std::vector<std::vector<size_t>> kernel_sizes;
        std::vector<std::vector<size_t>> padding_sizes;
        
        void extractFeatures(const std::vector<std::vector<float>>& input);
        void applyPooling(const std::vector<FeatureMap>& input);
        void fuseMultiScaleFeatures();
    };
    
    HierarchicalFeatureExtractor() : num_levels_(4), base_feature_dim_(64),
                                   hierarchical_depth_(3), multi_scale_factor_(2.0f) {}
    
    // Hierarchical feature extraction from input
    std::vector<FeatureMap> extractHierarchicalFeatures(const std::vector<float>& input);
    
    // Multi-scale feature aggregation
    std::vector<std::vector<float>> aggregateMultiScaleFeatures(
        const std::vector<FeatureMap>& feature_maps);
    
    // Deep feature learning with hierarchical optimization
    void trainHierarchical(const std::vector<std::vector<float>>& training_inputs,
                          const std::vector<std::vector<float>>& targets, int epochs);
    
    // Get extracted features
    std::vector<FeatureMap> getFeatureMaps() const { return hierarchical_features_; }
    
    // Set parameters
    void setNumLevels(int levels) { num_levels_ = levels; }
    void setBaseFeatureDim(int dim) { base_feature_dim_ = dim; }
    void setHierarchicalDepth(int depth) { hierarchical_depth_ = depth; }
    
private:
    int num_levels_;
    int base_feature_dim_;
    int hierarchical_depth_;
    float multi_scale_factor_;
    
    std::vector<HierarchicalLayer> hierarchical_layers_;
    std::vector<FeatureMap> hierarchical_features_;
};

// Advanced activation functions and normalization
class AdvancedActivations {
public:
    // Advanced activation functions
    static float swish(float x, float beta = 1.0f);
    static float mish(float x);
    static float gelu(float x, float approximate = 1.0f);
    static float hardshrink(float x, float lambd = 0.5f);
    static float selu(float x, float alpha = 1.6732632421124086f, float scale = 1.050700987574482f);
    static float spikeFunction(float x, float threshold = 0.0f, float refractory = 0.0f);
    static float dendriticActivation(float x, float dendrite_potential, float threshold);
    
    // Normalization functions
    static void batchNorm(std::vector<float>& x, const std::vector<float>& mean,
                         const std::vector<float>& var, float gamma, float beta,
                         float epsilon = 1e-5f);
    static void layerNorm(std::vector<float>& x, const std::vector<float>& gamma,
                         const std::vector<float>& beta, float epsilon = 1e-5f);
    static void instanceNorm(std::vector<float>& x, const std::vector<float>& gamma,
                            const std::vector<float>& beta, float epsilon = 1e-5f);
    static void groupNorm(std::vector<float>& x, const std::vector<float>& gamma,
                         const std::vector<float>& beta, int num_groups, float epsilon = 1e-5f);
    
    // Activation derivatives for backpropagation
    static float swishDerivative(float x, float beta = 1.0f);
    static float mishDerivative(float x);
    static float geluDerivative(float x, float approximate = 1.0f);
    
    // Adaptive activation selection
    static float adaptiveActivation(float x, const std::string& activation_type,
                                    float parameter = 1.0f, float threshold = 0.0f);
};

} // namespace nlm
