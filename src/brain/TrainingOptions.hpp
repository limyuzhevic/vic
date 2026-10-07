#pragma once

#include <string>
#include <vector>
#include <functional>

namespace nlm {

// Training data structure for supervised learning
struct TrainingData {
    // Input data
    std::vector<float> input;              // Input features
    std::vector<float> target;             // Target values
    float reward = 1.0f;                   // Associated reward (for reinforcement learning)
    
    // Metadata
    std::string label;                     // Sample label
    size_t classId = 0;                     // Class ID for classification
    double timestamp = 0.0;                 // Data collection timestamp
    
    // Processing flags
    bool processed = false;                 // Whether this data has been processed
    bool used = false;                      // Whether this data has been used in training
    
    TrainingData() : reward(1.0f), classId(0), timestamp(0.0), processed(false), used(false) {}
    
    TrainingData(const std::vector<float>& inputData, const std::vector<float>& targetData)
        : input(inputData), target(targetData), reward(1.0f), classId(0), timestamp(0.0),
          processed(false), used(false) {}
};

// Neural network layer configuration
struct NeuralNetworkLayer {
    size_t neurons;                         // Number of neurons in this layer
    std::string activation;                // Activation function (relu, sigmoid, tanh, linear)
    bool bias = true;                      // Whether to include bias term
    float learningRate = 0.01f;            // Learning rate for this layer
    float regularization = 0.001f;         // L2 regularization for this layer
    
    NeuralNetworkLayer() : neurons(100), activation("relu"), bias(true),
                          learningRate(0.01f), regularization(0.001f) {}
    
    NeuralNetworkLayer(size_t n, const std::string& act, bool b = true,
                      float lr = 0.01f, float reg = 0.001f)
        : neurons(n), activation(act), bias(b), learningRate(lr), regularization(reg) {}
};

// Neural network architecture configuration
struct NeuralNetworkConfig {
    std::vector<NeuralNetworkLayer> layers; // Network layers
    std::string outputActivation = "softmax";  // Output layer activation
    std::string optimizer = "adam";        // Optimizer (adam, sgd, rmsprop)
    bool useBatchNorm = false;              // Whether to use batch normalization
    bool useDropout = false;                // Whether to use dropout
    float dropoutRate = 0.5f;              // Dropout rate
    
    // Training hyperparameters
    float learningRate = 0.001f;           // Global learning rate
    float momentum = 0.9f;                 // Momentum
    float beta1 = 0.9f;                    // Adam beta1
    float beta2 = 0.999f;                  // Adam beta2
    float epsilon = 1e-8f;                 // Adam epsilon
    
    NeuralNetworkConfig() : outputActivation("softmax"), optimizer("adam"),
                           useBatchNorm(false), useDropout(false), dropoutRate(0.5f),
                           learningRate(0.001f), momentum(0.9f), beta1(0.9f), beta2(0.999f), epsilon(1e-8f) {}
};

} // namespace nlm