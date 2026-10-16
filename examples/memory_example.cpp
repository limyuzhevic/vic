// memory_example.cpp
// Working with memory systems example
// Demonstrates NLM's working memory, episodic memory, and associative memory

#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

// MemoryDemonstrationAgent: Agent that demonstrates NLM's memory capabilities
class MemoryDemonstrationAgent {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    
    // Memory system demonstrations
    std::vector<float> workingMemory;
    std::vector<std::pair<std::vector<float>, float>> episodicMemory;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> associativeMemory;
    std::vector<float> conceptPrototypes;
    
    bool workingMemoryEnabled;
    bool episodicMemoryEnabled;
    bool associativeMemoryEnabled;
    bool conceptFormationEnabled;
    
public:
    MemoryDemonstrationAgent(std::shared_ptr<Brain> b) : brain(b), 
                                                          workingMemoryEnabled(false),
                                                          episodicMemoryEnabled(false),
                                                          associativeMemoryEnabled(false),
                                                          conceptFormationEnabled(false) {}
    
    void initialize(std::shared_ptr<SimpleWorld> w) {
        world = w;
        // Initialize memory systems
        setupMemorySystems();
        std::cout << "MemoryDemonstrationAgent initialized with memory systems" << std::endl;
    }
    
    void setupMemorySystems() {
        // Configure memory system parameters
        workingMemory.resize(100, 0.0f);  // 100 slots for working memory
        std::cout << "  Working memory: 100 slots established" << std::endl;
        
        episodicMemory.reserve(50);  // Store up to 50 episodes
        std::cout << "  Episodic memory: 50 episodes capacity established" << std::endl;
        
        associativeMemory.reserve(200);  // Store associations
        std::cout << "  Associative memory: 200 associations capacity established" << std::endl;
        
        conceptPrototypes.resize(50, 0.0f);  // 50 concept prototypes
        std::cout << "  Concept formation: 50 prototypes established" << std::endl;
    }
    
    void processSensoryInput(const std::vector<float>& percept) {
        brain->receiveSensoryInput(percept);
        
        // Update memory systems with new input
        updateMemorySystems(percept);
    }
    
    Action decodeMotorCommand() {
        return brain->produceAction();
    }
    
    void updateMemorySystems(const std::vector<float>& percept) {
        float timestamp = world->getSimulationTime();
        
        // Working memory: maintain current information
        if (workingMemoryEnabled) {
            updateWorkingMemory(percept, timestamp);
        }
        
        // Episodic memory: store complete experiences
        if (episodicMemoryEnabled) {
            encodeEpisodicMemory(percept, timestamp);
        }
        
        // Associative memory: form connections between patterns
        if (associativeMemoryEnabled) {
            formAssociations(percept, timestamp);
        }
        
        // Concept formation: discover abstract patterns
        if (conceptFormationEnabled) {
            updateConceptPrototypes(percept, timestamp);
        }
    }
    
    void enableWorkingMemory(bool enable) { workingMemoryEnabled = enable; }
    void enableEpisodicMemory(bool enable) { episodicMemoryEnabled = enable; }
    void enableAssociativeMemory(bool enable) { associativeMemoryEnabled = enable; }
    void enableConceptFormation(bool enable) { conceptFormationEnabled = enable; }
    
    void updateWorkingMemory(const std::vector<float>& percept, float timestamp) {
        // Simple working memory: store current percept
        workingMemory[0] = timestamp;  // Store timestamp
        
        for (size_t i = 0; i < std::min(percept.size(), workingMemory.size() - 1); ++i) {
            workingMemory[i + 1] = percept[i];  // Store percept values
        }
        
        // Competitive dynamics: weaker entries decay
        for (float& entry : workingMemory) {
            entry *= 0.95f;  // Decay
        }
        
        // Winner-take-more: strengthen the most recent entry
        if (!workingMemory.empty()) {
            workingMemory[0] = timestamp + 1.0f;  // Boost timestamp
        }
    }
    
