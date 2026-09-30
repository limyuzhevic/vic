# Memory System Tutorial

## Overview of NLM Memory Architecture

The NLM brain implements a sophisticated multi-level memory system inspired by biological cognitive architecture. This system allows the brain to store, retrieve, and process information at different timescales and granularities.

### Memory Hierarchy

```
Working Memory (STM)          <- Immediate processing (seconds)
   ↓
Episodic Memory (Autobiographical) <- Experiences (hours-days)
   ↓
Semantic Memory (Declarative)   <- Facts & knowledge (weeks-years)
   ↓
Procedural Memory (Skills)     <- Skills & habits (months-years)
   ↓
Associative Memory (Connections) <- Pattern relationships (variable)
```

## Working Memory Tutorial

### What is Working Memory?

Working memory (also called short-term memory) is the brain's temporary workspace for active information processing. It's where:

- Current goals are maintained
- Information is actively manipulated
- Decisions are being formed
- Attention is focused

### Basic Working Memory Operations

```cpp
#include "nlm/memory/NeuralWorkingMemory.hpp"

void basicWorkingMemoryExample() {
    // Create a brain with working memory support
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", 100, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get the working memory system
    auto workingMem = brain.getWorkingMemory();
    if (!workingMem) {
        std::cerr << "Working memory not available!" << std::endl;
        return;
    }
    
    // 1. Store information
    NeuronId taskId = 42;
    float taskValue = 1.5f;
    workingMem->store(taskId, taskValue);
    std::cout << "Stored task " << taskId << " with value " << taskValue << std::endl;
    
    // 2. Retrieve information
    float retrievedValue = workingMem->retrieve(taskId);
    std::cout << "Retrieved value: " << retrievedValue << std::endl;
    
    // 3. Check if information exists
    if (workingMem->contains(taskId)) {
        std::cout << "Task " << taskId << " is in working memory" << std::endl;
    }
    
    // 4. Perform operations on stored data
    float processedValue = retrievedValue * 2.0f + 0.5f;
    workingMem->store(taskId, processedValue);
    std::cout << "Processed value stored: " << processedValue << std::endl;
    
    // 5. Check memory capacity
    size_t capacity = workingMem->getCapacity();
    size_t currentSize = workingMem->getCurrentSize();
    std::cout << "Memory usage: " << currentSize << "/" << capacity << std::endl;
    
    // 6. Decay simulation (automatic in real brains)
    workingMem->decay(0.01f);  // 1% decay per step
    std::cout << "After decay - value: " << workingMem->retrieve(taskId) << std::endl;
    
    // 7. Clear all working memory
    workingMem->clear();
    std::cout << "Working memory cleared" << std::endl;
}
```

### Working Memory in Agent Processing

```cpp
void workingMemoryInAgent() {
    // Create agent with working memory
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", 150, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(&brain);
    
    // Simulate agent processing with working memory
    for (int step = 0; step < 50; ++step) {
        // Get sensory input
        nlm::SensoryInput percept;
        percept.vision = generateRandomVision();
        percept.touch = generateRandomTouch();
        
        // Store current perception in working memory
        if (agent.getWorkingMemory()) {
            agent.getWorkingMemory()->store(NeuronId(step), percept.energy);
        }
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain thinks
        brain.step(step);
        
        // Use working memory for decision making
        if (agent.getWorkingMemory() && step > 10) {
            float recentInput = agent.getWorkingMemory()->retrieve(NeuronId(step - 5));
            
            // Make decision based on recent working memory
            if (recentInput > 0.7f) {
                // High energy input - be more cautious
                auto action = generateSafeAction();
            } else {
                // Low energy input - be more exploratory
                auto action = generateExploratoryAction();
            }
        }
        
        brain.step(step);
    }
}
```

## Episodic Memory Tutorial

### What is Episodic Memory?

Episodic memory stores personal experiences and events with temporal and spatial context. It's what allows you to remember:
- What you had for breakfast
- Where you parked your car
- Who you talked to yesterday
- What you did last weekend

