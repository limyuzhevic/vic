#include "AdvancedNeuralNetwork.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <stdexcept>

namespace nlm {

void DendriticNeuralNetwork::Layer::forwardPropagate(const std::vector<float>& inputs, float dt) {
    // Ensure we have enough inputs for the layer
    size_t input_size = std::min(inputs.size(), activation_output.size());
    
    // Initialize activation output with inputs
    activation_output.assign(inputs.begin(), inputs.begin() + input_size);
    
    // Apply dendritic computations
    for (auto& dendrite : dendritic_units) {
        if (dendrite.inputs.size() > 0) {
            float total_input = 0.0f;
            for (size_t i = 0; i < std::min(dendrite.inputs.size(), dendrite.weights.size()); ++i) {
                total_input += dendrite.inputs[i] * dendrite.weights[i];
            }
            total_input += dendrite.biases.size() > 0 ? dendrite.biases[0] : 0.0f;
            
            // Update dendritic potential
            dendrite.dendritic_potentials.push_back(
                dendrite.dendritic_potentials.empty() ? total_input :
                dendrite.dendritic_potentials.back() * dendrite.synaptic_decay + total_input
            );
            
            // Apply threshold-based activation
            float potential = dendrite.dendritic_potentials.back();
            float activation = potential >= dendrite.dendritic_threshold ? potential : 0.0f;
            
            // Apply activation function with scaling
            activation = activation * activation_scale + activation_shift;
            activation_output.push_back(activation);
        }
    }
}

void DendriticNeuralNetwork::Layer::updateDendriticStates(float dt) {
    for (auto& dendrite : dendritic_units) {
        if (!dendrite.dendritic_potentials.empty()) {
            // Apply exponential decay
            for (auto& potential : dendrite.dendritic_potentials) {
                potential *= std::exp(-dt / dendrite.integration_time_constant);
            }
        }
    }
}

void DendriticNeuralNetwork::Layer::computeActivationOutputs() {
    for (auto& dendrite : dendritic_units) {
        if (!dendrite.dendritic_potentials.empty()) {
            float max_potential = *std::max_element(dendrite.dendritic_potentials.begin(),
                                                   dendrite.dendritic_potentials.end());
            float sum_potential = std::accumulate(dendrite.dendritic_potentials.begin(),
                                                 dendrite.dendritic_potentials.end(), 0.0f);
            
            // Compute output based on max potential (winner-take-all style)
            float output = max_potential > dendrite.dendritic_threshold ? max_potential : 0.0f;
            output = output * activation_scale + activation_shift;
            
            // Store the last output
            if (!dendrite.outputs.empty()) {
                dendrite.outputs.back() = output;
            }
        }
    }
}

void DendriticNeuralNetwork::buildNetwork(int depth, int width, int branching_factor) {
    depth_ = depth;
    width_ = width;
    branching_factor_ = branching_factor;
    
    layers_.clear();
    layer_inputs_.clear();
    layer_errors_.clear();
    
    // Create layers with dendritic units
    for (int l = 0; l < depth_; ++l) {
        Layer layer;
        int effective_width = width_;
        
        // Apply branching factor
        if (l > 0) {
            effective_width = std::max(1, width_ / (l + 1));
        }
        
        // Create dendritic units
        int num_units = effective_width * branching_factor_;
        for (int u = 0; u < num_units; ++u) {
            DendriticCompartment dendrite;
            // Initialize with random weights
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> weight_dist(-1.0f, 1.0f);
            
            for (int w = 0; w < 10; ++w) {  // Assume 10 inputs per unit
                dendrite.weights.push_back(weight_dist(gen));
            }
            
            for (int b = 0; b < 5; ++b) {  // 5 biases per unit
                dendrite.biases.push_back(weight_dist(gen) * 0.1f);
            }
            
            dendrite.inputs.resize(10, 0.0f);
            dendrite.outputs.resize(1, 0.0f);
            dendrite.dendritic_potentials.resize(1, 0.0f);
            
            layer.dendritic_units.push_back(dendrite);
        }
        
        // Set connectivity pattern
        layer.connectivity_pattern.resize(num_units);
        for (auto& conn : layer.connectivity_pattern) {
            // Create sparse connectivity
            int connections = std::max(1, num_units / 5);
            conn.resize(connections);
            for (int& c : conn) {
                c = rand() % num_units;
            }
        }
        
        layers_.push_back(layer);
    }
}

std::vector<float> DendriticNeuralNetwork::forwardPropagate(const std::vector<float>& inputs, float dt) {
    std::vector<float> current_output = inputs;
    layer_inputs_.clear();
    
    // Forward propagate through each layer
    for (size_t l = 0; l < layers_.size(); ++l) {
        layer_inputs_.push_back(current_output);
        
        // Update dendritic states
        layers_[l].updateDendriticStates(dt);
        
        // Forward propagate through layer
        layers_[l].forwardPropagate(current_output, dt);
        
        // Compute activation outputs
        layers_[l].computeActivationOutputs();
        
        // Collect outputs from all units
        current_output.clear();
        for (const auto& dendrite : layers_[l].dendritic_units) {
            if (!dendrite.outputs.empty()) {
                current_output.push_back(dendrite.outputs[0]);
            }
        }
        
        if (current_output.empty()) {
            break;
        }
    }
    
    current_output_ = current_output;
    return current_output_;
}

void DendriticNeuralNetwork::train(const std::vector<std::vector<float>>& training_inputs,
                                  const std::vector<std::vector<float>>& targets, int epochs) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int epoch = 0; epoch < epochs; ++epoch) {
        float total_loss = 0.0f;
        
        for (size_t i = 0; i < training_inputs.size(); ++i) {
            // Forward pass
            std::vector<float> output = forwardPropagate(training_inputs[i], 0.01f);
            
            // Compute error
            std::vector<float> error(targets[i].size(), 0.0f);
            for (size_t j = 0; j < std::min(output.size(), targets[i].size()); ++j) {
                error[j] = targets[i][j] - output[j];
                total_loss += error[j] * error[j];
            }
            
            // Update weights
            updateWeights(training_inputs[i], error, 0.01f);
        }
        
        // Log progress
        if (epoch % 100 == 0) {
            // In real implementation, would log to file or console
        }
    }
}

