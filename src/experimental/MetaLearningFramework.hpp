#pragma once

#include <vector>
#include <memory>
#include <string>
#include <random>
#include "../core/NeuralNetwork.hpp"

namespace nlm {
namespace experimental {

/**
 * @brief Meta-learning framework for few-shot learning and rapid adaptation
 * 
 * Implements MAML (Model-Agnostic Meta-Learning) and related algorithms
 * for fast adaptation to new tasks with minimal data.
 */
class MetaLearningFramework {
public:
    struct MetaTask {
        std::vector<float> support_data;      // Support set for adaptation
        std::vector<float> query_data;        // Query set for evaluation
        std::vector<float> support_labels;    // Support set labels
        std::vector<float> query_labels;      // Query set labels
        size_t num_support_samples;            // Number of support samples
        size_t num_query_samples;             // Number of query samples
    };

    struct MetaModel {
        std::shared_ptr<NeuralNetwork> base_network;
        std::vector<float> meta_weights;      // Meta-learned weights
        std::vector<float> task_specific_weights;  // Task-specific weights
        float learning_rate;
        size_t adaptation_steps;
    };

    MetaLearningFramework(size_t inner_loop_lr = 0.01f, 
                         size_t num_adaptation_steps = 5,
                         float meta_lr = 0.001f);

    // Create a meta-task for few-shot learning
    MetaTask create_meta_task(const std::vector<float>& all_data,
                             const std::vector<float>& all_labels,
                             size_t num_support_per_class,
                             size_t num_query_per_class,
                             size_t num_classes);

    // Adapt base model to a specific task using support data
    MetaModel adapt_to_task(const MetaModel& base_model,
                          const MetaTask& task);

    // Perform meta-training with multiple tasks
    void meta_train(const std::vector<MetaTask>& tasks,
                   size_t num_meta_epochs,
                   size_t task_batch_size);

    // Evaluate adapted model on query data
    std::vector<float> evaluate_meta_model(const MetaModel& adapted_model,
                                          const MetaTask& task);

    // Get base network architecture for initialization
    std::shared_ptr<NeuralNetwork> create_base_network(
        const std::vector<size_t>& architecture,
        const std::string& activation = "relu");

private:
    float inner_loop_lr_;
    size_t num_adaptation_steps_;
    float meta_lr_;
    std::mt19937 rng_;
};

/**
 * @brief Transfer learning utilities for cross-domain learning
 */
class TransferLearning {
public:
    struct TransferConfig {
        std::string source_domain;
        std::string target_domain;
        std::string transfer_strategy;  // "fine_tuning", "knowledge_distillation", "adversarial"
        float adaptation_rate;
        bool freeze_base_layers;
        std::vector<size_t> custom_layers;  // Layers to customize for target domain
    };

    TransferLearning();

    // Learn transferable representations from source domain
    std::shared_ptr<NeuralNetwork> learn_source_representation(
        const std::vector<float>& source_data,
        const std::vector<float>& source_labels,
        const std::vector<size_t>& architecture);

    // Adapt source representation to target domain
    std::shared_ptr<NeuralNetwork> adapt_to_target(
        const std::shared_ptr<NeuralNetwork>& source_model,
        const std::vector<float>& target_data,
        const TransferConfig& config);

    // Perform domain-invariant feature learning
    std::shared_ptr<NeuralNetwork> learn_domain_invariant_features(
        const std::vector<float>& source_data,
        const std::vector<float>& target_data,
        const std::vector<size_t>& architecture);

    // Evaluate transfer performance
    float evaluate_transfer_performance(
        const std::shared_ptr<NeuralNetwork>& target_model,
        const std::vector<float>& target_data,
        const std::vector<float>& target_labels);

private:
    std::mt19937 rng_;
};

/**
 * @brief Custom architecture builder for novel neural structures
 */
class CustomArchitectureBuilder {
public:
    struct LayerConfig {
        std::string type;                    // "dense", "conv", "lstm", "attention", "custom"
        size_t input_size;
        size_t output_size;
        std::string activation;
        std::vector<size_t> kernel_size;    // For conv layers
        std::string pooling;                 // "max", "avg", "none"
        bool skip_connection;                // Whether to add skip connection
        std::string normalization;           // "batch_norm", "layer_norm", "instance_norm", "none"
    };

    struct ConnectionConfig {
        size_t source_layer;
        size_t target_layer;
        std::string connection_type;         // "feedforward", "residual", "skip", "attention"
        float connection_weight;
    };

    CustomArchitectureBuilder();

