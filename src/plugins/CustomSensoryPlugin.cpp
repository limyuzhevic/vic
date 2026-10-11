#pragma once

#include "CustomSensoryPlugin.hpp"
#include "Core/PluginInterface.hpp"
#include "Core/Types/Types.hpp"
#include "Sensory/Vision.hpp"
#include "Sensory/Audio.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {
namespace plugins {

class CustomSensoryPlugin : public Plugin {
public:
    CustomSensoryPlugin() : baselineSensoryGain_(1.0f), attentionModulation_(1.0f),
                           featureExtractionEnabled_(true), multimodalFusion_(true),
                           adaptiveThresholding_(true), selectiveAttention_(true) {}
    
    const std::string& getName() const override {
        static const std::string name = "CustomSensoryPlugin";
        return name;
    }
    
    const std::string& getVersion() const override {
        static const std::string version = "1.1.0";
        return version;
    }
    
    const std::string& getAuthor() const override {
        static const std::string author = "NLM Development Team";
        return author;
    }
    
    const std::string& getDescription() const override {
        static const std::string description = "Advanced sensory processing with adaptive gain control, selective attention, and feature extraction for multimodal integration";
        return description;
    }
    
    bool initialize(std::shared_ptr<PluginManager> manager, 
                   std::shared_ptr<Config> config) override {
        pluginManager_ = manager;
        config_ = config;
        
        // Initialize random number generator
        std::random_device rd;
        rng_.seed(rd());
        
        // Apply configuration if available
        if (config_) {
            baselineSensoryGain_ = config_->getOr<float>("plugin_sensory_baseline_gain", baselineSensoryGain_);
            attentionModulation_ = config_->getOr<float>("plugin_sensory_attention_modulation", attentionModulation_);
            featureExtractionEnabled_ = config_->getOr<bool>("plugin_sensory_feature_extraction", featureExtractionEnabled_);
            multimodalFusion_ = config_->getOr<bool>("plugin_sensory_multimodal_fusion", multimodalFusion_);
            adaptiveThresholding_ = config_->getOr<bool>("plugin_sensory_adaptive_thresholding", adaptiveThresholding_);
            selectiveAttention_ = config_->getOr<bool>("plugin_sensory_selective_attention", selectiveAttention_);
        }
        
        // Register sensory processing hooks
        if (pluginManager_) {
            pluginManager_->executePluginFunction(getName(), "register_sensory_processing", {"vision", "audio", "internal"});
        }
        
        return true;
    }
    
    void shutdown() override {
        if (pluginManager_) {
            pluginManager_->executePluginFunction(getName(), "unregister_sensory_processing", {});
        }
        
        // Clear all processed data
        sensoryBuffer_.clear();
        featureVectors_.clear();
        attentionWeights_.clear();
        processedStreams_.clear();
    }
    
    void update(TimestepDuration dt) override {
        // Update sensory processing state
        currentTime_ += dt;
        
        // Update adaptive thresholds
        updateAdaptiveThresholds(dt);
        
        // Process sensory buffers
        processSensoryBuffers(dt);
        
        // Apply selective attention
        if (selectiveAttention_) {
            applySelectiveAttention();
        }
        
        // Perform feature extraction
        if (featureExtractionEnabled_) {
            extractFeatures();
        }
        
        // Fuse multimodal data
        if (multimodalFusion_) {
            fuseModalities();
        }
        
        // Apply gain modulation
        applyGainModulation(dt);
    }
    
    bool onPluginMessage(const std::string& message, 
                        const std::vector<std::string>& data) override {
        if (message == "sensory_input") {
            return handleSensoryInput(data);
        } else if (message == "attention_update") {
            return handleAttentionUpdate(data);
        } else if (message == "get_sensory_features") {
            return provideSensoryFeatures(data);
        } else if (message == "sensory_statistics") {
            return provideSensoryStatistics(data);
        } else if (message == "get_fused_perception") {
            return provideFusedPerception(data);
        }
        return false;
    }
    
