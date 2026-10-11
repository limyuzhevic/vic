#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <typeinfo>
#include <stdexcept>
#include <algorithm>

#include "SensoryInput.hpp"

namespace nlm {

// Forward declaration
class SensoryProcessor;
using SensoryProcessorPtr = std::unique_ptr<SensoryProcessor>;
using SensoryProcessorFactoryFunc = std::function<SensoryProcessorPtr()>;

// Abstract base class for all sensory processors
class SensoryProcessor {
public:
    virtual ~SensoryProcessor() = default;
    
    // Get processor type name
    virtual const char* getTypeName() const = 0;
    
    // Process sensory input and extract features
    virtual void process(const SensoryInput& input) = 0;
    
    // Get processed feature vector
    virtual const std::vector<float>& getFeatures() const = 0;
    
    // Get feature dimensionality
    virtual size_t getFeatureDimensions() const = 0;
    
    // Validate input before processing
    virtual bool validateInput(const SensoryInput& input) const {
        return !input.getData().empty();
    }
    
    // Reset internal state
    virtual void reset() {
        features_.clear();
        featureDimensions_ = 0;
    }
    
    // Get processing confidence (0.0 to 1.0)
    virtual double getProcessingConfidence() const {
        return 1.0;
    }
    
protected:
    std::vector<float> features_;
    size_t featureDimensions_ = 0;
    
    // Helper function to compute feature statistics
    std::pair<float, float> computeFeatureStats() const {
        if (features_.empty()) {
            return {0.0f, 0.0f};
        }
        
        float sum = 0.0f;
        float sumSq = 0.0f;
        for (float val : features_) {
            sum += val;
            sumSq += val * val;
        }
        
        float mean = sum / features_.size();
        float variance = (sumSq / features_.size()) - (mean * mean);
        float stddev = std::sqrt(std::max(0.0f, variance));
        
        return {mean, stddev};
    }
    
    // Normalize features to zero mean, unit variance
    void normalizeFeatures() {
        if (features_.empty()) return;
        
        auto [mean, stddev] = computeFeatureStats();
        if (stddev < 1e-6f) return;  // Avoid division by zero
        
        for (float& val : features_) {
            val = (val - mean) / stddev;
        }
    }
};

// Sensory processor factory
class SensoryProcessorFactory {
public:
    static SensoryProcessorFactory& getInstance() {
        static SensoryProcessorFactory instance;
        return instance;
    }
    
    // Register a sensory processor type
    template<typename ProcessorType>
    void registerProcessor(const std::string& name) {
        processors_[name] = []() -> SensoryProcessorPtr {
            return std::make_unique<ProcessorType>();
        };
    }
    
    // Create a processor by name
    SensoryProcessorPtr createProcessor(const std::string& name) {
        auto it = processors_.find(name);
        if (it == processors_.end()) {
            throw std::runtime_error("Unknown sensory processor: " + name);
        }
        return it->second();
    }
    
    // Check if processor type is registered
    bool isRegistered(const std::string& name) const {
        return processors_.find(name) != processors_.end();
    }
    
    // Get list of registered processor names
    std::vector<std::string> getRegisteredNames() const {
        std::vector<std::string> names;
        for (const auto& pair : processors_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Create processor based on input type
    SensoryProcessorPtr createForInput(const SensoryInput& input) {
        // Map input type to processor type
        std::string inputType = input.getType();
        
        if (inputType == "Vision") {
            return createProcessor("Vision");
        } else if (inputType == "Audio") {
            return createProcessor("Audio");
        } else if (inputType == "InternalSignals") {
            return createProcessor("InternalSignals");
        }
        
        throw std::runtime_error("No processor available for input type: " + inputType);
    }
    
private:
    SensoryProcessorFactory() = default;
    ~SensoryProcessorFactory() = default;
    
    // Delete copy and move operations
    SensoryProcessorFactory(const SensoryProcessorFactory&) = delete;
    SensoryProcessorFactory& operator=(const SensoryProcessorFactory&) = delete;
    SensoryProcessorFactory(SensoryProcessorFactory&&) = delete;
    SensoryProcessorFactory& operator=(SensoryProcessorFactory&&) = delete;
    
    std::map<std::string, SensoryProcessorFactoryFunc> processors_;
};

// Macro for registering processors
#define REGISTER_SENSORY_PROCESSOR(ProcessorClass, Name) \
    namespace { \
    struct ProcessorClass##_registrar { \
        ProcessorClass##_registrar() { \
            SensoryProcessorFactory::getInstance().registerProcessor<ProcessorClass>(Name); \
        } \
    }; \
    ProcessorClass##_registrar ProcessorClass##_instance; \
    } \

// Common neural processing utilities
namespace sensory_utils {
    
    // Compute sparse representation of features
    std::vector<float> computeSparseRepresentation(const std::vector<float>& features, 
                                                    float threshold = 0.1f) {
        std::vector<float> sparseFeatures;
        sparseFeatures.reserve(features.size());
        
        for (float val : features) {
            if (std::abs(val) > threshold) {
                sparseFeatures.push_back(val);
            }
        }
        
        return sparseFeatures;
    }
    
    // Compute temporal derivatives (simplified)
    std::vector<float> computeTemporalDerivative(const std::vector<float>& current,
                                                  const std::vector<float>& previous,
                                                  float dt = 1.0f) {
        std::vector<float> derivative;
        derivative.reserve(current.size());
        
        size_t size = std::min(current.size(), previous.size());
        for (size_t i = 0; i < size; ++i) {
            derivative.push_back((current[i] - previous[i]) / dt);
        }
        
        return derivative;
    }
    
    // Compute normalization factor
    float computeNormalizationFactor(const std::vector<float>& features) {
        if (features.empty()) return 1.0f;
        
        float maxAbs = 0.0f;
        for (float val : features) {
            maxAbs = std::max(maxAbs, std::abs(val));
        }
        
        return maxAbs > 0.0f ? maxAbs : 1.0f;
    }
    
    // Apply attention mechanism (simplified)
    std::vector<float> applyAttention(const std::vector<float>& features,
                                       const std::vector<float>& attentionWeights) {
        std::vector<float> attendedFeatures;
        attendedFeatures.reserve(features.size());
        
        for (size_t i = 0; i < features.size(); ++i) {
            if (i < attentionWeights.size()) {
                attendedFeatures.push_back(features[i] * attentionWeights[i]);
            } else {
                attendedFeatures.push_back(features[i]);
            }
        }
        
        return attendedFeatures;
    }
    
} // namespace sensory_utils

} // namespace nlm
