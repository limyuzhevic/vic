// AgentBrain cognition and thought module
// Handles cognitive functions including planner interaction, concepts, and self-model

#include "AgentBrainCognition.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/SelfModel.hpp"
#include "../cognition/SocialLearning.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Motor command name helper for debugging
declare const char* phase5CmdName(MotorCommand cmd);

void AgentBrainCognition::updateSelfModel(
    SelfModel& selfModel,
    bool& selfModelReady,
    const std::vector<float>& lastVision,
    const std::vector<float>& currentVision,
    MotorCommand lastDecoded,
    Brain* brain) {
    
    if (hasLastDecoded_ && !lastVision.empty() && !currentVision.empty() && !selfModelReady) {
        if (brain) {
            selfModel.initialize(brain);
            selfModelReady = true;
        }
    }
    
    if (selfModelReady) {
        ActionType act = ActionType::Wait;
        switch (lastDecoded) {
            case MotorCommand::MoveForward: act = ActionType::MoveForward; break;
            case MotorCommand::MoveBackward: act = ActionType::MoveBackward; break;
            case MotorCommand::TurnLeft: act = ActionType::TurnLeft; break;
            case MotorCommand::TurnRight: act = ActionType::TurnRight; break;
            case MotorCommand::Interact: act = ActionType::Interact; break;
            default: break;
        }
        selfModel.recordSelfAction(act, lastVision, currentVision);
    }
}

void AgentBrainCognition::updatePlannerInteraction(
    Brain* brain,
    bool& hasThoughtPlanned,
    MotorCommand& thoughtPlanned,
    int& plannerDecisionCount,
    int& plannerAgreementCount,
    int& plannerOverrideCount,
    std::vector<float>& lastVision,
    const std::string& lastText) {
    
    if (!brain->getPlanner() || lastVision.empty()) return;
    
    ActionType planned = brain->getPlanner()->planAction(lastVision);
    switch (planned) {
        case ActionType::MoveForward: thoughtPlanned = MotorCommand::MoveForward; break;
        case ActionType::MoveBackward: thoughtPlanned = MotorCommand::MoveBackward; break;
        case ActionType::TurnLeft: thoughtPlanned = MotorCommand::TurnLeft; break;
        case ActionType::TurnRight: thoughtPlanned = MotorCommand::TurnRight; break;
        case ActionType::Interact: thoughtPlanned = MotorCommand::Interact; break;
        default: thoughtPlanned = MotorCommand::Wait; break;
    }
    hasThoughtPlanned = true;
}

void AgentBrainCognition::updateConceptFormation(
    Brain* brain,
    int& conceptBiasCount,
    const std::vector<float>& lastVision,
    const std::vector<float>& lastTouch,
    float reward,
    DevelopmentalStage developmentalStage) {
    
    if (!brain->getConceptFormation() || lastVision.empty()) return;
    
    auto pat = conceptPattern();
    float norm = 0.0f;
    for (float v : pat) norm += v * v;
    if (norm > 0.01f) {
        brain->getConceptFormation()->presentExperience(pat, lastTouch, reward, 0);
        size_t cid = brain->getConceptFormation()->getMatchingConcept(pat);
        const auto* foundConcept = cid ? brain->getConceptFormation()->getConcept(cid) : nullptr;
        if (foundConcept && !foundConcept->instances.empty()) {
            float avg = 0.0f;
            for (const auto& inst : foundConcept->instances) avg += inst.avgReward;
            avg /= static_cast<float>(foundConcept->instances.size());
            
            if (avg > 0.1f && lastTouch.size() && lastTouch[0] > 0.05f) {
                conceptBiasCount++;
            }
        }
    }
}

void AgentBrainCognition::updateSocialLearning(
    SocialLearning& socialLearning,
    bool& socialReady,
    size_t& socialObservationCount,
    const std::string& lastText) {
    
    if (!socialReady && brain_) {
        socialLearning.initialize(brain_.get());
        socialReady = true;
    }
    
    if (socialReady) {
        socialObservationCount = socialLearning.getObservationCount();
    }
}

const char* AgentBrainCognition::phase5CmdName(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        default: return "Wait";
    }
}

std::vector<float> AgentBrainCognition::conceptPattern(
    const std::vector<float>& lastVision,
    const std::vector<float>& lastTouch) {
    std::vector<float> pat;
    for (float v : lastVision) pat.push_back(1.0f - v);
    for (float t : lastTouch) pat.push_back(t);
    return pat;
}

} // namespace nlm