    void encodeEpisodicMemory(const std::vector<float>& percept, float timestamp) {
        // Store complete episode with temporal and spatial context
        std::pair<std::vector<float>, float> episode(percept, timestamp);
        
        episodicMemory.push_back(episode);
        
        // Keep only recent episodes (simplified aging)
        if (episodicMemory.size() > 50) {
            episodicMemory.erase(episodicMemory.begin());
        }
        
        // Pattern completion demonstration
        if (episodicMemory.size() >= 2) {
            auto lastEpisode = episodicMemory.back();
            auto secondLastEpisode = episodicMemory[episodicMemory.size() - 2];
            
            float similarity = calculatePatternSimilarity(lastEpisode.first, secondLastEpisode.first);
            if (similarity > 0.7f) {
                std::cout << "  Episodic memory: Pattern completion detected (similarity: " 
                          << similarity << ")" << std::endl;
            }
        }
    }
    
    void formAssociations(const std::vector<float>& percept, float timestamp) {
        // Create associations between current percept and outcomes
        // Simplified: associate percept patterns with rewards
        
        // Check if we've seen this pattern before
        bool patternSeen = false;
        std::vector<float> associatedOutcome;
        
        for (const auto& association : associativeMemory) {
            float similarity = calculatePatternSimilarity(percept, association.first);
            if (similarity > 0.8f) {  // Strong association
                patternSeen = true;
                associatedOutcome = association.second;
                break;
            }
        }
        
        if (!patternSeen) {
            // Create new association (simplified reward)
            std::vector<float> reward(1, 1.0f);  // Unit reward for demonstration
            associativeMemory.push_back({percept, reward});
            
            if (associativeMemory.size() <= 5) {  // Log first few associations
                std::cout << "  Associative memory: Formed new association " 
                          << associativeMemory.size() << std::endl;
            }
        }
        
        // Use associations to guide behavior
        if (patternSeen && !associatedOutcome.empty()) {
            float rewardValue = associatedOutcome[0];
            if (rewardValue > 0.5f) {
                std::cout << "  Associative memory: Strong association predicts positive outcome" << std::endl;
            }
        }
    }
    
    void updateConceptPrototypes(const std::vector<float>& percept, float timestamp) {
        // Concept formation: cluster similar patterns into prototypes
        if (conceptPrototypes.empty()) {
            conceptPrototypes = percept;
            return;
        }
        
        // Find closest prototype (simplified centroid calculation)
        int closestPrototype = 0;
        float minDistance = calculatePatternDistance(percept, conceptPrototypes);
        
        for (size_t i = 1; i < conceptPrototypes.size(); ++i) {
            float distance = calculatePatternDistance(percept, conceptPrototypes);
            if (distance < minDistance) {
                minDistance = distance;
                closestPrototype = i;
            }
        }
        
        // Update prototype if close enough
        if (minDistance < 0.5f) {
            for (size_t i = 0; i < std::min(percept.size(), conceptPrototypes.size()); ++i) {
                conceptPrototypes[i] = conceptPrototypes[i] * 0.8f + percept[i] * 0.2f;
            }
            
            std::cout << "  Concept formation: Updated prototype " << closestPrototype 
                      << " (distance: " << minDistance << ")" << std::endl;
        } else {
            // Create new prototype
            if (conceptPrototypes.size() < conceptPrototypes.size()) {
                conceptPrototypes = percept;
                std::cout << "  Concept formation: Created new prototype " << conceptPrototypes.size() - 1 << std::endl;
            }
        }
    }
    
    float calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
        if (pattern1.size() != pattern2.size() || pattern1.empty()) return 0.0f;
        
        float dotProduct = 0.0f;
        float norm1 = 0.0f;
        float norm2 = 0.0f;
        
        for (size_t i = 0; i < pattern1.size(); ++i) {
            dotProduct += pattern1[i] * pattern2[i];
            norm1 += pattern1[i] * pattern1[i];
            norm2 += pattern2[i] * pattern2[i];
        }
        
        norm1 = std::sqrt(norm1);
        norm2 = std::sqrt(norm2);
        
        if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
        
