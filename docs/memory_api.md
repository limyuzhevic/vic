# NLM Memory Systems API

## Overview

The Memory Systems module implements the cognitive architecture for storing, maintaining, and retrieving information in the NLM artificial brain. Memory is fundamental to learning, decision making, and intelligent behavior, enabling the brain to accumulate experience, learn from patterns, and maintain internal states.

## Key Features

- **Working memory**: Short-term storage for active information processing
- **Episodic memory**: Experience recording with temporal and spatial context
- **Semantic memory**: General knowledge and concepts
- **Procedural memory**: Skills and habits learned through practice
- **Associative memory**: Pattern associations and connections between concepts
- **Memory consolidation**: Process of stabilizing and integrating memories
- **Memory retrieval**: Cue-based access to stored information
- **Memory updating**: Integration of new information with existing memories
- **Memory interference**: Management of competing memory traces
- **Memory capacity management**: Efficient allocation and utilization of memory resources

## Core Classes

### MemoryBase Class

Abstract base class for all memory systems.

```cpp
class MemoryBase {
public:
    virtual ~MemoryBase() = default;
    
    // Memory capacity
    virtual size_t getCapacity() const = 0;
    virtual void setCapacity(size_t capacity) = 0;
    
    // Memory usage
    virtual size_t getUsage() const = 0;
    virtual float getUtilization() const = 0;
    
    // Memory management
    virtual void clear() = 0;
    virtual bool isFull() const = 0;
    virtual bool isEmpty() const = 0;
    
    // Memory statistics
    virtual size_t getMaxStorage() const = 0;
    virtual double getAverageAccessTime() const = 0;
    virtual float getRetrievalSuccessRate() const = 0;
};
```

### NeuralWorkingMemory Class

Working memory system for active information maintenance.

```cpp
class NeuralWorkingMemory : public MemoryBase {
public:
    // Construction and lifecycle
    NeuralWorkingMemory();
    explicit NeuralWorkingMemory(size_t capacity);
    ~NeuralWorkingMemory() override;
    
    // Working memory operations
    void storeInformation(const std::string& content);
    std::string retrieveInformation(const std::string& cue);
    void updateContents();                             // Maintain active representations
    
    // Competition and selection
    void activatePattern(const Pattern& pattern);
    void inhibitPattern(const Pattern& pattern);
    Pattern getCurrentFocus() const;
    
    // Capacity management
    void adjustCapacity(float factor);
    
    // State management
    void reset();
    void saveState();
    void restoreState();
    
    // Statistics
    size_t getActiveItems() const;
    float getActivationLevel(const Pattern& pattern) const;
    std::vector<Pattern> getActivePatterns() const;
};
```

### NeuralEpisodicMemory Class

Episodic memory system for recording specific experiences.

```cpp
class NeuralEpisodicMemory : public MemoryBase {
public:
    // Construction and lifecycle
    NeuralEpisodicMemory();
    explicit NeuralEpisodicMemory(size_t capacity);
    ~NeuralEpisodicMemory() override;
    
    // Encoding
    void encodeExperience(const Experience& experience);
    void replayExperience(const EpisodeId& episodeId);
    
    // Retrieval
    EpisodeId findSimilarExperience(const Experience& query);
    std::vector<EpisodeId> findTemporalSequence(Timestamp start, Timestamp end);
    Experience retrieveExperience(const EpisodeId& episodeId) const;
    
    // Integration with other systems
    void integrateWithSemanticMemory();           // Extract patterns
    void consolidateMemory();                      // Strengthen important memories
    
    // State management
    void reset();
    void saveState();
    void restoreState();
    
    // Statistics
    size_t getEpisodeCount() const;
    float getMemoryFreshness(const EpisodeId& episodeId) const;
    std::vector<EpisodeId> getRecentEpisodes(size_t count) const;
};
```

### NeuralSemanticMemory Class

Semantic memory system for general knowledge and concepts.

