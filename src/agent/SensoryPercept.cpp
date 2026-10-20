#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <tuple>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations for helper functions
namespace {
    template<typename T>
    std::vector<size_t> computeHistogram(const std::vector<T>& data, size_t bins = 10);
    
    float computeMean(const std::vector<float>& data);
    float computeStdDev(const std::vector<float>& data, float mean);
    float computeEntropy(const std::vector<float>& data);
}

struct SensoryPercept::Impl {
    // Modality management
    struct ModalityInfo {
        SensoryModality type;
        size_t dimensions;
        std::string name;
        bool active;
    };
    std::vector<ModalityInfo> modalities_;
    
    // Advanced sensory data
    Vision vision_;
    std::vector<std::pair<size_t, float>> touch_;
    std::vector<std::pair<size_t, float>> internal_;
    std::vector<std::tuple<float, float, float, float, float, float>> proprioception_;
    Audio audio_;
    
    // Auditory scene analysis
    struct SoundSource {
        float x, y, z;
        float level;
        float frequency;
        float directionality;
        float arrivalTime;
    };
    std::vector<SoundSource> soundSources_;
    
    // Signal processing state
    std::vector<float> visionBuffer_;
    std::vector<float> touchBuffer_;
    std::vector<float> internalBuffer_;
    std::vector<float> proprioceptionBuffer_;
    
    // Configuration
    bool sensoryFilteringEnabled_;
    float sensoryThreshold_;
    std::vector<std::string> preprocessingSteps_;
    
    // Statistics
    double lastProcessingTime_;
    size_t totalSignalsProcessed_;
};

