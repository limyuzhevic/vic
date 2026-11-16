#pragma once

/**
 * Phase6TextDemo - Phase 6 Text Association Demo
 * 
 * This demo demonstrates Phase 6's text-as-a-sense capability, where words
 * become sensory patterns that can be learned through association with
 * visual scenes, just like how real language learning works.
 * 
 * The demo shows:
 * - Text encoding as sparse sensory patterns
 * - Association learning between words and scenes
 * - Concept formation from text-scene pairs
 * - Self-model development for text understanding
 * - Social learning through observation
 * - Planning with text concepts
 * 
 * This demo highlights Phase 6's advanced capabilities:
 * - Text as a direct sensory modality (not just symbolic processing)
 * - Emergent concept formation from language exposure
 * - Integration of text with perception, action, and memory
 * - Learning from teacher demonstrations
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

/**
 * Text Association Configuration
 */
struct TextAssociationConfig {
    // Text learning parameters
    size_t maxTextSamples = 100;
    size_t maxConceptCount = 20;
    size_t minContextSize = 10;
    
    // World configuration
    float worldWidth = 16.0f;
    float worldHeight = 16.0f;
    size_t visionWidth = 8;
    size_t visionHeight = 8;
    
    // Demo parameters
    size_t maxSteps = 2000;
    size_t teacherEpisodes = 50;
    size_t studentEpisodes = 100;
    
    // Feature toggles
    bool enableSelfModel = true;
    bool enableSocialLearning = true;
    bool enablePlannerOverrides = true;
    bool enableConceptBias = true;
    
    // Text content
    std::vector<std::string> teacherCommands = {
        "forward", "left", "right", "interact", "look", "wait"
    };
    
    TextAssociationConfig() = default;
};

/**
 * Text Association Result
 */
struct TextAssociationResult {
    // Text learning metrics
    size_t wordsLearned;
    size_t conceptsFormed;
    size_t associationsEstablished;
    
    // Self-model development
    float selfModelConfidence;
    size_t selfConceptCount;
    
    // Social learning
    size_t observationsMade;
    size_t imitationsSuccessful;
    
    // Planning effectiveness
    size_t plannerOverrides;
    size_t plannerAgreements;
    
    // Performance metrics
    float conceptStability;
    float associationStrength;
    float generalizationAbility;
    
    // Timing metrics
    double totalWallClockTime;
    
    TextAssociationResult() = default;
};

/**
 * Phase 6 Text Association Demo
 * 
 * Demonstrates how Phase 6 brains can learn language through exposure,
 * forming concepts and associations between words and their meanings.
 */
class Phase6TextDemo {
public:
    Phase6TextDemo();
    ~Phase6TextDemo();
    
    /**
     * Run the text association demo
     */
    bool run(const TextAssociationConfig& config);
    
    /**
     * Get the demo result
     */
    const TextAssociationResult& getResult() const { return result_; }
    
    /**
     * Print demo summary
     */
    void printSummary() const;
    
    /**
     * Check if demo completed successfully
     */
    bool success() const;
    
private:
    TextAssociationResult result_;
    std::unique_ptr<Brain> brain_;
    std::unique_ptr<AgentBrain> agent_;
    std::unique_ptr<SimpleWorld> world_;
    
    /**
     * Initialize demo with configuration
     */
    bool initialize(const TextAssociationConfig& config);
    
    /**
     * Run teacher phase (demonstrate commands)
     */
    void runTeacherPhase(const TextAssociationConfig& config);
    
    /**
     * Run student phase (learn from observation)
     */
    void runStudentPhase(const TextAssociationConfig& config);
    
    /**
     * Create text pattern for a given word
     */
    std::vector<size_t> createTextPattern(const std::string& word) const;
    
    /**
     * Inject text pattern into brain
     */
    void injectTextPattern(const std::vector<size_t>& pattern) const;
    
    /**
     * Get teacher command with context
     */
    std::string getTeacherCommand(const std::string& word, size_t context) const;
    
    /**
     * Process agent's thought trace for learning
     */
    void processThoughtTrace(const std::string& thoughtTrace);
    
    /**
     * Print demo banner
     */
    void printBanner() const;
    
    /**
     * Print configuration summary
     */
    void printConfiguration(const TextAssociationConfig& config) const;
    
    /**
     * Print learning progress
     */
    void printLearningProgress(size_t steps) const;
};

} // namespace nlm
