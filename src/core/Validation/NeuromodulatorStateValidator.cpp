#include "NeuromodulatorStateValidator.hpp"
#include <iostream>
#include <sstream>

namespace nlm {
namespace validation {

class NeuromodulatorStateValidator::Impl {
public:
    bool validateNeuromodulatorLevel_ = true;
    bool validateNeuromodulatorEffect_ = true;
    bool validateNeuromodulatorInteraction_ = true;
    bool validateNeuromodulatorFeedback_ = true;
    size_t totalValidations = 0;
    double totalValidationTime = 0.0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    mutable std::mutex mutex;
};

NeuromodulatorStateValidator::NeuromodulatorStateValidator() : pImpl(std::make_unique<Impl>()) {}
NeuromodulatorStateValidator::~NeuromodulatorStateValidator() = default;

NeuromodulatorSystemValidationResult NeuromodulatorStateValidator::validate(const Brain& brain) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NeuromodulatorSystemValidationResult result;
    
    // Validate neuromodulator levels
    if (pImpl->validateNeuromodulatorLevel_) {
        if (brain.getDopamine()) {
            auto dopamineLevel = brain.getDopamine()->getLevel();
            if (dopamineLevel < -1.0f || dopamineLevel > 1.0f) {
                result.errors.push_back("Dopamine level out of expected range");
                result.isValid = false;
            }
        }
        
        if (brain.getCuriosity()) {
            // Validate curiosity level
        }
        
        if (brain.getPredictionErrorSignal()) {
            // Validate prediction error level
        }
        
        if (brain.getNovelty()) {
            // Validate novelty level
        }
    }
    
    // Validate neuromodulator effects
    if (pImpl->validateNeuromodulatorEffect_) {
        if (brain.getDopamine()) {
            auto dopamineLevel = brain.getDopamine()->getLevel();
            // Check for reasonable effects based on dopamine level
            if (std::abs(dopamineLevel) > 0.5f) {
                result.warnings.push_back("High dopamine level may indicate unusual brain state");
            }
        }
        
        // Validate other neuromodulators
        if (brain.getCuriosity()) {
            result.warnings.push_back("Curiosity neuromodulator level validation placeholder");
        }
        
        if (brain.getPredictionErrorSignal()) {
            result.warnings.push_back("Prediction error neuromodulator level validation placeholder");
        }
        
        if (brain.getNovelty()) {
            result.warnings.push_back("Novelty neuromodulator level validation placeholder");
        }
    }
    
    // Validate neuromodulator interactions
    if (pImpl->validateNeuromodulatorInteraction_) {
        // Check for potential interactions
        if (brain.getDopamine() && brain.getCuriosity()) {
            result.warnings.push_back("Dopamine-curiosity interaction validation placeholder");
        }
        
        // Validate cross-system interactions
        if (brain.getDopamine() && brain.getPredictionErrorSignal()) {
            result.warnings.push_back("Dopamine-prediction error interaction validation placeholder");
        }
    }
    
    // Validate neuromodulator feedback
    if (pImpl->validateNeuromodulatorFeedback_) {
        // Check for feedback loops
        if (brain.getDopamine()) {
            auto dopamineLevel = brain.getDopamine()->getLevel();
            // Validate feedback based on dopamine level
            result.warnings.push_back("Neuromodulator feedback validation placeholder");
        }
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

void NeuromodulatorStateValidator::lock() const {
    pImpl->mutex.lock();
}

void NeuromodulatorStateValidator::unlock() const {
    pImpl->mutex.unlock();
}

bool NeuromodulatorStateValidator::tryLock() const {
    return pImpl->mutex.try_lock();
}

std::string NeuromodulatorStateValidator::dump() const {
    std::stringstream ss;
    ss << "NeuromodulatorStateValidator:" << std::endl;
    ss << "  Total validations: " << pImpl->totalValidations << std::endl;
    ss << "  Average validation time: " << (pImpl->totalValidations > 0 ? 
        pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    ss << "  Errors: " << pImpl->totalErrors << std::endl;
    ss << "  Warnings: " << pImpl->totalWarnings << std::endl;
    ss << "  Neuromodulator level validation: " << (pImpl->validateNeuromodulatorLevel_ ? "enabled" : "disabled") << std::endl;
    ss << "  Neuromodulator effect validation: " << (pImpl->validateNeuromodulatorEffect_ ? "enabled" : "disabled") << std::endl;
    ss << "  Neuromodulator interaction validation: " << (pImpl->validateNeuromodulatorInteraction_ ? "enabled" : "disabled") << std::endl;
    ss << "  Neuromodulator feedback validation: " << (pImpl->validateNeuromodulatorFeedback_ ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

} // namespace validation
} // namespace nlm
