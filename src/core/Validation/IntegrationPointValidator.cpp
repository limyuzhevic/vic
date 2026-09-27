#include "IntegrationPointValidator.hpp"
#include <iostream>
#include <sstream>

namespace nlm {
namespace validation {

class IntegrationPointValidator::Impl {
public:
    bool validateIntegrationPoints_ = true;
    bool validateComponentIntegration_ = true;
    bool validateInterfaceIntegration_ = true;
    bool validateDataFlowIntegration_ = true;
    size_t totalValidations = 0;
    double totalValidationTime = 0.0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    mutable std::mutex mutex;
};

IntegrationPointValidator::IntegrationPointValidator() : pImpl(std::make_unique<Impl>()) {}
IntegrationPointValidator::~IntegrationPointValidator() = default;

IntegrationValidationResult IntegrationPointValidator::validate(const Brain& brain) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    IntegrationValidationResult result;
    
    // Validate integration points
    if (pImpl->validateIntegrationPoints_) {
        // Check if key integration points exist
        if (!brain.getWorkingMemory()) {
            result.errors.push_back("Missing working memory integration");
            result.isValid = false;
        }
        
        if (!brain.getPredictionSystem()) {
            result.errors.push_back("Missing prediction system integration");
            result.isValid = false;
        }
        
        if (!brain.getPlanner()) {
            result.errors.push_back("Missing planner integration");
            result.isValid = false;
        }
        
        if (!brain.getAttention()) {
            result.errors.push_back("Missing attention system integration");
            result.isValid = false;
        }
        
        if (!brain.getConceptFormation()) {
            result.errors.push_back("Missing concept formation integration");
            result.isValid = false;
        }
        
        if (!brain.getDevelopmentSystem()) {
            result.errors.push_back("Missing development system integration");
            result.isValid = false;
        }
        
        if (!brain.getDopamine()) {
            result.errors.push_back("Missing dopamine neuromodulation integration");
            result.isValid = false;
        }
        
        if (!brain.getCuriosity()) {
            result.errors.push_back("Missing curiosity neuromodulation integration");
            result.isValid = false;
        }
        
        if (!brain.getNovelty()) {
            result.errors.push_back("Missing novelty neuromodulation integration");
            result.isValid = false;
        }
        
        if (!brain.getPredictionErrorSignal()) {
            result.errors.push_back("Missing prediction error integration");
            result.isValid = false;
        }
    }
    
    // Validate component integration
    if (pImpl->validateComponentIntegration_) {
        // Check if components are properly integrated
        if (brain.getWorkingMemory() && brain.getPredictionSystem()) {
            result.warnings.push_back("Working memory and prediction system integration placeholder");
        }
        
        if (brain.getPlanner() && brain.getAttention()) {
            result.warnings.push_back("Planner and attention system integration placeholder");
        }
        
        if (brain.getConceptFormation() && brain.getDevelopmentSystem()) {
            result.warnings.push_back("Concept formation and development system integration placeholder");
        }
        
        if (brain.getDopamine() && brain.getCuriosity()) {
            result.warnings.push_back("Dopamine and curiosity neuromodulation integration placeholder");
        }
        
        if (brain.getNovelty() && brain.getPredictionErrorSignal()) {
            result.warnings.push_back("Novelty and prediction error neuromodulation integration placeholder");
        }
    }
    
    // Validate interface integration
    if (pImpl->validateInterfaceIntegration_) {
        // Check interface consistency
        result.warnings.push_back("Interface integration validation placeholder");
    }
    
    // Validate data flow integration
    if (pImpl->validateDataFlowIntegration_) {
        // Check data flow consistency
        result.warnings.push_back("Data flow integration validation placeholder");
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

void IntegrationPointValidator::lock() const {
    pImpl->mutex.lock();
}

void IntegrationPointValidator::unlock() const {
    pImpl->mutex.unlock();
}

bool IntegrationPointValidator::tryLock() const {
    return pImpl->mutex.try_lock();
}

std::string IntegrationPointValidator::dump() const {
    std::stringstream ss;
    ss << "IntegrationPointValidator:" << std::endl;
    ss << "  Total validations: " << pImpl->totalValidations << std::endl;
    ss << "  Average validation time: " << (pImpl->totalValidations > 0 ? 
        pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    ss << "  Errors: " << pImpl->totalErrors << std::endl;
    ss << "  Warnings: " << pImpl->totalWarnings << std::endl;
    ss << "  Integration points validation: " << (pImpl->validateIntegrationPoints_ ? "enabled" : "disabled") << std::endl;
    ss << "  Component integration validation: " << (pImpl->validateComponentIntegration_ ? "enabled" : "disabled") << std::endl;
    ss << "  Interface integration validation: " << (pImpl->validateInterfaceIntegration_ ? "enabled" : "disabled") << std::endl;
    ss << "  Data flow integration validation: " << (pImpl->validateDataFlowIntegration_ ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

} // namespace validation
} // namespace nlm
