#pragma once

/**
 * @file DevelopmentSystem.hpp
 * @brief Developmental processes and maturation system
 * 
 * This header defines the DevelopmentSystem class for the brain's developmental
 * processes and maturation system. The development system orchestrates neural
 * development across multiple time scales, implementing developmental stages,
 * plasticity modulation, and structural changes that occur throughout the
 * lifespan of the agent.
 * 
 * Development Theory:
 * - Development progresses through distinct stages with different characteristics
 * - Neural plasticity decreases with age (critical periods)
 * - Synaptogenesis, pruning, and maturation shape neural architecture
 * - Developmental experience shapes functional specialization
 * - Age-appropriate capabilities guide system design and learning
 * 
 * Developmental Functions:
 * - Stage management: Controls progression through developmental stages
 * - Plasticity modulation: Adjusts learning rates based on developmental state
 * - Structural changes: Implements synaptic and neural development
 * - Experience integration: Shapes development based on environmental input
 * - Capability tuning: Adapts system parameters for age-appropriate behavior
 * 
 * Integration Points:
 * - Connected to all major systems (neuromodulation, memory, cognition)
 * - Modulates plasticity in STDP, Hebbian, and structural systems
 * - Influences reward processing and motivational systems
 * - Affects attention and working memory capacity
 * - Guides learning rates and developmental plasticity
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class DevelopmentSystem;
class RandomGenerator;

/**
 * @brief Developmental stage enumeration
 * 
 * Defines distinct developmental stages that the agent progresses through,
 * each with characteristic neural properties, learning capabilities, and
 * behavioral patterns. These stages guide development from initial formation
 * through maturity and into specialized adult capabilities.
 * 
 * DevelopmentalStage Values:
 * - Initial: Early formation stage, high plasticity, basic capabilities
 * - Infant: Rapid learning, exploration, foundational skill development
 * - Juvenile: Skill refinement, social learning, basic reasoning
 * - Adolescent: Advanced cognitive development, emotional regulation
 * - Adult: Mature capabilities, specialized knowledge, stable performance
 * - Senescent: Age-related changes, reduced plasticity, maintenance
 * 
 * Stage Properties:
 * - Plasticity profile: Learning rates vary by stage
 * - Capability development: Age-appropriate skills emerge
 * - Critical periods: Sensitive windows for specific learning
 * - Behavioral patterns: Characteristic behaviors by stage
 * - Neural maturation: Brain development follows stage sequence
 */
enum class DevelopmentalStage {
    /** Initial formation stage, high plasticity, basic capabilities */
    Initial,
    /** Rapid learning and exploration phase */
    Infant,
    /** Skill refinement and social learning phase */
    Juvenile,
    /** Advanced cognitive development and emotional regulation */
    Adolescent,
    /** Mature capabilities and specialized knowledge */
    Adult,
    /** Age-related changes and maintenance phase */
    Senescent
};

/**
 * @brief Developmental processes and maturation system
 * 
 * Orchestrates neural development and maturation processes across multiple
 * time scales. The development system manages developmental stages, plasticity
 * modulation, and structural changes that occur throughout the agent's lifespan.
 * It provides the foundation for age-appropriate learning and behavior.
 * 
 * Development System Architecture:
 * 1. Stage Management: Controls progression through developmental stages
 * 2. Plasticity Regulation: Adjusts learning rates based on developmental state
 * 3. Structural Development: Implements synaptic and neural maturation
 * 4. Experience Integration: Shapes development based on environmental input
 * 5. Capability Tuning: Adapts system parameters for age-appropriate behavior
 * 
 * Developmental Processes:
 * - Synaptogenesis: Formation of new synaptic connections
 * - Pruning: Elimination of unused synaptic connections
 * - Myelination: Insulation of neural pathways for faster transmission
 * - Neurogenesis: Generation of new neurons in specific brain regions
 * - Circuit refinement: Specialization of neural circuits
 * 
 * Integration Points:
 * - Works with all major brain systems to guide development
 * - Modulates plasticity in learning systems (STDP, Hebbian)
 * - Influences neuromodulator development and function
 * - Shapes memory system capabilities and capacity
 * - Guides cognitive development and reasoning abilities
 * 
 * Thread Safety: DevelopmentSystem is thread-safe for concurrent access
 * from different brain components. Development updates are protected
 * with mutexes.
 * 
 * Memory Management: DevelopmentSystem uses pImpl pattern with pointer storage
 * for efficient memory usage and to reduce compilation dependencies.
 * 
 * @note DevelopmentSystem is essential for proper agent maturation and
 *       age-appropriate behavior. It guides the entire developmental
 *       trajectory from initial formation to mature capabilities.
 * @see Brain for integration with other systems
 * @see RandomGenerator for stochastic developmental processes
 */
class DevelopmentSystem {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a development system with initial state. The development system
     * uses the pImpl pattern for efficient memory usage and to reduce
     * compilation dependencies.
     */
    DevelopmentSystem();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of DevelopmentSystem's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~DevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage that the agent is in.
     * Each stage has characteristic properties and capabilities.
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getStage() const;
    