void DendriticNeuralNetwork::updateWeights(const std::vector<std::vector<float>>& inputs,
                                         const std::vector<std::vector<float>>& errors, float dt) {
    // Simplified weight update (backpropagation approximation)
    for (size_t l = 0; l < layers_.size(); ++l) {
        // Get input to this layer
        const std::vector<float>& layer_input = (l < layer_inputs_.size()) ? 
                                               layer_inputs_[l] : std::vector<float>(width_, 0.0f);
        
        // Update each layer's weights
        for (size_t u = 0; u < layers_[l].dendritic_units.size(); ++u) {
            auto& dendrite = layers_[l].dendritic_units[u];
            
            // Update weights based on error signal
            for (size_t w = 0; w < dendrite.weights.size(); ++w) {
                float gradient = learning_rate_ * errors.empty() ? 0.0f : errors[0][w % errors[0].size()];
                dendrite.weights[w] += gradient * layer_input[w % layer_input.size()] * dt;
                
                // Apply weight constraints
                dendrite.weights[w] = std::clamp(dendrite.weights[w], -10.0f, 10.0f);
            }
            
            // Update biases
            for (size_t b = 0; b < dendrite.biases.size(); ++b) {
                float gradient = learning_rate_ * errors.empty() ? 0.0f : errors[0][b];
                dendrite.biases[b] += gradient * dt;
                
                // Apply bias constraints
                dendrite.biases[b] = std::clamp(dendrite.biases[b], -5.0f, 5.0f);
            }
        }
    }
}

void AdvancedAttention::MultiHeadAttention::forward(const std::vector<std::vector<float>>& inputs) {
    // Simplified multi-head attention implementation
    combined_output = inputs;
    attention_weights.resize(inputs.size(), std::vector<float>(inputs.size(), 0.0f));
    gating_signals.resize(inputs.size(), std::vector<float>(inputs.size(), 0.0f));
}

