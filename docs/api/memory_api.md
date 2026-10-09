# NLM Memory System API Reference

## Overview

The Memory System API provides access to NLM's comprehensive memory architecture. NLM implements multiple memory systems that work together to enable intelligent behavior:

1. **Working Memory** - Short-term, active information storage with persistent activity
2. **Episodic Memory** - Event-based memory with experience encoding and replay
3. **Associative Memory** - Pattern relationship storage with Hebbian learning
4. **Concept Formation** - Abstract pattern discovery without labels
5. **Spatial Representation** - Learned spatial relationships
6. **Temporal Relations** - Before/after/sequence encoding

## Working Memory API

### WorkingMemoryStats Structure

```cpp
struct WorkingMemoryStats {
    size_t capacity;              // Maximum number of memory items
    size_t activeCount;           // Currently active items
    float fillRatio;              // Memory utilization (0.0-1.0)
    float avgRetentionTime;       // Average retention duration (steps)
    float interferenceLevel;      // Level of memory interference
};
```

### Working Memory Operations

#### void addPatternToWorkingMemory(const std::vector<float>& pattern, float priority = 1.0f)

**Description**: Adds a neural pattern to working memory with specified priority.

**Parameters**:
- `pattern`: Neural pattern to store (typically 10-100 elements)
- `priority`: Storage priority (default 1.0)

**Example**:
```cpp
std::vector<float> sensory_pattern = {0.8f, 0.5f, 0.3f, 0.9f, 0.2f};
brain->addPatternToWorkingMemory(sensory_pattern, 0.8f);
```

#### std::vector<float> retrieveFromWorkingMemory(size_t age) const

**Description**: Retrieves pattern from working memory based on age (how long ago it was stored).

**Parameters**:
- `age`: Memory age in simulation steps

**Returns**: Retrieved pattern or empty vector if not found.

**Example**:
```cpp
std::vector<float> pattern = brain->retrieveFromWorkingMemory(10);  // 10 steps ago
```

#### void updateWorkingMemoryPattern(size_t age, const std::vector<float>& new_pattern)

**Description**: Updates existing working memory pattern.

**Parameters**:
- `age`: Memory age
- `new_pattern`: New pattern to store

**Example**:
```cpp
brain->updateWorkingMemoryPattern(5, {0.9f, 0.4f, 0.7f});
```

#### float getWorkingMemoryLoad() const

**Description**: Gets current working memory load.

**Returns**: Memory load (0.0-1.0, where 1.0 = full capacity).

**Example**:
```cpp
float load = brain->getWorkingMemoryLoad();
```

#### WorkingMemoryStats getWorkingMemoryStats() const

**Description**: Gets comprehensive working memory statistics.

**Returns**: WorkingMemoryStats structure with current state.

**Example**:
```cpp
WorkingMemoryStats stats = brain->getWorkingMemoryStats();
```

#### void enableWorkingMemory(bool enable)

**Description**: Enables or disables working memory system.

**Parameters**:
- `enable`: Whether to enable working memory

**Example**:
```cpp
agent->enableWorkingMemory(true);
```

### Working Memory Configuration Parameters

#### setWorkingMemoryCapacity(size_t capacity)

**Description**: Sets working memory capacity.

**Parameters**:
- `capacity`: Maximum number of memory items

#### setWorkingMemoryDecayRate(float decay_rate)

**Description**: Sets working memory decay rate.

**Parameters**:
- `decay_rate`: Decay rate per step (0.0-1.0)

#### setWorkingMemoryReplay(bool enable)

**Description**: Enables or disables working memory replay.

**Parameters**:
- `enable`: Whether to enable replay

## Episodic Memory API

### Experience Structure

```cpp
struct Experience {
    size_t id;                           // Unique experience identifier
    std::vector<float> sensory_input;   // Sensory inputs
    std::vector<ActionType> actions;    // Actions taken
    std::vector<float> rewards;         // Rewards received
    size_t timestamp;                   // When experience occurred
    std::string context;                // Context/category
    DevelopmentalStage stage;           // Developmental stage
    float novelty;                       // Novelty score
    float prediction_error;             // Prediction error during experience
};
```

### Episodic Memory Operations

#### void storeEpisodicMemory(const Experience& experience)

**Description**: Stores an episodic experience in memory.

