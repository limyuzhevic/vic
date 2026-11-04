/**
     * @class BrainInitializer
     * @brief Handles brain initialization and setup for NLM Phase 6 integration
     * 
     * The BrainInitializer class encapsulates all initialization logic that was previously
     * contained within the monolithic Brain.cpp file. This separation makes the
     * initialization process more modular, testable, and maintainable.
     * 
     * The class handles:
     * - Region creation and neural population setup
     * - Neuron lookup system configuration (O(1) access)
     * - Random connectivity initialization
     * - Integrated memory systems setup
     * - Neuromodulation system configuration
     * - Spike system registration and handlers
     * - Checkpoint system configuration
     * - Plasticity system parameter configuration
     * 
     * @note This class is designed to work with the Brain::Impl implementation detail,
     *       providing initialization services without exposing internal implementation.
     * 
     * @see Brain for main brain interface
     * @see Brain.cpp for simulation loop implementation
     */
class BrainInitializer {
public:
    BrainInitializer() = default;
    ~BrainInitializer() = default;
public:
    BrainInitializer() = default;
    ~BrainInitializer() = default;

    /**
     * @brief Initialize a brain with configuration
     * @param impl Implementation detail pointer to Brain::Impl
     * @return bool True if initialization succeeded, false otherwise
     * @note This is the main initialization method that orchestrates all setup steps
     */
    bool initializeBrain(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Reset brain state for re-initialization
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Clears all regions, neurons, and internal state for fresh initialization
     */
    void resetBrain(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Load simulation parameters from configuration
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Reads timestep, replay intervals, and other simulation settings from config
     */
    void loadSimulationParameters(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize all integrated brain systems
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Sets up memory, prediction, cognition, and neuromodulation systems
     */
    void initializeIntegratedSystems(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize plasticity systems (STDP, Hebbian, structural)
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Configures plasticity rules and their parameters from configuration
     */
    void initializePlasticitySystems(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Configure neuromodulation systems
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Sets up dopamine, curiosity, novelty, and prediction error systems
     */
    void configureNeuromodulation(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Setup spike system and event handlers
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Registers handlers for spike processing and synaptic delivery
     */
    void setupSpikeSystem(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Configure checkpoint manager
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Configures checkpoint saving and loading capabilities
     */
    void configureCheckpointSystem(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Create neural populations across regions
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates Sensory, Internal, Inhibitory, and Motor populations
     */
    void createNeuralPopulations(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Setup neuron lookup system for O(1) access
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates fast lookup from neuron ID to neuron pointer for synaptic delivery
     */
    void setupNeuronLookup(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize connectivity with random weights
     * @param impl Implementation detail pointer to Brain::Impl
     * @param connectionProbability Probability of connection between neurons
     * @note Randomly connects neurons with specified connection probability
     */
    void initializeConnectivity(std::shared_ptr<Impl> impl, float connectionProbability);
    
    /**
     * @brief Configure memory systems
     * @param impl Implementation detail pointer to Brain::Impl
     * @param neuronCount Total number of neurons in brain
     * @note Initializes working memory, episodic memory, and associative memory
     */
    void configureMemorySystems(std::shared_ptr<Impl> impl, size_t neuronCount);
    
    /**
     * @brief Configure cognition systems
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Initializes planner, concept formation, and attention systems
     */
    void configureCognitionSystems(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Configure neuromodulation systems
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Configures curiosity, novelty, dopamine, and prediction error systems
     */
    void configureNeuromodulationSystems(std::shared_ptr<Impl> impl);

/**
     * @brief Helper function to setup sensory and motor neuron collections
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Populates sensoryNeurons and motorNeurons vectors for I/O operations
     */
    void setupSensoryMotorNeurons(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize working memory system
     * @param impl Implementation detail pointer to Brain::Impl
     * @param neuronCount Total neuron count for capacity calculation
     * @note Creates working memory with capacity based on brain size
     */
    void initializeWorkingMemory(std::shared_ptr<Impl> impl, size_t neuronCount);
    
    /**
     * @brief Initialize episodic memory system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates episodic memory with episode storage capacity
     */
    void initializeEpisodicMemory(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize associative memory system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates associative memory for pattern associations
     */
    void initializeAssociativeMemory(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize prediction system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Sets up prediction system for sensory prediction and error computation
     */
    void initializePredictionSystem(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize cognition systems (planner, concept formation, attention)
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Configures neural planner with planning depth and attention with inhibition/excitation
     */
    void initializePlanner(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize concept formation system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates concept formation system for pattern discovery and abstraction
     */
    void initializeConceptFormation(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize attention system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates attention system with competitive selection dynamics
     */
    void initializeAttention(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize neuromodulation systems
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Configures novelty, curiosity, dopamine, and prediction error systems
     */
    void initializeNovelty(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize curiosity system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates curiosity system for exploration motivation
     */
    void initializeCuriosity(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize dopamine system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates dopamine system for reward prediction error and reinforcement
     */
    void initializeDopamine(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize prediction error system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates prediction error system for learning and adaptation
     */
    void initializePredictionError(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Initialize development system
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Creates development system for age-dependent plasticity and maturation
     */
    void initializeDevelopmentSystem(std::shared_ptr<Impl> impl);
    
    /**
     * @brief Apply STDP and Hebbian plasticity configurations
     * @param impl Implementation detail pointer to Brain::Impl
     * @note Configures STDP parameters (LTP/LTD weights, time constant) and
     *       structural plasticity parameters (synaptogenesis, pruning rates)
     */
    void applyPlasticityConfigurations(std::shared_ptr<Impl> impl);
};

} // namespace nlm