```cpp
class NeuralSemanticMemory : public MemoryBase {
public:
    // Construction and lifecycle
    NeuralSemanticMemory();
    explicit NeuralSemanticMemory(size_t capacity);
    ~NeuralSemanticMemory() override;
    
    // Association storage
    void storeAssociation(const Concept& concept1, const Concept& concept2, 
                          float strength);
    
    // Pattern completion
    std::vector<Concept> completePattern(const Concept& partial);
    
    // Generalization
    void generalizeFromExamples(const std::vector<Concept>& examples);
    Concept extractPrototype(const std::vector<Concept>& concepts);
    
    // Concept management
    void addConcept(const Concept& concept);
    void removeConcept(const ConceptId& conceptId);
    Concept getConcept(const ConceptId& conceptId) const;
    
    // State management
    void reset();
    void saveState();
    void restoreState();
    
    // Statistics
    size_t getConceptCount() const;
    float getConceptStrength(const ConceptId& conceptId) const;
    std::vector<ConceptId> getStrongConcepts(float threshold) const;
};
```

### NeuralAssociativeMemory Class

Associative memory system for pattern connections and associations.

```cpp
class NeuralAssociativeMemory : public MemoryBase {
public:
    // Construction and lifecycle
    NeuralAssociativeMemory();
    explicit NeuralAssociativeMemory(size_t capacity);
    ~NeuralAssociativeMemory() override;
    
    // Association formation
    void formAssociation(const Pattern& pattern1, const Pattern& pattern2, 
                         float strength);
    
    // Pattern completion
    std::vector<Pattern> recallPattern(const Pattern& cue);
    float getAssociationStrength(const Pattern& pattern1, const Pattern& pattern2) const;
    
    // Memory indexing
    void indexPattern(const Pattern& pattern, const MemoryId& memoryId);
    std::vector<MemoryId> findMemoriesByPattern(const Pattern& pattern);
    
    // State management
    void reset();
    void saveState();
    void restoreState();
    
    // Statistics
    size_t getAssociationCount() const;
    float getAverageAssociationStrength() const;
    std::vector<std::pair<Pattern, Pattern>> getStrongAssociations(float threshold) const;
};
```

### NeuralProceduralMemory Class

Procedural memory system for skills and habits.

```cpp
class NeuralProceduralMemory : public MemoryBase {
public:
    // Construction and lifecycle
    NeuralProceduralMemory();
    explicit NeuralProceduralMemory(size_t capacity);
    ~NeuralProceduralMemory() override;
    
    // Skill storage
    void learnSkill(const Skill& skill);
    void refineSkill(const SkillId& skillId, const TrainingData& trainingData);
    
    // Skill retrieval
    Skill getSkill(const SkillId& skillId) const;
    std::vector<SkillId> getRelevantSkills(const Context& context);
    
    // Habit formation
    void formHabit(const Action& action, float rewardHistory);
    void adjustHabitStrength(const Action& action, float adjustment);
    
    // State management
    void reset();
    void saveState();
    void restoreState();
    
    // Statistics
    size_t getSkillCount() const;
    float getSkillProficiency(const SkillId& skillId) const;
    std::vector<SkillId> getHighProficiencySkills(float threshold) const;
};
```

## Memory Architecture

### Hierarchical Memory Organization

The NLM memory system uses a hierarchical organization for efficient information management:

```
+-------------------------+      +-------------------------+
|   Working Memory        |      |   Episodic Memory       |
| (Short-term)            |      | (Experience Recording)  |
|                         |      |                         |
| - Active maintenance    |      | - Temporal context       |
| - Competition           |      | - Spatial context        |
| - Selection             |      | - Sensory details        |
+-------------------------+      +-------------------------+
         |                                 |
         v                                 v
+-------------------------+      +-------------------------+
|   Semantic Memory       |      |   Procedural Memory     |
| (General Knowledge)     |      | (Skills and Habits)      |
|                         |      |                         |
| - Concept formation     |      | - Motor skills           |
| - Knowledge integration |      | - Habit learning         |
| - Abstraction          |      | - Skill refinement       |
+-------------------------+      +-------------------------+
```

### Memory Processing Pipeline

