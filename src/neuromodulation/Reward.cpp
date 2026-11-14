float Reward::computeReward(const AgentState& agentState, 
                              const WorldState& worldState, 
                              const Brain& brain) const {
    // Simple reward system based on proximity to goals and avoiding hazards
    float totalReward = 0.0f;
    
    // Reward for proximity to goal
    float goalProximity = agentState.distanceToGoal;
    if (goalProximity > 0) {
        // Exponential reward - closer to goal = higher reward
        totalReward += 1.0f / (1.0f + goalProximity);
    }
    
    // Penalty for approaching hazards
    for (const auto& hazard : worldState.hazards) {
        float hazardDistance = agentState.position.distanceTo(hazard.position);
        if (hazardDistance < hazard.radius * 2.0f) {
            // Strong penalty for getting too close to hazards
            totalReward -= 5.0f / (hazardDistance + 0.1f);
        }
    }
    
    // Bonus reward for goal achievement
    if (worldState.goalAchieved) {
        totalReward += 10.0f;
    }
    
    // Energy conservation reward
    totalReward += (agentState.energy / 100.0f) * 0.1f;
    
    return totalReward;
}