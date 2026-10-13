// NLM Advanced User Examples
// ==========================
// 
// This file demonstrates advanced usage of the NLM brain system,
// showcasing the integration of working memory, episodic memory,
// associative memory, neuromodulation, and prediction systems.
// 
// These examples are designed for:
// - Researchers studying memory systems
// - Developers extending NLM functionality
// - Students learning about brain-inspired architectures
// - Engineers implementing NLM in real applications

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

// NLM Core Systems
#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/agent/AgentBrain.hpp"

namespace nlm_examples {

// ============================================================================
// Advanced Working Memory Example
// ============================================================================
class WorkingMemoryAdvancedExample {
public:
    WorkingMemoryAdvancedExample() : brain_(nullptr), workingMemory_(nullptr) {}
    
    ~WorkingMemoryAdvancedExample() {
        if (brain_) {
            delete brain_;
        }
    }
    
    void demonstratePatternMemory() {
        std::cout << "\n=== Advanced Working Memory Example ===" << std::endl;
        
        // Initialize brain with memory capacity
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000);
        config->set("simulation_timestep", 0.001);
        
        brain_ = new Brain(config);
        brain_->initialize();
        
        workingMemory_ = brain_->getWorkingMemory();
        workingMemory_->setCapacity(50);  // Limit memory traces
        workingMemory_->setDecayRate(0.005f);  // 0.5% decay per timestep
        
        std::cout << "Working memory initialized with capacity: " 
                  << workingMemory_->getCapacity() << std::endl;
        
        // Demonstrate 1: Pattern Storage and Retrieval
        demonstratePatternStorage();
        
        // Demonstrate 2: Competition Dynamics
        demonstrateCompetitionDynamics();
        
        // Demonstrate 3: Memory Rehearsal and Strengthening
        demonstrateMemoryRehearsal();
        
        // Demonstrate 4: Memory Interference and Resolution
        demonstrateMemoryInterference();
    }
    
private:
    void demonstratePatternStorage() {
        std::cout << "\n1. Pattern Storage and Retrieval:" << std::endl;
        
        // Store multiple patterns with different characteristics
        std::vector<std::pair<std::string, std::vector<float>>> patterns = {
            {"SensoryPattern", {0.1f, 0.3f, 0.7f, 0.9f, 0.2f}},
            {"MotorPattern", {0.8f, 0.4f, 0.6f, 0.3f, 0.1f}},
            {"InternalPattern", {0.5f, 0.5f, 0.5f, 0.5f, 0.5f}},
            {"GoalPattern", {0.9f, 0.8f, 0.7f, 0.6f, 0.5f}}
        };
        
        for (const auto& pattern : patterns) {
            workingMemory_->store(pattern.second, 1.0f);
            std::cout << "   Stored " << pattern.first << " with " 
                      << pattern.second.size() << " dimensions" << std::endl;
        }
        
        // Retrieve and analyze stored patterns
        auto retrieved = workingMemory_->retrieve();
        std::cout << "   Retrieved " << retrieved.size() << " pattern dimensions" << std::endl;
        
        // Calculate pattern similarity
        float similarity = calculatePatternSimilarity(patterns[0].second, retrieved);
        std::cout << "   Pattern similarity after retrieval: " << similarity << std::endl;
    }
    