Memory operations follow a consistent pipeline:

1. **Encoding**: Transform experience into neural representation
2. **Storage**: Allocate memory and maintain persistence
3. **Consolidation**: Strengthen and integrate with existing knowledge
4. **Retrieval**: Access information based on cues
5. **Updating**: Modify memories with new information

## Memory Implementation

### Encoding Strategies

#### Episodic Encoding

```cpp
void NeuralEpisodicMemory::encodeExperience(const Experience& experience) {
    // Extract temporal information
    EpisodeMetadata metadata;
    metadata.timestamp = experience.timestamp;
    metadata.duration = calculateDuration(experience);
    metadata.context = extractContext(experience);
    
    // Extract spatial information
    metadata.spatialContext = experience.environmentState;
    
    // Extract sensory information
    metadata.sensoryInput = experience.sensoryData;
    
    // Create episodic memory
    EpisodeId episodeId = generateEpisodeId();
    Episode episode;
    episode.id = episodeId;
    episode.experience = experience;
    episode.metadata = metadata;
    
    // Store in memory
    storeEpisode(std::move(episode));
}
```

#### Semantic Encoding

```cpp
void NeuralSemanticMemory::generalizeFromExamples(const std::vector<Concept>& examples) {
    // Extract common features
    std::vector<float> commonFeatures = extractCommonFeatures(examples);
    
    // Create prototype concept
    Concept prototype = createPrototype(commonFeatures);
    
    // Store prototype
    addConcept(prototype);
    
    // Create associations
    for (size_t i = 0; i < examples.size(); ++i) {
        for (size_t j = i + 1; j < examples.size(); ++j) {
            storeAssociation(examples[i], examples[j], calculateSimilarity(examples[i], examples[j]));
        }
    }
}
```

### Retrieval Mechanisms

#### Cue-Based Retrieval

```cpp
std::string NeuralWorkingMemory::retrieveInformation(const std::string& cue) {
    // Normalize cue
    std::string normalizedCue = normalizeCue(cue);
    
    // Find matching patterns
    std::vector<Pattern> matchingPatterns = findMatchingPatterns(normalizedCue);
    
    if (matchingPatterns.empty()) {
        return ""; // No matching information found
    }
    
    // Select best match based on activation strength
    Pattern selectedPattern = selectBestPattern(matchingPatterns);
    
    // Retrieve associated information
    std::string retrievedInfo = getAssociatedInformation(selectedPattern);
    
    // Update activation for future retrieval
    increasePatternActivation(selectedPattern);
    
    return retrievedInfo;
}
```

#### Temporal Retrieval

```cpp
std::vector<EpisodeId> NeuralEpisodicMemory::findTemporalSequence(Timestamp start, Timestamp end) {
    std::vector<EpisodeId> matchingEpisodes;
    
    for (const auto& episode : episodes_) {
        if (episode.metadata.timestamp >= start && 
            episode.metadata.timestamp <= end) {
            matchingEpisodes.push_back(episode.id);
        }
    }
    
    return matchingEpisodes;
}
```

### Memory Consolidation

#### Synaptic Consolidation

```cpp
void NeuralEpisodicMemory::consolidateMemory() {
    // Identify strong memories
    std::vector<EpisodeId> strongEpisodes;
    for (const auto& episode : episodes_) {
        if (isMemoryStrong(episode)) {
            strongEpisodes.push_back(episode.id);
        }
    }
    
    // Strengthen synaptic connections
    for (const auto& episodeId : strongEpisodes) {
        strengthenMemoryTrace(episodeId);
    }
    
    // Integrate with semantic memory
    integrateWithSemanticMemory();
}

bool NeuralEpisodicMemory::isMemoryStrong(const Episode& episode) const {
    // Check multiple factors
    float recencyStrength = calculateRecencyStrength(episode.metadata.timestamp);
    float frequencyStrength = calculateFrequencyStrength(episode);
    float emotionalStrength = calculateEmotionalStrength(episode.experience);
    
    float totalStrength = recencyStrength + frequencyStrength + emotionalStrength;
    return totalStrength > CONSOLIDATION_THRESHOLD;
}
```