**Parameters**:
- `experience`: Experience to store

**Example**:
```cpp
Experience exp;
exp.id = 123;
exp.sensory_input = {0.5, 0.7, 0.3, 0.8, 0.4};
exp.actions = {ActionType::MoveForward, ActionType::Interact};
exp.rewards = {0.1, 1.0};
exp.timestamp = current_step;
exp.context = "resource_discovery";
exp.stage = brain->getDevelopmentalStage();
brain->storeEpisodicMemory(exp);
```

#### bool recallFromEpisodicMemory(const std::vector<float>& pattern, std::vector<float>& result) const

**Description**: Recalls episodic memory based on pattern matching.

**Parameters**:
- `pattern`: Pattern to match
- `result`: Output parameter for recalled pattern

**Returns**: `true` if recall successful, `false` otherwise.

**Example**:
```cpp
std::vector<float> recalled_pattern;
bool success = brain->recallFromEpisodicMemory(search_pattern, recalled_pattern);
```

#### std::vector<Experience> recallSimilarExperiences(const std::vector<float>& pattern, float similarity_threshold = 0.7f) const

**Description**: Recalls experiences similar to given pattern.

**Parameters**:
- `pattern`: Pattern to find similar experiences for
- `similarity_threshold`: Minimum similarity for recall

**Returns**: List of similar experiences.

**Example**:
```cpp
std::vector<Experience> similar = brain->recallSimilarExperiences(pattern, 0.8f);
```

#### EpisodicMemoryStats getEpisodicMemoryStats() const

**Description**: Gets episodic memory statistics.

**Returns**: EpisodicMemoryStats structure.

**Example**:
```cpp
EpisodicMemoryStats stats = brain->getEpisodicMemoryStats();
```

#### void enableEpisodicMemory(bool enable)

**Description**: Enables or disables episodic memory system.

**Parameters**:
- `enable`: Whether to enable episodic memory

#### void configureMemoryReplay(bool enable, size_t replay_interval, size_t consolidation_steps)

**Description**: Configures memory replay parameters.

**Parameters**:
- `enable`: Whether replay is enabled
- `replay_interval`: Steps between replay events
- `consolidation_steps`: Steps for memory consolidation

#### void setReplayStrength(float strength)

**Description**: Sets memory replay strength.

**Parameters**:
- `strength`: Replay strength (0.0-1.0)

### EpisodicMemoryStats Structure

```cpp
struct EpisodicMemoryStats {
    size_t episodesStored;           // Number of stored episodes
    float avgEpisodeLength;          // Average episode length (steps)
    float recallAccuracy;           // Recall accuracy (0.0-1.0)
    float replayFrequency;          // Replay frequency (times/step)
    float consolidationProgress;    // Consolidation progress (0.0-1.0)
    float interferenceLevel;        // Memory interference level (0.0-1.0)
};
```

## Associative Memory API

### AssociativeMemoryStats Structure

```cpp
struct AssociativeMemoryStats {
    size_t associationsCount;          // Number of established associations
    float avgAssociationStrength;     // Average association strength (0.0-1.0)
    float patternDiversity;           // Pattern diversity (0.0-1.0)
    float interferenceLevel;          // Memory interference level (0.0-1.0)
    float completionAccuracy;         // Pattern completion accuracy (0.0-1.0)
};
```

### Associative Memory Operations

#### void formAssociation(const std::vector<float>& trigger, const std::vector<float>& response)

**Description**: Forms association between trigger and response patterns.

**Parameters**:
- `trigger`: Trigger pattern
- `response`: Response pattern

**Example**:
```cpp
brain->formAssociation({0.9f, 0.2f, 0.1f}, {0.3f, 0.8f, 0.5f});
```

#### std::vector<float> completePattern(const std::vector<float>& partial) const

**Description**: Completes partial pattern using associative memory.

**Parameters**:
- `partial`: Partial pattern to complete

**Returns**: Completed pattern or empty vector if completion not possible.

**Example**:
```cpp
std::vector<float> completed = brain->completePattern({0.8f, 0.2f});
```

#### void enableAssociativeMemory(bool enable)

**Description**: Enables or disables associative memory system.

**Parameters**:
- `enable`: Whether to enable associative memory

#### void setAssociationLearningRate(float rate)

**Description**: Sets association learning rate.