    void demonstrateCompetitionDynamics() {
        std::cout << "\n2. Competition Dynamics and Selective Attention:" << std::endl;
        
        // Create patterns with varying strengths
        std::vector<std::vector<float>> competitionPatterns = {
            {0.9f, 0.1f, 0.1f, 0.1f, 0.1f},  // Strong pattern (should win)
            {0.2f, 0.8f, 0.2f, 0.2f, 0.2f},  // Medium pattern
            {0.1f, 0.1f, 0.9f, 0.1f, 0.1f},  // Weak pattern (should lose)
            {0.1f, 0.1f, 0.1f, 0.9f, 0.1f}   // Medium pattern
        };
        
        for (size_t i = 0; i < competitionPatterns.size(); ++i) {
            workingMemory_->store(competitionPatterns[i], 1.0f);
        }
        
        // Run competition to select winners
        workingMemory_->runCompetition();
        
        // Check which neurons won
        std::cout << "   Competition completed, analyzing winners:" << std::endl;
        for (size_t i = 0; i < competitionPatterns.size(); ++i) {
            // Find corresponding neuron (simplified for example)
            float activation = workingMemory_->getNeuronActivation(
                NeuronId(1000 + i));
            bool isWinner = workingMemory_->isWinning(NeuronId(1000 + i));
            
            std::cout << "   Neuron " << (1000 + i) << ": activation=" 
                      << activation << ", winner=" << (isWinner ? "YES" : "NO")
                      << std::endl;
        }
        
        std::cout << "   Winner determines which pattern dominates attention" << std::endl;
    }
    
    void demonstrateMemoryRehearsal() {
        std::cout << "\n3. Memory Rehearsal and Strengthening:" << std::endl;
        
        // Store a pattern
        std::vector<float> originalPattern = {0.3f, 0.6f, 0.9f, 0.4f, 0.2f};
        workingMemory_->store(originalPattern, 1.0f);
        
        float initialStrength = workingMemory_->getMemoryActivity();
        std::cout << "   Initial memory strength: " << initialStrength << std::endl;
        
        // Rehearsal: repeatedly activate the pattern to strengthen it
        for (int rehearsalStep = 0; rehearsalStep < 10; ++rehearsalStep) {
            workingMemory_->strengthenMemory(1.1f); // 10% increase per rehearsal
            float currentStrength = workingMemory_->getMemoryActivity();
            std::cout << "   Rehearsal " << (rehearsalStep + 1) << ": strength=" 
                      << currentStrength << std::endl;
        }
        
        std::cout << "   Rehearsal demonstrates memory consolidation" << std::endl;
    }
    
    void demonstrateMemoryInterference() {
        std::cout << "\n4. Memory Interference and Resolution:" << std::endl;
        
        // Store similar patterns that interfere with each other
        workingMemory_->clear();
        
        std::vector<std::vector<float>> similarPatterns = {
            {0.5f, 0.6f, 0.7f, 0.8f, 0.9f},  // Base pattern
            {0.52f, 0.62f, 0.72f, 0.82f, 0.92f},  // Similar (high interference)
            {0.3f, 0.4f, 0.5f, 0.6f, 0.7f}   // Different (low interference)
        };
        
        for (const auto& pattern : similarPatterns) {
            workingMemory_->store(pattern, 1.0f);
        }
        
        // Update to allow decay and interference resolution
        for (int step = 0; step < 100; ++step) {
            workingMemory_->update(TimestepDuration(0.001f));
            
            if (step % 50 == 0) {
                float interferenceLevel = calculateInterferenceLevel();
                std::cout << "   Step " << step << ": interference level=" 
                          << interferenceLevel << std::endl;
            }
        }
        
        std::cout << "   Interference resolution shows how similar patterns compete" << std::endl;
    }
    
    float calculatePatternSimilarity(const std::vector<float>& pattern1, 
                                    const std::vector<float>& pattern2) {
        if (pattern1.size() != pattern2.size()) return 0.0f;
        
        float sumSquaredDiff = 0.0f;
        for (size_t i = 0; i < pattern1.size(); ++i) {
            float diff = pattern1[i] - pattern2[i];
            sumSquaredDiff += diff * diff;
        }
        
        return 1.0f - std::sqrt(sumSquaredDiff / pattern1.size());
    }
    
    float calculateInterferenceLevel() {
        // Simple interference calculation based on memory activity
        auto traces = workingMemory_->retrieve();
        float maxTrace = 0.0f;
        for (float trace : traces) {
            if (trace > maxTrace) maxTrace = trace;
        }
        return maxTrace;
    }
};

// ============================================================================
// Episodic Memory Advanced Example
// ============================================================================
class EpisodicMemoryAdvancedExample {
public:
    EpisodicMemoryAdvancedExample() : brain_(nullptr), episodicMemory_(nullptr) {}
    
