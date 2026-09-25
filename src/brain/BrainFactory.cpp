#include "BrainFactory.hpp"
#include "Brain.hpp"
#include <spdlog/spdlog.h>

namespace nlm {

std::unique_ptr<Brain> BrainFactory::createBrain(const ObjectParams& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        spdlog::warn("BrainFactory not initialized, creating basic brain");
        return std::make_unique<Brain>();
    }
    
    // Get configuration from params or use defaults
    std::string type = params.getParam<std::string>("type", "basic");
    std::vector<std::string> regions = params.getParam<std::vector<std::string>>("regions", {});
    std::vector<std::pair<std::string, std::string>> connections = params.getParam<std::vector<std::pair<std::string, std::string>>>("connections", {});
    std::string plasticityType = params.getParam<std::string>("plasticityType", "none");
    std::string adaptationStrategy = params.getParam<std::string>("adaptationStrategy", "none");
    
    // Handle different brain creation scenarios based on type
    if (type == "structured" && !regions.empty()) {
        return createStructuredBrain(regions);
    } else if (type == "connected" && !connections.empty()) {
        return createConnectedBrain(connections);
    } else if (type == "plastic" && !plasticityType.empty()) {
        return createPlasticBrain(plasticityType);
    } else if (type == "adaptive" && !adaptationStrategy.empty()) {
        return createAdaptiveBrain(adaptationStrategy);
    } else {
        // Default basic brain
        return createBasicBrain();
    }
}

std::unique_ptr<Brain> BrainFactory::createBasicBrain() {
    spdlog::debug("Creating basic brain");
    return std::make_unique<Brain>();
}

std::unique_ptr<Brain> BrainFactory::createStructuredBrain(const std::vector<std::string>& regions) {
    spdlog::debug("Creating structured brain with {} regions", regions.size());
    auto brain = std::make_unique<Brain>();
    // In a real implementation, would add regions to the brain
    return brain;
}

std::unique_ptr<Brain> BrainFactory::createConnectedBrain(const std::vector<std::pair<std::string, std::string>>& connections) {
    spdlog::debug("Creating connected brain with {} connections", connections.size());
    auto brain = std::make_unique<Brain>();
    // In a real implementation, would add connections to the brain
    return brain;
}

std::unique_ptr<Brain> BrainFactory::createPlasticBrain(const std::string& plasticityType) {
    spdlog::debug("Creating plastic brain with plasticity type: {}", plasticityType);
    auto brain = std::make_unique<Brain>();
    // In a real implementation, would add plasticity mechanism to the brain
    return brain;
}

std::unique_ptr<Brain> BrainFactory::createAdaptiveBrain(const std::string& adaptationStrategy) {
    spdlog::debug("Creating adaptive brain with adaptation strategy: {}", adaptationStrategy);
    auto brain = std::make_unique<Brain>();
    // In a real implementation, would add adaptation mechanism to the brain
    return brain;
}

std::unique_ptr<Brain> BrainFactory::acquireBrainFromPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!brainPool_.empty()) {
        auto brain = std::move(brainPool_.back());
        brainPool_.pop_back();
        spdlog::debug("Acquired brain from pool");
        return brain;
    }
    return nullptr;
}

void BrainFactory::releaseBrainToPool(std::unique_ptr<Brain> brain) {
    if (brain) {
        std::lock_guard<std::mutex> lock(mutex_);
        brainPool_.push_back(std::move(brain));
        spdlog::debug("Released brain to pool");
    }
}

} // namespace nlm