### Storing and Retrieving Episodes

```cpp
#include "nlm/memory/NeuralEpisodicMemory.hpp"

void episodicMemoryExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 1000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto episodicMem = brain.getEpisodicMemory();
    if (!episodicMem) {
        std::cerr << "Episodic memory not available!" << std::endl;
        return;
    }
    
    // 1. Create an episodic memory item
    nlm::EpisodicMemoryItem experience1;
    experience1.timestamp = 0;
    experience1.metadata = "Visited laboratory - Room A"
    
    // Store neural pattern
    experience1.neurons.push_back(100);
    experience1.values.push_back(0.8f);
    experience1.neurons.push_back(101);
    experience1.values.push_back(0.6f);
    experience1.neurons.push_back(102);
    experience1.values.push_back(0.9f);
    
    // Store the episode
    episodicMem->storeEpisode(experience1);
    std::cout << "Stored episode: " << experience1.metadata << std::endl;
    
    // 2. Create another episode
    nlm::EpisodicMemoryItem experience2;
    experience2.timestamp = 1;
    experience2.metadata = "Met colleague - Dr. Smith"
    
    experience2.neurons.push_back(200);
    experience2.values.push_back(0.7f);
    experience2.neurons.push_back(201);
    experience2.values.push_back(0.5f);
    
    episodicMem->storeEpisode(experience2);
    
    // 3. Retrieve specific episodes
    auto episode1 = episodicMem->retrieveEpisode(0);
    std::cout << "Retrieved: " << episode1.metadata << std::endl;
    std::cout << "Pattern: [";
    for (float val : episode1.values) {
        std::cout << val << " ";
    }
    std::cout << "]" << std::endl;
    
    // 4. Get recent episodes
    auto recentEpisodes = episodicMem->getRecentEpisodes(3);
    std::cout << "Recent episodes:" << std::endl;
    for (size_t i = 0; i < recentEpisodes.size(); ++i) {
        std::cout << "  " << i << ": " << recentEpisodes[i].metadata << std::endl;
    }
    
    // 5. Check memory statistics
    size_t episodeCount = episodicMem->getEpisodeCount();
    std::cout << "Total episodes stored: " << episodeCount << std::endl;
    
    // 6. Consolidate important memories
    episodicMem->consolidate(0.6f);  // Keep episodes with relevance > 60%
    std::cout << "Memory consolidation completed" << std::endl;
    
    // 7. Clear all episodes
    episodicMem->clear();
    std::cout << "All episodes cleared" << std::endl;
}
```

### Episodic Memory in Agent Learning

```cpp
void episodicMemoryAgentLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 2000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(&brain);
    agent.enableCuriosity(true);
    
    // Simulate an agent learning from experiences
    std::vector<std::string> locations = {"Kitchen", "LivingRoom", "Lab", "Office"};
    
    for (int episode = 0; episode < 20; ++episode) {
        // Create a new episode
        nlm::EpisodicMemoryItem episodeData;
        episodeData.timestamp = episode;
        
        // Navigate to random location
        std::string currentLocation = locations[episode % locations.size()];
        episodeData.metadata = "Agent visited " + currentLocation;
        
        // Record neural activation pattern
        for (int i = 0; i < 5; ++i) {
            episodeData.neurons.push_back(static_cast<NeuronId>(100 + i * episode));
            episodeData.values.push_back(static_cast<float>(rand()) / RAND_MAX);
        }
        
        // Store the experience
        auto episodicMem = brain.getEpisodicMemory();
        if (episodicMem) {
            episodicMem->storeEpisode(episodeData);
        }
        
        // Agent operates in the world
        simulateWorldInteraction(agent, episode);
        
        // Agent learns from the experience
        float reward = calculateReward(episode);
        agent.applyRewardModulation(reward, 0.0f);
        
        // Occasionally review past experiences
        if (episode % 5 == 0 && episodicMem) {
            auto recent = episodicMem->getRecentEpisodes(3);
            reviewRecentExperiences(recent);
        }
    }
    
    // Use stored experiences for better decision making
    applyLessonsFromExperience(brain);
}
```

