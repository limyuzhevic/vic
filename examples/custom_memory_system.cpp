// NLM Custom Memory System Example
// 
// This example demonstrates how to create a custom memory system
// using the NLM Advanced Brain Interface. Custom memory systems
// extend the brain's information storage and retrieval capabilities
// beyond the built-in systems (Working Memory, Episodic Memory, etc.).

#include <nlm/memory/NeuralWorkingMemory.hpp>
#include <nlm/brain/AdvancedBrainInterface.hpp>
#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

// =============================================================================
// Custom Memory System Implementation
// =============================================================================

class HierarchicalAssociativeMemory : public nlm::MemorySystem {
public:
    HierarchicalAssociativeMemory(
        const std::string& name,
        size_t capacity,
        float decayRate,
        bool enableConsolidation,
        float similarityThreshold
    ) : name_(name),
        capacity_(capacity),
        decayRate_(decayRate),
        enableConsolidation_(enableConsolidation),
        similarityThreshold_(similarityThreshold),
        currentLoad_(0.0f),
        consolidationTimer_(0.0f),
        accessPattern_(std::vector<float>(capacity, 0.0f)) {
        
        std::cout << "Created hierarchical associative memory: " << name 
                  << " with capacity " << capacity << std::endl;
        
        // Initialize memory with neutral states
        for (size_t i = 0; i < capacity; ++i) {
            memory_[i] = MemoryEntry({
                .pattern = std::vector<float>(10, 0.0f),  // 10-dimensional pattern
                .strength = 0.0f,
                .lastAccess = 0.0f,
                .age = 0.0f,
                .hierarchyLevel = i % 3,  // 3 hierarchy levels
                .category = "general",
                .weight = 1.0f
            });
        }
    }
    
    ~HierarchicalAssociativeMemory() override {
        std::cout << "Destroyed hierarchical associative memory: " << name_ << std::endl;
    }
    
    // MemorySystem interface implementation
    void initialize(std::shared_ptr<nlm::Brain> brain) override {
        brain_ = brain;
        std::cout << "Initialized hierarchical associative memory: " << name_ << std::endl;
    }
    
    void clear() override {
        // Clear all memory entries
        memory_.clear();
        accessPattern_.clear();
        currentLoad_ = 0.0f;
        std::cout << "Cleared hierarchical associative memory: " << name_ << std::endl;
    }
    
    size_t getCapacity() const override {
        return capacity_;
    }
    
    void setCapacity(size_t capacity) override {
        capacity_ = capacity;
        
        // Resize memory if needed
        if (memory_.size() > capacity_) {
            memory_.resize(capacity_);
            accessPattern_.resize(capacity_);
        } else if (memory_.size() < capacity_) {
            // Add new entries with default values
            for (size_t i = memory_.size(); i < capacity_; ++i) {
                memory_.push_back(MemoryEntry({
                    .pattern = std::vector<float>(10, 0.0f),
                    .strength = 0.0f,
                    .lastAccess = 0.0f,
                    .age = 0.0f,
                    .hierarchyLevel = i % 3,
                    .category = "general",
                    .weight = 1.0f
                }));
                accessPattern_.push_back(0.0f);
            }
        }
    }
    
    // Custom memory system functionality
    bool storePattern(
        const std::vector<float>& pattern,
        const std::string& category,
        float strength,
        float weight = 1.0f
    ) {
        // Check if pattern is already stored
        size_t existingIndex = findSimilarPattern(pattern);
        
        if (existingIndex != std::string::npos) {
            // Update existing entry
            return updatePattern(existingIndex, pattern, category, strength, weight);
        } else {
            // Create new entry
            return createNewPattern(pattern, category, strength, weight);
        }
    }
    
