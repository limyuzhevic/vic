#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

namespace nlm {

class DevelopmentalMilestones {
public:
    DevelopmentalMilestones();
    ~DevelopmentalMilestones();
    
    // Initialize milestones for each developmental stage
    void initializeMilestones(Brain* brain);
    
    // Update milestone tracking
    void update(Brain* brain, double age, double dt);
    
    // Check if any milestones are achieved
    std::vector<const char*> getAchievedMilestones(double age) const;
    
    // Get next milestone(s) to achieve
    std::vector<const char*> getNextMilestones(DevelopmentalStage stage) const;
    
    // Check if a milestone has been achieved
    bool isMilestoneAchieved(const char* milestoneName) const;
    
    // Record achievement of a milestone
    void recordMilestoneAchievement(const char* milestoneName, double age);
    
    // Get milestone progress for current stage
    float getStageMilestoneProgress(DevelopmentalStage stage) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm