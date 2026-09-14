// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// 
// This class serves as the integration layer between the neural brain and the
// external world. It handles the complete agent-world interaction loop:
// - Receives sensory percepts from the world (vision, touch, internal signals, proprioception)
// - Processes sensory input through the brain (sensory transduction)
// - Decodes motor commands from brain activity (motor decoding)
// - Applies neuromodulation based on rewards (reward prediction)
// - Manages development system updates (ontogenetic changes)
// - Provides curiosity-driven exploration for adaptive behavior
// 
// The AgentBrain manages collections of sensory and motor neurons that provide
// a standardized interface between the brain's internal neural activity and
// the external world's actions. It implements the core loop of embodied cognition:
// perception → processing → action → feedback.
// 
// This integration layer enables the brain to interact with simulated environments,
// creating an embodied agent that can perceive its surroundings, take actions,
// learn from consequences, and develop over time.

class AgentBrain {
public:
    // Create agent brain interface
    // 
    // Constructs an AgentBrain instance that connects the given brain to the
    // agent interface. This establishes the link between the neural computation
// and the world interaction systems.
    // 
    // Args:
    //   brain: Shared pointer to the brain that this agent will control.
    //          The brain provides the neural processing capabilities.
    // 
    // The agent brain will automatically organize the brain's neurons into
    // functional groups (motor actions, sensory modalities) for efficient
    // processing of world interactions.

    AgentBrain(std::shared_ptr<Brain> brain);

    ~AgentBrain();

    // Initialize agent with world interface
    // 
    // Prepares the agent for world interaction by establishing the sensory
    // and motor interfaces. This must be called before processing sensory input
    // or decoding motor commands.
    // 
    // Args:
    //   world: Reference to the SimpleWorld that this agent will interact with.
    //          The world provides the environment for perception and action.
    // 
    // Initialization includes:
    // - Setting up previous vision history for novelty detection
    // - Resetting developmental and neuromodulatory state
    // - Configuring plasticity modifiers based on developmental stage
    // - Logging the agent's sensory capabilities
    void initialize(const SimpleWorld& world);

    // Get expected sensory input size
    // 
    // Returns the total number of sensory input values expected by the brain.
    // This determines the size of sensory percepts that the agent will receive.
    // 
    // Returns:
    //   Total number of sensory inputs (vision + touch + internal + proprioception)
    //   Calculated as: 256 (vision) + 8 (touch) + 4 (internal) + 6 (proprioception) = 274

    size_t getSensoryInputSize() const;

    // Get expected motor output size
    // 
    // Returns the total number of possible motor outputs from the agent.
    // This determines the size of action space available for movement and interaction.
    // 
    // Returns:
    //   Total number of motor commands (6 basic actions: forward, backward, 
    //   turn left, turn right, interact, wait)

    size_t getMotorOutputSize() const;

    // Process sensory percept from world
    // 
    // Receives a sensory percept from the world and processes it through the
    // agent's sensory systems. This implements the perception phase of the
    // agent-world loop.
    // 
    // The processing includes:
    // 1. Injecting sensory currents into appropriate neural populations
    // 2. Computing novelty detection (changes from previous vision)
    // 3. Updating curiosity level based on novelty and prediction error
    // 4. Storing vision for next frame comparison
    // 
    // Args:
    //   percept: The sensory percept containing vision, touch, internal,
    //            and proprioception data from the world
    void processSensoryInput(const SensoryPercept& percept);

    // Decode motor command from brain activity
    // 
    // Interprets current brain activity to determine what action the agent
    // should take. This implements the action selection phase of the
    // agent-world loop.
    // 
    // The decoding process:
    // 1. Calculates average activity in each motor neuron group
    // 2. Selects the motor command with highest activity
    // 3. Applies curiosity-driven exploration if enabled
    // 4. Returns the chosen MotorCommand
    // 
    // Returns:
    //   The motor command to execute (e.g., MoveForward, TurnLeft, Interact)
    MotorCommand decodeMotorCommand();

    // Apply neuromodulation based on reward
    // 
    // Implements reward prediction error (RPE) learning. This is how the agent
    // learns from its experiences through dopamine-like signals.
    // 
    // The neuromodulation process:
    // 1. Computes prediction error (reward - expected reward)
    // 2. Updates expected reward (exponential moving average)
    // 3. Generates dopamine signal based on prediction error
    // 4. Applies reward modulation to synapses with eligibility traces
    // 5. Updates STDP learning rates based on neuromodulation
    // 
    // Args:
    //   reward: The actual reward received from the world
    //   predictedReward: The expected reward predicted by the agent
    void applyRewardModulation(float reward, float predictedReward);

    // Update development system
    // 
    // Advances the agent's development over time. Development represents the
    // ontogenetic changes that occur as the agent ages, affecting learning rates
    // and neural plasticity.
    // 
    // Development stages:
    // - Initial: High plasticity, rapid learning
    // - CriticalPeriod: Modified plasticity with sensitive periods
    // - Maturation: Stabilizing plasticity
    // - Adult: Low plasticity, stable behavior
    // - Aging: Declining plasticity and performance
    // 
    // Args:
    //   timestep: The time elapsed since the last development update
    void updateDevelopment(double timestep);

    // Get current developmental stage
    // 
    // Returns the current developmental stage of the agent. Development affects
    // how the agent learns and processes information.
    // 
    // Returns:
    //   The current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;

    // Get current neuromodulation level
    // 
    // Returns the current level of neuromodulation (dopamine), which affects
    // neural excitability and learning rate.
    // 
    // Returns:
    //   Dopamine level (-1.0 to 1.0, where positive increases excitability)

    float getNeuromodulationLevel() const;

    // Get current curiosity level
    // 
    // Returns the current curiosity level, which drives exploration behavior
    // and novelty seeking.
    // 
    // Returns:
    //   Curiosity level (0.0 to 1.0, where higher values indicate more exploration)

    float getCuriosityLevel() const;

    // Get current novelty level
    // 
    // Returns the current novelty level, which indicates how novel the current
    // sensory input is compared to previous input.
    // 
    // Returns:
    //   Novelty level (0.0 to 1.0, where higher values indicate more novelty)

    float getNoveltyLevel() const;

    // Get current prediction error
    // 
    // Returns the current prediction error, which indicates the difference
    // between expected and actual outcomes.
    // 
    // Returns:
    //   Prediction error (-1.0 to 1.0, where large values indicate surprise)

    float getPredictionError() const;

    // Reset agent for new episode
    // 
    // Resets the agent's internal state to prepare for a new episode or
    // simulation session. This clears all neuromodulatory signals and
    // developmental state.
    void reset();

    // Get brain pointer
    // 
    // Returns a raw pointer to the brain that this agent controls.
    // 
    // Returns:
    //   Pointer to the brain (must be used with caution - brain owns itself)
    Brain* getBrain() { return brain_.get(); }

    // Configuration
    // 
    // Subsystem enable/disable functions for configuring the agent's
    // capabilities and behavior. These can be called after initialization
    // to change the agent's configuration.

    // Enable/disable reward modulation (learning from rewards)
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    // Enable/disable structural plasticity (synaptogenesis/pruning)
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    // Enable/disable development (ontogenetic changes)
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    // Enable/disable curiosity (exploration drive)
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

    // Check if reward modulation is enabled
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    // Check if structural plasticity is enabled
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    // Check if development is enabled
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    // Check if curiosity is enabled
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

private:
    // Motor decoding: convert neural activity to motor command
    // 
    // Helper function that decodes motor commands from brain motor neuron activity.
    // This is used by decodeMotorCommand() and processes activity from the
    // organized motor neuron groups (forward, backward, left, right, interact, wait).
    // 
    // Returns:
    //   The motor command with highest activity, or Wait if no meaningful activity
    MotorCommand decodeFromMotorNeurons();

    // Motor command selection with curiosity/exploration
    // 
    // Random exploration based on curiosity level. When curiosity is high,
    // the agent occasionally selects random actions instead of the highest-activity
    // action, promoting exploration of novel behaviors.
    // 
    // Args:
    //   defaultCmd: The motor command selected by normal decoding (activity-based)
    // 
    // Returns:
    //   The command to execute, possibly random if curiosity is high
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);

    std::shared_ptr<Brain> brain_;

    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;

    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;

    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;

    // Development state
    double developmentalAge_;
    float plasticityModifier_;

    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;

    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