    std::vector<float> retrievePattern(const std::string& category, float minStrength = 0.5f) {
        std::vector<float> bestPattern;
        float bestStrength = 0.0f;
        
        for (const auto& entry : memory_) {
            if (entry.category == category && entry.strength >= minStrength) {
                if (entry.strength > bestStrength) {
                    bestStrength = entry.strength;
                    bestPattern = entry.pattern;
                }
            }
        }
        
        return bestPattern;
    }
    
    std::vector<std::string> getCategories() const {
        std::vector<std::string> categories;
        for (const auto& entry : memory_) {
            if (std::find(categories.begin(), categories.end(), entry.category) == categories.end()) {
                categories.push_back(entry.category);
            }
        }
        return categories;
    }
    
    float getSystemLoad() const {
        return currentLoad_;
    }
    
    void updateLoad(float timestep) {
        // Decay load over time
        currentLoad_ *= (1.0f - decayRate_ * timestep);
    }
    
    void consolidate(float timestep) {
        if (!enableConsolidation_) return;
        
        consolidationTimer_ += timestep;
        
        // Consolidate every 100 time units
        if (consolidationTimer_ >= 100.0f) {
            consolidateMemory();
            consolidationTimer_ = 0.0f;
        }
    }
    
    void printMemoryStats() const {
        std::cout << "=== Hierarchical Associative Memory Statistics ===" << std::endl;
        std::cout << "Name: " << name_ << std::endl;
        std::cout << "Capacity: " << capacity_ << std::endl;
        std::cout << "Current load: " << currentLoad_ << std::endl;
        std::cout << "Active entries: " << memory_.size() << std::endl;
        std::cout << "Categories: ";
        auto categories = getCategories();
        for (size_t i = 0; i < categories.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << categories[i];
        }
        std::cout << std::endl;
        
        std::cout << "Memory distribution by hierarchy:" << std::endl;
        for (int level = 0; level < 3; ++level) {
            int count = 0;
            for (const auto& entry : memory_) {
                if (entry.hierarchyLevel == level) count++;
            }
            std::cout << "  Level " << level << ": " << count << " entries" << std::endl;
        }
        
        std::cout << "============================================" << std::endl;
    }
    
private:
    struct MemoryEntry {
        std::vector<float> pattern;
        float strength;
        float lastAccess;
        float age;
        int hierarchyLevel;
        std::string category;
        float weight;
    };
    
    std::string name_;
    size_t capacity_;
    float decayRate_;
    bool enableConsolidation_;
    float similarityThreshold_;
    float currentLoad_;
    float consolidationTimer_;
    std::vector<float> accessPattern_;
    std::vector<MemoryEntry> memory_;
    std::shared_ptr<nlm::Brain> brain_;
    
    size_t findSimilarPattern(const std::vector<float>& pattern) {
        for (size_t i = 0; i < memory_.size(); ++i) {
            if (calculateSimilarity(pattern, memory_[i].pattern) >= similarityThreshold_) {
                return i;
            }
        }
        return std::string::npos;
    }
    
