// Brain cognition module - handles cognitive systems (planning, attention, concept formation)
// 
// This module provides functions for managing cognitive systems that are part of Phase 6.
// It handles neural planning, attentional selection, concept formation, and developmental
// processes that enable goal-directed behavior and learning.
#pragma once

#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainCognition
 * @brief Manages cognitive systems for goal-directed behavior and learning
 * 
 * The BrainCognition class provides functions for managing cognitive systems that are
 * part of Phase 6: Final Integration. It handles neural planning, attentional selection,
 * concept formation, and developmental processes that enable goal-directed behavior
 * and learning.
 * 
 * @details This class implements the cognitive systems required for the integrated
 * brain architecture:
 * - Neural planner: Plans actions based on goals and current state
 * - Attentional selection: Selects focus areas based on competitive dynamics
 * - Concept formation: Discovers and forms concepts from patterns
 * - Developmental processes: Modulates cognitive capabilities based on development
 * 
 * @note Cognitive systems are essential for intelligent behavior, enabling the brain
 * to plan actions, focus attention, form abstract concepts, and develop over time.
 */
class BrainCognition {
public:
    /**
     * @brief Update attention system
     * 
     * This method updates the attention system, selecting focus areas based on
     * competitive dynamics among working memory items and current cognitive goals.
     * Attention is fundamental for focused processing and resource allocation.
     * 
     * @param brain Pointer to Brain instance
     * @param timestep Time step size (seconds)
     * 
     * @pre brain must not be null, attention system must be initialized
     * @post Attention system updated with competitive selection
     */
    static void updateAttention(Brain* brain, TimestepDuration timestep);
    
    /**
     * @brief Process competition among working memory items
     * 
     * This method processes competition among working memory items, implementing
     * attentional selection based on activation levels and competitive dynamics.
     * The winning item receives attentional focus and resources.
     * 
     * @param brain Pointer to Brain instance
     * @param competitors Vector of neuron IDs in competition for attention
     * 
     * @pre brain must not be null, attention system must be initialized
     * @post Competition processed, attentional focus assigned
     */
    static void processAttentionCompetition(Brain* brain, const std::vector<NeuronId>& competitors);
    
    /**
     * @brief Get current attentional focus
     * 
     * This method returns the current attentional focus, typically the neuron ID
     * of the item that is currently receiving attention. This determines what
     * information is being processed and prioritized.
     * 
     * @param brain Pointer to Brain instance
     * @return Neuron ID of currently attended item (INVALID_NEURON_ID if none)
     * 
     * @pre brain must not be null
     * @post Attentional focus returned
     */
    static NeuronId getCurrentAttentionalFocus(Brain* brain);
    
    /**
     * @brief Update concept formation system
     * 
     * This method updates the concept formation system, processing current neural
     * activity patterns to discover and form abstract concepts. Concept formation
     * enables generalization from specific experiences to abstract knowledge.
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null, concept formation system must be initialized
     * @post Concept formation system updated with current patterns
     */
    static void updateConceptFormation(Brain* brain);
    
    /**
     * @brief Get current developmental stage
     * 
     * This method returns the current developmental stage of the brain, which
     * modulates cognitive capabilities, learning rates, and system plasticity.
     * Developmental stages include Initial, CriticalPeriod, Maturation, Adult, and Aging.
     * 
     * @param brain Pointer to Brain instance
     * @return Current developmental stage
     * 
     * @pre brain must not be null
     * @post Developmental stage returned
     */
    static DevelopmentalStage getDevelopmentalStage(Brain* brain);
    
    /**
     * @brief Update developmental effects
     * 
     * This method applies developmental effects based on the current developmental
     * stage, modulating cognitive capabilities, learning rates, and system properties.
     * Development affects plasticity, maturation, and system optimization.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * @param timestep Time step size (seconds)
     * 
     * @pre brain must not be null, development system must be initialized
     * @post Developmental effects applied
     */
    static void updateDevelopment(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    
    /**
     * @brief Get planning depth
     * 
     * This method returns the planning depth of the neural planner, which determines
     * how many steps ahead the brain can plan actions. Higher planning depth enables
     * more sophisticated goal-directed behavior.
     * 
     * @param brain Pointer to Brain instance
     * @return Planning depth (number of steps ahead)
     * 
     * @pre brain must not be null, planning system must be initialized
     * @post Planning depth returned
     */
    static size_t getPlanningDepth(Brain* brain);
    
    /**
     * @brief Set planning depth
     * 
     * This method sets the planning depth of the neural planner, configuring how
     * many steps ahead the brain can plan actions. Planning depth affects computational
     * complexity and planning horizon.
     * 
     * @param brain Pointer to Brain instance
     * @param depth Planning depth (number of steps ahead)
     * 
     * @pre brain must not be null, planning system must be initialized
     * @post Planning depth set
     */
    static void setPlanningDepth(Brain* brain, size_t depth);
};

} // namespace nlm