SensoryPercept::SensoryPercept() : pImpl(std::make_unique<Impl>()) {
    // Initialize legacy fields
    pImpl->vision_.resize(16 * 16, 0.0f);
    pImpl->vision_.setTimestamp(0.0);
    
    pImpl->touch_.resize(8, std::make_pair<size_t, float>(0, 0.0f));
    pImpl->internal_.resize(4, std::make_pair<size_t, float>(0, 0.0f));
    pImpl->proprioception_.resize(6, std::make_tuple<float, float, float, float, float, float>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
    pImpl->audio_.resize(0, 0.0f);
    
    // Initialize advanced fields
    pImpl->vision_.setWidth(16);
    pImpl->vision_.setHeight(16);
    pImpl->vision_.setChannels(1);
    
    // Configure default modalities
    configureModality(SensoryModality::Vision, 16 * 16, "vision");
    configureModality(SensoryModality::Touch, 8, "touch");
    configureModality(SensoryModality::Internal, 4, "internal");
    configureModality(SensoryModality::Proprioception, 6, "proprioception");
    
    // Set defaults
    pImpl->sensoryFilteringEnabled_ = false;
    pImpl->sensoryThreshold_ = 0.5f;
    pImpl->lastProcessingTime_ = 0.0;
    pImpl->totalSignalsProcessed_ = 0;
}

SensoryPercept::~SensoryPercept() = default;

SensoryPercept::SensoryPercept(SensoryPercept&& other) noexcept = default;
SensoryPercept& SensoryPercept::operator=(SensoryPercept&& other) noexcept = default;

void SensoryPercept::configureModality(SensoryModality modality, size_t dimensions, const std::string& name) {
    auto it = std::find_if(pImpl->modalities_.begin(), pImpl->modalities_.end(),
                         [modality](const Impl::ModalityInfo& info) { return info.type == modality; });
    
    if (it != pImpl->modalities_.end()) {
        it->dimensions = dimensions;
        it->name = name;
    } else {
        Impl::ModalityInfo info;
        info.type = modality;
        info.dimensions = dimensions;
        info.name = name;
        info.active = true;
        pImpl->modalities_.push_back(info);
    }
}

void SensoryPercept::configureModality(SensoryModality modality, size_t width, size_t height, size_t channels) {
    size_t dimensions = width * height * channels;
    configureModality(modality, dimensions, modalityToString(modality));
}

void SensoryPercept::configureModality(SensoryModality modality, size_t sampleRate, size_t numSamples) {
    size_t dimensions = numSamples;
    configureModality(modality, dimensions, modalityToString(modality) + "_audio");
}

bool SensoryPercept::hasModality(SensoryModality modality) const {
    auto it = std::find_if(pImpl->modalities_.begin(), pImpl->modalities_.end(),
                         [modality](const Impl::ModalityInfo& info) { return info.type == modality; });
    return it != pImpl->modalities_.end() && it->active;
}

size_t SensoryPercept::getModalityDimensions(SensoryModality modality) const {
    auto it = std::find_if(pImpl->modalities_.begin(), pImpl->modalities_.end(),
                         [modality](const Impl::ModalityInfo& info) { return info.type == modality; });
    if (it != pImpl->modalities_.end()) {
        return it->dimensions;
    }
    return 0;
}

std::string SensoryPercept::getModalityName(SensoryModality modality) const {
    auto it = std::find_if(pImpl->modalities_.begin(), pImpl->modalities_.end(),
                         [modality](const Impl::ModalityInfo& info) { return info.type == modality; });
    if (it != pImpl->modalities_.end()) {
        return it->name;
    }
    return "";
}

void SensoryPercept::setVision(size_t width, size_t height, const std::vector<float>& data) {
    setVision(width, height, 1, data);
}

void SensoryPercept::setVision(size_t width, size_t height, size_t channels,
                               const std::vector<float>& data) {
    pImpl->vision_.resize(width * height * channels);
    pImpl->vision_.setData(data);
    pImpl->vision_.setWidth(width);
    pImpl->vision_.setHeight(height);
    pImpl->vision_.setChannels(channels);
    pImpl->vision_.setTimestamp(timestamp_);
    
    // Update legacy field
    vision_.assign(data.begin(), data.end());
    visionWidth_ = width;
    visionHeight_ = height;
}

void SensoryPercept::enhanceVision(float enhancement) {
    std::vector<float> enhancedData = pImpl->vision_.getData();
    for (float& value : enhancedData) {
        value = std::min(1.0f, std::max(0.0f, value * (1.0f + enhancement)));
    }
    pImpl->vision_.setData(enhancedData);
}

void SensoryPercept::applyVisionFilter(const std::vector<float>& kernel) {
    // Simple convolution filter
    std::vector<float> filteredData = pImpl->vision_.getData();
    size_t width = pImpl->vision_.getWidth();
    size_t height = pImpl->vision_.getHeight();
    
    // Apply simple smoothing filter
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            size_t idx = y * width + x;
            float sum = 0.0f;
            int kernelSize = static_cast<int>(std::sqrt(kernel.size()));
            int halfKernel = kernelSize / 2;
            
            for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
                for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                    size_t srcY = y + ky;
                    size_t srcX = x + kx;
                    if (srcY < height && srcX < width) {
                        size_t srcIdx = srcY * width + srcX;
                        sum += filteredData[srcIdx] * kernel[(ky + halfKernel) * kernelSize + (kx + halfKernel)];
                    }
                }
            }
            filteredData[idx] = sum;
        }
    }
    
    pImpl->vision_.setData(filteredData);
}

void SensoryPercept::addTouch(size_t index, float pressure, float intensity, float duration, float friction) {
    // Check if touch already exists
    auto it = std::find_if(pImpl->touch_.begin(), pImpl->touch_.end(),
                          [index](const std::pair<size_t, float>& touch) { return touch.first == index; });
    
    if (it != pImpl->touch_.end()) {
        // Update existing touch
        it->second = pressure * intensity * (1.0f - friction);
    } else {
        // Add new touch
        pImpl->touch_.push_back(std::make_pair(index, pressure * intensity * (1.0f - friction)));
    }
}

void SensoryPercept::addInternalSignal(size_t index, float value, float timestamp, float decayRate, bool isModulated) {
    auto it = std::find_if(pImpl->internal_.begin(), pImpl->internal_.end(),
                          [index](const std::pair<size_t, float>& signal) { return signal.first == index; });\n    if (it != pImpl->internal_.end()) {
        it->second = value;
    } else {
        pImpl->internal_.push_back(std::make_pair(index, value));
    }
}