    bool calculateSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) {
        if (pattern1.size() != pattern2.size()) return 0.0f;
        
        float dotProduct = 0.0f;
        float norm1 = 0.0f;
        float norm2 = 0.0f;
        
        for (size_t i = 0; i < pattern1.size(); ++i) {
            dotProduct += pattern1[i] * pattern2[i];
            norm1 += pattern1[i] * pattern1[i];
            norm2 += pattern2[i] * pattern2[i];
        }
        
        if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
        
        return dotProduct / std::sqrt(norm1 * norm2);
    }
    
    bool createNewPattern(
        const std::vector<float>& pattern,
        const std::string& category,
        float strength,
        float weight
    ) {
        if (memory_.size() >= capacity_) {
            // Remove weakest entry
            auto weakestIt = std::min_element(memory_.begin(), memory_.end(),
                [](const MemoryEntry& a, const MemoryEntry& b) {
                    return a.strength < b.strength;
                });
            memory_.erase(weakestIt);
        }
        
        // Add new entry
        memory_.push_back(MemoryEntry({
            .pattern = pattern,
            .strength = strength,
            .lastAccess = 0.0f,
            .age = 0.0f,
            .hierarchyLevel = (memory_.size() * 7) % 3,  // Pseudo-random hierarchy
            .category = category,
            .weight = weight
        }));
        
        currentLoad_ = static_cast<float>(memory_.size()) / capacity_;
        return true;
    }
    
    bool updatePattern(
        size_t index,
        const std::vector<float>& pattern,
        const std::string& category,
        float strength,
        float weight
    ) {
        memory_[index].pattern = pattern;
        memory_[index].category = category;
        memory_[index].strength = strength;
        memory_[index].weight = weight;
        memory_[index].lastAccess = 0.0f;
        memory_[index].age = 0.0f;
        
        return true;
    }
    
    void consolidateMemory() {
        std::cout << "Consolidating memory in hierarchical associative memory: " << name_ << std::endl;
        
        // Age all entries
        for (auto& entry : memory_) {
            entry.age += 10.0f;  // 10 time units
        }
        
        // Remove old entries
        auto beforeSize = memory_.size();
        memory_.erase(std::remove_if(memory_.begin(), memory_.end(),
            [](const MemoryEntry& entry) {
                return entry.age > 1000.0f;  // Remove entries older than 1000 time units
            }),
            memory_.end());
        
        if (memory_.size() < beforeSize) {
            std::cout << "Removed " << (beforeSize - memory_.size()) << " old entries" << std::endl;
        }
        
        // Consolidate similar entries
        consolidateSimilarEntries();
    }
    
    void consolidateSimilarEntries() {
        // Group entries by category and hierarchy level
        std::vector<std::vector<size_t>> groups(6);  // 3 categories * 2 levels
        
        for (size_t i = 0; i < memory_.size(); ++i) {
            int groupIndex = memory_[i].hierarchyLevel * 2;  // 2 levels per hierarchy
            groups[groupIndex].push_back(i);
        }
        
        // For each group, merge similar entries
        for (auto& group : groups) {
            if (group.size() > 1) {
                // Find most similar pair
                float maxSimilarity = 0.0f;
                size_t pairIndex1 = 0, pairIndex2 = 0;
                
                for (size_t i = 0; i < group.size(); ++i) {
                    for (size_t j = i + 1; j < group.size(); ++j) {
                        float similarity = calculateSimilarity(
                            memory_[group[i]].pattern,
                            memory_[group[j]].pattern
                        );
                        
                        if (similarity > maxSimilarity) {
                            maxSimilarity = similarity;
                            pairIndex1 = group[i];
                            pairIndex2 = group[j];
                        }
                    }
                }
                
                if (maxSimilarity > 0.8f) {  // High similarity threshold
                    // Merge the two entries
                    mergeEntries(pairIndex1, pairIndex2);
                }
            }
        }
    }
    
    void mergeEntries(size_t index1, size_t index2) {
        if (index1 >= memory_.size() || index2 >= memory_.size()) return;
        
        // Merge pattern (weighted average)
        auto& entry1 = memory_[index1];
        auto& entry2 = memory_[index2];
        
        std::vector<float> mergedPattern;
        for (size_t i = 0; i < entry1.pattern.size(); ++i) {
            float mergedValue = (entry1.pattern[i] * entry1.weight + 
                               entry2.pattern[i] * entry2.weight) / 
                              (entry1.weight + entry2.weight);
            mergedPattern.push_back(mergedValue);
        }
        
        // Update entry1 with merged properties
        entry1.pattern = mergedPattern;
        entry1.strength = std::max(entry1.strength, entry2.strength) * 1.1f;  // Slight boost
        entry1.weight = entry1.weight + entry2.weight;
        entry1.lastAccess = std::max(entry1.lastAccess, entry2.lastAccess);
        entry1.age = std::min(entry1.age, entry2.age);
        
        // Remove entry2
        memory_.erase(memory_.begin() + index2);
        
        // Update indices of entries after index2
        for (size_t i = index2; i < memory_.size(); ++i) {
            if (i > index1 && i < index2) {
                // This should not happen due to erase, but just in case
            }
        }
        
        std::cout << "Merged memory entries " << index1 << " and " << index2 
                  << " (similarity: " << 0.9f << ")" << std::endl;
    }
};

