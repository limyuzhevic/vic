#include "PerformanceValidator.hpp"
#include <iostream>
#include <sstream>

namespace nlm {
namespace validation {

class PerformanceValidator::Impl {
public:
    bool validatePerformanceConsistency_ = true;
    bool validatePerformanceCorrectness_ = true;
    bool validatePerformanceCompleteness_ = true;
    bool validatePerformanceCompatibility_ = true;
    size_t totalValidations = 0;
    double totalValidationTime = 0.0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    mutable std::mutex mutex;
};

PerformanceValidator::PerformanceValidator() : pImpl(std::make_unique<Impl>()) {}
PerformanceValidator::~PerformanceValidator() = default;

PerformanceValidationResult PerformanceValidator::validate(const Brain& brain) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    PerformanceValidationResult result;
    
    // Validate performance consistency
    if (pImpl->validatePerformanceConsistency_) {
        // Check for performance consistency
        result.warnings.push_back("Performance consistency validation placeholder");
    }
    
    // Validate performance correctness
    if (pImpl->validatePerformanceCorrectness_) {
        // Check for performance correctness
        result.warnings.push_back("Performance correctness validation placeholder");
    }
    
    // Validate performance completeness
    if (pImpl->validatePerformanceCompleteness_) {
        // Check for performance completeness
        result.warnings.push_back("Performance completeness validation placeholder");
    }
    
    // Validate performance compatibility
    if (pImpl->validatePerformanceCompatibility_) {
        // Check for performance compatibility
        result.warnings.push_back("Performance compatibility validation placeholder");
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

void PerformanceValidator::lock() const {
    pImpl->mutex.lock();
}

void PerformanceValidator::unlock() const {
    pImpl->mutex.unlock();
}

bool PerformanceValidator::tryLock() const {
    return pImpl->mutex.try_lock();
}

std::string PerformanceValidator::dump() const {
    std::stringstream ss;
    ss << "PerformanceValidator:" << std::endl;
    ss << "  Total validations: " << pImpl->totalValidations << std::endl;
    ss << "  Average validation time: " << (pImpl->totalValidations > 0 ? 
        pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    ss << "  Errors: " << pImpl->totalErrors << std::endl;
    ss << "  Warnings: " << pImpl->totalWarnings << std::endl;
    ss << "  Performance consistency validation: " << (pImpl->validatePerformanceConsistency_ ? "enabled" : "disabled") << std::endl;
    ss << "  Performance correctness validation: " << (pImpl->validatePerformanceCorrectness_ ? "enabled" : "disabled") << std::endl;
    ss << "  Performance completeness validation: " << (pImpl->validatePerformanceCompleteness_ ? "enabled" : "disabled") << std::endl;
    ss << "  Performance compatibility validation: " << (pImpl->validatePerformanceCompatibility_ ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

} // namespace validation
} // namespace nlm