    std::string getPluginSpecificInfo() const override {
        json info;
        info["baseline_sensory_gain"] = baselineSensoryGain_;
        info["attention_modulation"] = attentionModulation_;
        info["feature_extraction_enabled"] = featureExtractionEnabled_;
        info["multimodal_fusion"] = multimodalFusion_;
        info["adaptive_thresholding"] = adaptiveThresholding_;
        info["selective_attention"] = selectiveAttention_;
        info["current_time"] = currentTime_;
        info["sensory_buffer_size"] = sensoryBuffer_.size();
        info["feature_vectors_count"] = featureVectors_.size();
        info["active_attention_channels"] = activeAttentionChannels_.size();
        info["multimodal_score"] = getMultimodalScore();
        
        std::stringstream ss;
        ss << info.dump(2);
        return ss.str();
    }
    
    bool requires(const std::string& dependency) const override {
        static const std::vector<std::string> dependencies = {"brain", "sensory", "attention", "motor"};
        return std::find(dependencies.begin(), dependencies.end(), dependency) != dependencies.end();
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"brain", "sensory", "attention", "motor"};
    }
    
    // Sensory control methods
    void setBaselineSensoryGain(float gain) { baselineSensoryGain_ = std::max(0.0f, std::min(gain, 2.0f)); }
    float getBaselineSensoryGain() const { return baselineSensoryGain_; }
    
    void setAttentionModulation(float modulation) { attentionModulation_ = std::max(0.0f, std::min(modulation, 3.0f)); }
    float getAttentionModulation() const { return attentionModulation_; }
    
    void enableFeatureExtraction(bool enable) { featureExtractionEnabled_ = enable; }
    bool isFeatureExtractionEnabled() const { return featureExtractionEnabled_; }
    
    void enableMultimodalFusion(bool enable) { multimodalFusion_ = enable; }
    bool isMultimodalFusionEnabled() const { return multimodalFusion_; }
    
    void enableAdaptiveThresholding(bool enable) { adaptiveThresholding_ = enable; }
    bool isAdaptiveThresholdingEnabled() const { return adaptiveThresholding_; }
    
    void enableSelectiveAttention(bool enable) { selectiveAttention_ = enable; }
    bool isSelectiveAttentionEnabled() const { return selectiveAttention_; }
    
    // Sensory processing methods
    void processVisionInput(const std::vector<float>& visionData, float timestamp) {
        if (!multimodalFusion_ && !featureExtractionEnabled_) return;
        
        std::string streamId = "vision_" + std::to_string(timestamp);
        SensoryStream stream;
        stream.id = streamId;
        stream.type = "vision";
        stream.data = visionData;
        stream.timestamp = timestamp;
        
        // Apply gain modulation
        float gain = baselineSensoryGain_ * (1.0f + attentionModulation_ * getAttentionStrength(streamId));
        
        for (auto& value : stream.data) {
            value *= gain;
            // Apply adaptive threshold
            if (adaptiveThresholding_) {
                value = applyAdaptiveThreshold(value, stream.type);
            }
        }
        
        // Extract features if enabled
        if (featureExtractionEnabled_) {
            std::vector<float> features = extractFeatures(stream.data);
            stream.features = features;
            
            // Store feature vector
            featureVectors_[streamId] = features;
        }
        
        // Add to buffer
        sensoryBuffer_[streamId] = stream;
        
        // Notify other plugins
        if (pluginManager_) {
            pluginManager_->executePluginFunction(
                "ActionPlugin",
                "sensory_processed",
                {streamId, std::to_string(stream.data.size())}
            );
        }
    }
    
    void processAudioInput(const std::vector<float>& audioData, float timestamp) {
        std::string streamId = "audio_" + std::to_string(timestamp);
        SensoryStream stream;
        stream.id = streamId;
        stream.type = "audio";
        stream.data = audioData;
        stream.timestamp = timestamp;
        
        // Audio-specific processing
        float gain = baselineSensoryGain_ * 0.8f; // Audio typically has lower gain
        
        for (auto& value : stream.data) {
            value *= gain;
            if (adaptiveThresholding_) {
                value = applyAdaptiveThreshold(value, stream.type);
            }
        }
        
        if (featureExtractionEnabled_) {
            std::vector<float> features = extractFeatures(stream.data);
            stream.features = features;
            featureVectors_[streamId] = features;
        }
        
        sensoryBuffer_[streamId] = stream;
    }
    