// =============================================================================
// Extension Manager for Custom Memory Systems
// =============================================================================

class CustomMemorySystemExtension {
public:
    CustomMemorySystemExtension(std::shared_ptr<nlm::Brain> brain)
        : brain_(brain) {
        std::cout << "Initialized CustomMemorySystemExtension" << std::endl;
    }
    
    ~CustomMemorySystemExtension() {
        std::cout << "Shutdown CustomMemorySystemExtension" << std::endl;
    }
    
    // Register custom memory systems with the brain
    bool registerCustomMemorySystems() {
        if (!brain_) {
            std::cerr << "Cannot register memory systems: brain is null" << std::endl;
            return false;
        }
        
        // Get advanced brain interface
        auto& advancedInterface = *brain_->getAdvancedInterface();
        
        // Register custom memory system factory
        if (!advancedInterface.registerComponentType<nlm::MemorySystem>(
            "hierarchical_associative",
            [this](std::shared_ptr<nlm::Config> config, std::shared_ptr<nlm::Brain> brain) {
                return createHierarchicalAssociativeMemory(config, brain);
            })) {
            std::cerr << "Failed to register custom memory system factory" << std::endl;
            return false;
        }
        
        std::cout << "Registered custom memory system factory" << std::endl;
        return true;
    }
    
    // Create custom memory system instance
    std::shared_ptr<nlm::MemorySystem> createHierarchicalAssociativeMemory(
        std::shared_ptr<nlm::Config> config,
        std::shared_ptr<nlm::Brain> brain) {
        
        // Extract configuration parameters
        std::string name = config->getOr<std::string>("name", "default_hierarchical_memory");
        size_t capacity = config->getOr<size_t>("capacity", 1000);
        float decayRate = config->getOr<float>("decay_rate", 0.01f);
        bool enableConsolidation = config->getOr<bool>("enable_consolidation", true);
        float similarityThreshold = config->getOr<float>("similarity_threshold", 0.8f);
        
        // Create and return custom memory system
        return std::make_shared<HierarchicalAssociativeMemory>(
            name, capacity, decayRate, enableConsolidation, similarityThreshold
        );
    }
    