void SensoryPercept::setProprioception(float x, float y, float orientation, float velocityX, float velocityY, float angularVelocity) {
    setProprioception(x, y, orientation, velocityX, velocityY, angularVelocity, 0.0f, 0.0f, 0.0f);
}

void SensoryPercept::setProprioception(float x, float y, float orientation, 
                                       float velocityX, float velocityY, float angularVelocity,
                                       float accelerationX, float accelerationY, float angularAcceleration) {
    pImpl->proprioception_.push_back(std::make_tuple(x, y, orientation, velocityX, velocityY, angularVelocity));
    
    // Update legacy field
    if (proprioception_.size() > 6) proprioception_.pop_front();
    proprioception_.push_back(x);
    proprioception_.push_back(y);
    proprioception_.push_back(orientation);
    proprioception_.push_back(velocityX);
    proprioception_.push_back(velocityY);
    proprioception_.push_back(angularVelocity);
}

void SensoryPercept::updateProprioception(float deltaX, float deltaY, float deltaOrientation,
                                         float deltaVelocityX, float deltaVelocityY, float deltaAngularVelocity) {
    if (!pImpl->proprioception_.empty()) {
        auto& lastProprioception = pImpl->proprioception_.back();
        std::get<0>(lastProprioception) += deltaX;
        std::get<1>(lastProprioception) += deltaY;
        std::get<2>(lastProprioception) += deltaOrientation;
        std::get<3>(lastProprioception) += deltaVelocityX;
        std::get<4>(lastProprioception) += deltaVelocityY;
        std::get<5>(lastProprioception) += deltaAngularVelocity;
    }
}

void SensoryPercept::setAudio(size_t sampleRate, const std::vector<float>& samples) {
    setAudio(sampleRate, samples, std::vector<float>());
}

void SensoryPercept::setAudio(size_t sampleRate, const std::vector<float>& samples,
                             const std::vector<float>& spectrum) {
    pImpl->audio_.resize(samples.size());
    pImpl->audio_.setData(samples);
    pImpl->audio_.setSampleRate(sampleRate);
    
    // If spectrum provided, store it
    if (!spectrum.empty()) {
        // Store spectral data for processing
        pImpl->audio_.setSpectrum(spectrum);
    }
}

void SensoryPercept::addAudioSample(float sample, float spectrumValue) {
    pImpl->audio_.addSample(sample);
    if (spectrumValue != 0.0f) {
        pImpl->audio_.addSpectrum(spectrumValue);
    }
}

void SensoryPercept::addSoundSource(float x, float y, float z, float level, float frequency) {
    Impl::SoundSource source;
    source.x = x;
    source.y = y;
    source.z = z;
    source.level = level;
    source.frequency = frequency;
    source.directionality = 1.0f - (std::min(1.0f, level / 10.0f)); // Simplified directionality calculation
    source.arrivalTime = timestamp_;
    
    pImpl->soundSources_.push_back(source);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Vision (flattened)
    const auto& visionData = pImpl->vision_.getData();
    all.insert(all.end(), visionData.begin(), visionData.end());
    
    // Touch (flattened)
    for (const auto& touch : pImpl->touch_) {
        all.push_back(touch.second);
    }
    
    // Internal (flattened)
    for (const auto& internal : pImpl->internal_) {
        all.push_back(internal.second);
    }
    
    // Proprioception (flattened)
    for (const auto& proprio : pImpl->proprioception_) {
        all.push_back(std::get<0>(proprio)); // x position
        all.push_back(std::get<1>(proprio)); // y position
    }
    
    // Audio (flattened)
    const auto& audioData = pImpl->audio_.getData();
    all.insert(all.end(), audioData.begin(), audioData.end());
    
    return all;
}

