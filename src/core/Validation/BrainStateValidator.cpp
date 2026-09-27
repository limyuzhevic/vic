#include "BrainStateValidator.hpp"
#include <iostream>
#include <sstream>

namespace nlm {
namespace validation {

class BrainStateValidator::Impl {
public:
    bool validateBrainInitialization_ = true;
    bool validateBrainOperation_ = true;
    bool validateBrainShutdown_ = true;
    bool validateBrainErrors_ = true;
    size_t totalValidations = 0;
    double totalValidationTime = 0.0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    mutable std::mutex mutex;
};

BrainStateValidator::BrainStateValidator() : pImpl(std::make_unique<Impl>()) {}
BrainStateValidator::~BrainStateValidator() = default;

BrainSystemValidationResult BrainStateValidator::validate(const Brain& brain) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    BrainSystemValidationResult result;
    
    // Validate brain initialization
    if (pImpl->validateBrainInitialization_) {
        // Check if brain has proper configuration
        if (!brain.getConfig()) {
            result.errors.push_back("Brain missing configuration");
            result.isValid = false;
        }
        
        // Check if brain has regions
        if (brain.getRegionCount() == 0) {
            result.warnings.push_back("Brain has no regions");
        }
        
        // Check if brain has neurons
        if (brain.getTotalNeuronCount() == 0) {
            result.warnings.push_back("Brain has no neurons");
        }
    }
    
    // Validate brain operation
    if (pImpl->validateBrainOperation_) {
        // Check if brain has active systems
        if (!brain.getWorkingMemory()) {
            result.warnings.push_back("Brain missing working memory");
        }
        
        if (!brain.getPredictionSystem()) {
            result.warnings.push_back("Brain missing prediction system");
        }
        
        // Check for potential inconsistencies
        auto neuronCount = brain.getTotalNeuronCount();
        auto spikeCount = brain.getTotalSpikeCount();
        if (neuronCount > 0 && spikeCount > neuronCount * 100) {
            result.warnings.push_back("Unusually high spike count relative to neuron count");
        }
    }
    
    // Validate brain shutdown
    if (pImpl->validateBrainShutdown_) {
        // This would check if brain is properly shut down
        result.warnings.push_back("Brain shutdown validation placeholder");
    }
    
    // Validate brain errors
    if (pImpl->validateBrainErrors_) {
        // Check for error states
        auto pendingSpikes = brain.getPendingSpikeEventCount();
        if (pendingSpikes > 1000) {
            result.errors.push_back("Too many pending spike events");
            result.isValid = false;
        }
        
        // Check E/I ratio
        auto eiRatio = brain.getExcitationInhibitionRatio();
        if (eiRatio < 0.1 || eiRatio > 10.0) {
            result.warnings.push_back("Unusual excitation-inhibition ratio");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    result.validationTime = duration.count() / 1000.0;
    result.regionsValidated = brain.getRegionCount();
    
    pImpl->totalValidations++;
    pImpl->totalValidationTime += result.validationTime;
    pImpl->totalErrors += result.errors.size();
    pImpl->totalWarnings += result.warnings.size();
    
    return result;
}

void BrainStateValidator::lock() const {
    pImpl->mutex.lock();
}

void BrainStateValidator::unlock() const {
    pImpl->mutex.unlock();
}

bool BrainStateValidator::tryLock() const {
    return pImpl->mutex.try_lock();
}

std::string BrainStateValidator::dump() const {
    std::stringstream ss;
    ss << "BrainStateValidator:" << std::endl;
    ss << "  Total validations: " << pImpl->totalValidations << std::endl;
    ss << "  Average validation time: " << (pImpl->totalValidations > 0 ? 
        pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    ss << "  Errors: " << pImpl->totalErrors << std::endl;
    ss << "  Warnings: " << pImpl->totalWarnings << std::endl;
    ss << "  Brain initialization validation: " << (pImpl->validateBrainInitialization_ ? "enabled" : "disabled") << std::endl;
    ss << "  Brain operation validation: " << (pImpl->validateBrainOperation_ ? "enabled" : "disabled") << std::endl;
    ss << "  Brain shutdown validation: " << (pImpl->validateBrainShutdown_ ? "enabled" : "disabled") << std::endl;
    ss << "  Brain error validation: " << (pImpl->validateBrainErrors_ ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

} // namespace validation
} // namespace nlm
