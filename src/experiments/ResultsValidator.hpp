#pragma once

#include "../core/Logger/Logger.hpp"
#include "ExperimentConstants.hpp"
#include <vector>
#include <string>
#include <cmath>

namespace nlm {

/**
 * Results validation and analysis
 * Validates and analyzes experiment results
 */
class ResultsValidator {
public:
    ResultsValidator() = default;
    ~ResultsValidator() = default;
    
    /**
     * Validate experiment results
     */
    bool validateResults(const Phase6IntegrationResult& result);
    
    /**
     * Validate experiment results against thresholds
     */
    bool validateAgainstThresholds(const Phase6IntegrationResult& result) const;
    
    /**
     * Validate result consistency
     */
    bool validateResultConsistency(const Phase6IntegrationResult& result) const;
    
    /**
     * Calculate result quality score
     */
    float calculateQualityScore(const Phase6IntegrationResult& result) const;
    
    /**
     * Get validation error messages
     */
    const std::vector<std::string>& getValidationErrors() const { return validationErrors_; }
    
    /**
     * Check if results are considered valid
     */
    bool areResultsValid() const { return isValid_; }
    
    /**
     * Get summary of validation issues
     */
    std::string getValidationSummary() const;
    
private:
    std::vector<std::string> validationErrors_;
    bool isValid_ = false;
};

} // namespace nlm