#### Semantic Consolidation

```cpp
void NeuralSemanticMemory::integrateWithWorkingMemory() {
    // Extract active concepts from working memory
    std::vector<Concept> activeConcepts = getActiveConcepts();
    
    // Update semantic network with new concepts
    for (const auto& concept : activeConcepts) {
        updateConceptNetwork(concept);
    }
    
    // Refine existing concepts
    refineConcepts(activeConcepts);
}
```

## Memory Integration

### Brain Integration

```cpp
class Brain {
public:
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralSemanticMemory* getSemanticMemory();
    NeuralProceduralMemory* getProceduralMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Memory coordination
    void coordinateMemorySystems();
    void updateMemoryFromExperience(const Experience& experience);
    void retrieveMemoryForTask(const Task& task);
};
```

### Memory Interaction

```cpp
void Brain::coordinateMemorySystems() {
    // Working memory provides active context
    auto workingMemory = getWorkingMemory();
    std::string currentContext = workingMemory->getCurrentFocus().content;
    
    // Episodic memory provides experience-based context
    auto episodicMemory = getEpisodicMemory();
    std::vector<EpisodeId> relevantEpisodes = 
        episodicMemory->findSimilarExperience(currentContext);
    
    // Semantic memory provides general knowledge
    auto semanticMemory = getSemanticMemory();
    std::vector<Concept> relevantConcepts = 
        semanticMemory->completePattern(currentContext);
    
    // Procedural memory provides behavioral patterns
    auto proceduralMemory = getProceduralMemory();
    std::vector<SkillId> relevantSkills = 
        proceduralMemory->getRelevantSkills(currentContext);
    
    // Associative memory provides pattern connections
    auto associativeMemory = getAssociativeMemory();
    std::vector<Pattern> associations = 
        associativeMemory->recallPattern(currentContext);
}
```

## Performance Optimization

### Memory Efficiency

```cpp
class MemoryPool {
public:
    // Allocate memory from pool
    template<typename T>
    T* allocate();
    
    // Deallocate memory to pool
    template<typename T>
    void deallocate(T* ptr);
    
    // Clear pool
    void clear();
    
    // Get memory statistics
    size_t getUsedMemory() const;
    size_t getTotalMemory() const;
    float getUtilization() const;
};
```

### Caching Strategies

```cpp
class MemoryCache {
public:
    // Cache memory items
    void cacheMemory(const MemoryId& id, const MemoryData& data);
    
    // Retrieve cached memory
    MemoryData getCachedMemory(const MemoryId& id);
    
    // Check if memory is cached
    bool isCached(const MemoryId& id) const;
    
    // Invalidate cache
    void invalidateCache();
    void invalidateMemory(const MemoryId& id);
    
    // Cache statistics
    size_t getCacheSize() const;
    size_t getCacheHitCount() const;
    size_t getCacheMissCount() const;
    float getCacheHitRate() const;
};
```

### Parallel Processing

```cpp
class ParallelMemoryProcessor {
public:
    // Process memories in parallel
    static void processMemories(std::vector<MemoryId>& memoryIds,
                                std::function<void(Memory&)> processor);
    
    // Retrieve memories in parallel
    static void retrieveMemories(const std::vector<MemoryId>& memoryIds,
                                 std::vector<Memory>& results);
    
    // Update memories in parallel
    static void updateMemories(const std::vector<MemoryId>& memoryIds,
                               std::function<void(Memory&)> updater);
};
```

## Thread Safety

### Read-Only Operations

```cpp
// Thread-safe read operations
MemoryId NeuralWorkingMemory::getCurrentFocus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentFocus_;
}

Pattern NeuralWorkingMemory::getPattern(const PatternId& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = patterns_.find(id);
    return (it != patterns_.end()) ? it->second : Pattern();
}
```

### Write Operations

```cpp
// Write operations with proper synchronization
void NeuralWorkingMemory::storeInformation(const std::string& content) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isFull()) {
        evictLeastRecentlyUsed();
    }
    
    Pattern pattern = createPattern(content);
    patterns_[pattern.id] = pattern;
    updateRecency(pattern.id);
}
```

