#pragma once

#include "../agent/AgentBody.hpp"
#include "WorldConstants.hpp"
#include <vector>

namespace nlm {

class Environment {
public:
    Environment() = default;
    ~Environment() = default;
    
    void initializeRandomWorld(AgentBody& agent, std::vector<WorldObject>& objects, 
                              float width, float height, uint64_t& rngState);
    void addResource(float x, float y, std::vector<WorldObject>& objects, uint64_t& rngState) const;
    void addHazard(float x, float y, std::vector<WorldObject>& objects, uint64_t& rngState) const;
    float calculateEnergyFromResource(float resourceValue) const;
    float calculateDamageFromHazard(float hazardValue) const;
};

} // namespace nlm