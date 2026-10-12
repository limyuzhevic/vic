#pragma once

#include "../Plugin.hpp"
#include <random>

namespace nlm {

class SpikeBasedSensoryPlugin : public Plugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "SensorySystem",
            "Sensory input system for environmental perception and feature extraction",
            "NLM Team",
            PluginVersion(1, 0, 0),
            PluginType::Sensory
        );
        return metadata;
    }
    
    bool onInitialize() override {
        // Sensory system parameters
        input_sensitivity_ = 1.0f;           // Input sensitivity
        noise_level_ = 0.01f;               // Noise level
        feature_extraction_ = true;         // Enable feature extraction
        spike_encoding_ = true;             // Enable spike-based encoding
        
        // Initialize random generator for noise
        rng_.seed(42);  // Fixed seed for reproducibility
        
        return true;
    }
    
    void enable() override {
        Plugin::enable();
        // Register extension point for sensory system creation
        if (getManager()) {
            getManager()->registerExtensionPoint("sensory_system", PluginType::Sensory,
                [this](std::shared_ptr<Plugin>) { return true; });
        }
    }
    
    // Process sensory input and generate spikes
    std::vector<float> processInput(const std::vector<float>& rawInput) {
        std::vector<float> processedInput;
        processedInput.reserve(rawInput.size());
        
        for (size_t i = 0; i < rawInput.size(); ++i) {
            float value = rawInput[i];
            
            // Add noise if enabled
            if (noise_level_ > 0.0f) {
                std::normal_distribution<float> distribution(0.0f, noise_level_);
                value += distribution(rng_);
            }
            
            // Apply sensitivity scaling
            value *= input_sensitivity_;
            
            // Spike-based encoding
            if (spike_encoding_) {
                // Convert to spike train using rate coding
                float firingRate = std::max(0.0f, std::min(100.0f, value * 10.0f));  // Scale to Hz
                float spikeProbability = firingRate * 0.001f;  // Probability per timestep
                
                if (spikeProbability > 0.0f && (rng_() % 1000) < spikeProbability * 1000) {
                    processedInput.push_back(1.0f);  // Spike
                } else {
                    processedInput.push_back(0.0f);  // No spike
                }
            } else {
                processedInput.push_back(value);
            }
        }
        
        return processedInput;
    }
    
    // Extract features from input
    std::vector<float> extractFeatures(const std::vector<float>& input) {
        std::vector<float> features;
        
        if (input.empty()) return features;
        
        // Simple feature extraction
        float sum = 0.0f;
        float max = input[0];
        float min = input[0];
        
        for (float value : input) {
            sum += value;
            max = std::max(max, value);
            min = std::min(min, value);
        }
        
        float mean = sum / input.size();
        
        features.push_back(mean);      // Mean
        features.push_back(max - min); // Range
        features.push_back(mean * mean);// Variance (approximation)
        
        return features;
    }
    
    // Get extension points
    std::vector<std::string> getExtensionPoints() const override {
        return {"sensory_system", "input_processing", "feature_extraction"};
    }
    
    // Get services
    std::vector<std::string> getProvidedServices() const override {
        return {"sensory_input", "environmental_perception", "feature_extraction"};
    }
    
    // Update method
    void update(float dt) override {
        currentTime_ += dt;
    }
    
    // Configuration methods
    void setInputSensitivity(float sensitivity) { input_sensitivity_ = sensitivity; }
    float getInputSensitivity() const { return input_sensitivity_; }
    
    void setNoiseLevel(float level) { noise_level_ = level; }
    float getNoiseLevel() const { return noise_level_; }
    
    void enableFeatureExtraction(bool enable) { feature_extraction_ = enable; }
    bool isFeatureExtractionEnabled() const { return feature_extraction_; }
    
    void enableSpikeEncoding(bool enable) { spike_encoding_ = enable; }
    bool isSpikeEncodingEnabled() const { return spike_encoding_; }
    
private:
    float input_sensitivity_ = 1.0f;      // Input sensitivity scaling
    float noise_level_ = 0.01f;          // Noise level for stochasticity
    bool feature_extraction_ = true;     // Enable feature extraction
    bool spike_encoding_ = true;         // Enable spike-based encoding
    float currentTime_ = 0.0f;           // Current simulation time
    std::mt19937 rng_;                   // Random number generator
};

} // namespace nlm