    /**
     * @brief Set current developmental stage
     * 
     * Sets the developmental stage. This is typically used for testing
     * or when development progresses to the next stage.
     * 
     * @param stage New developmental stage
     */
    void setStage(DevelopmentalStage stage);
    
    /**
     * @brief Advance developmental stage
     * 
     * Advances the developmental stage to the next stage in the sequence.
     * This is the primary method for developmental progression.
     * 
     * @note Thread-safe with mutex protection.
     *       Development progression is irreversible and follows the
     *       predefined sequence of stages.
     */
    void advanceStage();
    
    /**
     * @brief Get stage name
     * 
     * Returns a human-readable name for the current developmental stage.
     * Useful for logging and debugging.
     * 
     * @return Stage name as C-string
     */
    const char* getStageName() const;
    
    /**
     * @brief Update development (simple version)
     * 
     * Updates development processes based on elapsed time. This is a simplified
     * version that handles basic development progression.
     * 
     * @param brain Reference to brain for system access
     * @param currentStep Current simulation step
     */
    void update(Brain* brain, SimulationStep currentStep);
    
    /**
     * @brief Update development with full parameters
     * 
     * Updates development processes based on elapsed time and random generator.
     * This version provides more detailed development control.
     * 
     * @param brain Reference to brain for system access
     * @param rng Random generator for stochastic developmental processes
     * @param dt Time step duration
     */
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    /**
     * @brief Get plasticity modifier for current stage
     * 
     * Returns the factor by which neural plasticity is modified based on
     * the current developmental stage. Higher values indicate greater plasticity
     * (faster learning, more adaptation).
     * 
     * @return Plasticity modifier (typically 0.0-2.0)
     * 
     * @note Early developmental stages have higher plasticity values,
     *       while later stages have lower plasticity for stability.
     */
    float getPlasticityModifier() const;
    
    /**
     * @brief Get critical period info
     * 
     * Returns whether the current developmental stage is a critical period,
     * along with the progress through the critical period.
     * 
     * @return Pair of (isCriticalPeriod, criticalPeriodProgress)
     */
    std::pair<bool, float> getCriticalPeriodInfo() const;
    
    /**
     * @brief Check if current stage is critical period
     * 
     * Returns whether the current developmental stage is a critical period
     * for specific learning or development.
     * 
     * @return true if current stage is a critical period
     */
    bool isCriticalPeriod() const;
    
    /**
     * @brief Get critical period progress
     * 
     * Returns the progress through the current critical period (0.0-1.0).
     * This is useful for timing critical learning experiences.
     * 
     * @return Critical period progress (0.0-1.0)
     */
    float getCriticalPeriodProgress() const;
    
    /**
     * @brief Get developmental age (in simulation time)
     * 
     * Returns the developmental age of the agent in simulation time units.
     * This corresponds to how long the agent has been developing.
     * 
     * @return Developmental age
     */
    double getDevelopmentalAge() const { return age_; }
    
    /**
     * @brief Set developmental age
     * 
     * Sets the developmental age. This is typically used for testing
     * or when development should progress to a specific age.
     * 
     * @param age Developmental age
     */
    void setDevelopmentalAge(double age) { age_ = age; }
    
    /**
     * @brief Get current development system parameters
     * 
     * Returns current development system parameters.
     * 
     * @return Vector containing current parameters [plasticityBase, criticalPeriodDuration, maturationRate]
     */
    std::vector<float> getParameters() const;
    
    /**
     * @brief Set development system parameters
     * 
     * Sets internal parameters for development processing.
     * 
     * @param plasticityBase Base plasticity value
     * @param criticalPeriodDuration Duration of critical periods
     * @param maturationRate Rate of maturation
     */
    void setParameters(float plasticityBase, float criticalPeriodDuration,
                       float maturationRate);
    
    /**
     * @brief Get developmental stage characteristics
     * 
     * Returns characteristics of the current developmental stage, including
     * plasticity profile, critical periods, and age-appropriate capabilities.
     * 
     * @return Map of developmental stage characteristics
     */
    std::map<std::string, float> getStageCharacteristics() const;
    
    /**
     * @brief Get developmental stage capabilities
     * 
     * Returns the capabilities associated with the current developmental stage,
     * such as learning speed, memory capacity, and cognitive abilities.
     * 
     * @return Map of developmental stage capabilities
     */
    std::map<std::string, float> getStageCapabilities() const;
    
    /**
     * @brief Get current developmental milestones
     * 
     * Returns the developmental milestones that should be achieved at the
     * current stage. This is useful for monitoring development progress.
     * 
     * @return Vector of milestone descriptions
     */
    std::vector<std::string> getDevelopmentalMilestones() const;
    
private:
    /**
     * @brief Implementation details (pImpl pattern)
     */
    struct Impl;
    Impl* pImpl;
    
    /**
     * @brief Developmental age in simulation time
     */
    double age_;
};

} // namespace nlm