void AdvancedAttention::MultiHeadAttention::computeAttentionWeights(
    const std::vector<std::vector<float>>& queries,
    const std::vector<std::vector<float>>& keys,
    const std::vector<std::vector<float>>& values) {
    
    if (queries.empty() || keys.empty()) return;
    
    size_t seq_len = queries.size();
    
    // Compute attention scores (simplified)
    attention_weights.resize(seq_len, std::vector<float>(seq_len, 0.0f));
    
    for (size_t i = 0; i < seq_len; ++i) {
        for (size_t j = 0; j < seq_len; ++j) {
            // Simple dot product similarity
            float score = 0.0f;
            for (size_t k = 0; k < std::min(queries[i].size(), keys[j].size()); ++k) {
                score += queries[i][k] * keys[j][k];
            }
            
            // Apply temperature scaling
            score /= std::sqrt(static_cast<float>(std::min(queries[i].size(), keys[j].size())));
            
            // Apply softmax
            float max_score = 0.0f;
            for (size_t k = 0; k < seq_len; ++k) {
                max_score = std::max(max_score, attention_weights[k][j]);
            }
            
            float exp_score = std::exp(score - max_score);
            float sum_exp = 0.0f;
            for (size_t k = 0; k < seq_len; ++k) {
                sum_exp += std::exp(attention_weights[k][j] - max_score);
            }
            
            attention_weights[i][j] = exp_score / (sum_exp + 1e-8f);
        }
    }
}

void AdvancedAttention::MultiHeadAttention::applyGatingSignals(
    const std::vector<float>& context_signals) {
    
    if (context_signals.empty()) return;
    
    // Apply gating to attention weights
    for (size_t i = 0; i < attention_weights.size(); ++i) {
        for (size_t j = 0; j < attention_weights[i].size(); ++j) {
            // Simple gating mechanism
            float context_score = 0.0f;
            for (float signal : context_signals) {
                context_score += signal;
            }
            context_score /= context_signals.size();
            
            gating_signals[i][j] = context_score * gating_strength;
            
            // Apply gating
            attention_weights[i][j] *= gating_signals[i][j];
        }
    }
}

void AdvancedAttention::PositionalEncoding::computeSinusoidalEncoding(int max_len, int d_model) {
    sinusoidal_encoding.resize(max_len, std::vector<float>(d_model, 0.0f));
    
    for (int pos = 0; pos < max_len; ++pos) {
        for (int i = 0; i < d_model; i += 2) {
            float angle = static_cast<float>(pos) / std::pow(10000.0f, static_cast<float>(i) / d_model);
            sinusoidal_encoding[pos][i] = std::sin(angle);
            if (i + 1 < d_model) {
                sinusoidal_encoding[pos][i + 1] = std::cos(angle);
            }
        }
    }
}

void AdvancedAttention::PositionalEncoding::computeRelativeEncoding(int seq_len) {
    relative_position_encoding.resize(seq_len, std::vector<float>(seq_len, 0.0f));
    
    for (int i = 0; i < seq_len; ++i) {
        for (int j = 0; j < seq_len; ++j) {
            int pos_diff = i - j;
            relative_position_encoding[i][j] = static_cast<float>(pos_diff) / seq_len;
        }
    }
}

std::vector<std::vector<float>> AdvancedAttention::computeMultiHeadAttention(
    const std::vector<std::vector<float>>& queries,
    const std::vector<std::vector<float>>& keys,
    const std::vector<std::vector<float>>& values,
    const std::vector<float>& attention_mask) {
    
    if (queries.empty() || keys.empty() || values.empty()) return {};
    
    // Simplified multi-head attention implementation
    std::vector<std::vector<float>> output = values;
    
    // Compute attention scores
    std::vector<std::vector<float>> attention_scores = computeAdvancedAttentionScores(queries, keys, attention_mask);
    
    // Apply attention to values
    for (size_t i = 0; i < output.size(); ++i) {
        for (size_t j = 0; j < output[i].size(); ++j) {
            output[i][j] = 0.0f;
            for (size_t k = 0; k < attention_scores.size(); ++k) {
                output[i][j] += attention_scores[k][j] * values[k][j];
            }
        }
    }
    
    return output;
}

