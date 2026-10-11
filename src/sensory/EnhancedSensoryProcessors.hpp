#pragma once

#include "SensoryProcessorBase.hpp"

namespace nlm {

// Vision processor implementation
class VisionProcessor : public SensoryProcessor {
public:
    VisionProcessor();
    ~VisionProcessor() override;
    
    const char* getTypeName() const override;
    
    void process(const SensoryInput& input) override;
    
    const std::vector<float>& getFeatures() const override;
    size_t getFeatureDimensions() const override;
    
    // Vision-specific processing methods
    void detectEdges();
    void extractColorFeatures();
    void computeTextureFeatures();
    void applyNeuralFilter();
    
    // Vision-specific validation
    bool validateInput(const SensoryInput& input) const override;
    
    // Get processed image properties
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getChannels() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural processing helpers
    void applyNeuronalResponse(const std::vector<float>& input, std::vector<float>& output);
    void computeSpatialModulation(const std::vector<float>& input, std::vector<float>& output);
    void applyTemporalPooling(const std::vector<float>& input, std::vector<float>& output);
};

// Audio processor implementation
class AudioProcessor : public SensoryProcessor {
public:
    AudioProcessor();
    ~AudioProcessor() override;
    
    const char* getTypeName() const override;
    
    void process(const SensoryInput& input) override;
    
    const std::vector<float>& getFeatures() const override;
    size_t getFeatureDimensions() const override;
    
    // Audio-specific processing methods
    void computeSpectralFeatures();
    void extractTemporalPatterns();
    void applyAudioSegmentation();
    void computeMelFrequencyCepstrum();
    
    // Audio-specific validation
    bool validateInput(const SensoryInput& input) const override;
    
    // Get audio properties
    size_t getSampleRate() const;
    size_t getNumSamples() const;
    float getDuration() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural processing helpers
    void applySpectralFiltering(const std::vector<float>& input, std::vector<float>& output);
    void computeCochlearFiltering(const std::vector<float>& input, std::vector<float>& output);
    void applyTemporalIntegration(const std::vector<float>& input, std::vector<float>& output);
};

// Internal signals processor implementation
class InternalSignalsProcessor : public SensoryProcessor {
public:
    InternalSignalsProcessor();
    ~InternalSignalsProcessor() override;
    
    const char* getTypeName() const override;
    
    void process(const SensoryInput& input) override;
    
    const std::vector<float>& getFeatures() const override;
    size_t getFeatureDimensions() const override;
    
    // Internal signals-specific processing methods
    void processHomeostaticSignals();
    void computeAllostaticLoad();
    void extractMetabolicState();
    void applyInternalStateMapping();
    
    // Internal signals-specific validation
    bool validateInput(const SensoryInput& input) const override;
    
    // Get signal properties
    const std::vector<float>& getProcessedSignals() const;
    const std::vector<float>& getHomeostaticSignals() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural processing helpers
    void computeInternalStateDynamics(const std::vector<float>& input, std::vector<float>& output);
    void applyHomeostaticRegulation(const std::vector<float>& input, std::vector<float>& output);
    void computeNeuralCoherence(const std::vector<float>& input, std::vector<float>& output);
};

} // namespace nlm
