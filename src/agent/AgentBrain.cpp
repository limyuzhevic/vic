#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;
// Enhanced AgentBrain Configuration System
struct AgentBrainConfig {
    // Neuron distribution configuration
    bool distributeEvenly = true;                    // Even distribution across groups
    size_t maxMotorGroups = 6;                      // Maximum motor neuron groups
    size_t maxSensoryGroups = 4;                    // Maximum sensory neuron groups
    
    // Neuromodulation configuration
    bool rewardModulationEnabled = true;
    bool structuralPlasticityEnabled = true;
    bool developmentEnabled = true;
    bool curiosityEnabled = true;
    
    // Neuromodulator parameters
    float curiosityThreshold = 0.3f;                // Threshold for curiosity-based behavior
    float explorationChance = 0.3f;                 // Base exploration probability
    float noveltyDecayRate = 0.99f;                  // Rate at which novelty decays
    float dopamineSensitivity = 1.0f;                // Sensitivity to dopamine signals
    float predictionErrorDecay = 0.95f;             // Rate at which prediction error decays
    
    // Developmental parameters
    double developmentalStageDuration = 300.0;       // Duration of each developmental stage
    float initialPlasticity = 1.0f;                  // Initial plasticity level
    float adultPlasticity = 0.2f;                    // Final plasticity level in adulthood
    
    // Sensory configuration
    float visionScaleFactor = 5.0f;                 // Scale factor for vision input
    float touchScaleFactor = 8.0f;                  // Scale factor for touch input
    float internalScaleFactor = 5.0f;               // Scale factor for internal signals
    float proprioceptionScaleFactor = 3.0f;         // Scale factor for proprioception
    
    // Motor configuration
    float motorActivationThreshold = 0.5f;           // Minimum activity for motor command
    bool enableCuriosityExploration = true;         // Enable curiosity-based exploration
    
    // Research and analysis
    bool enableDetailedLogging = false;              // Enable detailed logging for research
    bool enableStatisticalAnalysis = true;           // Enable statistical analysis
    std::string outputPrefix = "agent_brain";       // Prefix for output files
    