## Semantic Memory Tutorial

### What is Semantic Memory?

Semantic memory contains facts, concepts, and general knowledge about the world. It's the "dictionary" and "encyclopedia" of your brain:

- Word meanings
- Facts and figures
- Concepts and categories
- Procedural knowledge

### Fact Storage and Retrieval

```cpp
#include "nlm/memory/SemanticMemory.hpp"

void semanticMemoryExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("semantic_memory_capacity", 5000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto semanticMem = brain.getSemanticMemory();
    if (!semanticMem) {
        std::cerr << "Semantic memory not available!" << std::endl;
        return;
    }
    
    // 1. Store facts
    semanticMem->storeFact("color_red", "#FF0000");
    semanticMem->storeFact("color_blue", "#0000FF");
    semanticMem->storeFact("gravity_acceleration", "9.8 m/s²");
    semanticMem->storeFact("water_boiling_point", "100°C");
    
    std::cout << "Stored facts:" << std::endl;
    auto allFacts = semanticMem->getAllFacts();
    for (const auto& fact : allFacts) {
        std::cout << "  " << fact.first << " = " << fact.second << std::endl;
    }
    
    // 2. Retrieve facts
    std::string redColor = semanticMem->retrieveFact("color_red");
    std::cout << "Red color code: " << redColor << std::endl;
    
    // 3. Check if fact exists
    if (semanticMem->hasFact("gravity_acceleration")) {
        std::cout << "Gravity fact is available" << std::endl;
    }
    
    // 4. Update facts
    semanticMem->storeFact("color_green", "#00FF00");
    std::string greenColor = semanticMem->retrieveFact("color_green");
    std::cout << "Green color code: " << greenColor << std::endl;
    
    // 5. Clear semantic memory
    semanticMem->clear();
    std::cout << "Semantic memory cleared" << std::endl;
}
```

### Semantic Memory in Cognitive Agents

```cpp
void semanticMemoryAgentExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Default);
    config->set("semantic_memory_capacity", 10000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto semanticMem = brain.getSemanticMemory();
    
    // Load common knowledge
    loadCommonKnowledge(semanticMem);
    
    // Create agent that uses semantic memory
    nlm::AgentBrain agent(&brain);
    agent.enableCuriosity(true);
    
    for (int step = 0; step < 100; ++step) {
        // Get sensory input
        nlm::SensoryInput percept = getCurrentPercept();
        
        // Process with semantic context
        agent.processSensoryInput(percept);
        brain.step(step);
        
        // Use semantic memory for interpretation
        std::string interpretation = interpretWithSemanticMemory(
            percept, semanticMem
        );
        
        // Make decision based on semantic knowledge
        auto action = makeDecision(interpretation);
        
        // Learn new semantic facts from experience
        learnNewSemanticFacts(experience, semanticMem);
        
        brain.step(step);
    }
    
    // Get all learned knowledge
    if (semanticMem) {
        auto allKnowledge = semanticMem->getAllFacts();
        std::cout << "Agent learned " << allKnowledge.size() << " facts" << std::endl;
    }
}

void loadCommonKnowledge(std::shared_ptr<nlm::SemanticMemory> semanticMem) {
    if (!semanticMem) return;
    
    // Basic physics facts
    semanticMem->storeFact("object_fall_speed", "9.8 m/s per second");
    semanticMem->storeFact("light_speed", "299,792,458 m/s");
    
    // Color facts
    semanticMem->storeFact("primary_colors", "Red, Blue, Yellow");
    semanticMem->storeFact("secondary_colors", "Green, Orange, Purple");
    
    // Animal facts
    semanticMem->storeFact("mammal_breathe_air", "true");
    semanticMem->storeFact("bird_fly", "true");
    semanticMem->storeFact("fish_swim", "true");
    
    // Basic math
    semanticMem->storeFact("pi_approximation", "3.14159");
    semanticMem->storeFact("eulers_number", "2.71828");
}
```

