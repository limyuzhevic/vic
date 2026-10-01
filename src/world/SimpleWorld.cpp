// SimpleWorld.cpp - Implementation of SimpleWorld using new architecture
#include "SimpleWorld.hpp"
#include <iostream>

namespace nlm {

SimpleWorld::SimpleWorld() : SimpleWorld(WorldConfig()) {}

SimpleWorld::SimpleWorld(const WorldConfig& config) : world_(std::make_unique<SimpleWorldCore>(config)) {
    // Configure world with default settings
    world_->configure(config);
    
    // Enable advanced features based on configuration
    enableMultithreading(true);
    enableAdaptiveSpatialIndex(true);
    enableMemoryPool(true);
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::configure(const WorldConfig& config) {
    world_->configure(config);
    
    // Propagate advanced feature settings
    enableMultithreading(multithreaded_);
    enableAdaptiveSpatialIndex(useAdaptiveIndex_);
    enableMemoryPool(useMemoryPool_);
}

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    WorldConfig config;
    config.width = width;
    config.height = height;
    config.visionWidth = visionWidth;
    config.visionHeight = visionHeight;
    configure(config);
}

void SimpleWorld::reset() {
    world_->reset();
}

void SimpleWorld::setAgentStart(float x, float y) {
    world_->setAgentStart(x, y);
}

void SimpleWorld::update(double timestep) {
    world_->update(timestep);
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    return world_->applyMotorCommand(cmd, currentTime);
}

void SimpleWorld::addObject(const WorldObject& obj) {
    world_->addObject(obj);
}

void SimpleWorld::removeObject(float x, float y) {
    world_->removeObject(x, y);
}

void SimpleWorld::setRandomSeed(uint64_t seed) {
    world_->setRandomSeed(seed);
}

void SimpleWorld::enableMultithreading(bool enabled) {
    multithreaded_ = enabled;
    // Note: In a real implementation, we'd propagate this to vision generator
}

void SimpleWorld::enableAdaptiveSpatialIndex(bool enabled) {
    useAdaptiveIndex_ = enabled;
    // Note: In a real implementation, we'd propagate this to collision detector
}

void SimpleWorld::enableMemoryPool(bool enabled) {
    useMemoryPool_ = enabled;
    // Note: In a real implementation, we'd propagate this to object pool
}

// Factory function for creating world with advanced features
std::unique_ptr<SimpleWorld> createAdvancedWorld(const WorldConfig& config) {
    return std::make_unique<SimpleWorld>(config);
}

} // namespace nlm