    // Parameter sweep support
    bool parameterSweepMode = false;                 // Enable parameter sweep
    std::string sweepName = "";
    std::map<std::string, std::vector<float>> sweepParameters;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename) {
        if (!fs::exists(filename)) {
            NLM_LOG_WARNING("Configuration file not found: " + filename);
            return false;
        }
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open configuration file: " + filename);
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                continue;
            }
            
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Parse configuration parameters
            if (key == "distribute_evenly") distributeEvenly = (value == "true" || value == "1");
            else if (key == "max_motor_groups") maxMotorGroups = static_cast<size_t>(std::stoi(value));
            else if (key == "max_sensory_groups") maxSensoryGroups = static_cast<size_t>(std::stoi(value));
            else if (key == "reward_modulation_enabled") rewardModulationEnabled = (value == "true" || value == "1");
            else if (key == "structural_plasticity_enabled") structuralPlasticityEnabled = (value == "true" || value == "1");
            else if (key == "development_enabled") developmentEnabled = (value == "true" || value == "1");
            else if (key == "curiosity_enabled") curiosityEnabled = (value == "true" || value == "1");
            else if (key == "curiosity_threshold") curiosityThreshold = std::stof(value);
            else if (key == "exploration_chance") explorationChance = std::stof(value);
            else if (key == "novelty_decay_rate") noveltyDecayRate = std::stof(value);
            else if (key == "dopamine_sensitivity") dopamineSensitivity = std::stof(value);
            else if (key == "prediction_error_decay") predictionErrorDecay = std::stof(value);
            else if (key == "developmental_stage_duration") developmentalStageDuration = std::stod(value);
            else if (key == "initial_plasticity") initialPlasticity = std::stof(value);
            else if (key == "adult_plasticity") adultPlasticity = std::stof(value);
            else if (key == "vision_scale_factor") visionScaleFactor = std::stof(value);
            else if (key == "touch_scale_factor") touchScaleFactor = std::stof(value);
            else if (key == "internal_scale_factor") internalScaleFactor = std::stof(value);
            else if (key == "proprioception_scale_factor") proprioceptionScaleFactor = std::stof(value);
            else if (key == "motor_activation_threshold") motorActivationThreshold = std::stof(value);
            else if (key == "enable_curiosity_exploration") enableCuriosityExploration = (value == "true" || value == "1");
            else if (key == "enable_detailed_logging") enableDetailedLogging = (value == "true" || value == "1");
            else if (key == "enable_statistical_analysis") enableStatisticalAnalysis = (value == "true" || value == "1");
            else if (key == "output_prefix") outputPrefix = value;
            else if (key == "parameter_sweep_mode") parameterSweepMode = (value == "true" || value == "1");
            else if (key == "sweep_name") sweepName = value;
        }
        
        return true;
    }
    
    // Print configuration summary
    void printConfiguration() const {
        NLM_LOG_INFO("=== AgentBrain Configuration ===");
        NLM_LOG_INFO("Neuron distribution: " + std::string(distributeEvenly ? "Even" : "Custom"));
        NLM_LOG_INFO("Motor groups: " + std::to_string(maxMotorGroups));
        NLM_LOG_INFO("Sensory groups: " + std::to_string(maxSensoryGroups));
        NLM_LOG_INFO("Reward modulation: " + std::string(rewardModulationEnabled ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("Structural plasticity: " + std::string(structuralPlasticityEnabled ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("Development: " + std::string(developmentEnabled ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("Curiosity: " + std::string(curiosityEnabled ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("Curiosity threshold: " + std::to_string(curiosityThreshold));
        NLM_LOG_INFO("Exploration chance: " + std::to_string(explorationChance));
        NLM_LOG_INFO("Novelty decay rate: " + std::to_string(noveltyDecayRate));
        NLM_LOG_INFO("Dopamine sensitivity: " + std::to_string(dopamineSensitivity));
        NLM_LOG_INFO("Prediction error decay: " + std::to_string(predictionErrorDecay));
        NLM_LOG_INFO("Detailed logging: " + std::string(enableDetailedLogging ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("Statistical analysis: " + std::string(enableStatisticalAnalysis ? "Enabled" : "Disabled"));
        if (parameterSweepMode) {
            NLM_LOG_INFO("Parameter sweep mode: Enabled (" + sweepName + ")");
        }
        NLM_LOG_INFO("===========================");
    }
};

// Advanced Command-Line Parser for AgentBrain
class AgentBrainCommandLineParser {
public:
    static AgentBrainConfig parse(int argc, char** argv) {
        AgentBrainConfig config;
        
        // Default help message
        if (argc <= 1) {
            printHelp();
            return config;
        }
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return config;
            }
            else if (arg == "--version") {
                printVersion();
                return config;
            }
            else if (arg == "--config") {
                if (i + 1 < argc) {
                    config.loadFromFile(argv[++i]);
                }
            }
            else if (arg.find("--distribute-") == 0) {
                std::string option = arg.substr(12); // Remove "--distribute-" prefix
                if (option == "even") config.distributeEvenly = true;
                else if (option == "custom") config.distributeEvenly = false;
            }
            else if (arg.find("--max-motor-groups") == 0) {
                if (arg.find("=") != std::string::npos) {
                    config.maxMotorGroups = static_cast<size_t>(std::stoi(arg.substr(arg.find("=") + 1)));
                } else if (i + 1 < argc) {
                    config.maxMotorGroups = static_cast<size_t>(std::stoi(argv[++i]));
                }
            }
            else if (arg.find("--max-sensory-groups") == 0) {
                if (arg.find("=") != std::string::npos) {
                    config.maxSensoryGroups = static_cast<size_t>(std::stoi(arg.substr(arg.find("=") + 1)));
                } else if (i + 1 < argc) {
                    config.maxSensoryGroups = static_cast<size_t>(std::stoi(argv[++i]));
                }
            }
            else if (arg.find("--curiosity-threshold") == 0) {
                if (arg.find("=") != std::string::npos) {
                    config.curiosityThreshold = std::stof(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < argc) {
                    config.curiosityThreshold = std::stof(argv[++i]);
                }
            }
            else if (arg.find("--exploration-chance") == 0) {
                if (arg.find("=") != std::string::npos) {
                    config.explorationChance = std::stof(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < argc) {
                    config.explorationChance = std::stof(argv[++i]);
                }
            }
            else if (arg.find("--novelty-decay-rate") == 0) {
                if (arg.find("=") != std::string::npos) {
                    config.noveltyDecayRate = std::stof(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < argc) {
                    config.noveltyDecayRate = std::stof(argv[++i]);
                }
            }
            else if (arg.find("--dopamine-sensitivity") == 0) {
                if (arg.find("=") != std::string::npos) {
                    config.dopamineSensitivity = std::stof(arg.substr(arg.find("=") + 1));
                } else if (i + 1 < argc) {
                    config.dopamineSensitivity = std::stof(argv[++i]);
                }
            }
            else if (arg.find("--enable-") == 0) {
                std::string option = arg.substr(7); // Remove "--enable-" prefix
                if (option == "reward_modulation") config.rewardModulationEnabled = true;
                else if (option == "structural_plasticity") config.structuralPlasticityEnabled = true;
                else if (option == "development") config.developmentEnabled = true;
                else if (option == "curiosity") config.curiosityEnabled = true;
                else if (option == "detailed_logging") config.enableDetailedLogging = true;
                else if (option == "statistical_analysis") config.enableStatisticalAnalysis = true;
            }
            else if (arg.find("--disable-") == 0) {
                std::string option = arg.substr(9); // Remove "--disable-" prefix
                if (option == "reward_modulation") config.rewardModulationEnabled = false;
                else if (option == "structural_plasticity") config.structuralPlasticityEnabled = false;
                else if (option == "development") config.developmentEnabled = false;
                else if (option == "curiosity") config.curiosityEnabled = false;
                else if (option == "detailed_logging") config.enableDetailedLogging = false;
                else if (option == "statistical_analysis") config.enableStatisticalAnalysis = false;
            }
            else {
                // Unknown argument, skip but continue
                continue;
            }
        }
        
        return config;
    }
    
private:
    static void printHelp() {
        std::cout << R"(
=== AgentBrain Advanced Configuration ===

Basic Usage:
  ./agent_brain [options]

Options:
  -h, --help                    Show this help message
  --version                     Show version information
  --config <file>               Load configuration from file

Neuron Distribution:
  --distribute-even              Distribute neurons evenly across groups (default)
  --distribute-custom            Use custom distribution pattern

Group Configuration:
  --max-motor-groups <N>        Maximum motor neuron groups (default: 6)
  --max-sensory-groups <N>       Maximum sensory neuron groups (default: 4)

Neuromodulation:
  --curiosity-threshold <T>     Threshold for curiosity-based behavior (default: 0.3)
  --exploration-chance <C>      Base exploration probability (default: 0.3)
  --novelty-decay-rate <R>       Novelty decay rate (default: 0.99)
  --dopamine-sensitivity <S>     Dopamine sensitivity (default: 1.0)

Development:
  --development-enabled <B>      Enable development system (default: true)
  --initial-plasticity <P>      Initial plasticity level (default: 1.0)
  --adult-plasticity <P>        Adult plasticity level (default: 0.2)

Sensory Processing:
  --vision-scale-factor <F>      Vision input scale factor (default: 5.0)
  --touch-scale-factor <F>       Touch input scale factor (default: 8.0)
  --internal-scale-factor <F>    Internal signals scale factor (default: 5.0)
  --proprioception-scale-factor <F> Proprioception scale factor (default: 3.0)

Motor Control:
  --motor-activation-threshold <T> Minimum activity for motor command (default: 0.5)
  --curiosity-exploration-enabled  Enable curiosity-based exploration (default: true)

Research and Analysis:
  --detailed-logging             Enable detailed logging for research
  --statistical-analysis         Enable statistical analysis
  --output-prefix <prefix>       Prefix for output files (default: "agent_brain")
  --parameter-sweep <name>       Enable parameter sweep mode

Configuration File Format:
  Config file should contain key=value pairs, one per line:
    distribute_evenly=true
    max_motor_groups=8
    curiosity_threshold=0.4
    exploration_chance=0.4
    enable_detailed_logging=true
    enable_statistical_analysis=true

Examples:
  ./agent_brain --curiosity-threshold 0.5 --exploration-chance 0.5
  ./agent_brain --max-motor-groups 8 --max-sensory-groups 6
  ./agent_brain --enable-detailed-logging --enable-statistical-analysis
  ./agent_brain --config agent_config.cfg

AgentBrain Philosophy:
  The AgentBrain connects NLM brain to the world
  Handles sensory transduction and motor decoding
  Implements neuromodulation and developmental systems
  Supports curiosity-driven exploration and learning
  Provides extensible architecture for research
)" << std::endl;
    }
    
    static void printVersion() {
        std::cout << "NLM AgentBrain - Advanced Agent Configuration" << std::endl;
        std::cout << "Version: 2.0" << std::endl;
        std::cout << "Architecture: Modular neural interface with neuromodulation" << std::endl;
        std::cout << "Features: Sensory transduction, motor decoding, curiosity-driven behavior" << std::endl;
    }
};

// Enhanced Statistics Collection for AgentBrain
struct AgentBrainStatistics {
    // Sensory statistics
    size_t totalSensoryNeurons;
    size_t activeSensoryNeurons;
    float averageSensoryInput;
    float sensoryInputVariance;
    
    // Motor statistics
    size_t totalMotorNeurons;
    size_t activeMotorNeurons;
    size_t selectedMotorCommands[8]; // Count per motor command type
    float motorOutputEntropy;
    
    // Neuromodulation statistics
    float dopamineLevelHistory[100];  // Rolling history
    float curiosityLevelHistory[100]; // Rolling history
    float noveltyLevelHistory[100];   // Rolling history
    float predictionErrorHistory[100]; // Rolling history
    
    // Development statistics
    double currentAge;
    DevelopmentalStage currentStage;
    float plasticityModifier;
    float developmentalProgress;
    
    // Learning statistics
    size_t totalLearningEvents;
    float learningEfficiency;
    size_t explorationEvents;
    size_t exploitationEvents;
    
    // Performance metrics
    float responseTime;
    float actionSuccessRate;
    float rewardPredictionAccuracy;
    
    AgentBrainStatistics() : totalSensoryNeurons(0), activeSensoryNeurons(0), averageSensoryInput(0.0f),
                           sensoryInputVariance(0.0f), totalMotorNeurons(0), activeMotorNeurons(0),
                           currentAge(0.0), currentStage(DevelopmentalStage::Initial),
                           plasticityModifier(1.0f), developmentalProgress(0.0f),
                           totalLearningEvents(0), learningEfficiency(0.0f),
                           explorationEvents(0), exploitationEvents(0),
                           responseTime(0.0f), actionSuccessRate(0.0f),
                           rewardPredictionAccuracy(0.0f) {
        for (int i = 0; i < 8; ++i) selectedMotorCommands[i] = 0;
        for (int i = 0; i < 100; ++i) {
            dopamineLevelHistory[i] = 0.0f;
            curiosityLevelHistory[i] = 0.0f;
            noveltyLevelHistory[i] = 0.0f;
            predictionErrorHistory[i] = 0.0f;
        }
    }
    
    void updateRollingHistory(float& historyArray[100], float newValue) {
        // Shift history and add new value
        for (int i = 99; i > 0; --i) {
            historyArray[i] = historyArray[i - 1];
        }
        historyArray[0] = newValue;
    }
    
    float getRollingAverage(float historyArray[100]) const {
        float sum = 0.0f;
        int validCount = 0;
        for (int i = 0; i < 100; ++i) {
            if (historyArray[i] != 0.0f) { // Skip initial zeros
                sum += historyArray[i];
                validCount++;
            }
        }
        return validCount > 0 ? sum / validCount : 0.0f;
    }
};

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
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
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
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
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
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
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
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