std::vector<std::pair<SensoryModality, float>> SensoryPercept::getAllSignalsWithModality() const {
    std::vector<std::pair<SensoryModality, float>> result;
    
    // Vision
    const auto& visionData = pImpl->vision_.getData();
    float visionMean = computeMean(visionData);
    result.push_back(std::make_pair(SensoryModality::Vision, visionMean));
    
    // Touch
    float touchMean = 0.0f;
    for (const auto& touch : pImpl->touch_) {
        touchMean += touch.second;
    }
    if (!pImpl->touch_.empty()) touchMean /= pImpl->touch_.size();
    result.push_back(std::make_pair(SensoryModality::Touch, touchMean));
    
    // Internal
    float internalMean = 0.0f;
    for (const auto& internal : pImpl->internal_) {
        internalMean += internal.second;
    }
    if (!pImpl->internal_.empty()) internalMean /= pImpl->internal_.size();
    result.push_back(std::make_pair(SensoryModality::Internal, internalMean));
    
    // Proprioception
    float proprioMean = 0.0f;
    for (const auto& proprio : pImpl->proprioception_) {
        proprioMean += std::get<0>(proprio); // x position
    }
    if (!pImpl->proprioception_.empty()) proprioMean /= pImpl->proprioception_.size();
    result.push_back(std::make_pair(SensoryModality::Proprioception, proprioMean));
    
    // Audio
    const auto& audioData = pImpl->audio_.getData();
    float audioMean = computeMean(audioData);
    result.push_back(std::make_pair(SensoryModality::Audio, audioMean));
    
    return result;
}

size_t SensoryPercept::getTotalSignalCount() const {
    size_t count = 0;
    
    // Vision
    count += pImpl->vision_.getWidth() * pImpl->vision_.getHeight() * pImpl->vision_.getChannels();
    
    // Touch
    count += pImpl->touch_.size();
    
    // Internal
    count += pImpl->internal_.size();
    
    // Proprioception
    count += pImpl->proprioception_.size() * 2; // x and y for each
    
    // Audio
    count += pImpl->audio_.getNumSamples();
    
    return count;
}

void SensoryPercept::reset() {
    // Clear all sensory data
    pImpl->vision_.setData(std::vector<float>());
    pImpl->touch_.clear();
    pImpl->internal_.clear();
    pImpl->proprioception_.clear();
    pImpl->audio_.setData(std::vector<float>());
    pImpl->soundSources_.clear();
    
    // Reset legacy fields
    vision_.clear();
    touch_.clear();
    internal_.clear();
    proprioception_.clear();
    audio_.clear();
    
    timestamp_ = 0.0;
    pImpl->lastProcessingTime_ = 0.0;
    pImpl->totalSignalsProcessed_ = 0;
}

void SensoryPercept::resetModality(SensoryModality modality) {
    switch (modality) {
        case SensoryModality::Vision:
            pImpl->vision_.setData(std::vector<float>());
            vision_.clear();
            break;
        case SensoryModality::Touch:
            pImpl->touch_.clear();
            touch_.clear();
            break;
        case SensoryModality::Internal:
            pImpl->internal_.clear();
            internal_.clear();
            break;
        case SensoryModality::Proprioception:
            pImpl->proprioception_.clear();
            proprioception_.clear();
            break;
        case SensoryModality::Audio:
            pImpl->audio_.setData(std::vector<float>());
            audio_.clear();
            break;
        default:
            break;
    }
}

bool SensoryPercept::operator==(const SensoryPercept& other) const {
    return getAllSignals() == other.getAllSignals() && 
           timestamp_ == other.timestamp_;
}

void SensoryPercept::saveToFile(const std::string& filepath, bool compress) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return;
    }
    
    // For now, use simple serialization
    // In a real implementation, you might use more efficient formats
    std::string data = serialize();
    
    if (compress) {
        // Simple compression: just write as text for now
        // You could use gzip, zlib, etc. in a real implementation
        file.write(data.c_str(), data.size());
    } else {
        file.write(data.c_str(), data.size());
    }
    
    file.close();
}

void SensoryPercept::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filepath);
        return;
    }
    
    // Read file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string data = buffer.str();
    
    file.close();
    
    deserialize(data);
}

