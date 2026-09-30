// Episode Builder Implementation

#include "EpisodeBuilder.hpp"
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

namespace nlm {

void EpisodeBuilder::SimpleEpisode::addSegment(const EpisodeSegment& segment) {
    segments_.push_back(segment);
}

float EpisodeBuilder::SimpleEpisode::getTotalReward() const {
    float total = 0.0f;
    for (const auto& segment : segments_) {
        if (segment.type == EpisodeSegment::SegmentType::Reward) {
            total += segment.getRewardData().rewardValue;
        }
    }
    return total;
}

std::string EpisodeBuilder::SimpleEpisode::serialize() const {
    return EpisodeBuilder::episodeToJSON(*this);
}

bool EpisodeBuilder::SimpleEpisode::deserialize(const std::string& data) {
    return EpisodeBuilder::episodeFromJSON(*this, data);
}

std::unique_ptr<EpisodeBuilder::Episode> EpisodeBuilder::buildFromNeuralData(
    const std::vector<NeuralActivitySample>& neuralSamples,
    const EpisodeConfig& config) {
    auto episode = EpisodeFactory::createEpisode(config.mode);

    for (const auto& sample : neuralSamples) {
        EpisodeSegment segment = createNeuralSegment(
            sample.neuronId, sample.activation, sample.membranePotential,
            sample.firingRate, sample.timestamp, episode->getStepCount()
        );
        episode->addSegment(segment);
    }

    return episode;
}

std::unique_ptr<EpisodeBuilder::Episode> EpisodeBuilder::buildFromRewardData(
    const std::vector<RewardSample>& rewardSamples,
    const EpisodeConfig& config) {
    auto episode = EpisodeFactory::createEpisode(config.mode);

    for (const auto& sample : rewardSamples) {
        EpisodeSegment segment = createRewardSegment(
            sample.rewardValue, sample.predictedReward, sample.predictionError,
            sample.timestamp, episode->getStepCount()
        );
        episode->addSegment(segment);
    }

    return episode;
}

std::unique_ptr<EpisodeBuilder::Episode> EpisodeBuilder::buildFromEnvironmentalData(
    const std::vector<EnvironmentalContext>& envSamples,
    const EpisodeConfig& config) {
    auto episode = EpisodeFactory::createEpisode(config.mode);

    for (const auto& sample : envSamples) {
        EpisodeSegment segment = createEnvironmentalSegment(
            sample.actionType, sample.sensoryInput, sample.proprioception,
            sample.environmentState, std::chrono::system_clock::now(),
            episode->getStepCount()
        );
        episode->addSegment(segment);
    }

    return episode;
}

std::unique_ptr<EpisodeBuilder::Episode> EpisodeBuilder::buildMixedEpisode(
    const std::vector<EpisodeSegment>& segments,
    const EpisodeConfig& config) {
    auto episode = EpisodeFactory::createEpisode(config.mode);

    for (const auto& segment : segments) {
        episode->addSegment(segment);
    }

    return episode;
}

std::unique_ptr<EpisodeBuilder::Episode> EpisodeBuilder::EpisodeFactory::createEpisode(ConstructionMode mode) {
    switch (mode) {
        case ConstructionMode::Continuous:
        case ConstructionMode::Episodic:
        case ConstructionMode::EventDriven:
        case ConstructionMode::TimeWindow:
        case ConstructionMode::Adaptive:
        default:
            return std::make_unique<SimpleEpisode>();
    }
}

std::unique_ptr<EpisodeBuilder::Episode> EpisodeBuilder::EpisodeFactory::createFromConfig(const EpisodeConfig& config) {
    return createEpisode(config.mode);
}

std::string EpisodeBuilder::serializeEpisode(const Episode& episode) {
    return episodeToJSON(episode);
}

bool EpisodeBuilder::deserializeEpisode(Episode& episode, const std::string& data) {
    return episodeFromJSON(episode, data);
}

std::string EpisodeBuilder::episodeToJSON(const Episode& episode) {
    json j;
    j["stepCount"] = episode.getStepCount();
    j["startTime"] = std::chrono::system_clock::to_time_t(episode.getStartTime());
    j["endTime"] = std::chrono::system_clock::to_time_t(episode.getEndTime());
    j["totalReward"] = episode.getTotalReward();
    
    json segments = json::array();
    for (const auto& segment : episode.getSegments()) {
        json segmentJson;
        segmentJson["type"] = static_cast<int>(segment.type);
        segmentJson["timestamp"] = std::chrono::system_clock::to_time_t(segment.timestamp);
        segmentJson["stepCount"] = segment.stepCount;
        
        switch (segment.type) {
            case EpisodeSegment::SegmentType::NeuralActivation: {
                auto& neuralData = segment.getNeuralData();
                segmentJson["neuronId"] = neuralData.neuronId;
                segmentJson["activation"] = neuralData.activation;
                segmentJson["membranePotential"] = neuralData.membranePotential;
                segmentJson["firingRate"] = neuralData.firingRate;
                break;
            }
            case EpisodeSegment::SegmentType::Reward: {
                auto& rewardData = segment.getRewardData();
                segmentJson["rewardValue"] = rewardData.rewardValue;
                segmentJson["predictedReward"] = rewardData.predictedReward;
                segmentJson["predictionError"] = rewardData.predictionError;
                break;
            }
            case EpisodeSegment::SegmentType::Environmental: {
                auto& envData = segment.getEnvData();
                segmentJson["actionType"] = envData.actionType;
                segmentJson["sensoryInput"] = envData.sensoryInput;
                segmentJson["proprioception"] = envData.proprioception;
                segmentJson["environmentState"] = envData.environmentState;
                break;
            }
        }
        
        segments.push_back(segmentJson);
    }
    
    j["segments"] = segments;
    return j.dump(2);
}

bool EpisodeBuilder::episodeFromJSON(Episode& episode, const std::string& jsonData) {
    try {
        json j = json::parse(jsonData);
        
        return true;
    } catch (const json::exception& e) {
        return false;
    }
}

bool EpisodeBuilder::validateEpisode(const Episode& episode) {
    if (episode.getStepCount() == 0) {
        return false;
    }
    
    const auto& segments = episode.getSegments();
    if (segments.empty()) {
        return false;
    }
    
    bool hasNeuralData = false;
    bool hasRewardData = false;
    
    for (const auto& segment : segments) {
        switch (segment.type) {
            case EpisodeSegment::SegmentType::NeuralActivation:
                hasNeuralData = true;
                break;
            case EpisodeSegment::SegmentType::Reward:
                hasRewardData = true;
                break;
        }
    }
    
    return hasNeuralData || hasRewardData;
}

float EpisodeBuilder::calculateEpisodeCohesion(const Episode& episode) {
    const auto& segments = episode.getSegments();
    if (segments.size() < 2) return 1.0f;
    
    float sumCohesion = 0.0f;
    for (size_t i = 1; i < segments.size(); ++i) {
        float cohesion = 1.0f;
        sumCohesion += cohesion;
    }
    
    return sumCohesion / segments.size();
}

float EpisodeBuilder::calculateEpisodeIntegrity(const Episode& episode) {
    return 1.0f;
}

float EpisodeBuilder::calculateMeanReward(const Episode& episode) {
    float totalReward = episode.getTotalReward();
    size_t stepCount = episode.getStepCount();
    return stepCount > 0 ? totalReward / stepCount : 0.0f;
}

float EpisodeBuilder::calculateMaxActivation(const Episode& episode) {
    float maxActivation = 0.0f;
    for (const auto& segment : episode.getSegments()) {
        if (segment.type == EpisodeSegment::SegmentType::NeuralActivation) {
            maxActivation = std::max(maxActivation, segment.getNeuralData().activation);
        }
    }
    return maxActivation;
}

size_t EpisodeBuilder::calculateActiveNeuronCount(const Episode& episode) {
    size_t activeCount = 0;
    std::vector<size_t> seenNeurons;
    
    for (const auto& segment : episode.getSegments()) {
        if (segment.type == EpisodeSegment::SegmentType::NeuralActivation) {
            size_t neuronId = segment.getNeuralData().neuronId;
            if (std::find(seenNeurons.begin(), seenNeurons.end(), neuronId) == seenNeurons.end()) {
                seenNeurons.push_back(neuronId);
                activeCount++;
            }
        }
    }
    
    return activeCount;
}

EpisodeSegment EpisodeBuilder::createNeuralSegment(size_t neuronId, float activation,
                                                 float membranePotential, float firingRate,
                                                 std::chrono::system_clock::time_point timestamp,
                                                 size_t stepCount) {
    EpisodeSegment segment;
    segment.type = EpisodeSegment::SegmentType::NeuralActivation;
    segment.timestamp = timestamp;
    segment.stepCount = stepCount;
    segment.getNeuralData().neuronId = neuronId;
    segment.getNeuralData().activation = activation;
    segment.getNeuralData().membranePotential = membranePotential;
    segment.getNeuralData().firingRate = firingRate;
    segment.getNeuralData().timestamp = timestamp;
    return segment;
}

EpisodeSegment EpisodeBuilder::createRewardSegment(float rewardValue, float predictedReward,
                                                 float predictionError,
                                                 std::chrono::system_clock::time_point timestamp,
                                                 size_t stepCount) {
    EpisodeSegment segment;
    segment.type = EpisodeSegment::SegmentType::Reward;
    segment.timestamp = timestamp;
    segment.stepCount = stepCount;
    segment.getRewardData().rewardValue = rewardValue;
    segment.getRewardData().predictedReward = predictedReward;
    segment.getRewardData().predictionError = predictionError;
    segment.getRewardData().timestamp = timestamp;
    return segment;
}

EpisodeSegment EpisodeBuilder::createEnvironmentalSegment(const std::string& actionType,
                                                       const std::vector<float>& sensoryInput,
                                                       const std::vector<float>& proprioception,
                                                       const std::string& environmentState,
                                                       std::chrono::system_clock::time_point timestamp,
                                                       size_t stepCount) {
    EpisodeSegment segment;
    segment.type = EpisodeSegment::SegmentType::Environmental;
    segment.timestamp = timestamp;
    segment.stepCount = stepCount;
    segment.getEnvData().actionType = actionType;
    segment.getEnvData().sensoryInput = sensoryInput;
    segment.getEnvData().proprioception = proprioception;
    segment.getEnvData().environmentState = environmentState;
    return segment;
}

EpisodeSegment EpisodeBuilder::createMetadataSegment(const std::string& key, const std::string& value,
                                                  std::chrono::system_clock::time_point timestamp,
                                                  size_t stepCount) {
    EpisodeSegment segment;
    segment.type = EpisodeSegment::SegmentType::Metadata;
    segment.timestamp = timestamp;
    segment.stepCount = stepCount;
    segment.getMetadataData().key = key;
    segment.getMetadataData().value = value;
    return segment;
}

} // namespace nlm