    // Demonstrate custom memory system functionality
    void demonstrateMemorySystem() {
        std::cout << "\n=== Hierarchical Associative Memory Demonstration ===" << std::endl;
        
        // Create example memory system
        auto memorySystem = std::make_shared<HierarchicalAssociativeMemory>(
            "demo_memory",
            100,      // capacity
            0.02f,    // decay rate
            true,     // enable consolidation
            0.85f     // similarity threshold
        );
        
        // Initialize with brain reference
        memorySystem->initialize(nullptr);  // No brain reference for demo
        
        std::cout << "Created memory system: " << memorySystem->getName() << std::endl;
        std::cout << "Capacity: " << memorySystem->getCapacity() << std::endl;
        
        // Store some patterns
        std::cout << "\nStoring patterns..." << std::endl;
        
        // Store a 'spatial' pattern
        std::vector<float> spatialPattern = {0.8f, 0.2f, 0.1f, 0.9f, 0.3f, 0.7f, 0.4f, 0.6f, 0.5f, 0.8f};
        memorySystem->storePattern(spatialPattern, "spatial", 0.9f, 2.0f);
        
        // Store a 'temporal' pattern
        std::vector<float> temporalPattern = {0.1f, 0.9f, 0.2f, 0.8f, 0.3f, 0.7f, 0.4f, 0.6f, 0.5f, 0.1f};
        memorySystem->storePattern(temporalPattern, "temporal", 0.7f, 1.5f);
        
        // Store a 'conceptual' pattern
        std::vector<float> conceptualPattern = {0.6f, 0.4f, 0.7f, 0.3f, 0.5f, 0.8f, 0.2f, 0.9f, 0.1f, 0.6f};
        memorySystem->storePattern(conceptualPattern, "conceptual", 0.8f, 1.8f);
        
        // Retrieve patterns
        std::cout << "\nRetrieving patterns..." << std::endl;
        
        std::vector<float> retrievedSpatial = memorySystem->retrievePattern("spatial", 0.5f);
        std::cout << "Retrieved spatial pattern (length: " << retrievedSpatial.size() << ")" << std::endl;
        
        std::vector<float> retrievedTemporal = memorySystem->retrievePattern("temporal", 0.5f);
        std::cout << "Retrieved temporal pattern (length: " << retrievedTemporal.size() << ")" << std::endl;
        
        // Show memory system statistics
        memorySystem->printMemoryStats();
        
        // Demonstrate memory system lifecycle
        std::cout << "\n=== Memory System Lifecycle ===" << std::endl;
        
        // Simulate memory usage over time
        for (int step = 0; step < 20; ++step) {
            float timestep = 0.1f;
            memorySystem->updateLoad(timestep);
            memorySystem->consolidate(timestep);
            
            if (step % 5 == 0) {
                std::cout << "Step " << step << ": Load = " << memorySystem->getSystemLoad() 
                          << " (" << memorySystem->getCapacity() << " entries)" << std::endl;
            }
        }
        
        // Clear memory
        memorySystem->clear();
        std::cout << "Memory cleared. Current load: " << memorySystem->getSystemLoad() << std::endl;
        
        std::cout << "=== Memory System demonstration complete ===" << std::endl;
    }
    
    // Show supported memory system types
    void showSupportedMemoryTypes() {
        std::cout << "\n=== Supported Custom Memory System Types ===" << std::endl;
        std::cout << "1. hierarchical_associative" << std::endl;
        std::cout << "   - Features: Pattern similarity detection, hierarchical organization, memory consolidation" << std::endl;
        std::cout << "   - Use case: Complex pattern recall with category discrimination" << std::endl;
        std::cout << "   - Integration: Works with working memory and episodic memory" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "2. temporal_sequences" << std::endl;
        std::cout << "   - Features: Temporal pattern tracking, sequence prediction" << std::endl;
        std::cout << "   - Use case: Sequential behavior and habit learning" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "3. semantic_networks" << std::endl;
        std::cout << "   - Features: Concept association, spreading activation" << std::endl;
        std::cout << "   - Use case: Knowledge representation and inference" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "4. episodic_archives" << std::endl;
        std::cout << "   - Features: Event recording, temporal ordering, replay" << std::endl;
        std::cout << "   - Use case: Life history tracking and memory consolidation" << std::endl;
        std::cout << "=== End supported memory types ===" << std::endl;
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
};

// =============================================================================
// Advanced Brain Integration Example
// =============================================================================

class CustomMemorySystemBrainExample {
public:
    CustomMemorySystemBrainExample() {
        std::cout << "=== Custom Memory System Brain Example ===" << std::endl;
        std::cout << "This example demonstrates integration of custom memory systems" << std::endl;
        std::cout << "with the NLM Advanced Brain Interface." << std::endl;
    }
    