**Parameters**:
- `rate`: Learning rate (0.0-1.0)

#### void enableInterferenceManagement(const std::string& strategy)

**Description**: Enables memory interference management.

**Parameters**:
- `strategy`: Interference management strategy ("hierarchical", "competitive", "additive")

## Concept Formation API

### ConceptFormationStats Structure

```cpp
struct ConceptFormationStats {
    size_t conceptsFormed;              // Number of concepts formed
    float conceptStability;            // Concept stability (0.0-1.0)
    float abstractionLevel;            // Abstraction level (0.0-1.0)
    float generalizationAbility;       // Generalization ability (0.0-1.0)
    float patternComplexity;            // Pattern complexity (0.0-1.0)
    float categoryCount;                // Number of categories discovered
};
```

### Concept Formation Operations

#### void processForConceptLearning(const std::vector<float>& pattern, size_t timestep)

**Description**: Processes pattern for concept learning.

**Parameters**:
- `pattern`: Pattern to process
- `timestep`: Current timestep

**Example**:
```cpp
brain->processForConceptLearning({0.6f, 0.8f, 0.3f}, current_step);
```

#### std::vector<std::string> getDiscoveredConcepts() const

**Description**: Gets list of discovered concepts.

**Returns**: List of concept names.

**Example**:
```cpp
std::vector<std::string> concepts = brain->getDiscoveredConcepts();
```

#### ConceptFormationStats getConceptFormationStats() const

**Description**: Gets concept formation statistics.

**Returns**: ConceptFormationStats structure.

**Example**:
```cpp
ConceptFormationStats stats = brain->getConceptFormationStats();
```

#### void enableConceptFormation(bool enable)

**Description**: Enables or disables concept formation system.

**Parameters**:
- `enable`: Whether to enable concept formation

#### void setConceptLearningThreshold(float threshold)

**Description**: Sets concept learning threshold.

**Parameters**:
- `threshold`: Learning threshold (0.0-1.0)

## Spatial Memory API

### Spatial Memory Operations

#### void learnSpatialLocation(const std::string& landmark, float x, float y)

**Description**: Learns spatial location relative to landmark.

**Parameters**:
- `landmark`: Landmark name
- `x`: X coordinate
- `y`: Y coordinate

#### std::pair<float, float> getSpatialLocation(const std::string& landmark) const

**Description**: Gets spatial location relative to landmark.

**Parameters**:
- `landmark`: Landmark name

**Returns**: (x, y) coordinates.

#### float getSpatialDistance(const std::string& landmark1, const std::string& landmark2) const

**Description**: Gets distance between two landmarks.

**Parameters**:
- `landmark1`: First landmark
- `landmark2`: Second landmark

**Returns**: Distance between landmarks.

## Temporal Memory API

### Temporal Memory Operations

#### void storeTemporalSequence(const std::vector<size_t>& sequence)

**Description**: Stores temporal sequence.

**Parameters**:
- `sequence`: Sequence of event identifiers

#### std::vector<size_t> retrieveTemporalSequence(size_t start_time, size_t duration) const

**Description**: Retrieves temporal sequence.

**Parameters**:
- `start_time`: Start time
- `duration`: Sequence duration

**Returns**: Retrieved sequence.

#### float predictTemporalEvent(const std::vector<size_t>& partial_sequence) const

**Description**: Predicts next event in temporal sequence.

**Parameters**:
- `partial_sequence`: Partial sequence

**Returns**: Predicted event identifier.

## Memory Integration API

### Brain Memory Operations

#### void enableMemorySystem(const std::string& system_name, bool enable)

**Description**: Enables or disables specific memory system.

**Parameters**:
- `system_name`: Name of memory system ("working", "episodic", "associative", "concept")
- `enable`: Whether to enable system

**Example**:
```cpp
brain->enableMemorySystem("working", true);
brain->enableMemorySystem("episodic", true);
brain->enableMemorySystem("associative", true);
brain->enableMemorySystem("concept", true);
```

#### bool isMemorySystemEnabled(const std::string& system_name) const

**Description**: Checks if memory system is enabled.

**Parameters**:
- `system_name`: Name of memory system

**Returns**: Whether system is enabled.

#### void configureMemorySystems(const MemoryConfig& config)

**Description**: Configures all memory systems.

**Parameters**:
- `config`: Memory configuration