    // Build a custom neural architecture
    std::shared_ptr<NeuralNetwork> build_architecture(
        size_t input_size,
        const std::vector<LayerConfig>& layer_configs,
        const std::vector<ConnectionConfig>& connections = std::vector<ConnectionConfig>(),
        const std::string& output_activation = "softmax");

    // Validate layer configuration
    bool validate_layer_config(const LayerConfig& config) const;

    // Get layer connections from configuration
    std::vector<std::vector<size_t>> get_connections(
        const std::vector<ConnectionConfig>& connections,
        size_t num_layers) const;

    // Generate architecture visualization
    std::string generate_architecture_summary(
        const std::vector<LayerConfig>& layer_configs,
        const std::vector<ConnectionConfig>& connections) const;

    // Create pre-defined architecture templates
    std::vector<LayerConfig> create_residual_network_architecture(
        size_t input_size,
        const std::vector<size_t>& hidden_sizes,
        size_t num_residual_blocks = 3) const;

    std::vector<LayerConfig> create_attention_architecture(
        size_t input_size,
        size_t hidden_size,
        size_t num_attention_heads = 8) const;

    std::vector<LayerConfig> create_hierarchical_architecture(
        size_t input_size,
        const std::vector<std::vector<size_t>>& level_sizes,
        const std::vector<std::string>& level_activations) const;

private:
    std::mt19937 rng_;
};

/**
 * @brief Attention mechanisms for cognitive modeling and transformer-like architectures
 */
class AttentionMechanism {
public:
    struct AttentionConfig {
        std::string attention_type;           // "self", "multi_head", "cross", "additive"
        size_t embedding_dim;
        size_t num_heads;
        size_t dropout_rate;
        bool use_bias;
        std::string activation;
    };

    AttentionMechanism();

    // Multi-head self-attention
    std::vector<float> multi_head_self_attention(
        const std::vector<float>& queries,
        const std::vector<float>& keys,
        const std::vector<float>& values,
        const AttentionConfig& config);

    // Scaled dot-product attention
    std::vector<float> scaled_dot_product_attention(
        const std::vector<float>& query,
        const std::vector<float>& keys,
        const std::vector<float>& values,
        float scale_factor = 1.0f);

    // Additive attention (Bahdanau)
    std::vector<float> additive_attention(
        const std::vector<float>& query,
        const std::vector<float>& memory,
        const std::vector<float>& mask = std::vector<float>());

    // Luong attention (dot-product, general, concat)
    std::vector<float> luong_attention(
        const std::vector<float>& query,
        const std::vector<float>& memory,
        const std::string& attention_type = "dot",
        const std::vector<float>& W = std::vector<float>());

    // Transformer encoder layer
    std::vector<float> transformer_encoder_layer(
        const std::vector<float>& input,
        const AttentionConfig& attention_config,
        float layer_norm_eps = 1e-6f);

    // Position encoding
    std::vector<float> positional_encoding(
        size_t sequence_length,
        size_t embedding_dim,
        std::string encoding_type = "sinusoidal");

    // Generate attention weights visualization
    std::string generate_attention_weights_plot(
        const std::vector<float>& attention_weights,
        const std::string& filename = "attention_weights.png");

private:
    std::mt19937 rng_;
};

/**
 * @brief Hierarchical network support for deep hierarchical representations
 */
class HierarchicalNetwork {
public:
    struct HierarchicalConfig {
        std::vector<size_t> layer_sizes;      // Sizes for each hierarchical level
        std::vector<std::string> level_types; // "encoding", "integration", "pooling"
        std::vector<size_t> pooling_sizes;    // Pooling factors for each level
        bool use_skip_connections;
        std::string pooling_type;             // "max", "avg", "attention"
    };

    HierarchicalNetwork();

    // Build hierarchical network
    std::shared_ptr<NeuralNetwork> build_hierarchical_network(
        size_t input_size,
        const HierarchicalConfig& config,
        const std::string& output_activation = "linear");

    // Process input through hierarchical levels
    std::vector<std::vector<float>> hierarchical_forward(
        const std::vector<float>& input,
        const HierarchicalConfig& config);

    // Multi-scale feature extraction
    std::vector<float> multi_scale_feature_extraction(
        const std::vector<float>& input,
        const std::vector<size_t>& scales,
        const std::vector<std::string>& feature_types = std::vector<std::string>());

    // Hierarchical attention mechanism
    std::vector<float> hierarchical_attention(
        const std::vector<float>& input,
        const HierarchicalConfig& config,
        size_t attention_head = 1);

    // Generate hierarchical feature visualizations
    std::string visualize_hierarchical_features(
        const std::vector<std::vector<float>>& features,
        const std::string& filename = "hierarchical_features.png");

private:
    std::mt19937 rng_;
};

} // namespace experimental
} // namespace nlm