        return dotProduct / (norm1 * norm2);
    }
    
    float calculatePatternDistance(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
        return 1.0f - calculatePatternSimilarity(pattern1, pattern2);
    }
    
    void printMemoryStatus() const {
        std::cout << "\nMemory System Status:" << std::endl;
        std::cout << "  Working memory: " << workingMemory.size() << " slots used" << std::endl;
        std::cout << "  Episodic memory: " << episodicMemory.size() << " episodes stored" << std::endl;
        std::cout << "  Associative memory: " << associativeMemory.size() << " associations formed" << std::endl;
        std::cout << "  Concept prototypes: " << conceptPrototypes.size() << " prototypes" << std::endl;
    }
};

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Memory Systems Example ===" << std::endl;
    
    // Create configuration optimized for memory demonstration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 77777, ConfigSource::Default);
    config->set("neuron_count", 1500, ConfigSource::Default);
    
    // Enable all memory systems for demonstration
    config->set("memory.working.enabled", True, ConfigSource::File);
    config->set("memory.episodic.enabled", True, ConfigSource::File);
    config->set("memory.associative.enabled", True, ConfigSource::File);
    config->set("memory.concept.enabled", True, ConfigSource::File);
    
    // Enable plasticity for learning in memory systems
    config->set("plasticity.stdp.enable", True, ConfigSource::File);
    config->set("plasticity.hebbian.enable", True, ConfigSource::File);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    std::cout << "Creating memory demonstration brain with " << brain->getTotalNeuronCount() 
              << " neurons..." << std::endl;
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create and configure world for memory demonstration
    auto world = std::make_shared<SimpleWorld>();
    world->configure(
        width(25,
        height: 25
    );
    world->reset();
    world->setAgentStart(12.5, 12.5);  // Center start
    
    // Add memory-relevant objects
    std::vector<std::pair<float, float>> objectPositions = {
        {5.0, 5.0}, {20.0, 5.0}, {5.0, 20.0}, {20.0, 20.0},
        {10.0, 10.0}, {15.0, 15.0}, {7.5, 7.5}, {17.5, 17.5}
    };
    
    for (size_t i = 0; i < objectPositions.size(); ++i) {
        // Different object types to demonstrate memory discrimination
        auto objType = static_cast<pynlm.WorldObjectType>(i % 3);
        if (objType == pynlm.WorldObjectType.Empty) objType = pynlm.WorldObjectType.Resource;
        
        float value = (i % 3 == 0) ? 10.0f : 5.0f;  // Different values
        
        world->addObject(pynlm.WorldObject(
            objectPositions[i].first,
            objectPositions[i].second,
            objType,
            value
        ));
    }
    
    std::cout << "World configured for memory demonstration: 25x25 cells with 16x16 vision" << std::endl;
    std::cout << "Objects placed: Resources, Hazards, and Markers for memory discrimination" << std::endl;
    
    // Create memory demonstration agent
    auto agent = std::make_shared<MemoryDemonstrationAgent>(brain);
    agent->initialize(world);
    
    // Enable memory systems for demonstration
    agent->enableWorkingMemory(true);
    agent->enableEpisodicMemory(true);
    agent->enableAssociativeMemory(true);
    agent->enableConceptFormation(true);
    
    // Run memory demonstration simulation
    std::cout << "\nStarting memory systems demonstration..." << std::endl;
    std::cout << "Step | Firing | Working | Episodic | Associative | Concepts | Action" << std::endl;
    std::cout << "-----|--------|---------|----------|-------------|----------|-------" << std::endl;
    
    for (SimulationStep step = 0; step < 2500; ++step) {
        // Update world
        world->update(0.05);  // 50ms timesteps
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain->step(step);
        
        // Get action
        Action action = agent->decodeMotorCommand();
        
        // Execute action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Print memory status every 250 steps
        if (step % 250 == 0) {
            agent->printMemoryStatus();
            std::cout << step << " | " << brain->getFiringNeuronCount() << " | "
                      << agent->getWorkingMemory()[0] << " | "
                      << agent->getEpisodicMemory().size() << " | "
                      << agent->getAssociativeMemory().size() << " | "
                      << agent->getConceptPrototypes().size() << " | "
                      << action.toString() << std::endl;
        }
        
        // Check for memory system completion
        if (agent->getAssociativeMemory().size() >= 10 && 
            agent->getConceptPrototypes().size() >= 5 && 
            step > 1000) {
            std::cout << "\nMemory systems demonstration completed!" << std::endl;
            break;
        }
    }
    
    std::cout << "\n=== Memory Systems Demonstration Complete ===" << std::endl;
    
    agent->printMemoryStatus();
    
    std::cout << "\nMemory Systems Summary:" << std::endl;
    std::cout << "  Working memory: Stores current information with competitive dynamics" << std::endl;
    std::cout << "  Episodic memory: Stores complete experiences with pattern completion" << std::endl;
    std::cout << "  Associative memory: Forms connections between related patterns" << std::endl;
    std::cout << "  Concept formation: Discovers abstract patterns and prototypes" << std::endl;
    
    std::cout << "\nMemory system demonstrations observed:" << std::endl;
    std::cout << "  - Working memory: Selective retention of current information" << std::endl;
    std::cout << "  - Episodic memory: Encoding and retrieval of experiences" << std::endl;
    std::cout << "  - Associative memory: Pattern association and generalization" << std::endl;
    std donc:  auto agent = std::make_shared<MemoryDemonstrationAgent>(brain);
    agent->initialize(world);

    // Enable memory systems for demonstration
    agent->enableWorkingMemory(true);
    agent->enableEpisodicMemory(true);
    agent->enableAssociativeMemory(true);
    agent->enableConceptFormation(true);

    // Run memory demonstration simulation
    std::cout << "\nStarting memory systems demonstration..." << std::endl;
    std::cout << "Step | Firing | Working | Episodic | Associative | Concepts | Action" << std::endl;
    std::cout << "-----|--------|---------|----------|-------------|----------|-------" << std::endl;

    for (SimulationStep step = 0; step < 2500; ++step) {
        // Update world
        world->update(0.05);  // 50ms timesteps

        // Get sensory input
        auto percept = world->getSensoryPercept();

        // Process sensory input
        agent->processSensoryInput(percept);

        // Brain computation
        brain->step(step);

        // Get action
        Action action = agent->decodeMotorCommand();

        // Execute action
        world->applyMotorCommand(action, world->getSimulationTime());

        // Print memory status every 250 steps
        if (step % 250 == 0) {
            agent->printMemoryStatus();
            std::cout << step << " | " << brain->getFiringNeuronCount() << " | "
                      << agent->getWorkingMemory()[0] << " | "
                      << agent->getEpisodicMemory().size() << " | "
                      << agent->getAssociativeMemory().size() << " | "
                      << agent->getConceptPrototypes().size() << " | "
                      << action.toString() << std::endl;
        }

        // Check for memory system completion
        if (agent->getAssociativeMemory().size() >= 10 && 
            agent->getConceptPrototypes().size() >= 5 && 
            step > 1000) {
            std::cout << "\nMemory systems demonstration completed!" << std::endl;
            break;
        }
    }

    std::cout << "\n=== Memory Systems Demonstration Complete ===" << std::endl;

    agent->printMemoryStatus();

    std::cout << "\nMemory Systems Summary:" << std::endl;
    std::cout << "  Working memory: Stores current information with competitive dynamics" << std::endl;
    std::cout << "  Episodic memory: Stores complete experiences with pattern completion" << std::endl;
    std::cout << "  Associative memory: Forms connections between related patterns" << std::endl;
    std::cout << "  Concept formation: Discovers abstract patterns and prototypes" << std::endl;

    std::cout << "\nMemory system demonstrations observed:" << std::endl;
    std::cout << "  - Working memory: Selective retention of current information" << std::endl;
    std::cout << "  - Episodic memory: Encoding and retrieval of experiences" << std::endl;
    std::cout << "  - Associative memory: Pattern association and generalization" << std::endl;
    std::cout << "  - Concept formation: Pattern discovery and abstraction" << std::endl;

    return 0;
}