### Agent Memory Operations

#### void enableMemoryForAgent(bool enable_working, bool enable_episodic, bool enable_associative)

**Description**: Configures memory systems for agent.

**Parameters**:
- `enable_working`: Enable working memory
- `enable_episodic`: Enable episodic memory
- `enable_associative`: Enable associative memory

#### MemoryStats getAgentMemoryStats() const

**Description**: Gets agent memory statistics.

**Returns**: Memory statistics.

## Memory Statistics API

### MemoryStats Structure

```cpp
struct MemoryStats {
    WorkingMemoryStats working;        // Working memory statistics
    EpisodicMemoryStats episodic;      // Episodic memory statistics
    AssociativeMemoryStats associative; // Associative memory statistics
    ConceptFormationStats concept;     // Concept formation statistics
    float totalMemoryLoad;             // Total memory load (0.0-1.0)
    float memoryEfficiency;            // Memory efficiency (0.0-1.0)
    float interferenceLevel;           // Overall interference level (0.0-1.0)
};
```

### Memory Operations

#### MemoryStats getMemoryStats() const

**Description**: Gets comprehensive memory statistics.

**Returns**: Memory statistics.

**Example**:
```cpp
MemoryStats stats = brain->getMemoryStats();
```

#### float calculateMemoryEfficiency() const

**Description**: Calculates memory system efficiency.

**Returns**: Efficiency score (0.0-1.0).

**Example**:
```cpp
float efficiency = brain->calculateMemoryEfficiency();
```

#### size_t getTotalMemoryCapacity() const

**Description**: Gets total memory capacity.

**Returns**: Total memory capacity in items.

**Example**:
```cpp
size_t capacity = brain->getTotalMemoryCapacity();
```

#### float getMemoryUtilization() const

**Description**: Gets current memory utilization.

**Returns**: Memory utilization (0.0-1.0).

**Example**:
```cpp
float utilization = brain->getMemoryUtilization();
```

## Python Memory API

The Python memory API provides simplified access to NLM memory systems:

### Working Memory

```python
class WorkingMemory:
    def __init__(self, brain):
        self.brain = brain
        
    def add_pattern(self, pattern, priority=1.0):
        # Add pattern to working memory
        
    def retrieve(self, age):
        # Retrieve pattern by age
        
    def get_stats(self):
        # Get working memory statistics
```

### Episodic Memory

```python
class EpisodicMemory:
    def __init__(self, brain):
        self.brain = brain
        
    def store_experience(self, experience):
        # Store episodic experience
        
    def recall(self, pattern):
        # Recall experiences by pattern
        
    def recall_similar(self, pattern, threshold=0.7):
        # Recall similar experiences
        
    def get_stats(self):
        # Get episodic memory statistics
```

### Associative Memory

```python
class AssociativeMemory:
    def __init__(self, brain):
        self.brain = brain
        
    def form_association(self, trigger, response):
        # Form association between patterns
        
    def complete_pattern(self, partial):
        # Complete partial pattern
        
    def get_stats(self):
        # Get associative memory statistics
```

### Concept Formation

```python
class ConceptFormation:
    def __init__(self, brain):
        self.brain = brain
        
    def process_for_learning(self, pattern, timestep):
        # Process pattern for concept learning
        
    def get_concepts(self):
        # Get discovered concepts
        
    def get_stats(self):
        # Get concept formation statistics
```

## Memory Configuration API

### MemoryConfig Structure

```cpp
struct MemoryConfig {
    bool enable_working_memory;           // Enable working memory
    size_t working_memory_capacity;       // Working memory capacity
    float working_memory_decay_rate;      // Working memory decay rate
    bool enable_episodic_memory;          // Enable episodic memory
    size_t episodic_memory_max_episodes; // Maximum episodes
    size_t episodic_memory_replay_interval; // Replay interval
    bool enable_associative_memory;       // Enable associative memory
    float associative_memory_learning_rate; // Learning rate
    bool enable_concept_formation;        // Enable concept formation
    float concept_formation_threshold;    // Learning threshold
    std::string interference_strategy;   // Interference management strategy
};
```

### Configuration Operations

#### MemoryConfig createDefaultMemoryConfig()

**Description**: Creates default memory configuration.

**Returns**: Default memory configuration.

#### MemoryConfig createResearchMemoryConfig()