    ~EpisodicMemoryAdvancedExample() {
        if (brain_) {
            delete brain_;
        }
    }
    
    void demonstrateExperienceReplay() {
        std::cout << "\n=== Advanced Episodic Memory Example ===" << std::endl;
        
        // Initialize brain with episodic memory capacity
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000);
        
        brain_ = new Brain(config);
        brain_->initialize();
        
        episodicMemory_ = brain_->getEpisodicMemory();
        episodicMemory_->setMaxEpisodes(100);
        episodicMemory_->setReplayEnabled(true);
        
        std::cout << "Episodic memory initialized with max episodes: " 
                  << episodicMemory_->getMaxEpisodes() << std::endl;
        
        // Demonstrate 1: Experience Encoding
        demonstrateExperienceEncoding();
        
        // Demonstrate 2: Temporal Retrieval
        demonstrateTemporalRetrieval();
        
        // Demonstrate 3: Similarity-Based Retrieval
        demonstrateSimilarityRetrieval();
        
        // Demonstrate 4: Replay Mechanisms
        demonstrateReplayMechanisms();
    }
    
private:
    void demonstrateExperienceEncoding() {
        std::cout << "\n1. Experience Encoding with Context:" << std::endl;
        
        // Create rich episodic memories with temporal, spatial, and causal context
        std::vector<EpisodicMemoryItem> experiences = {
            // Experience 1: Finding food
            createExperience(
                "food_find",  // action
                100,          // temporal context
                {3, 5},       // spatial context
                {0.7f, 0.9f}, // sensory state
                {0.8f, 0.6f}  // motor state
            ),
            
            // Experience 2: Avoiding predator
            createExperience(
                "predator_avoid",  // action
                200,               // temporal context
                {8, 2},            // spatial context
                {0.2f, 0.4f},      // sensory state
                {0.1f, 0.9f}       // motor state
            ),
            
            // Experience 3: Social interaction
            createExperience(
                "social_interaction",  // action
                150,                   // temporal context
                {5, 7},                // spatial context
                {0.6f, 0.8f},          // sensory state
                {0.7f, 0.5f}           // motor state
            )
        };
        
        for (const auto& experience : experiences) {
            episodicMemory_->storeEpisode(experience);
            std::cout << "   Stored experience: " << experience.action << " at time " 
                      << experience.temporalContext << std::endl;
        }
        
        std::cout << "   Total episodes stored: " << episodicMemory_->getEpisodeCount() << std::endl;
    }
    
    void demonstrateTemporalRetrieval() {
        std::cout << "\n2. Temporal Order Memory and Retrieval:" << std::endl;
        
        // Retrieve episodes in temporal order
        auto allEpisodes = episodicMemory_->getAllEpisodes();
        std::sort(allEpisodes.begin(), allEpisodes.end(),
                  [](const EpisodicMemoryItem& a, const EpisodicMemoryItem& b) {
                      return a.temporalContext < b.temporalContext;
                  });
        
        std::cout << "   Temporal retrieval results:" << std::endl;
        for (const auto& episode : allEpisodes) {
            std::cout << "   Time " << episode.temporalContext << ": " 
                      << episode.action << std::endl;
        }
        
        // Demonstrate retrieval by temporal proximity
        std::vector<EpisodicMemoryItem> recent = 
            episodicMemory_->retrieveByTemporalRange(100, 250);
        std::cout << "   Recent experiences (100-250 time units): " 
                  << recent.size() << " episodes" << std::endl;
    }
    
    void demonstrateSimilarityRetrieval() {
        std::cout << "\n3. Similarity-Based Memory Retrieval:" << std::endl;
        
        // Create a query based on current sensory state
        std::vector<float> currentSensoryState = {0.7f, 0.8f, 0.3f};
        
        // Find similar past experiences
        auto similarExperiences = episodicMemory_->retrieveSimilar(currentSensoryState);
        
        std::cout << "   Current sensory state: " << vectorToString(currentSensoryState) << std::endl;
        std::cout << "   Found " << similarExperiences.size() << " similar past experiences:" << std::endl;
        
        for (size_t i = 0; i < std::min(size_t(3), similarExperiences.size()); ++i) {
            const auto& experience = similarExperiences[i];
            float similarity = calculateSensorySimilarity(currentSensoryState, 
                                                         experience.sensoryState);
            std::cout << "   Similarity " << similarity << ": " << experience.action 
                      << " at time " << experience.temporalContext << std::endl;
        }
    }
    
    void demonstrateReplayMechanisms() {
        std::cout << "\n4. Memory Replay and Consolidation:" << std::endl;
        
        // Initialize replay system
        episodicMemory_->enableReplay(true);
        
        // Run some neural steps to generate activity patterns
        for (int step = 0; step < 50; ++step) {
            // Simulate neural activity
            std::vector<float> currentActivity = generateRandomActivity();
            
            // Store as episode during replay
            EpisodicMemoryItem replayEpisode;
            replayEpisode.temporalContext = step + 300;  // Later time
            replayEpisode.action = "replay_step_" + std::to_string(step);
            replayEpisode.sensoryState = currentActivity;
            
            episodicMemory_->storeEpisode(replayEpisode);
        }
        
        // Check if replay can retrieve episodes
        auto replayed = episodicMemory_->getEpisodesForReplay(3);
        std::cout << "   Replay completed, retrieved " << replayed.size() << "/3 requested episodes" << std::endl;
        
        // Demonstrate replay consolidation
        episodicMemory_->consolidateRecentEpisodes(50);  // Consolidate last 50 steps
        std::cout <<   "   Replay consolidation completed" << std::endl;
    }
    
    EpisodicMemoryItem createExperience(const std::string& action,
                                        int temporalContext,
                                        const std::pair<int, int>& spatialContext,
                                        const std::vector<float>& sensoryState,
                                        const std::vector<float>& motorState) {
        EpisodicMemoryItem item;
        item.action = action;
        item.temporalContext = temporalContext;
        item.spatialContext.x = spatialContext.first;
        item.spatialContext.y = spatialContext.second;
        item.sensoryState = sensoryState;
        item.motorState = motorState;
        item.age = 0;
        return item;
    }
    
    float calculateSensorySimilarity(const std::vector<float>& state1,
                                    const std::vector<float>& state2) {
        if (state1.size() != state2.size()) return 0.0f;
        
        float sumSquaredDiff = 0.0f;
        for (size_t i = 0; i < state1.size(); ++i) {
            float diff = state1[i] - state2[i];
            sumSquaredDiff += diff * diff;
        }
        
        return 1.0f - std::sqrt(sumSquaredDiff / state1.size());
    }
    
    std::string vectorToString(const std::vector<float>& vec) {
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::to_string(vec[i]);
        }
        result += "]";
        return result;
    }
    
    std::vector<float> generateRandomActivity() {
        // Generate random neural activity for demonstration
        std::vector<float> activity(3);
        for (int i = 0; i < 3; ++i) {
            activity[i] = static_cast<float>(rand()) / RAND_MAX * 0.8f + 0.1f;
        }
        return activity;
    }
};

