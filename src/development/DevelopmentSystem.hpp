#pragma once

#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

/**
 * @class DevelopmentSystem
 * @brief Orchestrates neural development and plasticity modulation
 * @details The DevelopmentSystem manages the developmental stages of the neural
 * network, implementing critical periods, plasticity modulation, and structural
 * changes over time. It provides the framework for lifelong learning and
 * adaptation, enabling the network to change its computational properties
 * based on developmental age and environmental experience.
 * 
 * Developmental Stages:
 * - Sensitive periods: Times of high plasticity for learning
 * - Critical periods: Windows for specific skill acquisition
 * - Juvenile plasticity: General learning phase
 * - Adult stability: Focused exploitation of learned skills
 * - Aging decline: Reduced plasticity and capacity
 * 
 * Development Mechanisms:
 * - Stage progression: Forward movement through developmental phases
 * - Plasticity modulation: Age-dependent synaptic modification rates
 * - Structural changes: Rewiring and new connection formation
 * - Resource allocation: Distribution of limited plasticity resources
 * 
 * Thread Safety: Thread-safe for concurrent access. Development updates are
 * synchronized with neural simulation and should not conflict with learning.
 * 
 * Performance: O(1) for state queries, O(1) for stage updates.
 * Memory usage is constant (~16 bytes per instance).
 * 
 * Plasticity Dynamics:
 * - Critical period: High plasticity (~1.0)
 * - Juvenile phase: Moderate plasticity (0.5)
 * - Adult phase: Low plasticity (0.1-0.3)
 * - Effect on STDP: Multiplies STDP learning rates
 * - Effect on Hebbian: Multiplies Hebbian learning rates
 * 
 * @code
 * // Initialize development system
 * DevelopmentSystem devSys;
 * 
 * // Start at birth
 * devSys.setStage(DevelopmentalStage::Juvenile);
 * 
 * // Simulate development over time
 * void simulateDevelopment(DevelopmentSystem& devSys, Brain* brain, 
 *                         RandomGenerator& rng, double timestep) {
 *     // Track developmental age
 *     double age = devSys.getDevelopmentalAge();
 *     
 *     // Advance stage when ready
 *     if (age >= devSys.getStageDuration()) {
 *         devSys.advanceStage();
 *     }
 *     
 *     // Update neural plasticity based on stage
 *     float plasticityModifier = devSys.getPlasticityModifier();
 *     
 *     // Apply to learning systems
 *     // STDP learning rates = base_rate * plasticityModifier
 *     // Hebbian learning rates = base_rate * plasticityModifier
 *     
 *     // Critical period enables specific learning
 *     if (devSys.isCriticalPeriod()) {
 *         // Specialized learning opportunities
 *         enableSkillLearning(brain, rng);
 *     }
 * }
 * 
 * // Development checkpoints
 * void developmentCheckpoints() {
 *     if (devSys.getStage() == DevelopmentalStage::Juvenile) {
 *         // Foundation learning phase
 *     } else if (devSys.getStage() == DevelopmentalStage::Adult) {
 *         // Specialization and skill mastery
 *     }
 * }
 * @endcode
 * 
 * Developmental Theory:
 * - Embodied development: Development interacts with environmental interaction
 * - Experience-expectant: Critical periods for optimal development
 * - Experience-dependent: Specific experiences shape neural architecture
 * - Adaptive plasticity: Dynamic adjustment of learning rates
 * 
 * Applications:
 * - Developmental robotics: Growing neural networks
 * - Lifespan learning: Continuous adaptation throughout life
 * - Educational AI: Optimal timing for teaching
 * - Rehabilitation: Critical period-based recovery
 * 
 * @see DevelopmentalStage
 * @see Brain
 * @see RandomGenerator
 * @see SimulationClock
 * @see STDP
 * @see Hebbian
 */
class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    void advanceStage();
    
    // Get stage name
    const char* getStageName() const;
    
    // Update development (simple version)
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update development with full parameters
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