**Description**: Creates memory configuration optimized for research.

**Returns**: Research-oriented memory configuration.

#### void applyMemoryConfig(const MemoryConfig& config)

**Description**: Applies memory configuration to brain.

**Parameters**:
- `config`: Memory configuration

## Memory Example Programs

### Example 1: Working Memory Demonstration

```cpp
// working_memory_example.cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"

int main() {
    // Create brain with working memory
    auto config = std::make_shared<Config>();
    config->set("memory.working.enable", true);
    config->set("memory.working.capacity", 20);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Demonstrate working memory
    std::cout << "Working Memory Example" << std::endl;
    std::cout << "======================" << std::endl;
    
    // Add patterns to working memory
    std::vector<float> pattern1 = {0.8f, 0.5f, 0.3f, 0.9f, 0.2f};
    std::vector<float> pattern2 = {0.3f, 0.7f, 0.4f, 0.6f, 0.8f};
    
    brain->addPatternToWorkingMemory(pattern1, 0.8f);
    brain->addPatternToWorkingMemory(pattern2, 0.6f);
    
    // Retrieve patterns
    std::vector<float> retrieved1 = brain->retrieveFromWorkingMemory(0);
    std::vector<float> retrieved2 = brain->retrieveFromWorkingMemory(1);
    
    std::cout << "Stored patterns: " << brain->getWorkingMemoryStats().activeCount << std::endl;
    std::cout << "Memory load: " << brain->getWorkingMemoryLoad() << std::endl;
    
    return 0;
}
```

### Example 2: Episodic Memory Demonstration

```cpp
// episodic_memory_example.cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"

int main() {
    // Create brain with episodic memory
    auto config = std::make_shared<Config>();
    config->set("memory.episodic.enable", true);
    config->set("memory.episodic.max_episodes", 50);
    config->set("memory.episodic.replay_interval", 20);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Demonstrate episodic memory
    std::cout << "Episodic Memory Example" << std::endl;
    std::cout << "======================" << std::endl;
    
    // Store experiences
    for (size_t step = 0; step < 50; ++step) {
        brain->step(step);
        
        // Create experience
        Experience exp;
        exp.id = step;
        exp.sensory_input = {0.5f, 0.7f, 0.3f, 0.8f, 0.4f};
        exp.actions = {ActionType::MoveForward, ActionType::Interact};
        exp.rewards = {0.1f, 1.0f};
        exp.timestamp = step;
        exp.context = "exploration";
        exp.stage = brain->getDevelopmentalStage();
        
        // Store experience
        brain->storeEpisodicMemory(exp);
        
        // Attempt recall
        std::vector<float> search_pattern = {0.5f, 0.7f, 0.3f, 0.8f, 0.4f};
        std::vector<float> recalled;
        bool success = brain->recallFromEpisodicMemory(search_pattern, recalled);
        
        if (success) {
            std::cout << "Step " << step << ": Recalled experience " << exp.id << std::endl;
        }
    }
    
    // Analyze memory statistics
    EpisodicMemoryStats stats = brain->getEpisodicMemoryStats();
    std::cout << "\nMemory Statistics:" << std::endl;
    std::cout << "  Episodes stored: " << stats.episodesStored << std::endl;
    std::cout << "  Recall accuracy: " << stats.recallAccuracy << std::endl;
    std::cout << "  Replay frequency: " << stats.replayFrequency << " times/step" << std::endl;
    
    return 0;
}
```

### Example 3: Associative Memory Demonstration

```cpp
// associative_memory_example.cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"

int main() {
    // Create brain with associative memory
    auto config = std::make_shared<Config>();
    config->set("memory.associative.enable", true);
    config->set("memory.associative.pattern_threshold", 0.7f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Demonstrate associative memory
    std::cout << "Associative Memory Example" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Form associations
    brain->formAssociation({0.9f, 0.2f, 0.1f}, {0.3f, 0.8f, 0.5f});
    brain->formAssociation({0.3f, 0.7f, 0.2f}, {0.6f, 0.1f, 0.9f});
    
    // Test pattern completion
    std::vector<float> partial = {0.8f, 0.2f};
    std::vector<float> completed = brain->completePattern(partial);
    
    std::cout << "Partial pattern: [0.8, 0.2]" << std::endl;
    std::cout << "Completed pattern: [";
    for (size_t i = 0; i < completed.size(); ++i) {
        std::cout << completed[i];
        if (i < completed.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    
    // Analyze memory statistics
    AssociativeMemoryStats stats = brain->getAssociativeMemoryStats();
    std::cout << "\nMemory Statistics:" << std::endl;
    std::cout << "  Associations formed: " << stats.associationsCount << std::endl;
    std::cout << "  Average strength: " << stats.avgAssociationStrength << std::endl;
    std::cout << "  Pattern diversity: " << stats.patternDiversity << std::endl;
    std::cout << "  Completion accuracy: " << stats.completionAccuracy << std::endl;
    
    return 0;
}
```