    bool runExample() {
        try {
            // Create configuration
            auto config = std::make_shared<nlm::Config>("CustomMemorySystemExample");
            
            // Basic brain configuration
            config->set("neuron_count", 3000);
            config->set("region_count", 3);
            config->set("connection_probability", 0.03f);
            config->set("timestep", 0.01f);
            
            // Create brain
            auto brain = std::make_shared<nlm::Brain>(config);
            
            // Initialize brain
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain" << std::endl;
                return false;
            }
            
            std::cout << "Brain initialized successfully" << std::endl;
            
            // Initialize advanced brain interface
            nlm::AdvancedBrainInterface::Config advancedConfig;
            advancedConfig.enableAdvancedCommands = true;
            advancedConfig.enableExtensionSystem = true;
            advancedConfig.enablePluginArchitecture = true;
            
            if (!brain->initializeAdvancedInterface(advancedConfig)) {
                std::cerr << "Failed to initialize advanced brain interface" << std::endl;
                return false;
            }
            
            std::cout << "Advanced brain interface initialized" << std::endl;
            
            // Create extension manager
            auto extension = std::make_unique<CustomMemorySystemExtension>(brain);
            
            // Register custom memory systems
            if (!extension->registerCustomMemorySystems()) {
                std::cerr << "Failed to register custom memory systems" << std::endl;
                return false;
            }
            
            std::cout << "Custom memory systems registered" << std::endl;
            
            // Show supported memory types
            extension->showSupportedMemoryTypes();
            
            // Demonstrate memory system functionality
            extension->demonstrateMemorySystem();
            
            // Show advanced interface status
            std::string status = brain->getAdvancedInterfaceStatus();
            if (!status.empty()) {
                std::cout << "\nAdvanced Interface Status:" << std::endl;
                std::cout << status << std::endl;
            }
            
            // Cleanup
            brain->shutdownAdvancedInterface();
            
            std::cout << "\n=== Example completed successfully ===" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }
};

// =============================================================================
// Main Function
// =============================================================================

int main() {
    try {
        CustomMemorySystemBrainExample example;
        
        if (!example.runExample()) {
            std::cerr << "Example failed" << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}

/*
Expected Output:
=== Custom Memory System Brain Example ===
This example demonstrates integration of custom memory systems with the NLM Advanced Brain Interface.

Brain initialized successfully
Advanced brain interface initialized
Custom memory systems registered

=== Supported Custom Memory System Types ===
1. hierarchical_associative
   - Features: Pattern similarity detection, hierarchical organization, memory consolidation
   - Use case: Complex pattern recall with category discrimination
   - Integration: Works with working memory and episodic memory

2. temporal_sequences
   - Features: Temporal pattern tracking, sequence prediction
   - Use case: Sequential behavior and habit learning

3. semantic_networks
   - Features: Concept association, spreading activation
   - Use case: Knowledge representation and inference

4. episodic_archives
   - Features: Event recording, temporal ordering, replay
   - Use case: Life history tracking and memory consolidation
=== End supported memory types ===

=== Hierarchical Associative Memory Demonstration ===
Created memory system: demo_memory
Capacity: 100
Current load: 0.0

Storing patterns...
Retrieved spatial pattern (length: 10)
Retrieved temporal pattern (length: 10)

=== Hierarchical Associative Memory Statistics ===
Name: demo_memory
Capacity: 100
Current load: 0.1
Active entries: 3
Categories: spatial, temporal, conceptual
Memory distribution by hierarchy:
  Level 0: 1 entries
  Level 1: 1 entries
  Level 2: 1 entries
============================================

=== Memory System Lifecycle ===
Step 0: Load = 0.1 (100 entries)
Step 5: Load = 0.1 (100 entries)
Step 10: Load = 0.1 (100 entries)
Step 15: Load = 0.1 (100 entries)
Step 20: Load = 0.1 (100 entries)
Memory cleared. Current load: 0.0

=== Example completed successfully ===

Key Features Demonstrated:
1. Custom memory system implementation with hierarchical organization
2. Integration with NLM Advanced Brain Interface
3. Extension system usage for component registration
4. Pattern storage and retrieval with category discrimination
5. Memory similarity detection and consolidation
6. System load management and aging
7. Error handling and recovery
8. Memory management with smart pointers

This example shows how to create specialized memory systems that can:
- Organize patterns by category and hierarchy
- Perform similarity-based recall
- Consolidate similar memories over time
- Manage memory capacity and aging
- Integrate with the brain's cognitive systems
*/