std::string SensoryPercept::serialize() const {
    std::stringstream ss;
    
    // Write sensory data
    ss << "VISION:" << pImpl->vision_.getWidth() << "," << pImpl->vision_.getHeight() << ",";
    const auto& visionData = pImpl->vision_.getData();
    for (size_t i = 0; i < visionData.size(); ++i) {
        ss << visionData[i];
        if (i < visionData.size() - 1) ss << ",";
    }
    ss << ";";
    
    // Write touch data
    ss << "TOUCH:";
    for (size_t i = 0; i < pImpl->touch_.size(); ++i) {
        ss << pImpl->touch_[i].first << ":" << pImpl->touch_[i].second;
        if (i < pImpl->touch_.size() - 1) ss << ",";
    }
    ss << ";";
    
    // Write internal data
    ss << "INTERNAL:";
    for (size_t i = 0; i < pImpl->internal_.size(); ++i) {
        ss << pImpl->internal_[i].first << ":" << pImpl->internal_[i].second;
        if (i < pImpl->internal_.size() - 1) ss << ",";
    }
    ss << ";";
    
    // Write proprioception data
    ss << "PROPRIOCEPTION:";
    for (size_t i = 0; i < pImpl->proprioception_.size(); ++i) {
        const auto& prop = pImpl->proprioception_[i];
        ss << std::get<0>(prop) << "," << std::get<1>(prop) << "," << std::get<2>(prop) << "," 
           << std::get<3>(prop) << "," << std::get<4>(prop) << "," << std::get<5>(prop);
        if (i < pImpl->proprioception_.size() - 1) ss << ",";
    }
    ss << ";";
    
    // Write audio data
    ss << "AUDIO:" << pImpl->audio_.getSampleRate() << ";";
    const auto& audioData = pImpl->audio_.getData();
    for (size_t i = 0; i < audioData.size(); ++i) {
        ss << audioData[i];
        if (i < audioData.size() - 1) ss << ",";
    }
    ss << ";";
    
    // Write timestamp
    ss << "TIMESTAMP:" << timestamp_ << ";";
    
    return ss.str();
}

void SensoryPercept::deserialize(const std::string& data) {
    std::stringstream ss(data);
    std::string line;
    
    while (std::getline(ss, line, ';')) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string type = line.substr(0, colonPos);
        std::string values = line.substr(colonPos + 1);
        
        if (type == "VISION") {
            size_t comma1 = values.find(',');
            size_t comma2 = values.find(',', comma1 + 1);
            if (comma1 != std::string::npos && comma2 != std::string::npos) {
                size_t width = std::stoul(values.substr(0, comma1));
                size_t height = std::stoul(values.substr(comma1 + 1, comma2 - comma1 - 1));
                
                std::vector<float> visionData;
                size_t start = comma2 + 1;
                while (start < values.size()) {
                    size_t comma = values.find(',', start);
                    if (comma == std::string::npos) {
                        visionData.push_back(std::stof(values.substr(start)));
                        break;
                    } else {
                        visionData.push_back(std::stof(values.substr(start, comma - start)));
                        start = comma + 1;
                    }
                }
                
                setVision(width, height, visionData);
            }
        } else if (type == "TOUCH") {
            std::vector<std::pair<size_t, float>> touchData;
            size_t start = 0;
            while (start < values.size()) {
                size_t colon = values.find(':', start);
                if (colon == std::string::npos) break;
                
                size_t index = std::stoul(values.substr(start, colon - start));
                size_t colon2 = values.find(':', colon + 1);
                if (colon2 == std::string::npos) break;
                
                float pressure = std::stof(values.substr(colon + 1, colon2 - colon - 1));
                
                touchData.push_back(std::make_pair(index, pressure));
                start = colon2 + 1;
            }
            pImpl->touch_ = touchData;
        } else if (type == "INTERNAL") {
            std::vector<std::pair<size_t, float>> internalData;
            size_t start = 0;
            while (start < values.size()) {
                size_t colon = values.find(':', start);
                if (colon == std::string::npos) break;
                
                size_t index = std::stoul(values.substr(start, colon - start));
                size_t colon2 = values.find(':', colon + 1);
                if (colon2 == std::string::npos) break;
                
                float value = std::stof(values.substr(colon + 1, colon2 - colon - 1));
                
                internalData.push_back(std::make_pair(index, value));
                start = colon2 + 1;
            }
            pImpl->internal_ = internalData;
        } else if (type == "PROPRIOCEPTION") {
            std::vector<std::tuple<float, float, float, float, float, float>> proprioData;
            size_t start = 0;
            while (start < values.size()) {
                size_t comma1 = values.find(',', start);
                size_t comma2 = values.find(',', comma1 + 1);
                size_t comma3 = values.find(',', comma2 + 1);
                size_t comma4 = values.find(',', comma3 + 1);
                size_t comma5 = values.find(',', comma4 + 1);
                
                if (comma1 == std::string::npos || comma2 == std::string::npos || 
                    comma3 == std::string::npos || comma4 == std::string::npos || comma5 == std::string::npos) break;
                
                float x = std::stof(values.substr(start, comma1 - start));
                float y = std::stof(values.substr(comma1 + 1, comma2 - comma1 - 1));
                float orientation = std::stof(values.substr(comma2 + 1, comma3 - comma2 - 1));
                float velocityX = std::stof(values.substr(comma3 + 1, comma4 - comma3 - 1));
                float velocityY = std::stof(values.substr(comma4 + 1, comma5 - comma4 - 1));
                float angularVelocity = std::stof(values.substr(comma5 + 1));
                
                proprioData.push_back(std::make_tuple(x, y, orientation, velocityX, velocityY, angularVelocity));
                start = comma5 + 1;
            }
            pImpl->proprioception_ = proprioData;
        } else if (type == "AUDIO") {
            size_t semicolon = values.find(';');
            if (semicolon != std::string::npos) {
                size_t sampleRate = std::stoul(values.substr(0, semicolon));
                std::string audioValues = values.substr(semicolon + 1);
                
                std::vector<float> audioData;
                size_t start = 0;
                while (start < audioValues.size()) {
                    size_t comma = audioValues.find(',', start);
                    if (comma == std::string::npos) {
                        audioData.push_back(std::stof(audioValues.substr(start)));
                        break;
                    } else {
                        audioData.push_back(std::stof(audioValues.substr(start, comma - start)));
                        start = comma + 1;
                    }
                }
                
                setAudio(sampleRate, audioData);
            }
        } else if (type == "TIMESTAMP") {
            timestamp_ = std::stod(values);
        }
    }
}