## Procedural Memory Tutorial

### What is Procedural Memory?

Procedural memory stores skills, habits, and motor patterns. It's what allows you to:

- Ride a bike
- Type on a keyboard
- Drive a car
- Tie your shoes

Without conscious thought!

### Skill Storage and Execution

```cpp
#include "nlm/memory/ProceduralMemory.hpp"

void proceduralMemoryExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("procedural_memory_capacity", 100, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto proceduralMem = brain.getProceduralMemory();
    if (!proceduralMem) {
        std::cerr << "Procedural memory not available!" << std::endl;
        return;
    }
    
    // 1. Learn a simple skill
    std::vector<NeuronId> reachingPattern;
    reachingPattern.push_back(100);
    reachingPattern.push_back(101);
    reachingPattern.push_back(102);
    reachingPattern.push_back(103);
    
    proceduralMem->learnSkill("reach_right", reachingPattern);
    std::cout << "Learned skill: reach_right" << std::endl;
    
    // 2. Another skill
    std::vector<NeuronId> walkingPattern;
    for (int i = 0; i < 5; ++i) {
        walkingPattern.push_back(static_cast<NeuronId>(200 + i));
    }
    
    proceduralMem->learnSkill("walk_forward", walkingPattern);
    
    // 3. Get skill information
    nlm::Skill* reachingSkill = proceduralMem->getSkill("reach_right");
    if (reachingSkill) {
        std::cout << "Skill: " << reachingSkill->name << std::endl;
        std::cout << "Pattern size: " << reachingSkill->neuralPattern.size() << std::endl;
        std::cout << "Proficiency: " << reachingSkill->proficiency << std::endl;
    }
    
    // 4. Update proficiency
    proceduralMem->updateProficiency("reach_right", 0.1f);  // Improve by 10%
    
    // 5. Get all skills
    auto allSkills = proceduralMem->getAllSkills();
    std::cout << "All skills:" << std::endl;
    for (auto* skill : allSkills) {
        if (skill) {
            std::cout << "  " << skill->name << " (proficiency: " 
                     << skill->proficiency << ")" << std::endl;
        }
    }
    
    // 6. Clear procedural memory
    proceduralMem->clear();
    std::cout << "Procedural memory cleared" << std::endl;
}
```

### Procedural Memory in Motor Skills

```cpp
void proceduralMemoryMotorExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("procedural_memory_capacity", 200, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto proceduralMem = brain.getProceduralMemory();
    
    // Learn motor skills
    learnDrivingSkill(proceduralMem);
    learnTypingSkill(proceduralMem);
    learnSportsSkill(proceduralMem);
    
    // Create agent
    nlm::AgentBrain agent(&brain);
    
    for (int step = 0; step < 200; ++step) {
        // Get current motor command
        auto action = agent.decodeMotorCommand();
        
        // Execute motor pattern based on action
        executeMotorPattern(action, proceduralMem);
        
        // Improve skills through practice
        improveSkillsThroughPractice(action, proceduralMem);
        
        brain.step(step);
    }
    
    // Show learned motor patterns
    displayMotorPatterns(proceduralMem);
}

void learnDrivingSkill(std::shared_ptr<nlm::ProceduralMemory> proceduralMem) {
    if (!proceduralMem) return;
    
    // Steering pattern
    std::vector<NeuronId> steeringPattern;
    steeringPattern.push_back(300);
    steeringPattern.push_back(301);
    steeringPattern.push_back(302);
    
    proceduralMem->learnSkill("steer_right", steeringPattern);
    
    // Accelerator pattern
    std::vector<NeuronId> accelPattern;
    accelPattern.push_back(303);
    accelPattern.push_back(304);
    accelPattern.push_back(305);
    
    proceduralMem->learnSkill("accelerate", accelPattern);
    
    // Brake pattern
    std::vector<NeuronId> brakePattern;
    brakePattern.push_back(306);
    brakePattern.push_back(307);
    brakePattern.push_back(308);
    
    proceduralMem->learnSkill("brake", brakePattern);
}

void executeMotorPattern(const nlm::MotorCommand& action, 
                        std::shared_ptr<nlm::ProceduralMemory> proceduralMem) {
    if (!proceduralMem) return;
    
    nlm::Skill* skill = proceduralMem->getSkill(getSkillForAction(action));
    if (skill) {
        // Execute the learned neural pattern
        executeNeuralPattern(skill->neuralPattern);
        
        // Update proficiency based on performance
        float performance = measurePerformance(action);
        proceduralMem->updateProficiency(skill->name, performance * 0.01f);
    }
}
```