std::vector<std::vector<float>> AdvancedAttention::applyPositionalEncoding(
    const std::vector<std::vector<float>>& inputs,
    PositionalEncoding& encoding) {
    
    if (inputs.empty()) return {};
    
    std::vector<std::vector<float>> output = inputs;
    
    // Add positional encoding
    for (size_t i = 0; i < inputs.size(); ++i) {
        for (size_t j = 0; j < inputs[i].size(); ++j) {
            if (i < encoding.sinusoidal_encoding.size() && 
                j < encoding.sinusoidal_encoding[i].size()) {
                output[i][j] += encoding.sinusoidal_encoding[i][j];
            }
        }
    }
    
    return output;
}

std::vector<std::vector<float>> AdvancedAttention::computeAdvancedAttentionScores(
    const std::vector<std::vector<float>>& queries,
    const std::vector<std::vector<float>>& keys,
    const std::vector<float>& mask) {
    
    if (queries.empty() || keys.empty()) return {};
    
    std::vector<std::vector<float>> scores;
    size_t seq_len = queries.size();
    
    // Compute dot product between queries and keys
    scores.resize(seq_len, std::vector<float>(keys.size(), 0.0f));
    
    for (size_t i = 0; i < seq_len; ++i) {
        for (size_t j = 0; j < keys.size(); ++j) {
            float score = 0.0f;
            for (size_t k = 0; k < std::min(queries[i].size(), keys[j].size()); ++k) {
                score += queries[i][k] * keys[j][k];
            }
            
            // Apply temperature scaling
            score /= std::sqrt(static_cast<float>(std::min(queries[i].size(), keys[j].size())));
            
            // Apply mask if provided
            if (!mask.empty() && j < mask.size()) {
                score += mask[j];  // Add large negative value for masked positions
            }
            
            scores[i][j] = score;
        }
    }
    
    return scores;
}

void ResidualNetwork::ResidualBlock::forward(std::vector<std::vector<float>>& input,
                                          float skip_scale) {
    if (input.empty()) return;
    
    // Store original input for skip connection
    std::vector<std::vector<float>> original_input = input;
    
    // Apply convolution (simplified)
    for (size_t i = 0; i < input.size(); ++i) {
        for (size_t j = 0; j < input[i].size(); ++j) {
            float new_value = 0.0f;
            
            // Simple convolution with kernel size 3x3 (simplified)
            for (int ki = -1; ki <= 1; ++ki) {
                for (int kj = -1; kj <= 1; ++kj) {
                    int ni = i + ki;
                    int nj = j + kj;
                    
                    if (ni >= 0 && ni < static_cast<int>(input.size()) &&
                        nj >= 0 && nj < static_cast<int>(input[i].size())) {
                        
                        float kernel_value = 0.0f;
                        if (!conv_weights.empty() && 
                            ki + 1 < static_cast<int>(conv_weights.size()) &&
                            kj + 1 < static_cast<int>(conv_weights[0].size())) {
                            kernel_value = conv_weights[ki + 1][kj + 1];
                        }
                        
                        new_value += input[ni][nj] * kernel_value;
                    }
                }
            }
            
            // Apply activation
            float activation = new_value;
            if (activation_function_ == "relu") {
                activation = std::max(0.0f, activation);
            } else if (activation_function_ == "leaky_relu") {
                activation = activation > 0 ? activation : activation * 0.01f;
            } else if (activation_function_ == "sigmoid") {
                activation = 1.0f / (1.0f + std::exp(-activation));
            }
            
            input[i][j] = activation;
        }
    }
    
    // Apply batch normalization (simplified)
    if (use_batch_norm_) {
        for (size_t i = 0; i < input.size(); ++i) {
            for (size_t j = 0; j < input[i].size(); ++j) {
                float normalized = input[i][j] - batch_norm_mean[j];
                normalized /= std::sqrt(batch_norm_var[j] + 1e-5f);
                input[i][j] = normalized * batch_norm_gamma[j] + batch_norm_beta[j];
            }
        }
    }
    
    // Apply skip connection
    if (use_projection || skip_connection_scale != 0.0f) {
        for (size_t i = 0; i < std::min(input.size(), original_input.size()); ++i) {
            for (size_t j = 0; j < std::min(input[i].size(), original_input[i].size()); ++j) {
                input[i][j] += skip_connection_scale * original_input[i][j];
            }
        }
    }
}

