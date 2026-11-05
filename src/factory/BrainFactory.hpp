#pragma once

#include "brain/Brain.hpp"
#include "performance/OptimizedBrain.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <stdexcept>

namespace nlm {

/**
 * BrainFactory - Factory for creating different types of brain implementations
 * 
 * This factory allows users to select different brain implementations based on
 * their performance and feature requirements. It supports:
 * 
 * - Basic Brain (default): Simple, reliable implementation with all core features
 * - OptimizedBrain: High-performance implementation with event-driven processing,
 *   SIMD vectorization, multithreading, and sparse connectivity
 * 
 * The factory uses configuration to determine which brain type to create.
 */
class BrainFactory {
public:
    /**
     * Create a brain based on configuration
     * 
     * @param config Configuration for the brain
     * @return Shared pointer to the created brain
     * 
     * This factory creates different types of brains based on the configuration:
     * - If "brain_type" is "basic" or not specified: creates a regular Brain
     * - If "brain_type" is "optimized": creates an OptimizedBrain
     * - If "brain_type" is unknown: defaults to regular Brain with warning
     */
    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Configuration cannot be null");
        }
        
        // Determine brain type from configuration
        std::string brainType = "basic"; // Default
        if (auto brainTypeOpt = config->get<std::string>("brain_type")) {
            brainType = *brainTypeOpt;
        }
        
        // Create brain based on type
        if (brainType == "optimized") {
            return createOptimizedBrain(config);
        } else if (brainType == "basic") {
            return createBasicBrain(config);
        } else {
            // Log warning for unknown brain type
            // For now, default to basic brain
            return createBasicBrain(config);
        }
    }
    
    /**
     * Create an optimized brain with performance optimizations
     * 
     * This creates a brain with all available optimizations enabled:
     * - Event-driven processing
     * - SIMD vectorization
     * - Multithreading
     * - Sparse connectivity
     * 
     * @param config Configuration for the optimized brain
     * @return Shared pointer to the optimized brain
     */
    static std::shared_ptr<OptimizedBrain> createOptimizedBrain(std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Configuration cannot be null");
        }
        
        auto optimizedBrain = std::make_shared<OptimizedBrain>(config);
        
        // Configure optimizations based on configuration
        configureOptimizations(*optimizedBrain, *config);
        
        return optimizedBrain;
    }
    
    /**
     * Create a basic brain without performance optimizations
     * 
     * This creates the original Brain implementation with all core features
     * but without the performance optimizations (event-driven, SIMD, multithreading,
     * sparse connectivity).
     * 
     * @param config Configuration for the basic brain
     * @return Shared pointer to the basic brain
     */
    static std::shared_ptr<Brain> createBasicBrain(std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Configuration cannot be null");
        }
        
        return std::make_shared<Brain>(config);
    }
    
    /**
     * Configure optimizations for an optimized brain
     * 
     * This sets up all available performance optimizations based on the
     * configuration settings.
     * 
     * @param brain Optimized brain to configure
     * @param config Configuration
     */
    static void configureOptimizations(OptimizedBrain& brain, const Config& config) {
        // Configure event-driven processing
        if (auto eventDrivenOpt = config.get<bool>("enable_event_driven")) {
            brain.enableEventDriven(*eventDrivenOpt);
        }
        
        // Configure SIMD vectorization
        if (auto simdOpt = config.get<bool>("enable_simd")) {
            brain.enableSIMD(*simdOpt);
        }
        
        // Configure multithreading
        if (auto multithreadingOpt = config.get<bool>("enable_multithreading")) {
            brain.enableMultithreading(*multithreadingOpt);
        }
        
        // Configure sparse connectivity
        if (auto sparseOpt = config.get<bool>("enable_sparse_connectivity")) {
            brain.enableSparseConnectivity(*sparseOpt);
        }
        
        // Set thread count if multithreading is enabled
        if (auto threadCountOpt = config.get<size_t>("thread_count")) {
            brain.setThreadCount(*threadCountOpt);
        }
    }
    
    /**
     * Get available brain types
     * 
     * @return Vector of available brain type names
     */
    static std::vector<std::string> getAvailableBrainTypes() {
        return {"basic", "optimized"};
    }
    
    /**
     * Check if a brain type is available
     * 
     * @param brainType Name of brain type to check
     * @return true if brain type is available, false otherwise
     */
    static bool isBrainTypeAvailable(const std::string& brainType) {
        auto availableTypes = getAvailableBrainTypes();
        return std::find(availableTypes.begin(), availableTypes.end(), brainType) != availableTypes.end();
    }
    
    /**
     * Get default brain type
     * 
     * @return Name of default brain type
     */
    static std::string getDefaultBrainType() {
        return "basic";
    }
    
    /**
     * Create brain with automatic optimization selection
     * 
     * This creates a brain with optimizations automatically selected based on
     * the configuration and system capabilities.
     * 
     * @param config Configuration for the brain
     * @return Shared pointer to the created brain
     */
    static std::shared_ptr<Brain> createBrainWithAutoOptimizations(std::shared_ptr<Config> config) {
        if (!config) {
            throw std::invalid_argument("Configuration cannot be null");
        }
        
        // Check if optimizations are explicitly enabled
        bool optimizationsEnabled = false;
        
        if (auto eventDrivenOpt = config->get<bool>("enable_event_driven")) {
            optimizationsEnabled = optimizationsEnabled || *eventDrivenOpt;
        }
        
        if (auto simdOpt = config->get<bool>("enable_simd")) {
            optimizationsEnabled = optimizationsEnabled || *simdOpt;
        }
        
        if (auto multithreadingOpt = config->get<bool>("enable_multithreading")) {
            optimizationsEnabled = optimizationsEnabled || *multithreadingOpt;
        }
        
        if (auto sparseOpt = config->get<bool>("enable_sparse_connectivity")) {
            optimizationsEnabled = optimizationsEnabled || *sparseOpt;
        }
        
        // Create brain based on whether optimizations are enabled
        if (optimizationsEnabled) {
            return createOptimizedBrain(config);
        } else {
            return createBasicBrain(config);
        }
    }
};

} // namespace nlm
