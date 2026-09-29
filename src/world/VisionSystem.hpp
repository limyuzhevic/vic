#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "WorldConstants.hpp"
#include <vector>
#include <cmath>

namespace nlm {

class VisionSystem {
public:
    VisionSystem() = default;
    ~VisionSystem() = default;
    
    void generateVision(const AgentBody& agent, const SimpleWorld& world,
                       SensoryPercept& sensory, double simTime) {
        generateRayVision(agent, world, sensory);
        generateTouchVision(agent, world, sensory);
        generateInternalSignals(agent, world, sensory);
        generateProprioception(agent, world, sensory);
    }
    
private:
    void generateRayVision(const AgentBody& agent, const SimpleWorld& world,
                          SensoryPercept& sensory);
    void generateTouchVision(const AgentBody& agent, const SimpleWorld& world,
                           SensoryPercept& sensory);
    void generateInternalSignals(const AgentBody& agent, const SimpleWorld& world,
                               SensoryPercept& sensory);
    void generateProprioception(const AgentBody& agent, const SimpleWorld& world,
                              SensoryPercept& sensory);
};

} // namespace nlm