void ResidualNetwork::ResidualBlock::updateBatchNormStats(const std::vector<std::vector<float>>& input,
                                                        float momentum) {
    if (input.empty()) return;
    
    // Update batch norm statistics (running average)
    size_t dim = input[0].size();
    
    // Compute batch statistics
    std::vector<float> batch_mean(dim, 0.0f);
    std::vector<float> batch_var(dim, 0.0f);
    
    for (const auto& row : input) {
        for (size_t j = 0; j < dim; ++j) {
            batch_mean[j] += row[j];
        }
    }
    
    for (size_t j = 0; j < dim; ++j) {
        batch_mean[j] /= input.size();
        
        float variance = 0.0f;
        for (const auto& row : input) {
            variance += (row[j] - batch_mean[j]) * (row[j] - batch_mean[j]);
        }
        batch_var[j] = variance / input.size();
    }
    
    // Update running statistics
    for (size_t j = 0; j < dim; ++j) {
        batch_norm_mean[j] = (1.0f - momentum) * batch_norm_mean[j] + 
                           momentum * batch_mean[j];
        batch_norm_var[j] = (1.0f - momentum) * batch_norm_var[j] + 
                          momentum * batch_var[j];
    }
}

void ResidualNetwork::buildResidualNetwork(int num_blocks, int input_dim, int hidden_dim, int output_dim) {
    num_residual_blocks_ = num_blocks;
    
    // Initialize residual blocks
    residual_blocks_.clear();
    
    for (int b = 0; b < num_blocks; ++b) {
        ResidualBlock block;
        
        // Initialize kernel weights
        block.conv_weights.resize(3, std::vector<float>(3, 0.0f));
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
        
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                block.conv_weights[i][j] = dist(gen);
            }
        }
        
        // Initialize batch norm parameters
        block.batch_norm_gamma.resize(64, 1.0f);  // Assume 64 channels
        block.batch_norm_beta.resize(64, 0.0f);
        block.batch_norm_mean.resize(64, 0.0f);
        block.batch_norm_var.resize(64, 1.0f);
        
        residual_blocks_.push_back(block);
    }
}

std::vector<float> ResidualNetwork::forward(const std::vector<float>& input) {
    if (input.empty()) return {};
    
    // Convert input to 2D representation (simplified)
    std::vector<std::vector<float>> current_input;
    int input_size = static_cast<int>(input.size());
    int cols = static_cast<int>(std::sqrt(input_size));
    
    for (int i = 0; i < cols; ++i) {
        std::vector<float> row;
        for (int j = 0; j < cols; ++j) {
            int idx = i * cols + j;
            if (idx < input_size) {
                row.push_back(input[idx]);
            } else {
                row.push_back(0.0f);
            }
        }
        current_input.push_back(row);
    }
    
    // Apply residual blocks
    for (size_t b = 0; b < residual_blocks_.size(); ++b) {
        residual_blocks_[b].forward(current_input);
        current_input = residual_blocks_[b].forward(current_input);
    }
    
    // Extract output from the last layer
    std::vector<float> output;
    for (const auto& row : current_input) {
        output.insert(output.end(), row.begin(), row.end());
    }
    
    return output;
}

void ResidualNetwork::trainDeep(const std::vector<std::vector<float>>& inputs,
                               const std::vector<std::vector<float>>& targets, int epochs) {
    // Simplified training for residual network
    for (int epoch = 0; epoch < epochs; ++epoch) {
        float total_loss = 0.0f;
        
        for (size_t i = 0; i < inputs.size(); ++i) {
            // Forward pass
            std::vector<float> output = forward(inputs[i]);
            
            // Compute loss
            float loss = 0.0f;
            for (size_t j = 0; j < std::min(output.size(), targets[i].size()); ++j) {
                float error = output[j] - targets[i][j];
                loss += error * error;
            }
            
            total_loss += loss;
            
            // In real implementation, would perform backpropagation
        }
        
        if (epoch % 100 == 0) {
            // In real implementation, would log loss
        }
    }
}