## Associative Memory Tutorial

### What is Associative Memory?

Associative memory creates links between related concepts, items, or experiences. It's how the brain:

- Connects related ideas
- Makes predictions
- Recognizes patterns
- Generalizes from experience

### Creating and Using Associations

```cpp
#include "nlm/memory/AssociativeMemory.hpp"

void associativeMemoryExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("associative_memory_capacity", 500, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto associativeMem = brain.getAssociativeMemory();
    if (!associativeMem) {
        std::cerr << "Associative memory not available!" << std::endl;
        return;
    }
    
    // 1. Create associations
    associativeMem->associate(100, 200, 0.8f);  // Strong association
    associativeMem->associate(101, 201, 0.6f);  // Medium association
    associativeMem->associate(102, 202, 0.3f);  // Weak association
    
    std::cout << "Created associations:" << std::endl;
    
    // 2. Get associated neurons
    std::vector<NeuronId> assoc100 = associativeMem->getAssociations(100);
    std::cout << "Neurons associated with 100: ";
    for (NeuronId id : assoc100) {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    
    // 3. Get association strength
    float strength = associativeMem->getAssociationStrength(100, 200);
    std::cout << "Association strength between 100 and 200: " << strength << std::endl;
    
    // 4. Update associations
    associativeMem->updateAssociation(100, 200, 0.1f);  // Strengthen
    strength = associativeMem->getAssociationStrength(100, 200);
    std::cout << "After strengthening: " << strength << std::endl;
    
    // 5. Create complex associations
    // A -> B -> C chain
    associativeMem->associate(1, 2, 0.9f);  // Strong A-B
    associativeMem->associate(2, 3, 0.8f);  // Strong B-C
    
    // Find transitive associations
    std::vector<NeuronId> path = findAssociationChain(1, 3, associativeMem);
    std::cout << "Association chain from 1 to 3: ";
    for (NeuronId id : path) {
        std::cout << id << " ";
    }
    std::cout << std::endl;
    
    // 6. Clear associative memory
    associativeMem->clear();
    std::cout << "Associative memory cleared" << std::endl;
}
```

### Associative Memory in Pattern Recognition

```cpp
void associativeMemoryPatternRecognition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("associative_memory_capacity", 1000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto associativeMem = brain.getAssociativeMemory();
    
    // Build a pattern database
    buildPatternDatabase(associativeMem);
    
    // Create agent for pattern recognition
    nlm::AgentBrain agent(&brain);
    
    for (int step = 0; step < 100; ++step) {
        // Get sensory input
        nlm::SensoryInput percept = getPercept();
        
        // Extract features
        std::vector<NeuronId> features = extractFeatures(percept);
        
        // Find similar patterns using associative memory
        std::vector<NeuronId> similar = findSimilarPatterns(features, associativeMem);
        
        // Make prediction based on associations
        NeuronId predictedAction = predictAction(similar, associativeMem);
        
        // Store new association
        if (!features.empty() && predictedAction != -1) {
            associativeMem->associate(features[0], predictedAction, 0.7f);
        }
        
        // Process and execute action
        agent.processSensoryInput(percept);
        brain.step(step);
    }
    
    // Show learned associations
    displayAssociativePatterns(associativeMem);
}

void buildPatternDatabase(std::shared_ptr<nlm::AssociativeMemory> associativeMem) {
    if (!associativeMem) return;
    
    // Build basic associations
    // Visual pattern associations
    associativeMem->associate(10, 20, 0.9f);  // Bright -> approach
    associativeMem->associate(10, 21, 0.3f);  // Bright -> avoid
    
    // Movement associations
    associativeMem->associate(30, 40, 0.8f);  // Reach -> grasp
    associativeMem->associate(40, 41, 0.7f);  // Grasp -> hold
    
    // Social associations
    associativeMem->associate(50, 60, 0.9f);  // Smile -> friendly
    associativeMem->associate(61, 62, 0.8f);  // Friendly -> approach
    
    // Survival associations
    associativeMem->associate(70, 80, 0.95f); // Fire -> danger
    associativeMem->associate(81, 82, 0.9f);  // Danger -> flee
}
```

