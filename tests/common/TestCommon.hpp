#pragma once

#include <memory>
#include <string>

namespace nlm {
    class Config;
    class Brain;
}

namespace TestCommon {
    // Create a basic test configuration
    inline std::shared_ptr<nlm::Config> createTestConfig() {
        auto config = std::make_shared<nlm::Config>();
        
        // Set common test parameters
        config->set("random_seed", uint64_t(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", size_t(100));
        config->set("region_count", size_t(1));
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        
        return config;
    }
    
    // Create a test brain
    inline std::shared_ptr<nlm::Brain> createTestBrain(std::shared_ptr<nlm::Config> config) {
        return std::make_shared<nlm::Brain>(config);
    }
}