void HierarchicalFeatureExtractor::HierarchicalLayer::extractFeatures(
    const std::vector<std::vector<float>>& input) {
    
    if (input.empty()) return;
    
    // Apply different feature extraction operations
    for (size_t i = 0; i < input.size(); ++i) {
        feature_maps[i].features.resize(input[i].size(), 0.0f);
        
        // Apply various feature extraction techniques
        for (size_t j = 0; j < input[i].size(); ++j) {
            float value = input[i][j];
            
            // Local feature detection (simplified Gabor filter)
            float local_feature = value * std::sin(2.0f * M_PI * j / 10.0f);
            
            // Edge detection (simplified Sobel operator)
            float edge_feature = 0.0f;
            if (i > 0 && i < input.size() - 1) {
                edge_feature = (input[i + 1][j] - input[i - 1][j]) * 0.5f;
            }
            
            // Texture feature
            float texture_feature = 0.0f;
            if (j > 0 && j < input[i].size() - 1) {
                texture_feature = (input[i][j + 1] - input[i][j - 1]) * 0.5f;
            }
            
            // Combine features
            feature_maps[i].features[j] = local_feature + edge_feature + texture_feature;
            
            // Apply non-linear activation
            feature_maps[i].features[j] = std::tanh(feature_maps[i].features[j]);
        }
    }
}

void HierarchicalFeatureExtractor::HierarchicalLayer::applyPooling(
    const std::vector<FeatureMap>& input) {
    
    for (size_t i = 0; i < input.size(); ++i) {
        feature_maps[i].features.clear();
        feature_maps[i].spatial_dimensions.clear();
        
        // Apply max pooling (simplified)
        for (size_t y = 0; y < input[i].spatial_dimensions[0]; y += 2) {
            std::vector<float> pooled_row;
            for (size_t x = 0; x < input[i].spatial_dimensions[1]; x += 2) {
                float max_val = -std::numeric_limits<float>::max();
                
                for (int dy = 0; dy < 2 && y + dy < input[i].spatial_dimensions[0]; ++dy) {
                    for (int dx = 0; dx < 2 && x + dx < input[i].spatial_dimensions[1]; ++dx) {
                        size_t orig_y = y + dy;
                        size_t orig_x = x + dx;
                        size_t idx = orig_y * input[i].spatial_dimensions[1] + orig_x;
                        
                        if (orig_y < input[i].features.size() && 
                            orig_x < input[i].features[orig_y].size()) {
                            max_val = std::max(max_val, input[i].features[orig_y][orig_x]);
                        }
                    }
                }
                
                pooled_row.push_back(max_val);
            }
            
            feature_maps[i].features.push_back(pooled_row);
        }
        
        feature_maps[i].spatial_dimensions[0] = feature_maps[i].features.size();
        feature_maps[i].spatial_dimensions[1] = feature_maps[i].features.empty() ? 0 : 
                                                  feature_maps[i].features[0].size();
    }
}

void HierarchicalFeatureExtractor::HierarchicalLayer::fuseMultiScaleFeatures() {
    // Fuse features from different scales
    for (auto& feature_map : feature_maps) {
        // Multi-scale fusion
        std::vector<std::vector<float>> fused_features = feature_map.features;
        
        // Apply multi-scale aggregation
        for (size_t y = 1; y < feature_map.features.size() - 1; ++y) {
            for (size_t x = 1; x < feature_map.features[y].size() - 1; ++x) {
                // Average surrounding features
                float sum = 0.0f;
                int count = 0;
                
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        size_t ny = y + dy;
                        size_t nx = x + dx;
                        
                        if (ny < feature_map.features.size() && nx < feature_map.features[ny].size()) {
                            sum += feature_map.features[ny][nx];
                            count++;
                        }
                    }
                }
                
                if (count > 0) {
                    fused_features[y][x] = sum / count;
                }
            }
        }
        
        feature_map.features = fused_features;
    }
}