std::string SensoryPercept::summary() const {
    std::stringstream ss;
    
    ss << "Sensory Percept Summary:" << std::endl;
    ss << "  Total signals: " << getTotalSignalCount() << std::endl;
    ss << "  Timestamp: " << timestamp_ << std::endl;
    ss << "  Vision: " << pImpl->vision_.getWidth() << "x" << pImpl->vision_.getHeight()
       << " @ " << pImpl->vision_.getChannels() << " channels" << std::endl;
    ss << "  Touch points: " << pImpl->touch_.size() << std::endl;
    ss << "  Internal signals: " << pImpl->internal_.size() << std::endl;
    ss << "  Proprioception samples: " << pImpl->proprioception_.size() << std::endl;
    ss << "  Audio samples: " << pImpl->audio_.getNumSamples()
       << " @ " << pImpl->audio_.getSampleRate() << " Hz" << std::endl;
    ss << "  Sound sources: " << pImpl->soundSources_.size() << std::endl;
    ss << "  Processing time: " << pImpl->lastProcessingTime_ << "s" << std::endl;
    
    return ss.str();
}

float SensoryPercept::getVisionEntropy() const {
    const auto& visionData = pImpl->vision_.getData();
    return computeEntropy(visionData);
}

float SensoryPercept::getTouchEntropy() const {
    std::vector<float> touchData;
    for (const auto& touch : pImpl->touch_) {
        touchData.push_back(touch.second);
    }
    return computeEntropy(touchData);
}

float SensoryPercept::getInternalEntropy() const {
    std::vector<float> internalData;
    for (const auto& internal : pImpl->internal_) {
        internalData.push_back(internal.second);
    }
    return computeEntropy(internalData);
}

float SensoryPercept::getAudioComplexity() const {
    const auto& audioData = pImpl->audio_.getData();
    if (audioData.empty()) return 0.0f;
    
    float mean = computeMean(audioData);
    float variance = 0.0f;
    for (float sample : audioData) {
        variance += (sample - mean) * (sample - mean);
    }
    variance /= audioData.size();
    return std::sqrt(variance);
}

