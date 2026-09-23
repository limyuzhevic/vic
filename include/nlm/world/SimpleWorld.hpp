#ifndef NLM_WORLD_SIMPLEWORLD_HPP
#define NLM_WORLD_SIMPLEWORLD_HPP

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

struct WorldState {
    size_t width;
    size_t height;
    std::vector<std::vector<float>> terrain;
    std::vector<std::vector<bool>> obstacles;
    std::vector<std::pair<size_t, size_t>> foodSources;
    std::vector<std::pair<size_t, size_t>> predators;
    
    WorldState(size_t w = 10, size_t h = 10);
};

class SimpleWorld {
public:
    SimpleWorld(std::shared_ptr<Config> config);
    ~SimpleWorld();
    
    // Non-copyable
    SimpleWorld(const SimpleWorld&) = delete;
    SimpleWorld& operator=(const SimpleWorld&) = delete;
    
    // Movable
    SimpleWorld(SimpleWorld&& other) noexcept;
    SimpleWorld& operator=(SimpleWorld&& other) noexcept;
    
    // World properties
    size_t getWidth() const { return state.width; }
    size_t getHeight() const { return state.height; }
    
    // World operations
    void update(double dt);
    void reset();
    
    // State access
    const WorldState& getState() const { return state; }
    WorldState& getState() { return state; }
    
    // Environmental sensing
    std::vector<float> getSensoryInput(const std::pair<size_t, size_t>& position) const;
    
    // Action effects
    void applyAction(const std::pair<size_t, size_t>& agentPosition, 
                     const Action& action);
    
    // Position validation
    bool isValidPosition(const std::pair<size_t, size_t>& position) const;
    std::pair<size_t, size_t> getRandomEmptyPosition() const;
    
    // Statistics
    size_t getFoodCount() const { return state.foodSources.size(); }
    size_t getPredatorCount() const { return state.predators.size(); }
    size_t getObstacleCount() const;
    
    // Configuration
    void configure(const std::string& terrainType, const std::string& difficulty);
    
    // Logging
    void logStatus() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    WorldState state;
    double currentTime;
    double timeStep;
};

} // namespace nlm

#endif // NLM_WORLD_SIMPLEWORLD_HPP
