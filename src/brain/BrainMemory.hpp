// Brain memory module - handles integrated memory systems
// 
// This module provides functions for managing the integrated memory systems
// (working memory, episodic memory, associative memory) that are part of Phase 6.
// It handles memory storage, retrieval, replay, consolidation, and interaction
// with neural activity patterns.
#pragma once

#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainMemory
 * @brief Manages integrated memory systems for the NLM brain
 * 
 * The BrainMemory class provides functions for managing the integrated memory systems
 * that are part of Phase 6: Final Integration. It handles working memory (transient
 * active information), episodic memory (experience storage), and associative memory
 * (pattern associations), along with memory replay, consolidation, and interaction
 * with neural activity patterns.
 * 
 * @details This class implements the memory systems required for the integrated
 * brain architecture:
 * - Working memory: Maintains active neural traces and competitive dynamics
 * - Episodic memory: Stores experiences as episodes with active neurons and rewards
 * - Associative memory: Forms pattern associations between experiences
 * - Memory replay: Replays important episodes to strengthen memory traces
 * - Memory consolidation: Integrates memories into long-term storage
 * 
 * @note All memory functions are static and operate on Brain* pointers,
 * providing a clean interface for memory management without requiring
 * knowledge of internal memory system implementations.
 */
class BrainMemory {
public:
    /**
     * @brief Update working memory with current neural activity
     * 
     * This method updates the working memory system with current neural activity,
     * maintaining active traces and implementing competitive dynamics among
     * memory items.
     * 
     * @param brain Pointer to Brain instance
     * @param timestep Time step size (seconds)
     * 
     * @pre brain must not be null, working memory must be initialized
     * @post Working memory updated with current neural activity
     */
    static void updateWorkingMemory(Brain* brain, TimestepDuration timestep);
    
    /**
     * @brief Store neural firing to working memory
     * 
     * This method stores the firing of a neuron to working memory with an
     * activation level. Working memory maintains a limited number of active
     * neural traces representing current sensory and cognitive information.
     * 
     * @param brain Pointer to Brain instance
     * @param neuronId ID of neuron that fired
     * @param activationLevel Activation level (0-1, where 1.0 is maximum)
     * 
     * @pre brain must not be null, working memory must be initialized
     * @post Neural firing stored to working memory
     */
    static void storeToWorkingMemory(Brain* brain, NeuronId neuronId, float activationLevel);
    
    /**
     * @brief Update episodic memory with current brain state
     * 
     * This method captures the current brain state as an episode of experience,
     * including active neurons, their activation levels, and reward signals.
     * Episodes are stored in episodic memory for later replay and consolidation.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time (seconds)
     * 
     * @pre brain must not be null, episodic memory must be initialized
     * @post Current brain state captured as episode and stored
     */
    static void updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Store an episode to episodic memory
     * 
     * This method stores an episodic memory item to the episodic memory system
     * for later retrieval and replay.
     * 
     * @param brain Pointer to Brain instance
     * @param episode Episodic memory item to store
     * 
     * @pre brain must not be null, episodic memory must be initialized
     * @post Episode stored to episodic memory
     */
    static void storeEpisode(Brain* brain, const EpisodicMemoryItem& episode);
    
    /**
     * @brief Get episodes for replay
     * 
     * This method retrieves episodes from episodic memory for replay, typically
     * important experiences that should be reinforced through rehearsal.
     * 
     * @param brain Pointer to Brain instance
     * @param count Number of episodes to retrieve
     * @return Vector of episode pointers (caller must not delete)
     * 
     * @pre brain must not be null, episodic memory must be initialized
     * @post Episodes retrieved for replay
     * 
     * @note Returned pointers are to internal episodic memory storage and should
     * not be deleted by the caller.
     */
    static std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(Brain* brain, size_t count);
    
    /**
     * @brief Replay an episode
     * 
     * This method replays an episode from episodic memory, simulating the neural
     * activity patterns of the original experience to strengthen memory traces
     * and integrate the experience into the brain's knowledge base.
     * 
     * @param brain Pointer to Brain instance
     * @param episode Episode to replay (must be valid pointer from episodic memory)
     * 
     * @pre brain must not be null, episodic memory must be initialized
     * @post Episode replayed, neural patterns reactivated
     */
    static void replayEpisode(Brain* brain, const EpisodicMemoryItem* episode);
    
    /**
     * @brief Consolidate memories
     * 
     * This method consolidates episodic memories, removing weak traces and
     * strengthening strong ones for long-term storage. Consolidation is important
     * for memory efficiency and long-term retention.
     * 
     * @param brain Pointer to Brain instance
     * @param threshold Consolidation threshold (0-1, where 1.0 is maximum)
     * 
     * @pre brain must not be null, episodic memory must be initialized
     * @post Memories consolidated, weak traces removed
     */
    static void consolidate(Brain* brain, float threshold);
    
    /**
     * @brief Get active working memory traces
     * 
     * This method returns the number of active traces in working memory,
     * representing the current focus of attention and active cognitive processing.
     * 
     * @param brain Pointer to Brain instance
     * @return Number of active traces in working memory
     * 
     * @pre brain must not be null, working memory must be initialized
     * @post Active trace count returned
     */
    static size_t getActiveWorkingMemoryTraces(Brain* brain);
    
    /**
     * @brief Get current episode count
     * 
     * This method returns the number of episodes currently stored in episodic memory.
     * 
     * @param brain Pointer to Brain instance
     * @return Number of stored episodes
     * 
     * @pre brain must not be null, episodic memory must be initialized
     * @post Episode count returned
     */
    static size_t getEpisodeCount(Brain* brain);
};

} // namespace nlm