std::vector<HierarchicalFeatureExtractor::FeatureMap> HierarchicalFeatureExtractor::extractHierarchicalFeatures(
    const std::vector<float>& input) {
    
    if (input.empty()) return {};
    
    // Convert input to 2D representation
    size_t input_dim = input.size();
    size_t side = static_cast<size_t>(std::sqrt(input_dim));
    
    std::vector<std::vector<float>> input_2d(side, std::vector<float>(side, 0.0f));
    for (size_t i = 0; i < input_dim; ++i) {
        size_t row = i / side;
        size_t col = i % side;
        if (row < side && col < side) {
            input_2d[row][col] = input[i];
        }
    }
    
    // Apply hierarchical layers
    hierarchical_features_.clear();
    
    // Level 1: Low-level features
    hierarchical_layers_[0].feature_maps.resize(1);
    hierarchical_layers_[0].feature_maps[0].features = input_2d;
    hierarchical_layers_[0].feature_maps[0].spatial_dimensions = {side, side};
    hierarchical_layers_[0].feature_maps[0].channel_depth = 1;
    
    hierarchical_layers_[0].extractFeatures(input_2d);
    hierarchical_layers_[0].applyPooling(hierarchical_layers_[0].feature_maps);
    
    // Higher levels
    for (size_t l = 1; l < hierarchical_layers_.size(); ++l) {
        hierarchical_layers_[l].extractFeatures(hierarchical_layers_[l-1].feature_maps[0].features);
        hierarchical_layers_[l].applyPooling(hierarchical_layers_[l-1].feature_maps);
        hierarchical_layers_[l].fuseMultiScaleFeatures();
    }
    
    return hierarchical_layers_.back().feature_maps;
}

std::vector<std::vector<float>> HierarchicalFeatureExtractor::aggregateMultiScaleFeatures(
    const std::vector<FeatureMap>& feature_maps) {
    
    if (feature_maps.empty()) return {};
    
    std::vector<std::vector<float>> aggregated;
    
    // Aggregate features from different scales
    for (size_t i = 0; i < feature_maps.size(); ++i) {
        std::vector<float> scale_features;
        
        for (const auto& row : feature_maps[i].features) {
            scale_features.insert(scale_features.end(), row.begin(), row.end());
        }
        
        aggregated.push_back(scale_features);
    }
    
    return aggregated;
}

void HierarchicalFeatureExtractor::trainHierarchical(const std::vector<std::vector<float>>& training_inputs,
                                                    const std::vector<std::vector<float>>& targets, int epochs) {
    // Simplified hierarchical training
    for (int epoch = 0; epoch < epochs; ++epoch) {
        float total_loss = 0.0f;
        
        for (size_t i = 0; i < training_inputs.size(); ++i) {
            // Extract hierarchical features
            std::vector<FeatureMap> features = extractHierarchicalFeatures(training_inputs[i]);
            
            // Aggregate multi-scale features
            std::vector<std::vector<float>> aggregated = aggregateMultiScaleFeatures(features);
            
            // Compute loss (simplified)
            float loss = 0.0f;
            if (!aggregated.empty() && !targets[i].empty()) {
                for (size_t j = 0; j < std::min(aggregated[0].size(), targets[i].size()); ++j) {
                    float error = aggregated[0][j] - targets[i][j];
                    loss += error * error;
                }
            }
            
            total_loss += loss;
            
            // In real implementation, would perform backpropagation through hierarchy
        }
        
        if (epoch % 100 == 0) {
            // In real implementation, would log loss
        }
    }
}

float AdvancedActivations::swish(float x, float beta) {
    return x / (1.0f + std::exp(-beta * x));
}

float AdvancedActivations::mish(float x) {
    return x * std::tanh(std::log(1.0f + std::exp(x)));
}

float AdvancedActivations::gelu(float x, float approximate) {
    if (approximate == 1.0f) {
        // Approximate GELU
        return x * (1.0f + std::tanh(std::sqrt(2.0f / M_PI) * (x + 0.044715f * x * x * x)));
    } else {
        // Exact GELU
        return 0.5f * x * (1.0f + std::erf(x / std::sqrt(2.0f)));
    }
}

float AdvancedActivations::hardshrink(float x, float lambd) {
    if (x > lambd) return x - lambd;
    else if (x < -lambd) return x + lambd;
    else return 0.0f;
}

float AdvancedActivations::selu(float x, float alpha, float scale) {
    if (x >= 0) return scale * x;
    else return scale * alpha * (std::exp(x) - 1.0f);
}

