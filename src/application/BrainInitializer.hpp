#pragma once

#include <memory>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class BrainInitializer {
public:
    BrainInitializer() = default;
    
    std::shared_ptr<Brain> initialize(const std::shared_ptr<Config>& config) {
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain!");
        }
        
        brain->logStatus();
        return brain;
    }
    
    bool initialize(std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config) {
        brain = std::make_shared<Brain>(config);
        return brain->initialize();
    }
    
    static void resetBrain(std::shared_ptr<Brain>& brain) {
        if (brain) {
            brain->reset();
            brain->initialize();
        }
    }
};