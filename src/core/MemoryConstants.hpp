// Memory Parameters
// Working memory, episodic memory, and attention constants

#ifndef SRC_CORE_MEMORY_CONSTANTS_HPP
#define SRC_CORE_MEMORY_CONSTANTS_HPP

namespace MemoryConstants {
    // Working memory
    static constexpr float WORKING_MEMORY_COMPETITION_THRESHOLD = 0.3f;     // Competition threshold
    static constexpr float WORKING_MEMORY_DECAY = 0.95f;                   // Working memory decay factor
    static constexpr float WORKING_MEMORY_UPDATE_THRESHOLD = 0.5f;         // Update threshold
    
    // Episodic memory
    static constexpr float EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;   // Consolidation threshold
    static constexpr float EPISODIC_MEMORY_MAX_SIMILARITY = 0.3f;            // Maximum similarity threshold
    static constexpr size_t EPISODIC_MEMORY_MAX_EPISODES = 1000;            // Maximum episodes
    
    // Prediction and surprise
    static constexpr float SURPRISE_THRESHOLD = 0.3f;                       // Surprise threshold
    static constexpr float PREDICTION_ERROR_THRESHOLD = 0.1f;                // Prediction error threshold
    
    // Attention
    static constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;           // Attention inhibition
    static constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;            // Attention excitation
    
    // Neuromodulation
    static constexpr float NOVELTY_DECAY_RATE = 0.05f;                      // Novelty decay rate
    static constexpr float NOVELTY_THRESHOLD = 0.3f;                        // Novelty threshold
    static constexpr float CURIOSITY_DECAY_RATE = 0.1f;                      // Curiosity decay rate
    
    // Exploration
    static constexpr float EXPLORATION_THRESHOLD = 0.3f;                     // Exploration threshold
    
    // Episodic memory scoring
    static constexpr float EPISODIC_MEMORY_SCORE_THRESHOLD = 0.3f;          // Score threshold
    static constexpr float EPISODIC_MEMORY_SIMILARITY_THRESHOLD = 0.3f;     // Similarity threshold
}

#endif // SRC_CORE_MEMORY_CONSTANTS_HPP