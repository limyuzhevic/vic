#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// NeuralAssociativeMemory: Forms associations between patterns and experiences
// NOT just simple lookup - uses neural dynamics and Hebbian learning
//
// Key mechanisms:
// - Temporal associations (pattern A followed by pattern B)
// - Contextual associations (pattern -> context -> strength)
// - Hebbian learning between neural patterns
// - Novelty-triggered association formation
// - Pattern completion through association chains

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Set connections to other memory systems
    void setWorkingMemory(class NeuralWorkingMemory* wm);
    void setEpisodicMemory(class NeuralEpisodicMemory* em);
    
    // Create association between two patterns
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength);
    
    // Create associations based on working memory content
    void createFromWorkingMemory(const std::vector<float>& wmPattern,
                                 float novelty);
    
    // Create associations based on episodic memory
    void createFromEpisodicMemory(const class NeuralEpisodicMemory* episode);
    
    // Create contextual association (pattern depends on context)
    void createContextualAssociation(const std::vector<float>& pattern,
                                    const std::vector<float>& context,
                                    float strength);
    
    // Create Hebbian connections between neural patterns
    void createHebbianConnections(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB,
                                 float strength);
    
    // Get association strength between patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;
    
    // Retrieve associated pattern for given input
    std::vector<float> retrieveAssociatedPattern(const std::vector<float>& pattern,
                                                  size_t maxResults) const;
    
    // Update associations from working memory changes
    void updateFromWorkingMemory(const std::vector<float>& newPattern,
                                 const std::vector<float>& oldPattern);
    
    // Gradually strengthen associations from episodic memory
    void updateFromEpisodicMemory();
    
    // Find matching neurons from pattern (for synaptic connections)
    std::vector<NeuronId> findNeuronsFromPattern(const std::vector<float>& pattern) const;
    
    // Extract pattern from episodic memory episode
    std::vector<float> extractPatternFromEpisode(const class NeuralEpisodicMemory::Episode* episode) const;
    
    // Get context vector for episode
    std::vector<float> getEpisodeContext(const class NeuralEpisodicMemory::Episode* episode) const;
    
    // SelfModel (continued from NeuralPlanner.hpp)
    struct SelfModel {
        SelfModel();
        ~SelfModel();
        
        // Initialize with brain reference
        void initialize(Brain* brain);
        
        // Record that taking an action caused a specific sensory change
        void recordSelfAction(ActionType action,
                             const std::vector<float>& beforeState,
                             const std::vector<float>& afterState);
        
        // Predict sensory consequence of an action
        // This is the "forward model" of the self
        std::vector<float> predictActionConsequence(ActionType action,
                                                    const std::vector<float>& currentState);
        
        // Get confidence in self-model for a given action
        float getSelfModelConfidence(ActionType action) const;
        
        // Is this change likely caused by self (action) vs external?
        float computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                           const std::vector<float>& afterState,
                                           ActionType action) const;
        
        // Get the body schema (preferred actions in different states)
        ActionType getPreferredAction(const std::vector<float>& state);
        
        // Update self-model based on prediction error
        void updateSelfModel(const std::vector<float>& predicted,
                            const std::vector<float>& actual,
                            ActionType action);
        
        // Get current capability level (0-1)
        float getCapabilityLevel() const { return capabilityLevel_; }
        
        // Get body awareness (how accurately can predict consequences)
        float getBodyAwareness() const;
        
        // Clear self-model
        void clear();
        
        // Has self-model formed?
        bool hasSelfModel() const { return !actionEffects_.empty(); }
        
    private:
        // Find best matching previous experience
        std::vector<float> findMatchingEffect(ActionType action,
                                              const std::vector<float>& beforeState) const;
        
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        Brain* brain_;
        float capabilityLevel_;
        
        // Action -> (beforeState -> afterState) mappings
        struct ActionEffect {
            std::vector<float> beforeState;
            std::vector<float> afterState;
            float confidence;
            size_t observationCount;
        };
        
        std::vector<std::vector<ActionEffect>> actionEffects_;  // Indexed by ActionType
    };
    
    // SocialLearning (continued from NeuralPlanner.hpp)
    struct SocialLearning {
        SocialLearning();
        ~SocialLearning();
        
        // Initialize with brain reference
        void initialize(Brain* brain);
        
        // Record observation of another agent's action and effect
        void observeAgentAction(ActionType observedAction,
                               const std::vector<float>& observerState,
                               const std::vector<float>& resultingState);
        
        // Can I imitate this observed action?
        bool canImitate(ActionType observedAction) const;
        
        // Get the best action to imitate given current state
        ActionType getImitationAction(const std::vector<float>& currentState);
        
        // Learn simple communication signal from another agent
        void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                     float signalReward);
        
        // Detect if another agent is signaling
        bool detectSignal(const std::vector<float>& neuralPattern) const;
        
        // Get learned signal pattern
        std::vector<float> getSignalPattern() const;
        
        // Get signal meaning (associated reward)
        float getSignalMeaning() const;
        
        // Update social knowledge based on interactions
        void updateSocialKnowledge(float interactionReward);
        
        // Clear social learning
        void clear();
        
        // Has learned from others?
        bool hasSocialKnowledge() const { return observationCount_ > 0; }
        
        // Get observation count
        size_t getObservationCount() const { return observationCount_; }
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        Brain* brain_;
        size_t observationCount_;
        
        // Observed action effects (other agent's actions)
        struct ObservedEffect {
            std::vector<float> state;
            std::vector<float> resultingState;
            float reward;
        };
        
        std::vector<std::pair<ActionType, ObservedEffect>> observedActions_;
        
        // Communication signal
        std::vector<float> signalPattern_;
        float signalMeaning_;
        
        float computeSimilarity(const std::vector<float>& a,
                            const std::vector<float>& b) const;
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    class NeuralWorkingMemory* workingMemory_;
    class NeuralEpisodicMemory* episodicMemory_;
    
    float computeSimilarity(const std::vector<float>& a,
                        const std::vector<float>& b) const;
    
    std::vector<float> interpolateVectors(const std::vector<float>& a,
                                          const std::vector<float>& b,
                                          float weight) const;
};

} // namespace nlm
