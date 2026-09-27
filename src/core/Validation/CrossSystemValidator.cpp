#include "CrossSystemValidator.hpp"
#include <iostream>
#include <sstream>

namespace nlm {
namespace validation {

class CrossSystemValidator::Impl {
public:
    bool validateCrossSystemCommunication_ = true;
    bool validateCrossSystemInteraction_ = true;
    bool validateCrossSystemDependency_ = true;
    bool validateCrossSystemSynchronization_ = true;
    size_t totalValidations = 0;
    double totalValidationTime = 0.0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    mutable std::mutex mutex;
};

CrossSystemValidator::CrossSystemValidator() : pImpl(std::make_unique<Impl>()) {}
CrossSystemValidator::~CrossSystemValidator() = default;

CrossSystemValidationResult CrossSystemValidator::validate(const Brain& brain) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CrossSystemValidationResult result;
    
    // Validate cross-system communication
    if (pImpl->validateCrossSystemCommunication_) {
        // Check if cross-system communication channels are established
        if (brain.getWorkingMemory() && brain.getPredictionSystem()) {
            result.warnings.push_back("Working memory-prediction system cross-system communication validation placeholder");
        }
        
        if (brain.getPlanner() && brain.getConceptFormation()) {
            result.warnings.push_back("Planner-concept formation cross-system communication validation placeholder");
        }
        
        if (brain.getAttention() && brain.getDevelopmentSystem()) {
            result.warnings.push_back("Attention-development system cross-system communication validation placeholder");
        }
        
        if (brain.getDopamine() && brain.getCuriosity()) {
            result.warnings.push_back("Dopamine-curiosity cross-system communication validation placeholder");
        }
        
        if (brain.getNovelty() && brain.getPredictionErrorSignal()) {
            result.warnings.push_back("Novelty-prediction error cross-system communication validation placeholder");
        }
    }
    
    // Validate cross-system interaction
    if (pImpl->validateCrossSystemInteraction_) {
        // Check if cross-system interactions are functioning
        result.warnings.push_back("Cross-system interaction validation placeholder");
    }
    
    // Validate cross-system dependency
    if (pImpl->validateCrossSystemDependency_) {
        // Check if cross-system dependencies are consistent
        result.warnings.push_back("Cross-system dependency validation placeholder");
    }
    
    // Validate cross-system synchronization
    if (pImpl->validateCrossSystemSynchronization_) {
        // Check if cross-system synchronization is working
        result.warnings.push_back("Cross-system synchronization validation placeholder");
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

void CrossSystemValidator::lock() const {
    pImpl->mutex.lock();
}

void CrossSystemValidator::unlock() const {
    pImpl->mutex.unlock();
}

bool CrossSystemValidator::tryLock() const {
    return pImpl->mutex.try_lock();
}

std::string CrossSystemValidator::dump() const {
    std::stringstream ss;
    ss << "CrossSystemValidator:" << std::endl;
    ss << "  Total validations: " << pImpl->totalValidations << std::endl;
    ss << "  Average validation time: " << (pImpl->totalValidations > 0 ? 
        pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    ss << "  Errors: " << pImpl->totalErrors << std::endl;
    ss << "  Warnings: " << pImpl->totalWarnings << std::endl;
    ss << "  Cross-system communication validation: " << (pImpl->validateCrossSystemCommunication_ ? "enabled" : "disabled") << std::endl;
    ss << "  Cross-system interaction validation: " << (pImpl->validateCrossSystemInteraction_ ? "enabled" : "disabled") << std::endl;
    ss << "  Cross-system dependency validation: " << (pImpl->validateCrossSystemDependency_ ? "enabled" : "disabled") << std::endl;
    ss << "  Cross-system synchronization validation: " << (pImpl->validateCrossSystemSynchronization_ ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

} // namespace validation
} // namespace nlm