    void processInternalInput(const std::vector<float>& internalData, float timestamp) {
        std::string streamId = "internal_" + std::to_string(timestamp);
        SensoryStream stream;
        stream.id = streamId;
        stream.type = "internal";
        stream.data = internalData;
        stream.timestamp = timestamp;
        
        // Internal signals have different gain profile
        float gain = baselineSensoryGain_ * 1.2f;
        
        for (auto& value : stream.data) {
            value *= gain;
            if (adaptiveThresholding_) {
                value = applyAdaptiveThreshold(value, stream.type);
            }
        }
        
        if (featureExtractionEnabled_) {
            std::vector<float> features = extractFeatures(stream.data);
            stream.features = features;
            featureVectors_[streamId] = features;
        }
        
        sensoryBuffer_[streamId] = stream;
    }
    
    // Feature extraction
    std::vector<float> extractFeatures(const std::vector<float>& data) {
        std::vector<float> features;
        
        if (data.empty()) return features;
        
        // Basic statistics
        float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        float variance = 0.0f;
        for (float val : data) {
            variance += (val - mean) * (val - mean);
        }
        variance /= data.size();
        float stddev = std::sqrt(variance);
        
        features.push_back(mean);
        features.push_back(stddev);
        features.push_back(variance);
        
        // Histogram features (simplified)
        int histogramBins = 5;
        float binWidth = 2.0f * stddev / histogramBins;
        std::vector<int> histogram(histogramBins, 0);
        
        for (float val : data) {
            int bin = static_cast<int>((val + stddev) / binWidth);
            if (bin >= 0 && bin < histogramBins) {
                histogram[bin]++;
            }
        }
        
        for (int count : histogram) {
            features.push_back(static_cast<float>(count) / data.size());
        }
        
        // Temporal features (if available)
        features.push_back(calculateTemporalCoherence(data));
        
        // Add some noise for biological realism
        std::normal_distribution<float> noise(0.0f, 0.01f);
        for (auto& feature : features) {
            feature += noise(rng_);
        }
        
        return features;
    }
    
    // Selective attention
    void applySelectiveAttention() {
        // Select most relevant sensory channels based on current context
        std::vector<std::string> allStreams;
        for (const auto& pair : sensoryBuffer_) {
            allStreams.push_back(pair.first);
        }
        
        // Compute attention weights based on feature similarity and relevance
        std::unordered_map<std::string, float> weights;
        
        for (const auto& streamId : allStreams) {
            auto it = sensoryBuffer_.find(streamId);
            if (it == sensoryBuffer_.end()) continue;
            
            float weight = computeAttentionWeight(it->second);
            weights[streamId] = weight;
            
            // Store for later use
            attentionWeights_[streamId] = weight;
            
            // Mark as active if weight > threshold
            if (weight > 0.5f) {
                activeAttentionChannels_.push_back(streamId);
            }
        }
        
        // Normalize weights
        float totalWeight = 0.0f;
        for (const auto& pair : weights) {
            totalWeight += pair.second;
        }
        
        if (totalWeight > 0.0f) {
            for (auto& pair : weights) {
                pair.second /= totalWeight;
            }
        }
    }
    
    float computeAttentionWeight(const SensoryStream& stream) const {
        float weight = 1.0f;
        
        // Based on feature variance (more variance = more attention)
        if (!stream.features.empty()) {
            float featureVariance = 0.0f;
            float featureMean = std::accumulate(stream.features.begin(), stream.features.end(), 0.0f) / stream.features.size();
            for (float feat : stream.features) {
                featureVariance += (feat - featureMean) * (feat - featureMean);
            }
            featureVariance /= stream.features.size();
            weight *= std::min(1.0f, featureVariance * 2.0f);
        }
        
        // Based on signal strength (mean absolute value)
        float signalStrength = 0.0f;
        for (float val : stream.data) {
            signalStrength += std::abs(val);
        }
        signalStrength /= stream.data.size();
        weight *= std::min(1.0f, signalStrength * 10.0f);
        
        // Random noise for biological realism
        std::uniform_real_distribution<float> noise(0.0f, 0.1f);
        weight *= (1.0f + noise(rng_) - 0.5f);
        
        return std::max(0.0f, std::min(weight, 1.0f));
    }
    
