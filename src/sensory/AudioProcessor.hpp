#include "Audio.hpp"
#include "AudioProcessor.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Audio features extracted by neural processing
struct AudioFeatures {
    // Frequency domain features
    std::vector<float> spectralPeaks;
    std::vector<float> spectralCentroid;
    std::vector<float> spectralSpread;
    
    // Temporal features
    std::vector<float> onsetTimes;
    std::vector<float> rhythmPeriodicity;
    std::vector<float> attackDecay;
    
    // Harmonic features
    std::vector<float> harmonicComponents;
    std::vector<float> percussiveComponents;
    
    // Spiking activity patterns
    std::vector<float> spikeRates;
    std::vector<float> temporalPatterns;
    
    AudioFeatures() = default;
    AudioFeatures(size_t length) {
        spectralPeaks.resize(length, 0.0f);
        spectralCentroid.resize(length, 0.0f);
        spectralSpread.resize(length, 0.0f);
        onsetTimes.resize(length, 0.0f);
        rhythmPeriodicity.resize(length, 0.0f);
        attackDecay.resize(length, 0.0f);
        harmonicComponents.resize(length, 0.0f);
        percussiveComponents.resize(length, 0.0f);
        spikeRates.resize(length, 0.0f);
        temporalPatterns.resize(length, 0.0f);
    }
};

// Main audio processing class
class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Processing pipeline
    void process(const Audio& input);
    void reset();
    
    // Feature access
    const std::vector<float>& getFeatures() const;
    size_t getFeatureDimensions() const;
    
    // Specialized feature access
    const std::vector<float>& getFrequencyFeatures() const;
    const std::vector<float>& getTemporalFeatures() const;
    const AudioFeatures& getInternalFeatures() const;
    
    // Parameter configuration
    void setParameters(float freqResolution, float timeResolution, 
                       float maxSpikeRate, float temporalWeight);
    
    // Neural component access
    const std::unique_ptr<NeuralRegion>& getCochlearRegion() const;
    const std::unique_ptr<SpikeSystem>& getSpikeSystem() const;
    
    // Processing status
    bool isInitialized() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core processing methods
    void initializeNeuralComponents(size_t length);
    void extractFrequencyFeatures(const std::vector<float>& inputData);
    void extractTemporalFeatures(const std::vector<float>& inputData);
    void computeHarmonicFeatures(const std::vector<float>& inputData);
    void convertToSpikes(const std::vector<float>& features);
    void applyTemporalProcessing(std::vector<float>& features);
    void combineFeatures(AudioFeatures& output);
    void generateOutputFeatures();
    
    friend class AudioProcessor::Impl;
};

} // namespace nlm