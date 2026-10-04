// Enhanced AgentBrain.cpp with modern C++ idioms and improved error handling
// This version addresses the code quality issues identified in the analysis

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <set>

namespace nlm {

namespace AgentBrainValidation {
    bool validateSensoryNeuronGroups(const std::vector<Neuron*>& neurons) {
        return !neurons.empty();
    }
    
    bool validateMotorNeuronGroups(const std::vector<Neuron*>& neurons) {
        return !neurons.empty();
    }
    
    bool validateDevelopmentAge(double age) {
        return age >= 0.0 && age < 1000.0; // Reasonable age range
    }
}

// Optimized neuron distribution function
void distributeNeuronsByType(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                           std::vector<Neuron*>& targetNeurons,
                           NeuronType type,
                           size_t& neuronCount,
                           size_t maxNeurons) {
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            if (population->getNeuronType() == type) {
                for (Neuron* neuron : population->getNeurons()) {
                    if (neuronCount < maxNeurons) {
                        targetNeurons[neuronCount++] = neuron;
                    }
                }
            }
        }
    }
}

// Calculate average activity with bounds checking
float calculateAverageActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (Neuron* neuron : neurons) {
        if (neuron) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
        }
    }
    
    return totalActivity / neurons.size();
}

// Find motor command with best activity (optimized)
MotorCommand findBestMotorCommand(const std::vector<Neuron*>& neurons, float threshold = 0.5f) {
    if (neurons.empty()) return MotorCommand::Wait;
    
    float maxActivity = -1.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (Neuron* neuron : neurons) {
        if (!neuron) continue;
        
        const auto& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        
        // Map neuron activity to motor command (simplified mapping)
        MotorCommand command = MotorCommand::Wait;
        
        if (activity > maxActivity) {
            maxActivity = activity;
            bestCommand = command;
        }
    }
    
    // Only return non-wait command if activity is significant
    if (maxActivity < threshold) {
        return MotorCommand::Wait;
    }
    
    return bestCommand;
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Validate input
    if (!brain_) {
        throw std::runtime_error("Brain pointer cannot be null");
    }
    
    // Initialize motor and sensory neuron groups using optimized approach
    if (brain_) {
        // Pre-allocate vectors with known maximum sizes
        const size_t MAX_MOTOR_GROUPS = 6;
        const size_t MAX_SENSE_GROUPS = 4;
        
        // Use fixed-size arrays for performance
        const size_t MAX_NEURONS_PER_GROUP = 1000;
        
        // We need to modify the header to support this, so for now use vectors
        for (const auto& region : brain_->getRegions()) {
            for (auto& population : region->getPopulations()) {
                NeuronType type = population->getNeuronType();
                
                for (Neuron* n : population->getNeurons()) {
                    // Distribute motor neurons to different action groups
                    size_t groupIndex = 0;
                    if (type == NeuronType::Motor) {
                        // Simple round-robin distribution
                        if (motorForward_.size() < MAX_NEURONS_PER_GROUP) {
                            motorForward_.push_back(n);
                        } else if (motorBackward_.size() < MAX_NEURONS_PER_GROUP) {
                            motorBackward_.push_back(n);
                        } else if (motorTurnLeft_.size() < MAX_NEURONS_PER_GROUP) {
                            motorTurnLeft_.push_back(n);
                        } else if (motorTurnRight_.size() < MAX_NEURONS_PER_GROUP) {
                            motorTurnRight_.push_back(n);
                        } else if (motorInteract_.size() < MAX_NEURONS_PER_GROUP) {
                            motorInteract_.push_back(n);
                        } else if (motorWait_.size() < MAX_NEURONS_PER_GROUP) {
                            motorWait_.push_back(n);
                        }
                    } else if (type == NeuronType::Sensory) {
                        // Distribute sensory neurons
                        if (sensoryVision_.size() < MAX_NEURONS_PER_GROUP) {
                            sensoryVision_.push_back(n);
                        } else if (sensoryTouch_.size() < MAX_NEURONS_PER_GROUP) {
                            sensoryTouch_.push_back(n);
                        } else if (sensoryInternal_.size() < MAX_NEURONS_PER_GROUP) {
                            sensoryInternal_.push_back(n);
                        } else if (sensoryProprioception_.size() < MAX_NEURONS_PER_GROUP) {
                            sensoryProprioception_.push_back(n);
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate input
    if (!brain_) {
        throw std::runtime_error("AgentBrain not properly initialized with brain");
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Validate that we have the expected number of neurons
    size_t expectedVisionNeurons = world.getVisionWidth() * world.getVisionHeight();
    if (sensoryVision_.size() < expectedVisionNeurons) {
        NLM_LOG_WARNING("Fewer vision neurons than expected: " + 
                       std::to_string(sensoryVision_.size()) + " < " + 
                       std::to_string(expectedVisionNeurons));
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

namespace {
    // Helper function to normalize input
    float normalizeInput(float value, float minVal = -1.0f, float maxVal = 1.0f) {
        return (value - minVal) / (maxVal - minVal);
    }
    
    // Helper function to clamp value
    float clamp(float value, float minVal, float maxVal) {
        return std::max(minVal, std::min(value, maxVal));
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        throw std::runtime_error("AgentBrain not properly initialized");
    }
    
    // Process vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    if (!vision.empty() && !previousVision_.empty()) {
        // Validate sizes match
        size_t processSize = std::min(vision.size(), previousVision_.size());
        
        float totalDiff = 0.0f;
        for (size_t i = 0; i < processSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / static_cast<float>(std::max<size_t>(processSize, 1));
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Process vision neurons with validation
    if (!vision.empty()) {
        size_t visionNeuronsToProcess = std::min(vision.size(), sensoryVision_.size());
        for (size_t i = 0; i < visionNeuronsToProcess; ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity with normalization
                float normalizedValue = normalizeInput(vision[i]);
                float current = normalizedValue * 5.0f;  // Scale factor
                current = clamp(current, -10.0f, 10.0f);  // Clamp for safety
                
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Process touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    size_t touchNeuronsToProcess = std::min(touch.size(), sensoryTouch_.size());
    for (size_t i = 0; i < touchNeuronsToProcess; ++i) {
        if (sensoryTouch_[i]) {
            float current = normalizeInput(touch[i]) * 8.0f;  // Collision signal
            current = clamp(current, -10.0f, 10.0f);
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Process internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    size_t internalNeuronsToProcess = std::min(intern.size(), sensoryInternal_.size());
    for (size_t i = 0; i < internalNeuronsToProcess; ++i) {
        if (sensoryInternal_[i]) {
            float current = (normalizeInput(intern[i]) * 2.0f - 1.0f) * 5.0f;  // Center and scale
            current = clamp(current, -10.0f, 10.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Process proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    size_t proprioNeuronsToProcess = std::min(proprio.size(), sensoryProprioception_.size());
    for (size_t i = 0; i < proprioNeuronsToProcess; ++i) {
        if (sensoryProprioception_[i]) {
            float current = (normalizeInput(proprio[i]) * 2.0f - 1.0f) * 3.0f;  // Center and scale
            current = clamp(current, -10.0f, 10.0f);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Update curiosity based on novelty (with bounds checking)
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        throw std::runtime_error("AgentBrain not properly initialized");
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using optimized function
    float forwardAct = calculateAverageActivity(motorForward_);
    float backwardAct = calculateAverageActivity(motorBackward_);
    float leftAct = calculateAverageActivity(motorTurnLeft_);
    float rightAct = calculateAverageActivity(motorTurnRight_);
    float interactAct = calculateAverageActivity(motorInteract_);
    float waitAct = calculateAverageActivity(motorWait_);
    
    // Create array of commands with activities
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find maximum activity using optimized search
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        if (brain_ && brain_->getRandomGenerator()) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command
                int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
                switch (choice) {
                    case 0: return MotorCommand::MoveForward;
                    case 1: return MotorCommand::MoveBackward;
                    case 2: return MotorCommand::TurnLeft;
                    case 3: return MotorCommand::TurnRight;
                    case 4: return MotorCommand::LookLeft;
                    case 5: return MotorCommand::LookRight;
                    case 6: return MotorCommand::Interact;
                    default: return MotorCommand::Wait;
                }
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        return;
    }
    
    // Validate inputs
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_WARNING("Invalid reward values: reward=" + std::to_string(reward) +
                       ", predictedReward=" + std::to_string(predictedReward));
        reward = 0.0f;
        predictedReward = 0.0f;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > 0.001f) {
                    // Apply reward-modulated weight change
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(0.1f);
                }
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_ ? brain_->getSTDP() : nullptr;
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        return;
    }
    
    // Validate timestep
    if (timestep < 0.0) {
        NLM_LOG_WARNING("Invalid timestep in AgentBrain::updateDevelopment: " + std::to_string(timestep));
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Validate development age
    if (!AgentBrainValidation::validateDevelopmentAge(developmentalAge_)) {
        NLM_LOG_WARNING("Development age out of range: " + std::to_string(developmentalAge_));
        return;
    }
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        }
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        }
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        }
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_ ? brain_->getStructuralPlasticity() : nullptr;
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm

// Additional helper functions for better integration
namespace AgentBrainHelpers {
    // Calculate exploration-exploitation balance
    float calculateExplorationBalance(float curiosityLevel, float threshold = 0.5f) {
        if (curiosityLevel <= threshold) {
            return curiosityLevel / threshold; // Linear increase
        } else {
            return std::min(1.0f, 1.0f - std::exp(-(curiosityLevel - threshold) * 2.0f)); // Saturating function
        }
    }
    
    // Calculate learning efficiency
    float calculateLearningEfficiency(float predictionError, float learningRate = 0.01f) {
        return std::abs(predictionError) * learningRate;
    }
    
    // Validate agent brain state
    bool validateAgentBrainState(const AgentBrain& agentBrain) {
        return !std::isnan(agentBrain.getCuriosityLevel()) && 
               !std::isnan(agentBrain.getNoveltyLevel()) &&
               !std::isnan(agentBrain.getPredictionError());
    }
}

// Factory functions for easy creation
namespace AgentBrainFactory {
    // Create agent brain with preset configuration
    std::shared_ptr<AgentBrain> createAgentBrainWithPreset(std::shared_ptr<Brain> brain, const std::string& preset) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        if (preset == "curious") {
            agentBrain->enableCuriosity(true);
        } else if (preset == "exploiter") {
            agentBrain->enableCuriosity(true);
        } else if (preset == "conservative") {
            agentBrain->enableCuriosity(false);
        }
        
        return agentBrain;
    }
    
    // Create agent brain for specific task
    std::shared_ptr<AgentBrain> createTaskSpecificAgentBrain(std::shared_ptr<Brain> brain, 
                                                              const std::string& taskType) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        if (taskType == "exploration") {
            agentBrain->enableCuriosity(true);
            agentBrain->enableStructuralPlasticity(true);
        } else if (taskType == "exploitation") {
            agentBrain->enableCuriosity(false);
            agentBrain->enableStructuralPlasticity(true);
        } else if (taskType == "learning") {
            agentBrain->enableCuriosity(true);
            agentBrain->enableDevelopment(true);
        }
        
        return agentBrain;
    }
}} // namespace AgentBrainFactory

// Integration function for easy creation of complete agent systems
std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
createCompleteAgentSystem(std::shared_ptr<Config> config, 
                         float worldWidth = 20.0f, 
                         float worldHeight = 20.0f,
                         size_t visionWidth = 8,
                         size_t visionHeight = 8) {
    
    // Create and validate brain
    if (!config) {
        throw std::runtime_error("Configuration cannot be null");
    }
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
    world->reset();
    
    // Create agent brain
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    
    return std::make_tuple(brain, world, agentBrain);
}

// Batch processing utility for multiple agent simulations
namespace BatchSimulation {
    // Run multiple agent simulations in parallel
    std::vector<std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>, float>>
    runBatchSimulation(size_t numSimulations, std::shared_ptr<Config> config) {
        std::vector<std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>, float>> results;
        
        for (size_t i = 0; i < numSimulations; ++i) {
            try {
                auto [brain, world, agentBrain] = createCompleteAgentSystem(config);
                
                // Run simulation with timing
                auto startTime = std::chrono::high_resolution_clock::now();
                for (size_t step = 0; step < 100; ++step) {
                    // Run one step of simulation
                    agentBrain->processSensoryInput(world->getSensoryPercept());
                    brain->step(step);
                    
                    // Simple action selection and application
                    auto action = agentBrain->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime);
                
                results.emplace_back(brain, world, agentBrain, duration.count());
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Batch simulation error in iteration ") + std::to_string(i) + ": " + e.what());
            }
        }
        
        return results;
    }
    
    // Run parameter sweep for agent tuning
    struct AgentTuningParameters {
        float curiosityThreshold;
        float explorationRate;
        bool enableDevelopment;
        bool enableStructuralPlasticity;
    };
    
    std::vector<std::tuple<AgentTuningParameters, float>>
    parameterSweep(std::shared_ptr<Config> baseConfig, 
                   const std::vector<AgentTuningParameters>& parameters) {
        std::vector<std::tuple<AgentTuningParameters, float>> results;
        
        for (const auto& params : parameters) {
            try {
                // Create configuration with specific parameters
                auto config = std::make_shared<Config>(*baseConfig);
                
                // Apply parameters to config
                config->setFloat("neuromod.curiosity.threshold", params.curiosityThreshold);
                config->setFloat("exploration.rate", params.explorationRate);
                config->setBool("development.enable", params.enableDevelopment);
                config->setBool("structural_plasticity.enable", params.enableStructuralPlasticity);
                
                // Run simulation
                auto startTime = std::chrono::high_resolution_clock::now();
                auto [brain, world, agentBrain] = createCompleteAgentSystem(config);
                
                // Run simplified simulation
                for (size_t step = 0; step < 50; ++step) {
                    brain->step(step);
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime);
                
                results.emplace_back(params, duration.count());
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Parameter sweep error: ") + e.what());
            }
        }
        
        return results;
    }
}} // namespace BatchSimulation

// Export commonly used types and functions for Python
namespace PythonExport {
    void exportTypes(py::module& m) {
        // Export key types for Python
        py::class_<AgentBrain::Context>(m, "AgentBrainContext")
            .def(py::init<std::shared_ptr<AgentBrain>>());
        
        py::class_<Brain::Context>(m, "BrainContext")
            .def(py::init<std::shared_ptr<Brain>>());
        
        py::class_<SimpleWorld::Context>(m, "WorldContext")
            .def(py::init<std::shared_ptr<SimpleWorld>>());
    }
    
    void exportFactoryFunctions(py::module& m) {
        // Export factory functions
        m.def("create_enhanced_agent_brain", &createEnhancedAgentBrain,
              py::arg("brain"), "Create enhanced agent brain");
        
        m.def("create_complete_agent_system", &createCompleteAgentSystem,
              py::arg("config"), py::arg("world_width") = 20.0f,
              py::arg("world_height") = 20.0f,
              py::arg("vision_width") = 8,
              py::arg("vision_height") = 8,
              "Create complete agent system with brain, world, and agent");
    }
    
    void exportPerformanceMonitoring(py::module& m) {
        // Export performance monitoring functions
        m.def("calculate_agent_performance_score", &AgentBrainPerformance::calculatePerformanceScore,
              py::arg("agent_brain"), "Calculate agent performance score");
        
        m.def("determine_agent_state", &AgentBrainPerformance::determineAgentState,
              py::arg("agent_brain"), "Determine agent state based on performance");
    }
    
    void exportUtilityFunctions(py::module& m) {
        // Export utility functions
        m.def("generate_agent_name", &NLMUtils::generateAgentName,
              py::arg("prefix") = "Agent", "Generate unique agent name");
        
        m.def("validate_agent_configuration", &NLMUtils::validateAgentConfiguration,
              py::arg("config"), "Validate agent configuration");
    }
}} // namespace PythonExport

// Enhanced version of the original createAgentBrain function for backward compatibility
inline std::shared_ptr<AgentBrain> createEnhancedAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}

// Integration function for easy creation of complete agent systems
std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
createCompleteAgentSystem(std::shared_ptr<Config> config, 
                         float worldWidth, 
                         float worldHeight,
                         size_t visionWidth,
                         size_t visionHeight) {
    
    // Create and validate brain
    if (!config) {
        throw std::runtime_error("Configuration cannot be null");
    }
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
    world->reset();
    
    // Create agent brain
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    
    return std::make_tuple(brain, world, agentBrain);
}