    float getAttentionStrength(const std::string& streamId) const {
        auto it = attentionWeights_.find(streamId);
        if (it != attentionWeights_.end()) {
            return it->second;
        }
        return 1.0f; // Default attention
    }
    
    // Multimodal fusion
    void fuseModalities() {
        // Fuse data from different sensory modalities
        std::unordered_map<std::string, std::vector<float>> fusedData;
        
        // Group streams by type
        std::unordered_map<std::string, std::vector<SensoryStream>> streamsByType;
        for (const auto& pair : sensoryBuffer_) {
            streamsByType[pair.second.type].push_back(pair.second);
        }
        
        // Fuse each modality group
        for (const auto& pair : streamsByType) {
            std::string modality = pair.first;
            const auto& streams = pair.second;
            
            if (streams.empty()) continue;
            
            // Average across streams of same type
            size_t totalElements = 0;
            std::vector<float> fused;
            
            for (const auto& stream : streams) {
                if (!fused.empty() && fused.size() != stream.data.size()) {
                    // Skip incompatible sizes
                    continue;
                }
                
                if (fused.empty()) {
                    fused = stream.data;
                    totalElements = stream.data.size();
                } else {
                    for (size_t i = 0; i < std::min(fused.size(), stream.data.size()); ++i) {
                        fused[i] += stream.data[i];
                    }
                    totalElements += stream.data.size();
                }
            }
            
            // Average
            for (auto& value : fused) {
                value /= streams.size();
            }
            
            fusedData[modality] = fused;
            
            // Store fused result
            fusedModality_[modality] = fused;
            
            // Notify other plugins
            if (pluginManager_) {
                pluginManager_->executePluginFunction(
                    "ActionPlugin",
                    "multimodal_fused",
                    {modality, std::to_string(fused.size())}
                );
            }
        }
    }
    
    float getMultimodalScore() const {
        // Calculate how well modalities are integrated
        if (fusedModality_.size() < 2) return 0.0f;
        
        float score = 0.0f;
        int count = 0;
        
        for (const auto& pair : fusedModality_) {
            const auto& data = pair.second;
            if (data.empty()) continue;
            
            // Calculate coherence (lower variance = more coherent)
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            
            score += std::exp(-variance);
            count++;
        }
        
        return count > 0 ? score / count : 0.0f;
    }
    
    std::vector<float> getFusedPerception() const {
        std::vector<float> result;
        
        // Combine all fused modalities
        for (const auto& pair : fusedModality_) {
            const auto& data = pair.second;
            result.insert(result.end(), data.begin(), data.end());
        }
        
        return result;
    }
    
    // Statistics and analysis
    float calculateTemporalCoherence(const std::vector<float>& data) const {
        // Simple coherence measure
        if (data.size() < 2) return 0.0f;
        
        float sum = 0.0f;
        for (size_t i = 1; i < data.size(); ++i) {
            sum += std::abs(data[i] - data[i-1]);
        }
        
        return 1.0f - std::min(1.0f, sum / (data.size() - 1));
    }
    
    float applyAdaptiveThreshold(float value, const std::string& modality) const {
        // Adaptive threshold based on modality statistics
        static float threshold = 0.5f;
        
        if (!adaptiveThresholding_) return value;
        
        // Simple adaptive threshold: decay over time, reset on strong signal
        threshold *= 0.99f;
        if (std::abs(value) > 0.8f) {
            threshold = 0.3f; // Reset threshold
        }
        
        return std::abs(value) > threshold ? value : 0.0f;
    }
    
    void updateAdaptiveThresholds(TimestepDuration dt) {
        // Update adaptive thresholds over time
        // In real implementation, would learn from experience
        adaptiveThreshold_ += dt.count() * 0.001f;
        if (adaptiveThreshold_ > 1.0f) adaptiveThreshold_ = 1.0f;
    }
    
