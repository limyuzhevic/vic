// Episode Builder Class
// Unify episode construction logic with factory pattern and standardized serialization

#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace nlm {

struct NeuralActivitySample {
    size_t neuronId;
    float activation;
    float membranePotential;
    float firingRate;
    std::chrono::system_clock::time_point timestamp;
};

struct RewardSample {
    float rewardValue;
    float predictedReward;
    float predictionError;
    std::chrono::system_clock::time_point timestamp;
};

struct EnvironmentalContext {
    std::string actionType;
    std::vector<float> sensoryInput;
    std::vector<float> proprioception;
    std::string environmentState;
};

struct EpisodeSegment {
    enum class SegmentType {
        NeuralActivation,
        Reward,
        Environmental,
        Transition,
        Metadata
    };

    SegmentType type;
    std::chrono::system_clock::time_point timestamp;
    size_t stepCount;
    
    // Segment data
    union {
        NeuralActivitySample neuralData;
        RewardSample rewardData;
        EnvironmentalContext envData;
        struct {
            float transitionProb;
            float eligibilityTrace;
        } transitionData;
        struct {
            std::string key;
            std::string value;
        } metadataData;
    };

    // Helper accessors
    NeuralActivitySample& getNeuralData() { return neuralData; }
    const NeuralActivitySample& getNeuralData() const { return neuralData; }
    
    RewardSample& getRewardData() { return rewardData; }
    const RewardSample& getRewardData() const { return rewardData; }
    
    EnvironmentalContext& getEnvData() { return envData; }
    const EnvironmentalContext& getEnvData() const { return envData; }
    
    auto& getTransitionData() { return transitionData; }
    const auto& getTransitionData() const { return transitionData; }
    
    auto& getMetadataData() { return metadataData; }
    const auto& getMetadataData() const { return metadataData; }
};

class EpisodeBuilder {
public:
    // Episode construction modes
    enum class ConstructionMode {
        Continuous,      // Continuous episode without boundaries
        Episodic,        // Episodic segmentation
        EventDriven,     // Event-driven segmentation
        TimeWindow,      // Fixed time window segmentation
        Adaptive        // Adaptive segmentation based on activity
    };

    // Configuration for episode building
    struct EpisodeConfig {
        ConstructionMode mode = ConstructionMode::Episodic;
        size_t maxEpisodeLength = 1000;
        float rewardThreshold = 0.1f;
        float activityThreshold = 0.5f;
        std::chrono::milliseconds timeWindow = std::chrono::milliseconds(100);
        bool includeNeuralData = true;
        bool includeRewardData = true;
        bool includeEnvironmentalData = true;
        bool standardizeActivation = false;
        float activationNormalizationFactor = 1.0f;
    };

    // Episode interface
    class Episode {
    public:
        virtual ~Episode() = default;
        virtual void addSegment(const EpisodeSegment& segment) = 0;
        virtual const std::vector<EpisodeSegment>& getSegments() const = 0;
        virtual size_t getStepCount() const = 0;
        virtual std::chrono::system_clock::time_point getStartTime() const = 0;
        virtual std::chrono::system_clock::time_point getEndTime() const = 0;
        virtual float getTotalReward() const = 0;
        virtual std::string serialize() const = 0;
        virtual bool deserialize(const std::string& data) = 0;
    };

    // Concrete episode implementation
    class SimpleEpisode : public Episode {
    public:
        SimpleEpisode() = default;
        
        void addSegment(const EpisodeSegment& segment) override;
        const std::vector<EpisodeSegment>& getSegments() const override { return segments_; }
        size_t getStepCount() const override { return segments_.empty() ? 0 : segments_.back().stepCount; }
        std::chrono::system_clock::time_point getStartTime() const override { 
            return segments_.empty() ? std::chrono::system_clock::time_point{} : segments_.front().timestamp;
        }
        std::chrono::system_clock::time_point getEndTime() const override {
            return segments_.empty() ? std::chrono::system_clock::time_point{} : segments_.back().timestamp;
        }
        float getTotalReward() const override;
        std::string serialize() const override;
        bool deserialize(const std::string& data) override;

    private:
        std::vector<EpisodeSegment> segments_;
    };

    // Construct episode from neural data
    std::unique_ptr<Episode> buildFromNeuralData(const std::vector<NeuralActivitySample>& neuralSamples,
                                                 const EpisodeConfig& config = EpisodeConfig());

    // Construct episode from reward data
    std::unique_ptr<Episode> buildFromRewardData(const std::vector<RewardSample>& rewardSamples,
                                                 const EpisodeConfig& config = EpisodeConfig());

    // Construct episode from environmental data
    std::unique_ptr<Episode> buildFromEnvironmentalData(const std::vector<EnvironmentalContext>& envSamples,
                                                       const EpisodeConfig& config = EpisodeConfig());

    // Construct episode from mixed data
    std::unique_ptr<Episode> buildMixedEpisode(const std::vector<EpisodeSegment>& segments,
                                              const EpisodeConfig& config = EpisodeConfig());

    // Create episode factory
    class EpisodeFactory {
    public:
        static std::unique_ptr<Episode> createEpisode(ConstructionMode mode);
        static std::unique_ptr<Episode> createFromConfig(const EpisodeConfig& config);
    };

    // Serialize/deserialize utilities
    static std::string serializeEpisode(const Episode& episode);
    static bool deserializeEpisode(Episode& episode, const std::string& data);
    static std::string episodeToJSON(const Episode& episode);
    static bool episodeFromJSON(Episode& episode, const std::string& jsonData);

    // Validation and quality checks
    static bool validateEpisode(const Episode& episode);
    static float calculateEpisodeCohesion(const Episode& episode);
    static float calculateEpisodeIntegrity(const Episode& episode);

    // Statistical analysis
    static float calculateMeanReward(const Episode& episode);
    static float calculateMaxActivation(const Episode& episode);
    static size_t calculateActiveNeuronCount(const Episode& episode);

private:
    // Internal helper functions
    static EpisodeSegment createNeuralSegment(size_t neuronId, float activation, 
                                             float membranePotential, float firingRate,
                                             std::chrono::system_clock::time_point timestamp,
                                             size_t stepCount);
    static EpisodeSegment createRewardSegment(float rewardValue, float predictedReward,
                                              float predictionError,
                                              std::chrono::system_clock::time_point timestamp,
                                              size_t stepCount);
    static EpisodeSegment createEnvironmentalSegment(const std::string& actionType,
                                                     const std::vector<float>& sensoryInput,
                                                     const std::vector<float>& proprioception,
                                                     const std::string& environmentState,
                                                     std::chrono::system_clock::time_point timestamp,
                                                     size_t stepCount);
    static EpisodeSegment createMetadataSegment(const std::string& key, const std::string& value,
                                                std::chrono::system_clock::time_point timestamp,
                                                size_t stepCount);
};

} // namespace nlm