## Integrating Memory Systems

### How Memory Systems Work Together

```cpp
void integratedMemoryExample() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("working_memory_capacity", 200, nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 1000, nlm::ConfigSource::Default);
    config->set("semantic_memory_capacity", 5000, nlm::ConfigSource::Default);
    config->set("associative_memory_capacity", 2000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(&brain);
    agent.enableCuriosity(true);
    
    // Integration example: learning to navigate a maze
    for (int episode = 0; episode < 50; ++episode) {
        // Reset for new episode
        resetForNewEpisode(agent, brain);
        
        // Run episode
        bool episodeCompleted = runMazeEpisode(agent, brain, episode);
        
        if (episodeCompleted) {
            // Learn from the episode
            learnFromEpisode(brain, episode);
            
            // Store in episodic memory
            storeEpisodeInMemory(brain, episode);
            
            // Extract semantic knowledge
            extractSemanticKnowledge(brain, episode);
            
            // Create associations
            createMemoryAssociations(brain, episode);
        }
        
        brain.step(episode);
    }
    
    // Apply integrated learning
    applyIntegratedLearning(brain);
}

void learnFromEpisode(nlm::Brain& brain, int episode) {
    // Use working memory to analyze current episode
    auto workingMem = brain.getWorkingMemory();
    if (workingMem) {
        // Extract key decisions
        std::vector<float> decisions = extractDecisions(episode);
        
        // Store in working memory for analysis
        for (size_t i = 0; i < decisions.size(); ++i) {
            workingMem->store(static_cast<NeuronId>(i), decisions[i]);
        }
        
        // Analyze patterns
        analyzeWorkingMemoryPatterns(workingMem);
    }
    
    // Learn from episodic memory
    auto episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        // Compare with past experiences
        auto pastExperiences = episodicMem->getRecentEpisodes(5);
        compareWithPastExperiences(pastExperiences, episode);
    }
}

void extractSemanticKnowledge(nlm::Brain& brain, int episode) {
    auto semanticMem = brain.getSemanticMemory();
    if (!semanticMem) return;
    
    // Extract general principles from episode
    std::vector<std::string> principles = extractGeneralPrinciples(episode);
    
    for (const auto& principle : principles) {
        // Store as semantic fact
        semanticMem->storeFact("maze_learning_" + std::to_string(episode), 
                             principle);
    }
}

void createMemoryAssociations(nlm::Brain& brain, int episode) {
    auto associativeMem = brain.getAssociativeMemory();
    if (!associativeMem) return;
    
    // Create associations between elements
    std::vector<NeuronId> episodeElements = getEpisodeElements(episode);
    
    for (size_t i = 0; i < episodeElements.size(); ++i) {
        for (size_t j = i + 1; j < episodeElements.size(); ++j) {
            // Create associations based on co-occurrence
            float strength = calculateAssociationStrength(
                episodeElements[i], episodeElements[j], episode
            );
            
            if (strength > 0.5f) {
                associativeMem->associate(episodeElements[i], 
                                         episodeElements[j], 
                                         strength);
            }
        }
    }
}

void applyIntegratedLearning(nlm::Brain& brain) {
    // Combine all memory systems for better performance
    auto workingMem = brain.getWorkingMemory();
    auto episodicMem = brain.getEpisodicMemory();
    auto semanticMem = brain.getSemanticMemory();
    auto associativeMem = brain.getAssociativeMemory();
    
    // Use semantic memory to guide working memory
    if (semanticMem && workingMem) {
        auto allFacts = semanticMem->getAllFacts();
        for (const auto& fact : allFacts) {
            // Apply semantic knowledge to current task
            applySemanticFact(fact, workingMem);
        }
    }
    
    // Use episodic memory to enrich understanding
    if (episodicMem) {
        auto recent = episodicMem->getRecentEpisodes(3);
        for (const auto& episode : recent) {
            // Learn from past experiences
            learnFromPastExperience(episode);
        }
    }
    
    // Use associative memory for prediction
    if (associativeMem) {
        // Find predicted patterns
        std::vector<NeuronId> predictions = makePredictions(associativeMem);
        
        // Store predictions in working memory
        for (NeuronId prediction : predictions) {
            if (workingMem) {
                workingMem->store(prediction, 0.5f);
            }
        }
    }
}
```

