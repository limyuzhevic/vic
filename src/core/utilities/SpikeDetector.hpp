// Spike Detection Utility Class
// Abstract common spike detection patterns with configurable thresholds and refractory periods

#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <functional>

namespace nlm {

struct SpikeEvent {
    size_t neuronId;
    float spikeTime;
    float amplitude;
    float thresholdCrossing;
    float refractoryRemaining;
    size_t stepCount;
};

struct SpikeDetectionConfig {
    float voltageThreshold = -55.0f;
    float amplitudeThreshold = 0.0f;
    float refractoryPeriod = 2.0f;
    
    float riseSlopeThreshold = 0.5f;
    float peakDetectionWindow = 2.0f;
    bool useDerivative = true;
    bool useIntegral = false;
    
    bool validateAmplitude = true;
    bool validateRefractory = true;
    bool validatePeak = true;
    float minPeakWidth = 0.1f;
    
    bool applySmoothing = false;
    float smoothingFactor = 0.5f;
    size_t minSpikesToValidate = 3;
};

class SpikeDetector {
public:
    using SpikeCallback = std::function<void(const SpikeEvent& event)>;
    
    explicit SpikeDetector(const SpikeDetectionConfig& config = SpikeDetectionConfig());

    void configure(const SpikeDetectionConfig& config);

    void update(float voltage, float timeStep = 1.0f);

    bool checkForSpike(float voltage, float time, size_t step);

    std::vector<SpikeEvent> detectSpikes(const std::vector<float>& voltageTrace,
                                        const std::vector<float>& timeTrace);

    const std::vector<SpikeEvent>& getRecentSpikes() const { return recentSpikes_; }

    void clearSpikes() { recentSpikes_.clear(); }

    size_t getSpikeCount() const { return recentSpikes_.size(); }

    void setSpikeCallback(SpikeCallback callback) { spikeCallback_ = callback; }

    bool validateSpike(const SpikeEvent& spike) const;

    struct SpikeCharacteristics {
        float amplitude;
        float widthAtHalfMax;
        float riseTime;
        float fallTime;
        float slopeMax;
        float integral;
    };

    SpikeCharacteristics calculateCharacteristics(const SpikeEvent& spike,
                                                 const std::vector<float>& voltageTrace,
                                                 const std::vector<float>& timeTrace) const;

private:
    bool detectThresholdCrossing(float voltage, float time, size_t step);
    bool detectDerivativeCrossing(float voltage, float prevVoltage, float timeStep);
    bool detectPeak(float voltage, float time, const std::vector<float>& voltageHistory,
                    const std::vector<float>& timeHistory);
    bool checkRefractoryPeriod(float time) const;
    float calculatePeakWidth(float peakVoltage, const std::vector<float>& voltageTrace,
                           const std::vector<float>& timeTrace) const;

    SpikeDetectionConfig config_;
    std::vector<SpikeEvent> recentSpikes_;
    std::vector<float> voltageHistory_;
    std::vector<float> timeHistory_;
    float previousVoltage_ = 0.0f;
    float previousTime_ = 0.0f;
    SpikeCallback spikeCallback_;
    bool spikePending_ = false;
    SpikeEvent pendingSpike_;
};

class SpikeDetectorFactory {
public:
    static SpikeDetector createStandardDetector();
    static SpikeDetector createSensitiveDetector();
    static SpikeDetector createRobustDetector();
    static SpikeDetector createCustomDetector(const SpikeDetectionConfig& config);

    static SpikeDetector createForNeuronType(size_t neuronType);
    static SpikeDetector createForFunction(const std::string& function);
};

} // namespace nlm