// ============================================================================
// Integrated Memory and Neuromodulation Example
// ============================================================================
class IntegratedMemoryNeuromodulationExample {
public:
    IntegratedMemoryNeuromodulationExample() : brain_(nullptr), agent_(nullptr) {}
    
    ~IntegratedMemoryNeuromodulationExample() {
        if (agent_) {
            delete agent_;
        }
        if (brain_) {
            delete brain_;
        }
    }
    
    void demonstrateIntegratedMemoryWithNeuromodulation() {
        std::cout << "\n=== Integrated Memory and Neuromodulation Example ===" << std::endl;
        
        // Create brain with full memory and neuromodulation systems
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 3000);
        config->set("enable_checkpointing", true);
        
        brain_ = new Brain(config);
        brain_->initialize();
        
        // Create agent with brain integration
        agent_ = new AgentBrain(brain_);
        SimpleWorld world;
        world.initialize(16, 16);
        agent_->initialize(world);
        
        // Enable advanced neuromodulation
        agent_->enableRewardModulation(true);
        agent_->enableStructuralPlasticity(true);
        agent_->enableDevelopment(true);
        agent_->enableCuriosity(true);
        
        std::cout << "Integrated brain and agent initialized" << std::endl;
        
        // Demonstrate 1: Working Memory with Dopamine Modulation
        demonstrateWorkingMemoryWithDopamine();
        