## Memory Performance Tips

### Optimizing Memory Usage

```cpp
void optimizeMemoryPerformance(nlm::Brain& brain) {
    auto workingMem = brain.getWorkingMemory();
    auto episodicMem = brain.getEpisodicMemory();
    auto semanticMem = brain.getSemanticMemory();
    auto associativeMem = brain.getAssociativeMemory();
    
    // Monitor memory usage
    if (workingMem) {
        size_t workingUsage = workingMem->getCurrentSize();
        size_t workingCapacity = workingMem->getCapacity();
        
        if (workingUsage > workingCapacity * 0.9f) {
            // Clear working memory if near capacity
            workingMem->clear();
        }
    }
    
    // Consolidate episodic memories periodically
    if (episodicMem) {
        size_t episodeCount = episodicMem->getEpisodeCount();
        if (episodeCount > 500) {
            episodicMem->consolidate(0.7f);  // Keep only relevant memories
        }
    }
    
    // Periodically review semantic memory
    if (semanticMem) {
        auto allFacts = semanticMem->getAllFacts();
        if (allFacts.size() > 10000) {
            // Remove outdated facts
            removeOutdatedFacts(semanticMem, 10000);
        }
    }
    
    // Optimize associative memory
    if (associativeMem) {
        // Remove weak associations
        removeWeakAssociations(associativeMem, 0.1f);
    }
}

void removeWeakAssociations(std::shared_ptr<nlm::AssociativeMemory> associativeMem, float threshold) {
    if (!associativeMem) return;
    
    // This is a simplified example - real implementation would need
    // to iterate through all associations to find and remove weak ones
    std::cout << "Removing associations with strength < " << threshold << std::endl;
    // associativeMem->removeWeakAssociations(threshold);
}
```

---

## Summary

### Key Points About Memory Systems

1. **Working Memory**: Temporary storage for active processing
2. **Episodic Memory**: Stores personal experiences and events
3. **Semantic Memory**: Contains facts, concepts, and general knowledge
4. **Procedural Memory**: Stores skills and motor patterns
5. **Associative Memory**: Creates connections between related items

### Integration Benefits

- **Working + Episodic**: Learn from past experiences
- **Episodic + Semantic**: Extract general knowledge from experiences
- **Semantic + Associative**: Connect concepts for better understanding
- **All Systems**: Create rich, integrated knowledge representation

### Common Pitfalls

1. **Memory Overflow**: Clear memory when near capacity
2. **Overfitting**: Balance between learning and forgetting
3. **Decay Management**: Proper decay rates for different memory types
4. **Integration Issues**: Ensure memory systems work together harmoniously

By understanding and effectively using NLM's memory systems, you can create sophisticated agents capable of rich learning, adaptation, and intelligent behavior.