    void processSensoryBuffers(TimestepDuration dt) {
        // Clean up old sensory buffers
        auto it = sensoryBuffer_.begin();
        while (it != sensoryBuffer_.end()) {
            if (currentTime_ - it->second.timestamp > 10.0f) { // 10 second retention
                it = sensoryBuffer_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void extractFeatures() {
        // Feature extraction is already done per stream
        // This could run periodic analysis on accumulated features
    }
    
    void applyGainModulation(TimestepDuration dt) {
        // Apply time-varying gain modulation
        gainModulation_ *= (1.0f - dt.count() * 0.01f);
        gainModulation_ += 0.01f; // Slowly increase
        gainModulation_ = std::min(2.0f, gainModulation_);
    }
    
    bool handleSensoryInput(const std::vector<std::string>& data) {
        if (data.size() < 2) return false;
        
        std::string modality = data[0];
        float timestamp = std::stof(data[1]);
        
        // In real implementation, would parse actual sensory data
        // For demo, create dummy data
        std::vector<float> dummyData(100, 0.0f);
        for (auto& val : dummyData) {
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
            val = dist(rng_);
        }
        
        if (modality == "vision") {
            processVisionInput(dummyData, timestamp);
        } else if (modality == "audio") {
            processAudioInput(dummyData, timestamp);
        } else if (modality == "internal") {
            processInternalInput(dummyData, timestamp);
        }
        
        return true;
    }
    
    bool handleAttentionUpdate(const std::vector<std::string>& data) {
        if (data.size() < 1) return false;
        
        // Update attention based on input
        // In real implementation, would use actual attention signals
        attentionModulation_ = std::stof(data[0]);
        return true;
    }
    
    bool provideSensoryFeatures(const std::vector<std::string>& data) {
        if (data.empty()) return false;
        
        std::string streamId = data[0];
        
        auto it = featureVectors_.find(streamId);
        if (it == featureVectors_.end()) {
            return false;
        }
        
        const auto& features = it->second;
        json featureJson = features;
        
        if (pluginManager_) {
            pluginManager_->executePluginFunction(
                getName(),
                "sensory_features",
                {streamId, featureJson.dump()}
            );
        }
        
        return true;
    }
    
    bool provideSensoryStatistics(const std::vector<std::string>& data) {
        json stats;
        stats["buffer_size"] = sensoryBuffer_.size();
        stats["feature_count"] = featureVectors_.size();
        stats["attention_channels"] = activeAttentionChannels_.size();
        stats["multimodal_score"] = getMultimodalScore();
        
        if (pluginManager_) {
            pluginManager_->executePluginFunction(
                getName(),
                "sensory_statistics",
                {stats.dump()}
            );
        }
        
        return true;
    }
    
    bool provideFusedPerception(const std::vector<std::string>& data) {
        std::vector<float> perception = getFusedPerception();
        json perceptionJson = perception;
        
        if (pluginManager_) {
            pluginManager_->executePluginFunction(
                getName(),
                "fused_perception",
                {perceptionJson.dump()}
            );
        }
        
        return true;
    }

private:
    struct SensoryStream {
        std::string id;
        std::string type;
        std::vector<float> data;
        std::vector<float> features;
        float timestamp = 0.0f;
    };
    
    float adaptiveThreshold_ = 0.5f;
    float gainModulation_ = 1.0f;
    float currentTime_ = 0.0f;
    std::mt19937 rng_;
    
    std::unordered_map<std::string, SensoryStream> sensoryBuffer_;
    std::unordered_map<std::string, std::vector<float>> featureVectors_;
    std::unordered_map<std::string, float> attentionWeights_;
    std::vector<std::string> activeAttentionChannels_;
    std::unordered_map<std::string, std::vector<float>> fusedModality_;
    std::unordered_map<std::string, std::vector<std::string>> processedStreams_;
};

} // namespace plugins
} // namespace nlm

// Plugin factory function
extern "C" {
    std::shared_ptr<nlm::cli::Plugin> createPlugin() {
        return std::make_shared<nlm::plugins::CustomSensoryPlugin>();
    }
}
