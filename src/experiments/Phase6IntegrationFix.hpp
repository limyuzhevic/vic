// Phase 6 Integration Fix Header
// This file declares the Phase 6 integration fix classes and utilities
// It provides backward compatibility and fixes for Phase 6 integration issues

#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include <string>
#include <memory>

namespace nlm {

/**
 * Phase 6 Integration Fix
 * 
 * This class provides utilities to fix common Phase 6 integration issues
 * and ensure backward compatibility with older API versions.
 */
class Phase6IntegrationFix {
public:
    Phase6IntegrationFix();
    ~Phase6IntegrationFix();
    
    /**
     * Check if all Phase 6 systems are properly integrated
     * 
     * @return true if all systems are integrated, false otherwise
     */
    bool checkSystemIntegration();
    
    /**
     * Fix Phase 6Demo.cpp API issues
     * 
     * This method applies fixes for common API mismatches in Phase 6Demo.cpp
     * to ensure backward compatibility with old API calls.
     */
    void fixPhase6DemoAPI();
    
    /**
     * Fix Phase6IntegratedExperiment API issues
     * 
     * This method applies fixes for API mismatches in Phase6IntegratedExperiment
     * to ensure compatibility with newer NLM versions.
     */
    void fixPhase6IntegratedExperimentAPI();
    
    /**
     * Verify backward compatibility
     * 
     * @return true if backward compatibility is verified, false otherwise
     */
    bool verifyBackwardCompatibility();
    
    /**
     * Generate migration guide for Phase 6 users
     * 
     * This method generates a comprehensive migration guide that helps users
     * transition from the old Phase 6 API to the new Phase 6 API.
     */
    void generateMigrationGuide();
private:
    bool initialized_;
};

/**
 * Phase 6 Integration Fix Utilities
 * 
 * This namespace contains utility functions for Phase 6 integration fixes.
 */
namespace Phase6IntegrationFixUtils {
    
    /**
     * Create a comprehensive test for Phase 6 integration
     * 
     * @return true if the comprehensive test passes, false otherwise
     */
    bool createPhase6IntegrationTest();
    
    /**
     * Generate Phase 6 configuration report
     * 
     * @param config The Phase 6 configuration to report on
     */
    void generatePhase6ConfigReport(const Phase6Config& config);
    
    /**
     * Check for API version compatibility
     * 
     * @return true if API versions are compatible, false otherwise
     */
    bool checkAPIVersionCompatibility();
    
} // namespace Phase6IntegrationFixUtils

// Global Phase 6 integration fix instance
extern Phase6IntegrationFix phase6Fix;

} // namespace nlm