float AdvancedActivations::spikeFunction(float x, float threshold, float refractory) {
    // Simplified spike function
    if (x >= threshold) {
        return 1.0f;
    } else {
        return 0.0f;
    }
}

float AdvancedActivations::dendriticActivation(float x, float dendrite_potential, float threshold) {
    // Dendritic activation considering local potential
    float combined_input = x + dendrite_potential;
    return combined_input >= threshold ? combined_input : 0.0f;
}

void AdvancedActivations::batchNorm(std::vector<float>& x, const std::vector<float>& mean,
                                   const std::vector<float>& var, float gamma, float beta,
                                   float epsilon) {
    if (x.empty() || mean.empty() || var.empty()) return;
    
    for (size_t i = 0; i < x.size(); ++i) {
        size_t idx = i % mean.size();  // Cycle through statistics
        float normalized = (x[i] - mean[idx]) / std::sqrt(var[idx] + epsilon);
        x[i] = gamma * normalized + beta;
    }
}

void AdvancedActivations::layerNorm(std::vector<float>& x, const std::vector<float>& gamma,
                                   const std::vector<float>& beta, float epsilon) {
    if (x.empty()) return;
    
    // Compute mean and variance
    float mean = std::accumulate(x.begin(), x.end(), 0.0f) / x.size();
    float var = 0.0f;
    for (float val : x) {
        var += (val - mean) * (val - mean);
    }
    var /= x.size();
    
    // Apply normalization
    for (size_t i = 0; i < x.size(); ++i) {
        float normalized = (x[i] - mean) / std::sqrt(var + epsilon);
        x[i] = gamma[i % gamma.size()] * normalized + beta[i % beta.size()];
    }
}

void AdvancedActivations::instanceNorm(std::vector<float>& x, const std::vector<float>& gamma,
                                      const std::vector<float>& beta, float epsilon) {
    // Simplified instance normalization (per feature)
    layerNorm(x, gamma, beta, epsilon);
}

void AdvancedActivations::groupNorm(std::vector<float>& x, const std::vector<float>& gamma,
                                   const std::vector<float>& beta, int num_groups, float epsilon) {
    // Simplified group normalization (uses layer norm for now)
    layerNorm(x, gamma, beta, epsilon);
}

float AdvancedActivations::swishDerivative(float x, float beta) {
    float sigmoid = 1.0f / (1.0f + std::exp(-beta * x));
    return sigmoid + beta * x * sigmoid * (1.0f - sigmoid);
}

float AdvancedActivations::mishDerivative(float x) {
    float tanh_val = std::tanh(std::log(1.0f + std::exp(x)));
    float sigmoid = 1.0f / (1.0f + std::exp(-x));
    return tanh_val + x * (1.0f - tanh_val * tanh_val) * sigmoid;
}

float AdvancedActivations::geluDerivative(float x, float approximate) {
    if (approximate == 1.0f) {
        // Approximate GELU derivative
        float tanh_val = std::tanh(std::sqrt(2.0f / M_PI) * (x + 0.044715f * x * x * x));
        return 0.5f * (1.0f + tanh_val) + 0.5f * x * (1.0f - tanh_val * tanh_val) * 
               std::sqrt(2.0f / M_PI) * (1.0f + 0.134145f * x * x);
    } else {
        // Exact GELU derivative
        float cdf = 0.5f * (1.0f + std::erf(x / std::sqrt(2.0f)));
        float pdf = std::exp(-x * x / 2.0f) / std::sqrt(2.0f * M_PI);
        return cdf + x * pdf;
    }
}

float AdvancedActivations::adaptiveActivation(float x, const std::string& activation_type,
                                              float parameter, float threshold) {
    if (activation_type == "swish") {
        return swish(x, parameter);
    } else if (activation_type == "mish") {
        return mish(x);
    } else if (activation_type == "gelu") {
        return gelu(x, parameter);
    } else if (activation_type == "hardshrink") {
        return hardshrink(x, parameter);
    } else if (activation_type == "selu") {
        return selu(x, parameter, 1.0f);
    } else if (activation_type == "spike") {
        return spikeFunction(x, threshold, 0.0f);
    } else {
        // Default to ReLU
        return std::max(0.0f, x);
    }
}

} // namespace nlm
