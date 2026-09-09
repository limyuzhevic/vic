#pragma once

#include "../development/DevelopmentSystem.hpp"

namespace nlm {

class Brain;

class DevelopmentManager {
public:
    DevelopmentManager();
    ~DevelopmentManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update development system
    void update();
    
    // Get development system
    DevelopmentSystem* get() { return developmentSystem_.get(); }
    const DevelopmentSystem* get() const { return developmentSystem_.get(); }
    
    // Configure from config
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
};

} // namespace nlm