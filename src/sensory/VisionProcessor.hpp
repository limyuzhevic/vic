#ifndef VISION_PROCESSOR_H
#define VISION_PROCESSOR_H

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "Vision.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace nlm {

// Vision features extracted using biologically plausible methods
struct VisionFeatures {
    // Low-level edge features (horizontal and vertical)
    std::vector<float> horizontalEdges;
    std::vector<float> verticalEdges;
    
    // Texture features
    std::vector<float> textureSmoothness;
    std::vector<float> textureContrast;
    
    // Color features (if RGB)
    std::vector<float> colorFeatures;
    
    // Higher-level feature maps
    std::vector<float> orientedFeatures;
    std::vector<float> motionFeatures;
    
    // Spiking activity patterns
    std::vector<float> spikeRates;
    
    VisionFeatures(size_t width, size_t height) {
        size_t halfWidth = width / 2;
        size_t halfHeight = height / 2;
        
        horizontalEdges.resize(halfWidth * halfHeight, 0.0f);
        verticalEdges.resize(halfWidth * halfHeight, 0.0f);
        textureSmoothness.resize(halfWidth * halfHeight, 0.0f);
        textureContrast.resize(halfWidth * halfHeight, 0.0f);
        
        if (height > 1) {
            colorFeatures.resize(halfWidth * halfHeight, 0.0f);
        }
        
        orientedFeatures.resize(halfWidth * halfHeight, 0.0f);
        motionFeatures.resize(halfWidth * halfHeight, 0.0f);
        spikeRates.resize(halfWidth * halfHeight, 0.0f);
    }
};

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input through spike-based neural computation
    void process(const Vision& input);
    
    // Get processed feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get specific feature maps
    const std::vector<float>& getEdgeFeatures() const;
    const std::vector<float>& getTextureFeatures() const;
    
    // Access to internal neural representation
    const VisionFeatures& getInternalFeatures() const;
    
    // Reset processor state
    void reset();
    
    // Initialize with biological parameters
    void setParameters(float edgeThreshold, float textureSensitivity, 
                      float maxSpikeRate, float inhibitionStrength);
    
private:
    // Helper methods for neural vision processing
    void initializeNeuralComponents(size_t inputWidth, size_t inputHeight);
    void processThroughRetina(const std::vector<float>& inputData);
    void extractEdgeFeatures(const std::vector<float>& inputData);
    void computeTextureFeatures(const std::vector<float>& inputData);
    void convertToSpikes(const std::vector<float>& features);
    void applyInhibition(std::vector<float>& features, float strength);
    void combineFeatures(VisionFeatures& output);
    
    // Spike-based neural dynamics
    void updateNeuronStates(float dt);
    void propagateSpikes();
    void generateOutputFeatures();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // VISION_PROCESSOR_H