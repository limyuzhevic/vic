#include "WorldFactory.hpp"
#include "World.hpp"
#include <spdlog/spdlog.h>

namespace nlm {

std::unique_ptr<World> WorldFactory::createWorld(const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        spdlog::warn("WorldFactory not initialized, creating basic world");
        return std::make_unique<World>();
    }
    
    // Get configuration from params or use defaults
    std::string type = params.getParam<std::string>("type", "simple");
    std::string simulationType = params.getParam<std::string>("simulationType", "default");
    std::string environmentType = params.getParam<std::string>("environmentType", "default");
    
    // Handle different world creation scenarios based on type
    if (type == "dynamic") {
        return createDynamicWorld();
    } else if (type == "simulation") {
        return createSimulationWorld(simulationType);
    } else if (type == "environment") {
        return createEnvironmentWorld(environmentType);
    } else {
        // Default simple world
        return createSimpleWorld();
    }
}

std::unique_ptr<World> WorldFactory::createSimpleWorld() {
    spdlog::debug("Creating simple world");
    return std::make_unique<World>();
}

std::unique_ptr<World> WorldFactory::createDynamicWorld() {
    spdlog::debug("Creating dynamic world");
    auto world = std::make_unique<World>();
    // In a real implementation, would add dynamic properties to the world
    return world;
}

std::unique_ptr<World> WorldFactory::createSimulationWorld(const std::string& simulationType) {
    spdlog::debug("Creating simulation world with type: {}", simulationType);
    auto world = std::make_unique<World>();
    // In a real implementation, would add simulation properties to the world
    return world;
}

std::unique_ptr<World> WorldFactory::createEnvironmentWorld(const std::string& environmentType) {
    spdlog::debug("Creating environment world with type: {}", environmentType);
    auto world = std::make_unique<World>();
    // In a real implementation, would add environment properties to the world
    return world;
}

std::unique_ptr<World> WorldFactory::createCustomWorld(const std::unordered_map<std::string, std::any>& customParams) {
    spdlog::debug("Creating custom world with {} parameters", customParams.size());
    auto world = std::make_unique<World>();
    // In a real implementation, would add custom properties to the world
    return world;
}

std::unique_ptr<World> WorldFactory::acquireWorldFromPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!worldPool_.empty()) {
        auto world = std::move(worldPool_.back());
        worldPool_.pop_back();
        spdlog::debug("Acquired world from pool");
        return world;
    }
    return nullptr;
}

void WorldFactory::releaseWorldToPool(std::unique_ptr<World> world) {
    if (world) {
        std::lock_guard<std::mutex> lock(mutex_);
        worldPool_.push_back(std::move(world));
        spdlog::debug("Released world to pool");
    }
}

} // namespace nlm
