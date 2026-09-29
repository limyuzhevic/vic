#include "Environment.hpp"
#include "../agent/AgentBody.hpp"
#include "../world/WorldObject.hpp"
#include "WorldConstants.hpp"

namespace nlm {

void Environment::initializeRandomWorld(AgentBody& agent, std::vector<WorldObject>& objects, 
                                      float width, float height, uint64_t& rngState) {
    // Reset agent to random position within bounds
    agent.x = Environment::urand(rngState, 2.0f, width - 2.0f);
    agent.y = Environment::urand(rngState, 2.0f, height - 2.0f);
    agent.energy = WorldConstants::DEFAULT_MAX_ENERGY;
    
    objects.clear();
    
    // Place resources
    for (int i = 0; i < WorldConstants::NUM_RESOURCES; ++i) {
        float ox = Environment::urand(rngState, 1.0f, width - 1.0f);
        float oy = Environment::urand(rngState, 1.0f, height - 1.0f);
        objects.emplace_back(ox, oy, WorldObjectType::Resource, 
                           WorldConstants::HAZARD_ENERGY_LOSS, WorldConstants::SMALL_OBJECT_RADIUS);
    }
    
    // Place hazards
    for (int i = 0; i < WorldConstants::NUM_HAZARDS; ++i) {
        float ox = Environment::urand(rngState, 1.0f, width - 1.0f);
        float oy = Environment::urand(rngState, 1.0f, height - 1.0f);
        objects.emplace_back(ox, oy, WorldObjectType::Hazard, 
                           WorldConstants::HAZARD_ENERGY_LOSS, WorldConstants::MEDIUM_OBJECT_RADIUS);
    }
}

void Environment::addResource(float x, float y, std::vector<WorldObject>& objects, uint64_t& rngState) const {
    objects.emplace_back(x, y, WorldObjectType::Resource, 
                        WorldConstants::HAZARD_ENERGY_LOSS, WorldConstants::SMALL_OBJECT_RADIUS);
}

void Environment::addHazard(float x, float y, std::vector<WorldObject>& objects, uint64_t& rngState) const {
    objects.emplace_back(x, y, WorldObjectType::Hazard, 
                        WorldConstants::HAZARD_ENERGY_LOSS, WorldConstants::MEDIUM_OBJECT_RADIUS);
}

float Environment::calculateEnergyFromResource(float resourceValue) const {
    return std::min(WorldConstants::DEFAULT_MAX_ENERGY, 
                   WorldConstants::DEFAULT_MAX_ENERGY + WorldConstants::RESOURCE_ENERGY_GAIN);
}

float Environment::calculateDamageFromHazard(float hazardValue) const {
    return -hazardValue;
}

uint64_t Environment::lcg(uint64_t& state) const {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state;
}

float Environment::urand(uint64_t& state, float min, float max) const {
    uint64_t r = lcg(state);
    float normalized = (r & 0xFFFFFFFFFFFFULL) / static_cast<float>(0xFFFFFFFFFFFFULL);
    return min + normalized * (max - min);
}

} // namespace nlm