## Error Handling

### Memory Errors

```cpp
enum class MemoryError {
    Success = 0,
    MemoryFull = 1,
    MemoryNotFound = 2,
    InvalidMemoryId = 3,
    MemoryCorrupted = 4,
    SerializationFailed = 5,
    DeserializationFailed = 6
};
```

### Error Handling Pattern

```cpp
MemoryId NeuralWorkingMemory::storeInformation(const std::string& content) {
    try {
        // Validate input
        if (content.empty()) {
            throw MemoryError::InvalidMemoryId;
        }
        
        // Check capacity
        if (isFull()) {
            throw MemoryError::MemoryFull;
        }
        
        // Store information
        Pattern pattern = createPattern(content);
        patterns_[pattern.id] = pattern;
        updateRecency(pattern.id);
        
        return pattern.id;
        
    } catch (const MemoryError& e) {
        // Handle error
        std::cerr << "Memory error: " << static_cast<int>(e) << std::endl;
        throw;
    }
}
```

## Usage Examples

### Working Memory Example

```cpp
#include <nlm/memory/WorkingMemory.hpp>

int main() {
    // Create working memory
    auto workingMemory = std::make_shared<nlm::NeuralWorkingMemory>(50);
    
    // Store information
    workingMemory->storeInformation("Current goal: reach the apple");
    workingMemory->storeInformation("Previous location: kitchen");
    
    // Retrieve information
    std::string goal = workingMemory->retrieveInformation("current goal");
    std::string location = workingMemory->retrieveInformation("previous location");
    
    std::cout << "Goal: " << goal << std::endl;
    std::cout << "Location: " << location << std::endl;
    
    return 0;
}
```

### Episodic Memory Example

```cpp
#include <nlm/memory/EpisodicMemory.hpp>

int main() {
    // Create episodic memory
    auto episodicMemory = std::make_shared<nlm::NeuralEpisodicMemory>(1000);
    
    // Create experience
    nlm::Experience experience;
    experience.id = nlm::ExperienceId{1};
    experience.timestamp = 1234567890.0;
    experience.sensoryData = std::vector<float>{0.5f, 0.8f, 0.3f};
    experience.action = nlm::Action{1, "move_forward"};
    experience.reward = 1.0f;
    
    // Encode experience
    episodicMemory->encodeExperience(experience);
    
    // Replay experience
    episodicMemory->replayExperience(experience.id);
    
    // Retrieve similar experiences
    std::vector<nlm::EpisodeId> similarEpisodes = 
        episodicMemory->findSimilarExperience(experience);
    
    std::cout << "Found " << similarEpisodes.size() << " similar experiences" << std::endl;
    
    return 0;
}
```

### Semantic Memory Example

```cpp
#include <nlm/memory/SemanticMemory.hpp>

int main() {
    // Create semantic memory
    auto semanticMemory = std::make_shared<nlm::NeuralSemanticMemory>(500);
    
    // Create concepts
    nlm::Concept concept1;
    concept1.id = nlm::ConceptId{1};
    concept1.features = std::vector<float>{0.7f, 0.9f, 0.2f};
    concept1.name = "apple";
    
    nlm::Concept concept2;
    concept2.id = nlm::ConceptId{2};
    concept2.features = std::vector<float>{0.8f, 0.9f, 0.3f};
    concept2.name = "fruit";
    
    // Add concepts
    semanticMemory->addConcept(concept1);
    semanticMemory->addConcept(concept2);
    
    // Store association
    semanticMemory->storeAssociation(concept1, concept2, 0.9f);
    
    // Complete pattern
    nlm::Concept partial;
    partial.features = std::vector<float>{0.75f, 0.85f, 0.25f};
    
    std::vector<nlm::Concept> completed = 
        semanticMemory->completePattern(partial);
    
    std::cout << "Completed pattern contains " << completed.size() << " concepts" << std::endl;
    
    return 0;
}
```

