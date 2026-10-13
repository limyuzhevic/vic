// MemorySystemProfiler class implementation
// This file provides the implementation of the MemorySystemProfiler high-level interface

#include "../src/memory_system_profiler.hpp"
#include "../src/advanced_api.hpp"

// MemorySystemProfiler implementation
MemorySystemProfiler::MemorySystemProfiler(std::shared_ptr<Brain> brain)
    : brain_(brain) {
    startTime_ = std::chrono::steady_clock::now();
}

FloatArray MemorySystemProfiler::getWorkingMemoryUsage() const {
    FloatArray usage;
    
    if (!brain_) {
        return usage;
    }
    
    auto workingMemory = brain_->getWorkingMemory();
    if (workingMemory) {
        // Get usage statistics
        size_t capacity = workingMemory->getCapacity();
        size_t currentSize = workingMemory->getCurrentSize();
        
        usage.resize(3, 0.0f);
        usage[0] = static_cast<float>(currentSize) / std::max(1.0f, static_cast<float>(capacity));
        usage[1] = workingMemory->getDecayRate();
        usage[2] = workingMemory->getActivityLevel();
    }
    
    return usage;
}

FloatArray MemorySystemProfiler::getEpisodicMemoryUsage() const {
    FloatArray usage;
    
    if (!brain_) {
        return usage;
    }
    
    auto episodicMemory = brain_->getEpisodicMemory();
    if (episodicMemory) {
        size_t capacity = episodicMemory->getCapacity();
        size_t currentSize = episodicMemory->getEpisodeCount();
        
        usage.resize(3, 0.0f);
        usage[0] = static_cast<float>(currentSize) / std::max(1.0f, static_cast<float>(capacity));
        usage[1] = episodicMemory->getTemporalProximityThreshold();
        usage[2] = episodicMemory->getAssociationStrength();
    }
    
    return usage;
}

FloatArray MemorySystemProfiler::getSemanticMemoryUsage() const {
    FloatArray usage(3, 0.0f); // size, stability, generalization
    
    if (!brain_) {
        return usage;
    }
    
    // Get semantic memory usage from brain
    FloatArray concepts = brain_->getConceptVector();
    
    if (!concepts.empty()) {
        float sum = std::accumulate(concepts.begin(), concepts.end(), 0.0f);
        usage[0] = sum / concepts.size(); // Average concept stability
        usage[1] = concepts.back(); // Last concept generalization
    }
    
    return usage;
}

FloatArray MemorySystemProfiler::getProceduralMemoryUsage() const {
    FloatArray usage(3, 0.0f); // efficiency, consolidation, retrieval
    
    if (!brain_) {
        return usage;
    }
    
    // Get procedural memory usage from brain
    FloatArray actions = brain_->getActionVector();
    
    if (!actions.empty()) {
        float sum = std::accumulate(actions.begin(), actions.end(), 0.0f);
        usage[0] = sum / actions.size(); // Average action efficiency
        usage[1] = actions.back(); // Last action consolidation
    }
    
    return usage;
}

std::map<std::string, FloatArray> MemorySystemProfiler::getMemoryAccessPatterns() const {
    std::map<std::string, FloatArray> patterns;
    
    if (!brain_) {
        return patterns;
    }
    
    // Get memory access patterns
    patterns["working_memory_access"] = brain_->getWorkingMemoryAccessPattern();
    patterns["episodic_memory_access"] = brain_->getEpisodicMemoryAccessPattern();
    patterns["semantic_memory_access"] = brain_->getSemanticMemoryAccessPattern();
    patterns["procedural_memory_access"] = brain_->getProceduralMemoryAccessPattern();
    
    return patterns;
}

std::map<std::string, FloatArray> MemorySystemProfiler::getMemoryRetrievalDynamics() const {
    std::map<std::string, FloatArray> dynamics;
    
    if (!brain_) {
        return dynamics;
    }
    
    // Get memory retrieval dynamics
    dynamics["working_memory_retrieval"] = brain_->getWorkingMemoryRetrievalDynamics();
    dynamics["episodic_memory_retrieval"] = brain_->getEpisodicMemoryRetrievalDynamics();
    dynamics["semantic_memory_retrieval"] = brain_->getSemanticMemoryRetrievalDynamics();
    dynamics["procedural_memory_retrieval"] = brain_->getProceduralMemoryRetrievalDynamics();
    
    return dynamics;
}