        // Demonstrate 2: Episodic Memory with Curiosity
        demonstrateEpisodicMemoryWithCuriosity();
        
        // Demonstrate 3: Memory Integration with Attention
        demonstrateMemoryWithAttention();
        
        // Demonstrate 4: Complete Integration Example
        demonstrateCompleteIntegration();
    }
    
private:
    void demonstrateWorkingMemoryWithDopamine() {
        std::cout << "\n1. Working Memory Modulated by Dopamine:" << std::endl;
        
        auto* workingMemory = brain_->getWorkingMemory();
        auto* dopamine = brain_->getDopamine();
        
        // Store initial pattern
        std::vector<float> pattern = {0.5f, 0.7f, 0.9f, 0.3f, 0.6f};
        workingMemory->store(pattern, 1.0f);
        
        // Set initial dopamine level
        dopamine->setLevel(0.5f);
        std::cout << "   Initial dopamine level: " << dopamine->getLevel() << std::endl;
        
        // Simulate reward and dopamine increase
        float reward = 1.0f;
        dopamine->receiveRewardSignal(reward);
        
        std::cout << "   After reward: dopamine level = " << dopamine->getLevel() << std::endl;
        std::cout <<   "   Higher dopamine strengthens memory encoding" << std::endl;
        
        // Update working memory with dopamine modulation
        workingMemory->update(TimestepDuration(0.001f));
        workingMemory->strengthenMemory(1.2f);  // 20% increase due to dopamine
        
        std::cout << "   Memory strengthened by dopamine modulation" << std::endl;
    }
    
    void demonstrateEpisodicMemoryWithCuriosity() {
        std::cout << "\n2. Episodic Memory Enhanced by Curiosity:" << std::endl;
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        auto* curiosity = brain_->getCuriosity();
        
        // Store an experience
        EpisodicMemoryItem episode;
        episode.action = "exploration_1";
        episode.temporalContext = 1000;
        episode.sensoryState = {0.3f, 0.7f, 0.5f};
        
        episodicMemory->storeEpisode(episode);
        
        // Set curiosity level
        curiosity->setLevel(0.8f);
        std::cout << "   Curiosity level: " << curiosity->getLevel() << std::endl;
        
        // Curiosity promotes novel exploration
        curiosity->triggerNovelExploration();
        
        std::cout <<   "   High curiosity drives exploration and memory formation" << std::endl;
        
        // Retrieve similar experiences based on curiosity
        std::vector<float> curiosityQuery = {0.4f, 0.8f, 0.6f};
        auto similar = episodicMemory->retrieveSimilar(curiosityQuery);
        
        std::cout << "   Curiosity-driven exploration found " << similar.size() << " similar experiences" << std::endl;
    }
    
    void demonstrateMemoryWithAttention() {
        std::cout << "\n3. Memory Integration with Attentional Selection:" << std::endl;
        
        auto* workingMemory = brain_->getWorkingMemory();
        auto* attention = brain_->getAttention();
        
        // Store multiple patterns competing for attention
        workingMemory->clear();
        
        std::vector<std::vector<float>> competingPatterns = {
            {0.1f, 0.9f, 0.2f},  // Pattern A - high relevance
            {0.8f, 0.3f, 0.7f},  // Pattern B - medium relevance
            {0.4f, 0.5f, 0.6f},  // Pattern C - low relevance
        };
        
        for (size_t i = 0; i < competingPatterns.size(); ++i) {
            workingMemory->store(competingPatterns[i], 1.0f);
        }
        
        // Configure attentional selection
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
        
        std::cout << "   Attentional selection configured: inhibition=0.5, excitation=1.5" << std::endl;
        
        // Run competition and attention
        workingMemory->runCompetition();
        attention->update(TimestepDuration(0.001f));
        
        std::cout <<   "   Attentional selection determines which memory traces receive resources" << std::endl;
    }
    
    void demonstrateCompleteIntegration() {
        std::cout << "\n4. Complete Memory-Neurosystem Integration:" << std::endl;
        
        SimpleWorld world;
        world.initialize(16, 16);
        
        // Create and configure agent
        AgentBrain agent(brain_);
        agent.initialize(world);
        agent.enableAllSystems(true);
        
        // Simulation loop demonstrating integrated memory and behavior
        std::cout <<   "   Starting integrated simulation..." << std::endl;
        std::cout <<   "   Simulation demonstrates how memory, neuromodulation, and action selection interact" << std::endl;
        
        // Example scenario: agent explores environment
        for (int step = 0; step < 20; ++step) {
            // Agent processes sensory input
            SensoryPercept percept = world.observe(nullptr);
            agent.processSensoryInput(percept);
            
            // Agent produces action based on memory and neuromodulation
            MotorCommand action = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(nullptr, action);
            
            // Compute reward and neuromodulation
            float reward = world.computeReward(nullptr);
            agent.applyRewardModulation(reward, 0.0f);
            
            // Update development and memory
            agent.updateDevelopment(0.001f);
            
            if (step % 5 == 0) {
                std::cout <<   "   Step " << step << ": Reward=" << reward 
                          << ", Novelty=" << agent.getNoveltyLevel() 
                          << ", Curiosity=" << agent.getCuriosityLevel() << std::endl;
            }
        }
        
        std::cout <<   "   Simulation complete - agent learned from experience through integrated memory systems" << std::endl;
    }
}; 

