#pragma once

namespace nlm {

/**
 * @file BrainDevelopment.cpp
 * @brief Implementation of brain development systems
 * 
 * This file contains the implementation of development systems that enable
 * the brain to mature and adapt its structure and function over time:
 * - DevelopmentSystem: Controls brain maturation and developmental stages
 * - DevelopmentalStage: Enumeration of brain developmental phases
 * 
 * Development systems regulate structural and functional changes that occur
 * throughout the brain's lifecycle, from initial formation to adult stability.
 */

#include "Brain.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>

/**
 * @brief Development system implementation for the Brain class
 * 
 * This section contains methods and helper functions that manage the brain's
 * development systems. Development mechanisms provide:
 * - Regulation of developmental stages
 * - Control of plasticity during critical periods
 * - Management of brain maturation processes
 * - Adaptation of connectivity based on developmental needs
 */

// Development system getter implementations
DevelopmentSystem* Brain::getDevelopmentSystem() {
    if (pImpl->developmentSystem) {
        return pImpl->developmentSystem.get();
    }
    return nullptr;
}

/**
 * @brief Get current developmental stage
 * 
 * @return Current developmental stage
 */
DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

/**
 * @brief Set developmental stage
 * 
 * @param stage New developmental stage to set
 */
void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

/**
 * @brief Apply developmental effects
 * 
 * This function applies developmental effects to the brain systems,
 * including:
 * - Regulation of plasticity rates based on developmental stage
 * - Structural changes appropriate to the developmental phase
 * - Maturation of cognitive and memory systems
 * - Adaptation of neuromodulatory systems
 * 
 * @param rng Random number generator for stochastic developmental processes
 * @param timestep Current simulation timestep
 */
void Brain::applyDevelopmentalEffects(RandomGenerator& rng, TimestepDuration timestep) {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(this, rng, timestep);
    }
    
    // Apply developmental stage-specific plasticity modulation
    if (pImpl->structuralPlasticity) {
        float plasticityMod = 1.0f;
        
        switch (pImpl->developmentalStage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;  // Stable
                break;
        }
        
        // Update plasticity system parameters based on developmental stage
        float currentSynaptogenesisRate = pImpl->structuralPlasticity->getSynaptogenesisRate();
        float currentPruningRate = pImpl->structuralPlasticity->getPruningRate();
        
        pImpl->structuralPlasticity->setSynaptogenesisRate(currentSynaptogenesisRate * plasticityMod);
        pImpl->structuralPlasticity->setPruningRate(currentPruningRate * (2.0f - plasticityMod));
    }
}

/**
 * @brief Get developmental stage statistics
 * 
 * @return String containing developmental stage statistics
 */
std::string Brain::getDevelopmentStats() const {
    std::stringstream stats;
    stats << "Development Statistics:\n";
    stats << "  Current Stage: " << static_cast<int>(pImpl->developmentalStage) << "\n";
    
    switch (pImpl->developmentalStage) {
        case DevelopmentalStage::Initial:
            stats << "  Description: Initial formation and high plasticity\n";
            break;
        case DevelopmentalStage::CriticalPeriod:
            stats << "  Description: Critical period for learning and adaptation\n";
            break;
        case DevelopmentalStage::Maturation:
            stats << "  Description: Maturation and refinement of connections\n";
            break;
        case DevelopmentalStage::Adult:
            stats << "  Description: Adult stability and maintenance\n";
            break;
    }
    
    return stats.str();
}

} // namespace nlm
