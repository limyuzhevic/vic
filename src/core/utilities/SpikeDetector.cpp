// Spike Detection Implementation

#include "SpikeDetector.hpp"
#include <algorithm>

namespace nlm {

SpikeDetector::SpikeDetector(const SpikeDetectionConfig& config) : config_(config) {}

void SpikeDetector::configure(const SpikeDetectionConfig& config) {
    config_ = config;
    clearSpikes();
}

void SpikeDetector::update(float voltage, float timeStep) {
    if (config_.applySmoothing && !voltageHistory_.empty()) {
        voltage = voltage * config_.smoothingFactor + previousVoltage_ * (1.0f - config_.smoothingFactor);
    }

    previousVoltage_ = voltage;
}

bool SpikeDetector::checkForSpike(float voltage, float time, size_t step) {
    bool spikeDetected = detectThresholdCrossing(voltage, time, step);

    if (spikeDetected) {
        SpikeEvent event;
        event.neuronId = step;
        event.spikeTime = time;
        event.amplitude = voltage;
        event.thresholdCrossing = config_.voltageThreshold;
        event.refractoryRemaining = config_.refractoryPeriod;
        event.stepCount = step;

        if (validateSpike(event)) {
            recentSpikes_.push_back(event);
            if (spikeCallback_) {
                spikeCallback_(event);
            }
            return true;
        }
    }

    return false;
}

std::vector<SpikeEvent> SpikeDetector::detectSpikes(const std::vector<float>& voltageTrace,
                                                   const std::vector<float>& timeTrace) {
    clearSpikes();
    
    if (voltageTrace.size() != timeTrace.size()) {
        return recentSpikes_;
    }

    for (size_t i = 0; i < voltageTrace.size(); ++i) {
        update(voltageTrace[i], i > 0 ? timeTrace[i] - timeTrace[i - 1] : 1.0f);
        checkForSpike(voltageTrace[i], timeTrace[i], i);
    }

    return recentSpikes_;
}

bool SpikeDetector::validateSpike(const SpikeEvent& spike) const {
    if (config_.validateAmplitude && std::abs(spike.amplitude) < config_.amplitudeThreshold) {
        return false;
    }

    if (config_.validateRefractory && spike.refractoryRemaining <= 0.0f) {
        return false;
    }

    return true;
}

bool SpikeDetector::detectThresholdCrossing(float voltage, float time, size_t step) {
    bool crossing = voltage >= config_.voltageThreshold;

    if (crossing) {
        SpikeEvent event;
        event.neuronId = step;
        event.spikeTime = time;
        event.amplitude = voltage;
        event.thresholdCrossing = config_.voltageThreshold;
        event.refractoryRemaining = config_.refractoryPeriod;
        event.stepCount = step;

        if (validateSpike(event)) {
            recentSpikes_.push_back(event);
            if (spikeCallback_) {
                spikeCallback_(event);
            }
            return true;
        }
    }

    return false;
}

SpikeDetector::SpikeCharacteristics SpikeDetector::calculateCharacteristics(const SpikeEvent& spike,
                                                                         const std::vector<float>& voltageTrace,
                                                                         const std::vector<float>& timeTrace) const {
    SpikeCharacteristics chars;
    return chars;
}

SpikeDetector SpikeDetectorFactory::createStandardDetector() {
    SpikeDetectionConfig config;
    config.voltageThreshold = -55.0f;
    config.amplitudeThreshold = 0.0f;
    config.refractoryPeriod = 2.0f;
    config.useDerivative = true;
    config.validateAmplitude = true;
    config.validateRefractory = true;
    config.applySmoothing = false;
    return SpikeDetector(config);
}

SpikeDetector SpikeDetectorFactory::createSensitiveDetector() {
    SpikeDetectionConfig config;
    config.voltageThreshold = -60.0f;
    config.amplitudeThreshold = 10.0f;
    config.refractoryPeriod = 1.0f;
    config.useDerivative = true;
    config.validateAmplitude = true;
    config.validateRefractory = true;
    config.applySmoothing = true;
    return SpikeDetector(config);
}

SpikeDetector SpikeDetectorFactory::createRobustDetector() {
    SpikeDetectionConfig config;
    config.voltageThreshold = -45.0f;
    config.amplitudeThreshold = 0.0f;
    config.refractoryPeriod = 5.0f;
    config.useDerivative = false;
    config.validateAmplitude = true;
    config.validateRefractory = true;
    config.validatePeak = true;
    config.applySmoothing = true;
    return SpikeDetector(config);
}

SpikeDetector SpikeDetectorFactory::createCustomDetector(const SpikeDetectionConfig& config) {
    return SpikeDetector(config);
}

SpikeDetector SpikeDetectorFactory::createForNeuronType(size_t neuronType) {
    return createStandardDetector();
}

SpikeDetector SpikeDetectorFactory::createForFunction(const std::string& function) {
    return createStandardDetector();
}

} // namespace nlm
