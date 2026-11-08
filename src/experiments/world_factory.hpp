#pragma once

#include "../agent/AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace nlm {

/**
 * Factory methods for creating world instances for experiments
 */
namespace WorldFactory {
    /**
     * Create a standard SimpleWorld instance
     */
    inline std::unique_ptr<SimpleWorld> createStandardWorld() {
        auto world = std::make_unique<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->setRandomSeed(42);
        world->reset();
        return world;
    }
    
    /**
     * Create a world with custom configuration
     */
    inline std::unique_ptr<SimpleWorld> createCustomWorld(float width, float height, 
                                                         int visionWidth, int visionHeight,
                                                         uint64_t seed = 42) {
        auto world = std::make_unique<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setRandomSeed(seed);
        world->reset();
        return world;
    }
    
    /**
     * Create a world from configuration
     */
    inline std::unique_ptr<SimpleWorld> createWorldFromConfig(std::shared_ptr<Config> config) {
        auto world = std::make_unique<SimpleWorld>();
        
        // Extract configuration values
        float width = config->getFloat("environment_width", 20.0f);
        float height = config->getFloat("environment_height", 20.0f);
        int visionWidth = config->getInt("vision_width", 16);
        int visionHeight = config->getInt("vision_height", 16);
        
        world->configure(width, height, visionWidth, visionHeight);
        
        uint64_t seed = config->getInt64("random_seed", 42);
        world->setRandomSeed(seed);
        
        world->reset();
        return world;
    }
    
    /**
     * Create a world with initial state
     */
    inline std::unique_ptr<SimpleWorld> createWorldWithState(float width, float height,
                                                           int visionWidth, int visionHeight,
                                                           float initialX, float initialY,
                                                           uint64_t seed = 42) {
        auto world = std::make_unique<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setRandomSeed(seed);
        world->reset();
        
        // Set initial agent position if the world supports it
        // This depends on the SimpleWorld implementation
        
        return world;
    }
    
    /**
     * Create a world for a specific experiment type
     */
    template<typename TWorld>
    inline std::unique_ptr<TWorld> createWorld() {
        return std::make_unique<TWorld>();
    }
    
    /**
     * World configuration helper functions
     */
    namespace WorldConfig {
        /**
         * Get standard world configuration
         */
        inline std::unordered_map<std::string, std::string> getStandardConfig() {
            return {
                {"environment_width", "20"},
                {"environment_height", "20"},
                {"vision_width", "16"},
                {"vision_height", "16"},
                {"random_seed", "42"}
            };
        }
        
        /**
         * Apply world configuration to a SimpleWorld instance
         */
        inline void applyConfig(SimpleWorld& world, const std::unordered_map<std::string, std::string>& config) {
            auto configPtr = std::make_shared<Config>();
            for (const auto& pair : config) {
                configPtr->set(pair.first, pair.second);
            }
            
            float width = configPtr->getFloat("environment_width", 20.0f);
            float height = configPtr->getFloat("environment_height", 20.0f);
            int visionWidth = configPtr->getInt("vision_width", 16);
            int visionHeight = configPtr->getInt("vision_height", 16);
            
            world.configure(width, height, visionWidth, visionHeight);
            
            uint64_t seed = configPtr->getInt64("random_seed", 42);
            world.setRandomSeed(seed);
            
            world.reset();
        }
        
        /**
         * Create world configuration from command line arguments
         */
        inline std::unordered_map<std::string, std::string> configFromArgs(int argc, char* argv[]) {
            std::unordered_map<std::string, std::string> config;
            
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                if (arg == "--world-width" && i + 1 < argc) {
                    config["environment_width"] = argv[++i];
                } else if (arg == "--world-height" && i + 1 < argc) {
                    config["environment_height"] = argv[++i];
                } else if (arg == "--vision-width" && i + 1 < argc) {
                    config["vision_width"] = argv[++i];
                } else if (arg == "--vision-height" && i + 1 < argc) {
                    config["vision_height"] = argv[++i];
                } else if (arg == "--world-seed" && i + 1 < argc) {
                    config["random_seed"] = argv[++i];
                }
            }
            
            return config;
        }
    }
}

} // namespace nlm
