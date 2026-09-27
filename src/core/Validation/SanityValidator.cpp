#include "SanityValidator.hpp"
#include <iostream>
#include <sstream>

namespace nlm {
namespace validation {

class SanityValidator::Impl {
public:
    bool validateBasicSanityChecks_ = true;
    bool validateSystemState_ = true;
    bool validateParameters_ = true;
    bool validateBoundaries_ = true;
    bool validateInitialState_ = true;
    size_t totalValidations = 0;
    double totalValidationTime = 0.0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    mutable std::mutex mutex;
};

SanityValidator::SanityValidator() : pImpl(std::make_unique<Impl>()) {}
SanityValidator::~SanityValidator() = default;

SanityValidationResult SanityValidator::validate(const Brain& brain) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    SanityValidationResult result;
    
    // Validate basic sanity checks
    if (pImpl->validateBasicSanityChecks_) {
        // Check basic sanity
        if (brain.getTotalNeuronCount() == 0) {
            result.errors.push_back("Brain has no neurons");
            result.isValid = false;
        }
        
        if (brain.getRegionCount() == 0) {
            result.errors.push_back("Brain has no regions");
            result.isValid = false;
        }
        
        // Check for reasonable bounds
        auto neuronCount = brain.getTotalNeuronCount();
        if (neuronCount > 100000) {
            result.warnings.push_back("Very high neuron count may indicate configuration issues");
        }
        
        auto spikeCount = brain.getTotalSpikeCount();
        if (spikeCount > 10000000) {
            result.warnings.push_back("Very high spike count may indicate abnormal activity");
        }
    }
    
    // Validate system state
    if (pImpl->validateSystemState_) {
        // Check system state
        result.warnings.push_back("System state validation placeholder");
    }
    
    // Validate parameters
    if (pImpl->validateParameters_) {
        // Check parameters
        result.warnings.push_back("Parameter validation placeholder");
    }
    
    // Validate boundaries
    if (pImpl->validateBoundaries_) {
        // Check boundaries
        result.warnings.push_back("Boundary validation placeholder");
    }
    
    // Validate initial state
    if (pImpl->validateInitialState_) {
        // Check initial state
        result.warnings.push_back("Initial state validation placeholder");
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    result.validationTime = duration.count() / 1000.0;
    
    pImpl->totalValidations++;
    pImpl->totalValidationTime += result.validationTime;
    pImpl->totalErrors += result.errors.size();
    pImpl->totalWarnings += result.warnings.size();
    
    return result;
}

void SanityValidator::lock() const {
    pImpl->mutex.lock();
}

void SanityValidator::unlock() const {
    pImpl->mutex.unlock();
}

bool SanityValidator::tryLock() const {
    return pImpl->mutex.try_lock();
}

std::string SanityValidator::dump() const {
    std::stringstream ss;
    ss << "SanityValidator:" << std::endl;
    ss << "  Total validations: " << pImpl->totalValidations << std::endl;
    ss << "  Average validation time: " << (pImpl->totalValidations > 0 ? 
        pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    ss << "  Errors: " << pImpl->totalErrors << std::endl;
    ss << "  Warnings: " << pImpl->totalWarnings << std::endl;
    ss << "  Basic sanity checks validation: " << (pImpl->validateBasicSanityChecks_ ? "enabled" : "disabled") << std::endl;
    ss << "  System state validation: " << (pImpl->validateSystemState_ ? "enabled" : "disabled") << std::endl;
    ss << "  Parameter validation: " << (pImpl->validateParameters_ ? "enabled" : "disabled") << std::endl;
    ss << "  Boundary validation: " << (pImpl->validateBoundaries_ ? "enabled" : "disabled") << std::endl;
    ss << "  Initial state validation: " << (pImpl->validateInitialState_ ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

} // namespace validation
} // namespace nlm