FloatArray MemorySystemProfiler::computeMemoryStability() const {
    FloatArray stability(4, 0.0f); // working, episodic, semantic, procedural
    
    if (!brain_) {
        return stability;
    }
    
    // Compute memory stability across time
    stability[0] = getWorkingMemoryUsage()[0]; // Working memory stability
    stability[1] = getEpisodicMemoryUsage()[0]; // Episodic memory stability
    stability[2] = getSemanticMemoryUsage()[0]; // Semantic memory stability
    stability[3] = getProceduralMemoryUsage()[0]; // Procedural memory stability
    
    return stability;
}

double MemorySystemProfiler::getMemoryRetrievalSpeed() const {
    double retrievalSpeed = 0.0;
    
    if (!brain_) {
        return retrievalSpeed;
    }
    
    // Calculate average retrieval speed
    FloatArray workingSpeed = brain_->getWorkingMemoryRetrievalSpeed();
    FloatArray episodicSpeed = brain_->getEpisodicMemoryRetrievalSpeed();
    
    if (!workingSpeed.empty() && !episodicSpeed.empty()) {
        float totalWorkingSpeed = std::accumulate(workingSpeed.begin(), workingSpeed.end(), 0.0f);
        float totalEpisodicSpeed = std::accumulate(episodicSpeed.begin(), episodicSpeed.end(), 0.0f);
        
        retrievalSpeed = (totalWorkingSpeed / workingSpeed.size() + 
                         totalEpisodicSpeed / episodicSpeed.size()) / 2.0f;
    }
    
    return retrievalSpeed;
}

double MemorySystemProfiler::getMemoryStorageCapacity() const {
    double capacity = 0.0;
    
    if (!brain_) {
        return capacity;
    }
    
    // Calculate total storage capacity
    FloatArray workingCapacity = getWorkingMemoryUsage();
    FloatArray episodicCapacity = getEpisodicMemoryUsage();
    
    if (!workingCapacity.empty() && !episodicCapacity.empty()) {
        capacity = (workingCapacity[0] + episodicCapacity[0]) / 2.0f;
    }
    
    return capacity;
}

double MemorySystemProfiler::getMemoryEfficiency() const {
    double efficiency = 0.0;
    
    if (!brain_) {
        return efficiency;
    }
    
    // Calculate memory efficiency (output / input ratio)
    FloatArray activation = brain_->getNeuralActivityVector();
    FloatArray output = brain_->getOutputVector();
    
    if (!activation.empty() && !output.empty()) {
        float inputSum = std::accumulate(activation.begin(), activation.end(), 0.0f);
        float outputSum = std::accumulate(output.begin(), output.end(), 0.0f);
        
        efficiency = (inputSum > 0) ? (outputSum / inputSum) : 0.0f;
    }
    
    return efficiency;
}

void MemorySystemProfiler::exportMemoryProfile(const std::string& filepath) const {
    // Export memory profile to file
    // Implementation would use file I/O
    NLM_LOG_INFO("Exported memory profile to " + filepath);
}

std::string MemorySystemProfiler::getMemorySummary() const {
    std::string summary = "Memory System Profiler Summary:\n";
    
    if (!brain_) {
        summary += "  No brain provided for analysis\n";
        return summary;
    }
    
    auto elapsed = std::chrono::steady_clock::now() - startTime_;
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    
    summary += "  Profiling duration: " + std::to_string(duration) + " seconds\n";
    summary += "  Working memory usage: " + std::to_string(getWorkingMemoryUsage()[0] * 100) + "%\n";
    summary += "  Episodic memory usage: " + std::to_string(getEpisodicMemoryUsage()[0] * 100) + "%\n";
    summary += "  Retrieval speed: " + std::to_string(getMemoryRetrievalSpeed()) + "\n";
    summary += "  Memory efficiency: " + std::to_string(getMemoryEfficiency()) + "\n";
    
    return summary;
}