float SensoryPercept::getTemporalCoherence() const {
    // Simple coherence calculation based on timestamp differences
    if (pImpl->proprioception_.size() < 2) return 1.0f;
    
    float totalCoherence = 0.0f;
    for (size_t i = 1; i < pImpl->proprioception_.size(); ++i) {
        const auto& prev = pImpl->proprioception_[i - 1];
        const auto& curr = pImpl->proprioception_[i];
        
        float dx = std::get<0>(curr) - std::get<0>(prev);
        float dy = std::get<1>(curr) - std::get<1>(prev);
        float dt = 0.01f; // Assumed time delta
        
        float velocity = std::sqrt(dx * dx + dy * dy) / dt;
        totalCoherence += velocity;
    }
    
    return std::min(1.0f, totalCoherence / pImpl->proprioception_.size());
}

void SensoryPercept::applyFilter(const std::vector<float>& signal, const std::vector<float>& kernel) {
    std::vector<float> filteredSignal = signal;
    int kernelSize = static_cast<int>(kernel.size());
    int halfKernel = kernelSize / 2;
    
    for (size_t i = halfKernel; i < signal.size() - halfKernel; ++i) {
        float sum = 0.0f;
        for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
            size_t idx = i + ky;
            if (idx < signal.size()) {
                sum += filteredSignal[idx] * kernel[ky + halfKernel];
            }
        }
        filteredSignal[i] = sum;
    }
}

void SensoryPercept::normalizeSignal(SensoryModality modality, float targetMean, float targetStd) {
    std::vector<float>& signal = getSignalByModality(modality);
    if (signal.empty()) return;
    
    float mean = computeMean(signal);
    float stdDev = computeStdDev(signal, mean);
    
    if (stdDev > 0.0f) {
        for (float& value : signal) {
            value = targetStd * ((value - mean) / stdDev) + targetMean;
        }
    }
}

void SensoryPercept::applyGain(SensoryModality modality, float gainFactor) {
    std::vector<float>& signal = getSignalByModality(modality);
    for (float& value : signal) {
        value *= gainFactor;
    }
}

void SensoryPercept::applyNoiseReduction(SensoryModality modality, float threshold) {
    std::vector<float>& signal = getSignalByModality(modality);
    for (float& value : signal) {
        if (std::abs(value) < threshold) {
            value = 0.0f;
        }
    }
}

std::vector<float> SensoryPercept::prepareForNeuralInput(SensoryModality modality) const {
    const std::vector<float>& signal = getSignalByModality(modality);
    return signal;
}

void SensoryPercept::applyNeuralPreprocessing(const std::vector<std::string>& preprocessingSteps) {
    pImpl->preprocessingSteps_ = preprocessingSteps;
    
    for (const auto& step : preprocessingSteps) {
        if (step == "normalize") {
            for (auto& modality : pImpl->modalities_) {
                if (modality.active) {
                    normalizeSignal(modality.type, 0.0f, 1.0f);
                }
            }
        } else if (step == "filter") {
            // Apply default filter
            std::vector<float> kernel = {0.1f, 0.2f, 0.4f, 0.2f, 0.1f};
            for (auto& modality : pImpl->modalities_) {
                if (modality.active) {
                    applyFilter(getSignalByModality(modality.type), kernel);
                }
            }
        } else if (step == "threshold") {
            for (auto& modality : pImpl->modalities_) {
                if (modality.active) {
                    applyNoiseReduction(modality.type, pImpl->sensoryThreshold_);
                }
            }
        }
    }
}

float SensoryPercept::getSignalRange(SensoryModality modality) const {
    const std::vector<float>& signal = getSignalByModality(modality);
    if (signal.empty()) return 0.0f;
    
    float min = signal[0];
    float max = signal[0];
    for (float value : signal) {
        min = std::min(min, value);
        max = std::max(max, value);
    }
    
    return max - min;
}

float SensoryPercept::getSignalMean(SensoryModality modality) const {
    const std::vector<float>& signal = getSignalByModality(modality);
    return computeMean(signal);
}

float SensoryPercept::getSignalStdDev(SensoryModality modality) const {
    const std::vector<float>& signal = getSignalByModality(modality);
    float mean = computeMean(signal);
    return computeStdDev(signal, mean);
}

