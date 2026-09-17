#include "DevelopmentalMilestones.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct DevelopmentalMilestones::Impl {
    std::vector<DevelopmentalMilestone> milestones;
    std::vector<const char*> achievedMilestones;
    
    Impl() {}
};

DevelopmentalMilestones::DevelopmentalMilestones() : pImpl(new Impl) {}

DevelopmentalMilestones::~DevelopmentalMilestones() = default;

void DevelopmentalMilestones::initializeMilestones(Brain* brain) {
    pImpl->milestones.clear();
    
    // Define developmental milestones
    pImpl->milestones.push_back(DevelopmentalMilestone("Sensory Processing", DevelopmentalStage::Initial, 30.0, 0.8f));
    pImpl->milestones.push_back(DevelopmentalMilestone("Motor Coordination", DevelopmentalStage::Initial, 40.0, 0.7f));
    pImpl->milestones.push_back(DevelopmentalMilestone("Basic Memory", DevelopmentalStage::CriticalPeriod, 120.0, 0.6f));
    pImpl->milestones.push_back(DevelopmentalMilestone("Planning Ability", DevelopmentalStage::CriticalPeriod, 200.0, 0.7f));
    pImpl->milestones.push_back(DevelopmentalMilestone("Abstract Thinking", DevelopmentalStage::Maturation, 400.0, 0.8f));
    pImpl->milestones.push_back(DevelopmentalMilestone("Social Behavior", DevelopmentalStage::Maturation, 500.0, 0.7f));
    pImpl->milestones.push_back(DevelopmentalMilestone("Adult Capabilities", DevelopmentalStage::Adult, 800.0, 0.9f));
    pImpl->milestones.push_back(DevelopmentalMilestone("Wisdom", DevelopmentalStage::Adult, 1000.0, 0.8f));
}

void DevelopmentalMilestones::update(Brain* brain, double age, double dt) {
    // Check for milestone achievements
    for (auto& milestone : pImpl->milestones) {
        if (!milestone.achieved && age >= milestone.age) {
            // Check if development is sufficient for the milestone
            float developmentProgress = getDevelopmentProgress(brain, milestone.stage);
            
            if (developmentProgress >= milestone.progressThreshold) {
                milestone.achieved = true;
                milestone.achievedAge = age;
                pImpl->achievedMilestones.push_back(milestone.name);
                
                NLM_LOG_INFO("Developmental milestone achieved: " + std::string(milestone.name) + 
                           " at age " + std::to_string(milestone.achievedAge));
            }
        }
    }
}

std::vector<const char*> DevelopmentalMilestones::getAchievedMilestones(double age) const {
    std::vector<const char*> achieved;
    for (const char* name : pImpl->achievedMilestones) {
        // Only include milestones achieved up to current age
        for (const auto& milestone : pImpl->milestones) {
            if (strcmp(milestone.name, name) == 0 && milestone.achieved) {
                achieved.push_back(name);
                break;
            }
        }
    }
    return achieved;
}

std::vector<const char*> DevelopmentalMilestones::getNextMilestones(DevelopmentalStage stage) const {
    std::vector<const char*> next;
    for (const auto& milestone : pImpl->milestones) {
        if (milestone.stage == stage && !milestone.achieved) {
            next.push_back(milestone.name);
        }
    }
    return next;
}

bool DevelopmentalMilestones::isMilestoneAchieved(const char* milestoneName) const {
    for (const auto& milestone : pImpl->milestones) {
        if (strcmp(milestone.name, milestoneName) == 0) {
            return milestone.achieved;
        }
    }
    return false;
}

void DevelopmentalMilestones::recordMilestoneAchievement(const char* milestoneName, double age) {
    for (auto& milestone : pImpl->milestones) {
        if (strcmp(milestone.name, milestoneName) == 0) {
            milestone.achieved = true;
            milestone.achievedAge = age;
            
            // Add to achieved list if not already there
            bool alreadyAchieved = false;
            for (const char* name : pImpl->achievedMilestones) {
                if (strcmp(name, milestoneName) == 0) {
                    alreadyAchieved = true;
                    break;
                }
            }
            if (!alreadyAchieved) {
                pImpl->achievedMilestones.push_back(milestone.name);
            }
            break;
        }
    }
}

float DevelopmentalMilestones::getStageMilestoneProgress(DevelopmentalStage stage) const {
    float totalProgress = 0.0f;
    int count = 0;
    
    for (const auto& milestone : pImpl->milestones) {
        if (milestone.stage == stage) {
            totalProgress += milestone.achieved ? 1.0f : 0.0f;
            count++;
        }
    }
    
    return count > 0 ? totalProgress / count : 0.0f;
}

float DevelopmentalMilestones::getDevelopmentProgress(Brain* brain, DevelopmentalStage stage) const {
    // Simplified developmental progress calculation
    // In a full implementation, this would use actual metrics
    double age = 0.0;  // Would come from developmental age
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Different stages have different development curves
    switch (stage) {
        case DevelopmentalStage::Initial:
            return std::min(1.0f, static_cast<float>(age / 60.0f) * 1.5f);
        case DevelopmentalStage::CriticalPeriod:
            return std::min(1.0f, static_cast<float>((age - 60.0) / 240.0f) * 1.5f);
        case DevelopmentalStage::Maturation:
            return std::min(1.0f, static_cast<float>((age - 300.0) / 360.0f) * 1.5f);
        case DevelopmentalStage::Adult:
            return std::min(1.0f, static_cast<float>((age - 660.0) / 360.0f) * 1.5f);
        case DevelopmentalStage::Aging:
            return std::max(0.0f, 1.0f - static_cast<float>((age - 1020.0) / 360.0f));
        default:
            return 0.0f;
    }
}

} // namespace nlm