## Memory API Reference Summary

| Category | Class | Key Methods |
|----------|-------|-------------|
| Working Memory | `WorkingMemory` | `addPattern()`, `retrieve()`, `getStats()` |
| Episodic Memory | `EpisodicMemory` | `storeExperience()`, `recall()`, `recallSimilar()`, `getStats()` |
| Associative Memory | `AssociativeMemory` | `formAssociation()`, `completePattern()`, `getStats()` |
| Concept Formation | `ConceptFormation` | `processForLearning()`, `getConcepts()`, `getStats()` |
| Spatial Memory | `SpatialMemory` | `learnSpatialLocation()`, `getSpatialLocation()`, `getSpatialDistance()` |
| Temporal Memory | `TemporalMemory` | `storeTemporalSequence()`, `retrieveTemporalSequence()`, `predictTemporalEvent()` |
| Integration | `MemoryManager` | `enableMemorySystem()`, `getMemoryStats()`, `getMemoryEfficiency()` |

## Python Memory API Reference Summary

| Category | Class | Key Methods |
|----------|-------|-------------|
| Working Memory | `WorkingMemory` | `add_pattern()`, `retrieve()`, `get_stats()` |
| Episodic Memory | `EpisodicMemory` | `store_experience()`, `recall()`, `recall_similar()`, `get_stats()` |
| Associative Memory | `AssociativeMemory` | `form_association()`, `complete_pattern()`, `get_stats()` |
| Concept Formation | `ConceptFormation` | `process_for_learning()`, `get_concepts()`, `get_stats()` |
| Memory Manager | `MemoryManager` | `enable_memory_system()`, `get_memory_stats()`, `get_memory_efficiency()` |

## Next Steps

1. **Get Started**: Try the basic examples in `docs/examples/memory_systems.cpp`
2. **Explore Documentation**: Read `configuration.md` for memory configuration details
3. **Run Demos**: Execute memory-specific demo programs
4. **Create Experiments**: Design memory-specific NLM experiments
5. **Extend Memory**: Add new memory features to NLM

The Memory API provides comprehensive access to NLM's memory systems. Whether you're interested in working memory, episodic memory, associative memory, concept formation, spatial memory, or temporal memory, the API provides the tools you need to create sophisticated memory experiments.

---

*Memory System API Reference*
- **Location**: `docs/api/memory_api.md`
- **Source**: Various memory system headers in `src/memory/`
- **Python Bindings**: Memory API in Python bindings
- **Documentation**: Complete memory API reference with examples

*Ready to start using the Memory API?*

1. **Read the examples**: Check `docs/examples/memory_systems.cpp`
2. **Try Python API**: Use `configuration.md` for Python examples
3. **Run demo programs**: Execute the built-in demo programs
4. **Create your own**: Design custom memory NLM experiments
5. **Contribute**: Share your memory API usage with the community

The Memory API is essential for NLM's intelligent behavior. It provides access to all memory systems that enable NLM to:
- **Store Information**: Maintain knowledge across time and experiences
- **Learn from Experience**: Improve through memory encoding and retrieval
- **Generalize Knowledge**: Apply learned patterns to new situations
- **Plan Actions**: Use memory to guide future behavior
- **Adapt to Environment**: Update memories based on new information

Your memory API research will help advance our understanding of how memory supports intelligent behavior!

---

*Memory API enables NLM to:
• Store information in working memory
• Encode experiences in episodic memory
• Form associations in associative memory
• Discover concepts through concept formation
• Navigate using spatial memory
• Track time using temporal memory
• Integrate across all memory systems

Your memory API research contributions will help advance neural system research!

---