### Procedural Memory Example

```cpp
#include <nlm/memory/ProceduralMemory.hpp>

int main() {
    // Create procedural memory
    auto proceduralMemory = std::make_shared<nlm::NeuralProceduralMemory>(200);
    
    // Create skill
    nlm::Skill skill;
    skill.id = nlm::SkillId{1};
    skill.name = "walk";
    skill.executableCode = "move_forward(); turn_left();";
    skill.proficiency = 0.8f;
    
    // Learn skill
    proceduralMemory->learnSkill(skill);
    
    // Refine skill
    nlm::TrainingData trainingData;
    trainingData.successCount = 10;
    trainingData.failureCount = 2;
    trainingData.totalTime = 5.0f;
    
    proceduralMemory->refineSkill(skill.id, trainingData);
    
    // Get skill proficiency
    float proficiency = proceduralMemory->getSkillProficiency(skill.id);
    std::cout << "Skill proficiency: " << proficiency << std::endl;
    
    return 0;
}
```

## Testing and Validation

### Unit Testing

```cpp
#include <nlm/memory/WorkingMemory.hpp>
#include <gtest/gtest.h>

TEST(WorkingMemoryTest, StoreAndRetrieve) {
    auto workingMemory = std::make_shared<nlm::NeuralWorkingMemory>(10);
    
    // Store information
    nlm::PatternId patternId = workingMemory->storeInformation("test information");
    
    // Retrieve information
    std::string retrieved = workingMemory->retrieveInformation("test");
    
    EXPECT_EQ(retrieved, "test information");
}

TEST(EpisodicMemoryTest, EncodeAndReplay) {
    auto episodicMemory = std::make_shared<nlm::NeuralEpisodicMemory>(100);
    
    // Create experience
    nlm::Experience experience;
    experience.id = nlm::ExperienceId{1};
    experience.timestamp = 1234567890.0;
    
    // Encode experience
    episodicMemory->encodeExperience(experience);
    
    // Verify encoding
    EXPECT_EQ(episodicMemory->getEpisodeCount(), 1);
    
    // Replay experience
    episodicMemory->replayExperience(experience.id);
    
    EXPECT_TRUE(episodicMemory->hasReplayed(experience.id));
}
```

### Performance Testing

```bash
// Run memory performance tests
./benchmark --memory --test working_memory

// Profile memory usage
./profiler --target memory --operation store_retrieve
```

## Migration Guide

### From Earlier Versions

Memory API changes:

- **Version 0.1.0**: Basic working memory only
- **Version 0.2.0**: Added episodic and semantic memory
- **Version 0.3.0**: Enhanced procedural and associative memory
- **Version 0.4.0**: Added memory consolidation and retrieval
- **Version 0.5.0**: Improved memory integration and coordination
- **Version 0.6.0**: Enhanced memory efficiency and performance optimization

### API Changes

- **Memory hierarchy**: Changed from single memory system to multiple specialized systems
- **Memory management**: Added automatic memory management and lifecycle control
- **Memory integration**: Enhanced integration with other brain systems
- **Performance optimization**: Added memory pools and caching strategies
- **Thread safety**: Added proper synchronization for concurrent access
- **Error handling**: Enhanced error handling and validation

## References

1. [Memory in Artificial Intelligence](https://en.wikipedia.org/wiki/Memory_(artificial_intelligence))
2. [Working Memory Models](https://en.wikipedia.org/wiki/Working_memory)
3. [Episodic Memory](https://en.wikipedia.org/wiki/Episodic_memory)
4. [Semantic Memory](https://en.wikipedia.org/wiki/Semantic_memory)
5. [Procedural Memory](https://en.wikipedia.org/wiki/Procedural_memory)

## See Also

- [Brain API](brain_api.md) - Brain integration with memory systems
- [Config API](config_api.md) - Configuration for memory systems
- [Logging API](logging_api.md) - Logging system integration
- [Neuromodulation API](neuromodulation_api.md) - Neuromodulation effects on memory

**Last updated:** October 18, 2026
**Version:** 0.6.0