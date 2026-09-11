// Optimized brain implementation for NLM
// Provides performance-optimized neural brain implementation

#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

/**
 * Performance-optimized brain implementation
 * Provides enhanced performance for neural simulation
 */
class OptimizedBrain : public Brain {
public:
    OptimizedBrain(std::shared_ptr<Config> config);
    ~OptimizedBrain();
    
    // Override optimization methods
    void enableSIMD(bool enable) override;
    void enableMultithreading(bool enable) override;
    void enableSparseConnectivity(bool enable) override;
    
    // Performance optimization
    void applySIMDOptimizations();
    void applyMemoryOptimizations();
    void applyCacheOptimizations();
    
    // Specialized operations
    void optimizedStep(SimulationStep currentStep, Timestamp currentTime);
    void optimizedInitialize();
    
    // Performance statistics
    float getPerformanceScore() const;
    size_t getSIMDEnabledOperations() const;
    uint64_t getOptimizedStepCount() const;
    
protected:
    // Use Brain's protected constructor
    friend class Brain;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