float SensoryPercept::getSignalSkew(SensoryModality modality) const {
    const std::vector<float>& signal = getSignalByModality(modality);
    if (signal.size() < 3) return 0.0f;
    
    float mean = computeMean(signal);
    float stdDev = computeStdDev(signal, mean);
    if (stdDev < 0.001f) return 0.0f;
    
    float skew = 0.0f;
    for (float value : signal) {
        skew += std::pow((value - mean) / stdDev, 3);
    }
    skew /= signal.size();
    
    return skew;
}

std::vector<float>& SensoryPercept::getSignalByModality(SensoryModality modality) {
    switch (modality) {
        case SensoryModality::Vision:
            return const_cast<std::vector<float>&>(pImpl->vision_.getData());
        case SensoryModality::Touch:
            // Return flattened touch data
            static std::vector<float> touchFlattened;
            touchFlattened.clear();
            for (const auto& touch : pImpl->touch_) {
                touchFlattened.push_back(touch.second);
            }
            return touchFlattened;
        case SensoryModality::Internal:
            // Return flattened internal data
            static std::vector<float> internalFlattened;
            internalFlattened.clear();
            for (const auto& internal : pImpl->internal_) {
                internalFlattened.push_back(internal.second);
            }
            return internalFlattened;
        case SensoryModality::Proprioception:
            // Return flattened proprioception data
            static std::vector<float> proprioceptionFlattened;
            proprioceptionFlattened.clear();
            for (const auto& proprio : pImpl->proprioception_) {
                proprioceptionFlattened.push_back(std::get<0>(proprio)); // x position
                proprioceptionFlattened.push_back(std::get<1>(proprio)); // y position
            }
            return proprioceptionFlattened;
        case SensoryModality::Audio:
            return const_cast<std::vector<float>&>(pImpl->audio_.getData());
        default:
            static std::vector<float> emptySignal;
            return emptySignal;
    }
}

namespace {

template<typename T>
std::vector<size_t> computeHistogram(const std::vector<T>& data, size_t bins) {
    std::vector<size_t> histogram(bins, 0);
    if (data.empty()) return histogram;
    
    T min = data[0];
    T max = data[0];
    for (const T& value : data) {
        min = std::min(min, value);
        max = std::max(max, value);
    }
    
    if (min == max) return histogram;
    
    float binWidth = (max - min) / bins;
    
    for (const T& value : data) {
        size_t binIndex = static_cast<size_t>((value - min) / binWidth);
        if (binIndex >= bins) binIndex = bins - 1;
        histogram[binIndex]++;
    }
    
    return histogram;
}

float computeMean(const std::vector<float>& data) {
    if (data.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float value : data) {
        sum += value;
    }
    
    return sum / data.size();
}

float computeStdDev(const std::vector<float>& data, float mean) {
    if (data.empty()) return 0.0f;
    
    float variance = 0.0f;
    for (float value : data) {
        float diff = value - mean;
        variance += diff * diff;
    }
    
    return std::sqrt(variance / data.size());
}

float computeEntropy(const std::vector<float>& data) {
    if (data.empty()) return 0.0f;
    
    std::vector<size_t> histogram = computeHistogram(data);
    float total = static_cast<float>(data.size());
    
    float entropy = 0.0f;
    for (size_t count : histogram) {
        if (count > 0) {
            float probability = count / total;
            entropy -= probability * std::log2(probability);
        }
    }
    
    return entropy;
}

} // namespace nlm

// Helper function to convert modality to string
std::string modalityToString(nlm::SensoryModality modality) {
    switch (modality) {
        case nlm::SensoryModality::Vision: return "vision";
        case nlm::SensoryModality::Touch: return "touch";
        case nlm::SensoryModality::Internal: return "internal";
        case nlm::SensoryModality::Proprioception: return "proprioception";
        case nlm::SensoryModality::Audio: return "audio";
        case nlm::SensoryModality::Vestibular: return "vestibular";
        case nlm::SensoryModality::Olfactory: return "olfactory";
        case nlm::SensoryModality::Gustatory: return "gustatory";
        default: return "unknown";
    }
}

// External function definitions (helpers)
namespace nlm {
    std::string modalityToString(SensoryModality modality) {
        return ::modalityToString(modality);
    }
}
