#include "VisionSystem.hpp"
#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "WorldConstants.hpp"
#include <cmath>

namespace nlm {

void VisionSystem::generateRayVision(const AgentBody& agent, const SimpleWorld& world,
                                    SensoryPercept& sensory) {
    std::vector<float> vision(WorldConstants::RAY_COUNT, 1.0f);
    
    for (int i = 0; i < WorldConstants::RAY_COUNT; ++i) {
        float angle = agent.orientation - WorldConstants::FOV_ANGLE / 2.0f +
                     (WorldConstants::FOV_ANGLE * i / (WorldConstants::RAY_COUNT - 1));
        
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < WorldConstants::VISION_MAX_RANGE) {
            float px = agent.x + dx * t;
            float py = agent.y + dy * t;
            
            if (!world.isValidPosition(px, py)) {
                vision[i] = t / WorldConstants::VISION_MAX_RANGE;
                break;
            }
            
            WorldObject* obj = world.getObjectAt(px, py);
            if (obj != nullptr) {
                if (obj->type == WorldObjectType::Resource) {
                    vision[i] = (WorldConstants::VISION_MAX_RANGE - t) /
                              WorldConstants::VISION_MAX_RANGE *
                              WorldConstants::RESOURCE_BRIGHTNESS;
                } else if (obj->type == WorldObjectType::Hazard) {
                    vision[i] = (WorldConstants::VISION_MAX_RANGE - t) /
                              WorldConstants::VISION_MAX_RANGE *
                              WorldConstants::HAZARD_DARKNESS;
                } else {
                    vision[i] = WorldConstants::EMPTY_VISION_VALUE;
                }
                break;
            }
            
            t += WorldConstants::RAY_STEP_SIZE;
        }
    }
    
    sensory.setVision(vision);
}

void VisionSystem::generateTouchVision(const AgentBody& agent, const SimpleWorld& world,
                                     SensoryPercept& sensory) {
    std::vector<float> touch(8, 0.0f);
    for (int i = 0; i < 8; ++i) {
        float angle = agent.orientation + 2.0f * M_PI * i / 8.0f;
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < WorldConstants::TOUCH_MAX_RANGE) {
            float px = agent.x + dx * t;
            float py = agent.y + dy * t;
            
            if (!world.isValidPosition(px, py) ||
                world.getObjectAt(px, py) != nullptr) {
                touch[i] = 1.0f - t / WorldConstants::TOUCH_MAX_RANGE;
                break;
            }
            t += WorldConstants::RAY_STEP_SIZE;
        }
    }
    
    sensory.setTouch(touch);
}

void VisionSystem::generateInternalSignals(const AgentBody& agent, const SimpleWorld& world,
                                         SensoryPercept& sensory) {
    std::vector<float> internal(4, 0.0f);
    internal[0] = agent.energy / WorldConstants::DEFAULT_MAX_ENERGY;
    internal[1] = agent.health;
    internal[2] = agent.isMoving ? 1.0f : 0.0f;
    internal[3] = agent.isTurning ? 1.0f : 0.0f;
    
    sensory.setInternal(internal);
}

void VisionSystem::generateProprioception(const AgentBody& agent, const SimpleWorld& world,
                                        SensoryPercept& sensory) {
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = agent.x / world.getWidth();
    proprioception[1] = agent.y / world.getHeight();
    proprioception[2] = (agent.orientation + M_PI) / (2.0f * M_PI);
    proprioception[3] = std::abs(agent.velocityX) / WorldConstants::MAX_VELOCITY_NORMALIZATION;
    proprioception[4] = std::abs(agent.velocityY) / WorldConstants::MAX_VELOCITY_NORMALIZATION;
    proprioception[5] = std::abs(agent.angularVelocity) / WorldConstants::MAX_ANGULAR_VELOCITY_NORMALIZATION;
    
    sensory.setProprioception(proprioception);
}

} // namespace nlm