// ============================================================================
// Main Demonstration Function
// ============================================================================
void runAllAdvancedExamples() {
    std::cout << "=== NLM Advanced User Examples ===" << std::endl;
    std::cout << "Demonstrating integrated use of working memory, episodic memory, "
              << "neuromodulation, and cognitive systems." << std::endl;
    
    // Example 1: Working Memory Advanced Features
    {
        WorkingMemoryAdvancedExample wmExample;
        wmExample.demonstratePatternMemory();
    }
    
    // Example 2: Episodic Memory Advanced Features
    {
        EpisodicMemoryAdvancedExample emExample;
        emExample.demonstrateExperienceReplay();
    }
    
    // Example 3: Integrated Memory and Neuromodulation
    {
        IntegratedMemoryNeuromodulationExample integratedExample;
        integratedExample.demonstrateIntegratedMemoryWithNeuromodulation();
    }
    
    std::cout << "\n=== All Advanced Examples Completed Successfully ===" << std::endl;
    std::cout << "\nThese examples demonstrate the powerful integration of memory systems "
              << "with neural computation, neuromodulation, and cognitive functions "
              << "in the NLM brain architecture." << std::endl;
}

} // namespace nlm_examples

// ============================================================================
// Main Function
// ============================================================================
int main() {
    try {
        // Run all advanced examples
        nlm_examples::runAllAdvancedExamples();
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "ADVANCED EXAMPLES SUCCESSFULLY COMPLETED" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        std::cout << "\nKey Learnings:" << std::endl;
        std::cout << "• Working memory maintains and manipulates patterns through neural dynamics" << std::endl;
        std::cout << "• Episodic memory stores rich experiences with temporal/spatial context" << std::endl;
        std::cout << "• Neuromodulation (dopamine, curiosity) guides learning and exploration" << std::endl;
        std::cout << "• Attention selects which memory traces receive processing resources" << std::endl;
        std::cout << "• Integration enables goal-directed behavior based on past experience" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}