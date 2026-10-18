#include "CuriosityManager.hpp"

namespace nlm {

CuriosityManager::CuriosityManager() 
    : curiosityGain_(2.0f), explorationRate_(0.3f), noveltyDecay_(0.99f),
      noveltyLevel_(0.0f), curiosityLevel_(0.0f), noveltyDecayCounter_(0.0f) { }

CuriosityManager::~CuriosityManager() = default;

float CuriosityManager::computeNovelty(const std::vector<float>& vision) const {
    if (vision.empty()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize by vision size
    float novelty = totalDiff / std::max<size_t>(vision.size(), 1);
    
    NLM_LOG_DEBUG("CuriosityManager: Computed novelty " + std::to_string(novelty));
    return novelty;
}

float CuriosityManager::computeCuriosity(float novelty, float predictionError) const {
    // Compute curiosity as weighted combination of novelty and prediction error
    float curiosity = novelty * curiosityGain_ + std::abs(predictionError) * 0.5f;
    
    NLM_LOG_DEBUG("CuriosityManager: Computed curiosity " + std::to_string(curiosity) + 
                 " (novelty: " + std::to_string(novelty) + ", predictionError: " + 
                 std::to_string(predictionError) + ")");
    return curiosity;
}

float CuriosityManager::clampCuriosity(float curiosity) const {
    float clamped = std::clamp(curiosity, 0.0f, 1.0f);
    if (clamped != curiosity) {
        NLM_LOG_DEBUG("CuriosityManager: Clamped curiosity from " + std::to_string(curiosity) + 
                     " to " + std::to_string(clamped));
    }
    return clamped;
}

bool CuriosityManager::shouldExplore(float curiosityLevel, float explorationThreshold) const {
    bool should = curiosityLevel > explorationThreshold;
    if (should) {
        NLM_LOG_DEBUG("CuriosityManager: Curiosity " + std::to_string(curiosityLevel) + 
                     " exceeds threshold " + std::to_string(explorationThreshold) + ", triggering exploration");
    }
    return should;
}

MotorCommand CuriosityManager::selectRandomMotorCommand(class RandomGenerator& rng) const {
    // Exploration: choose random action
    int choice = rng.uniformInt(0, 7);
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

void CuriosityManager::reset() {
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    noveltyDecayCounter_ = 0.0f;
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
MotorCommand CuriosityManager::selectRandomMotorCommand(class RandomGenerator& rng) const {
    // Exploration: choose random action
    int choice = rng.uniformInt(0, 7);
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

std::vector<std::pair<MotorCommand, int>> CuriosityManager::getActionSelectionStats() const {
    std::vector<std::pair<MotorCommand, int>> stats;
    
    // For now, return empty stats since we don't track action selection statistics
    // In a full implementation, this would return counts of how many times
    // each motor command was selected by the agent
    stats.push_back({MotorCommand::MoveForward, 0});
    stats.push_back({MotorCommand::MoveBackward, 0});
    stats.push_back({MotorCommand::TurnLeft, 0});
    stats.push_back({MotorCommand::TurnRight, 0});
    stats.push_back({MotorCommand::LookLeft, 0});
    stats.push_back({MotorCommand::LookRight, 0});
    stats.push_back({MotorCommand::Interact, 0});
    stats.push_back({MotorCommand::Wait, 0});
    
    return stats;
}

void CuriosityManager::reset() {
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    noveltyDecayCounter_ = 0.0f;
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    NLM_LOG_INFO("CuriosityManager: Reset");
}

} // namespace nlm
