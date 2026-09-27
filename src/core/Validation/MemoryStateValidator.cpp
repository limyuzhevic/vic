#include "MemoryStateValidator.hpp"
#include <iostream>
#include <sstream>

namespace nlm {
namespace validation {

class MemoryStateValidator::Impl {
public:
    bool validateMemoryEncoding_ = true;
    bool validateMemoryStorage_ = true;
    bool validateMemoryRetrieval_ = true;
    bool validateMemoryConsolidation_ = true;
    bool validateMemoryReplay_ = true;
    size_t totalValidations = 0;
    double totalValidationTime = 0.0;
    size_t totalErrors = 0;
    size_t totalWarnings = 0;
    mutable std::mutex mutex;
};

MemoryStateValidator::MemoryStateValidator() : pImpl(std::make_unique<Impl>()) {}
MemoryStateValidator::~MemoryStateValidator() = default;

MemorySystemValidationResult MemoryStateValidator::validate(const Brain& brain) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    MemorySystemValidationResult result;
    
    // Validate memory encoding
    if (pImpl->validateMemoryEncoding_) {
        if (brain.getWorkingMemory()) {
            auto activeTraces = brain.getWorkingMemory()->getActiveTraces();
            if (activeTraces > 1000) {
                result.warnings.push_back("Working memory has high number of active traces");
            }
        }
        
        if (brain.getEpisodicMemory()) {
            auto episodeCount = brain.getEpisodicMemory()->getEpisodeCount();
            if (episodeCount > 10000) {
                result.warnings.push_back("Episodic memory has large number of episodes");
            }
        }
    }
    
    // Validate memory storage
    if (pImpl->validateMemoryStorage_) {
        if (brain.getWorkingMemory() && brain.getWorkingMemory()->getCapacity() == 0) {
            result.errors.push_back("Working memory capacity is zero");
            result.isValid = false;
        }
        
        // Check memory consistency
        if (brain.getEpisodicMemory() && brain.getEpisodicMemory()->getMaxEpisodes() < 100) {
            result.warnings.push_back("Episodic memory has low maximum episodes");
        }
    }
    
    // Validate memory retrieval
    if (pImpl->validateMemoryRetrieval_) {
        // Check if memory systems are responding
        if (brain.getWorkingMemory()) {
            auto activeTraces = brain.getWorkingMemory()->getActiveTraces();
            if (activeTraces == 0) {
                result.warnings.push_back("Working memory has no active traces");
            }
        }
    }
    
    // Validate memory consolidation
    if (pImpl->validateMemoryConsolidation_) {
        // This would check consolidation processes
        result.warnings.push_back("Memory consolidation validation placeholder");
    }
    
    // Validate memory replay
    if (pImpl->validateMemoryReplay_) {
        // This would check replay processes
        result.warnings.push_back("Memory replay validation placeholder");
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

void MemoryStateValidator::lock() const {
    pImpl->mutex.lock();
}

void MemoryStateValidator::unlock() const {
    pImpl->mutex.unlock();
}

bool MemoryStateValidator::tryLock() const {
    return pImpl->mutex.try_lock();
}

std::string MemoryStateValidator::dump() const {
    std::stringstream ss;
    ss << "MemoryStateValidator:" << std::endl;
    ss << "  Total validations: " << pImpl->totalValidations << std::endl;
    ss << "  Average validation time: " << (pImpl->totalValidations > 0 ? 
        pImpl->totalValidationTime / pImpl->totalValidations : 0.0) << "s" << std::endl;
    ss << "  Errors: " << pImpl->totalErrors << std::endl;
    ss << "  Warnings: " << pImpl->totalWarnings << std::endl;
    ss << "  Memory encoding validation: " << (pImpl->validateMemoryEncoding_ ? "enabled" : "disabled") << std::endl;
    ss << "  Memory storage validation: " << (pImpl->validateMemoryStorage_ ? "enabled" : "disabled") << std::endl;
    ss << "  Memory retrieval validation: " << (pImpl->validateMemoryRetrieval_ ? "enabled" : "disabled") << std::endl;
    ss << "  Memory consolidation validation: " << (pImpl->validateMemoryConsolidation_ ? "enabled" : "disabled") << std::endl;
    ss << "  Memory replay validation: " << (pImpl->validateMemoryReplay_ ? "enabled" : "disabled") << std::endl;
    return ss.str();
}

} // namespace validation
} // namespace nlm
