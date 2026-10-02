// Brain Plasticity Manager - Handles all plasticity operations
// Separates plasticity coordination from the main Brain class

#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

/**
 * @brief Manages all plasticity operations including STDP, Hebbian, and structural plasticity
 * 
 * This class separates the plasticity coordination logic from the main Brain class,
 * making it more modular and testable. It handles:
 * - STDP (Spike-Timing-Dependent Plasticity) application
 * - Hebbian learning application
 * - Structural plasticity (synaptogenesis and pruning)
 * - Neuromodulation effects on plasticity
 * - Weight change application with bounds checking
 * 
 * @note This is a key part of the refactoring effort to reduce Brain.cpp size
 * and improve code organization by using the manager pattern. It encapsulates
 * all plasticity-specific logic in a dedicated class.
 */
class BrainPlasticityManager {
public:
    /**
     * @brief Constructor
     * 
     * @param brain Shared pointer to the Brain instance to manage
     */
    explicit BrainPlasticityManager(std::shared_ptr<Brain> brain)
        : brain_(brain) {}
    
    /**
     * @brief Destructor
     */
    ~BrainPlasticityManager() = default;
    
    /**
     * @brief Apply all plasticity rules for the current simulation step
     * 
     * This method applies all plasticity rules (STDP, Hebbian, structural) to synapses
     * based on recent neural activity patterns and neuromodulation effects.
     * 
     * @param currentTime Current simulation time
     */
    void applyPlasticity(Timestamp currentTime);
    
    /**
     * @brief Get STDP system pointer
     * 
     * Returns a pointer to the STDP system managed by this class.
     * 
     * @return STDP* Pointer to the STDP system
     */
    STDP* getSTDP() const;
    
    /**
     * @brief Get Hebbian system pointer
     * 
     * Returns a pointer to the Hebbian system managed by this class.
     * 
     * @return Hebbian* Pointer to the Hebbian system
     */
    Hebbian* getHebbian() const;
    
    /**
     * @brief Get structural plasticity system pointer
     * 
     * Returns a pointer to the structural plasticity system managed by this class.
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity() const;
    
    /**
     * @brief Apply neuromodulation factor to plasticity
     * 
     * Applies neuromodulation effects (such as dopamine levels) to scale plasticity
     * rates and learning dynamics.
     */
    void applyNeuromodulationFactor();
    
private:
    /**
     * @brief Shared pointer to the Brain instance being managed
     */
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm
