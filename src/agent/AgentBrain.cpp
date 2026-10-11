#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

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
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() {
    // Clean up neuron pointer vectors to avoid memory leaks
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    previousVision_.clear();
}

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

void AgentBrain::enableDebugMode(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Debug mode enabled - detailed logging will be output");
        // Enable debug logging across all systems
        // This would typically set logging levels to DEBUG
        // Also enable additional diagnostic information
        debugModeEnabled_ = true;
    } else {
        NLM_LOG_INFO("Debug mode disabled");
        debugModeEnabled_ = false;
    }
}

void AgentBrain::enableProfiling(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Performance profiling enabled");
        // Start performance counters
        // This would typically start timers for various operations
        profilingEnabled_ = true;
        startPerformanceTimer();
    } else {
        NLM_LOG_INFO("Performance profiling disabled");
        profilingEnabled_ = false;
    }
}

void AgentBrain::enablePerformanceMonitoring(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Real-time performance monitoring enabled");
        // Start monitoring system performance
        // This would typically track CPU, memory, and I/O
        performanceMonitoringEnabled_ = true;
    } else {
        NLM_LOG_INFO("Real-time performance monitoring disabled");
        performanceMonitoringEnabled_ = false;
    }
}

void AgentBrain::saveAgentState(const std::string& filepath) const {
    NLM_LOG_INFO("Saving agent state to: " + filepath);
    
    std::ofstream outFile(filepath);
    if (outFile.is_open()) {
        // Save all agent state
        outFile << "=== NLM Agent State ===\n";
        outFile << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        // Save configuration
        outFile << "Configuration:\n";
        outFile << "  Reward Modulation: " << (rewardModulationEnabled_ ? "Enabled" : "Disabled") << "\n";
        outFile << "  Structural Plasticity: " << (structuralPlasticityEnabled_ ? "Enabled" : "Disabled") << "\n";
        outFile << "  Development: " << (developmentEnabled_ ? "Enabled" : "Disabled") << "\n";
        outFile << "  Curiosity: " << (curiosityEnabled_ ? "Enabled" : "Disabled") << "\n";
        outFile << "  Debug Mode: " << (debugModeEnabled_ ? "Enabled" : "Disabled") << "\n";
        outFile << "  Profiling: " << (profilingEnabled_ ? "Enabled" : "Disabled") << "\n";
        outFile << "  Performance Monitoring: " << (performanceMonitoringEnabled_ ? "Enabled" : "Disabled") << "\n\n";
        
        // Save neuromodulation state
        outFile << "Neuromodulation State:\n";
        outFile << "  Dopamine Level: " << dopamineLevel_ << "\n";
        outFile << "  Novelty Level: " << noveltyLevel_ << "\n";
        outFile << "  Curiosity Level: " << curiosityLevel_ << "\n";
        outFile << "  Prediction Error: " << predictionError_ << "\n";
        outFile << "  Expected Reward: " << expectedReward_ << "\n\n";
        
        // Save development state
        outFile << "Development State:\n";
        outFile << "  Developmental Age: " << developmentalAge_ << "\n";
        outFile << "  Plasticity Modifier: " << plasticityModifier_ << "\n\n";
        
        // Save neuron group information
        outFile << "Neuron Group Information:\n";
        outFile << "  Motor Forward: " << motorForward_.size() << " neurons\n";
        outFile << "  Motor Backward: " << motorBackward_.size() << " neurons\n";
        outFile << "  Motor Turn Left: " << motorTurnLeft_.size() << " neurons\n";
        outFile << "  Motor Turn Right: " << motorTurnRight_.size() << " neurons\n";
        outFile << "  Motor Interact: " << motorInteract_.size() << " neurons\n";
        outFile << "  Motor Wait: " << motorWait_.size() << " neurons\n";
        outFile << "  Sensory Vision: " << sensoryVision_.size() << " neurons\n";
        outFile << "  Sensory Touch: " << sensoryTouch_.size() << " neurons\n";
        outFile << "  Sensory Internal: " << sensoryInternal_.size() << " neurons\n";
        outFile << "  Sensory Proprioception: " << sensoryProprioception_.size() << " neurons\n\n";
        
        outFile << "=== End Agent State ===\n";
        outFile.close();
        
        NLM_LOG_INFO("Agent state saved successfully");
    } else {
        NLM_LOG_WARNING("Failed to save agent state to: " + filepath);
    }
}

void AgentBrain::loadAgentState(const std::string& filepath) {
    NLM_LOG_INFO("Loading agent state from: " + filepath);
    
    std::ifstream inFile(filepath);
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            // Parse configuration from file
            if (line.find("Reward Modulation:") != std::string::npos) {
                rewardModulationEnabled_ = (line.find("Enabled") != std::string::npos);
            } else if (line.find("Structural Plasticity:") != std::string::npos) {
                structuralPlasticityEnabled_ = (line.find("Enabled") != std::string::npos);
            } else if (line.find("Development:") != std::string::npos) {
                developmentEnabled_ = (line.find("Enabled") != std::string::npos);
            } else if (line.find("Curiosity:") != std::string::npos) {
                curiosityEnabled_ = (line.find("Enabled") != std::string::npos);
            } else if (line.find("Debug Mode:") != std::string::npos) {
                debugModeEnabled_ = (line.find("Enabled") != std::string::npos);
            } else if (line.find("Profiling:") != std::string::npos) {
                profilingEnabled_ = (line.find("Enabled") != std::string::npos);
            } else if (line.find("Performance Monitoring:") != std::string::npos) {
                performanceMonitoringEnabled_ = (line.find("Enabled") != std::string::npos);
            } else if (line.find("Dopamine Level:") != std::string::npos) {
                try {
                    dopamineLevel_ = std::stof(line.substr(line.find(":") + 1));
                } catch (...) {}
            } else if (line.find("Novelty Level:") != std::string::npos) {
                try {
                    noveltyLevel_ = std::stof(line.substr(line.find(":") + 1));
                } catch (...) {}
            } else if (line.find("Curiosity Level:") != std::string::npos) {
                try {
                    curiosityLevel_ = std::stof(line.substr(line.find(":") + 1));
                } catch (...) {}
            } else if (line.find("Prediction Error:") != std::string::npos) {
                try {
                    predictionError_ = std::stof(line.substr(line.find(":") + 1));
                } catch (...) {}
            } else if (line.find("Expected Reward:") != std::string::npos) {
                try {
                    expectedReward_ = std::stof(line.substr(line.find(":") + 1));
                } catch (...) {}
            } else if (line.find("Developmental Age:") != std::string::npos) {
                try {
                    developmentalAge_ = std::stod(line.substr(line.find(":") + 1));
                } catch (...) {}
            } else if (line.find("Plasticity Modifier:") != std::string::npos) {
                try {
                    plasticityModifier_ = std::stof(line.substr(line.find(":") + 1));
                } catch (...) {}
            }
        }
        
        inFile.close();
        NLM_LOG_INFO("Agent state loaded successfully");
    } else {
        NLM_LOG_WARNING("Failed to load agent state from: " + filepath);
    }
}

void AgentBrain::exportBrainState(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting brain neural state to: " + filepath);
    
    std::ofstream outFile(filepath);
    if (outFile.is_open()) {
        outFile << "=== NLM Brain State ===\n";
        outFile << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        if (brain_) {
            // Export all regions
            for (const auto& region : brain_->getRegions()) {
                outFile << "Region " << region->getId().value << ":\n";
                outFile << "  Neurons: " << region->getAllNeurons().size() << "\n";
                outFile << "  Synapses: " << region->getSynapses().size() << "\n\n";
                
                // Export neuron information
                for (const auto& neuron : region->getAllNeurons()) {
                    outFile << "    Neuron " << neuron->getId().value << ":\n";
                    outFile << "      Type: " << static_cast<int>(neuron->getType()) << "\n";
                    outFile << "      Membrane Potential: " << neuron->getMembranePotential() << "\n";
                    outFile << "      Threshold: " << neuron->getThreshold() << "\n";
                    outFile << "      Resting Potential: " << neuron->getState().restingPotential << "\n";
                    outFile << "      Firing State: " << static_cast<int>(neuron->getState().firingState) << "\n";
                }
                
                // Export synapse information
                for (const auto& synapse : region->getSynapses()) {
                    outFile << "    Synapse " << synapse->getId().value << ":\n";
                    outFile << "      Weight: " << synapse->getWeight() << "\n";
                    outFile << "      Type: " << static_cast<int>(synapse->getType()) << "\n";
                    outFile << "      Plasticity: Hebbian=" << synapse->isHebbianPlasticityEnabled() 
                           << ", STDP=" << synapse->isSTDPPlasticityEnabled() 
                           << ", Structural=" << synapse->isStructuralPlasticityEnabled() << "\n";
                }
            }
        }
        
        outFile << "=== End Brain State ===\n";
        outFile.close();
        
        NLM_LOG_INFO("Brain state exported successfully");
    } else {
        NLM_LOG_WARNING("Failed to export brain state to: " + filepath);
    }
}

void AgentBrain::importBrainState(const std::string& filepath) {
    NLM_LOG_INFO("Importing brain neural state from: " + filepath);
    
    // Note: This is a placeholder implementation
    // In a real implementation, this would restore brain state from file
    // and update all neural properties, synaptic weights, etc.
    
    NLM_LOG_INFO("Brain state import is a placeholder - not yet implemented");
}

// Performance and debugging methods
void AgentBrain::recordPerformanceMetrics(double timestep) {
    if (performanceMetricsEnabled_) {
        double currentTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Record timestep
        recordedTimesteps_.push_back(currentTime);
        
        // Calculate metrics
        if (recordedTimesteps_.size() > 1) {
            double dt = recordedTimesteps_.back() - recordedTimesteps_.front();
            if (dt > 0.0) {
                double stepsPerSecond = (recordedTimesteps_.size() - 1) / dt;
                performanceMetrics_.stepsPerSecond = stepsPerSecond;
                performanceMetrics_.avgStepTime = dt / (recordedTimesteps_.size() - 1);
            }
        }
        
        // Record memory usage (placeholder)
        performanceMetrics_.memoryUsage = recordMemoryUsage();
        
        // Record neuron activity metrics
        performanceMetrics_.neuronActivity = brain_->getFiringNeuronCount();
        performanceMetrics_.spikeCount = brain_->getTotalSpikeCount();
        
        // Log metrics periodically
        if (recordedTimesteps_.size() % 100 == 0) {
            logPerformanceMetrics();
        }
    }
}

void AgentBrain::logPerformanceMetrics() {
    if (performanceMetricsEnabled_) {
        NLM_LOG_INFO("=== Performance Metrics ===");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(performanceMetrics_.stepsPerSecond));
        NLM_LOG_INFO("  Average step time: " + std::to_string(performanceMetrics_.avgStepTime * 1000) + " ms");
        NLM_LOG_INFO("  Memory usage: " << performanceMetrics_.memoryUsage << " MB");
        NLM_LOG_INFO("  Active neurons: " << performanceMetrics_.neuronActivity << "/" << brain_->getTotalNeuronCount());
        NLM_LOG_INFO("  Total spikes: " << performanceMetrics_.spikeCount << "/" << brain_->getTotalSpikeCount());
        NLM_LOG_INFO("  Neuromodulation levels - Dopamine: " << dopamineLevel_ 
                    ", Curiosity: " << curiosityLevel_ 
                    ", Novelty: " << noveltyLevel_);
        NLM_LOG_INFO("============================");
    }
}

double AgentBrain::recordMemoryUsage() const {
    // Record current memory usage (placeholder implementation)
    // In a real implementation, this would use system calls to get memory usage
    return 0.0;  // Placeholder
}

void AgentBrain::startPerformanceTimer() {
    if (performanceMetricsEnabled_) {
        timerStart_ = std::chrono::high_resolution_clock::now();
    }
}

double AgentBrain::stopPerformanceTimer() {
    if (performanceMetricsEnabled_ && timerStart_ != std::chrono::steady_clock::time_point()) {
        auto timerEnd = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd - timerStart_);
        timerStart_ = std::chrono::steady_clock::time_point();
        return duration.count() / 1000.0;  // Convert to seconds
    }
    return 0.0;
}

void AgentBrain::exportDebugInfo(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting debug information to: " << filepath);
    
    std::ofstream outFile(filepath);
    if (outFile.is_open()) {
        outFile << "=== NLM Debug Information ===\n";
        outFile << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        // Write brain state
        outFile << "Brain State:\n";
        outFile << "  Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
        outFile << "  Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
        outFile << "  Active Neurons: " << brain_->getFiringNeuronCount() << "\n";
        outFile << "  Total Spikes: " << brain_->getTotalSpikeCount() << "\n";
        outFile << "  Average Firing Rate: " << brain_->getAverageFiringRate() << "\n\n";
        
        // Write agent state
        outFile << "Agent State:\n";
        outFile << "  Developmental Stage: " << static_cast<int>(getDevelopmentalStage()) << "\n";
        outFile << "  Dopamine Level: " << getNeuromodulationLevel() << "\n";
        outFile << "  Curiosity Level: " << getCuriosityLevel() << "\n";
        outFile << "  Novelty Level: " << getNoveltyLevel() << "\n";
        outFile << "  Prediction Error: " << getPredictionError() << "\n\n";
        
        // Write performance metrics
        if (performanceMetricsEnabled_) {
            outFile << "Performance Metrics:\n";
            outFile << "  Steps per second: " << performanceMetrics_.stepsPerSecond << "\n";
            outFile << "  Average step time: " << performanceMetrics_.avgStepTime * 1000 << " ms\n";
            outFile << "  Memory usage: " << performanceMetrics_.memoryUsage << " MB\n";
            outFile << "  Neuron activity: " << performanceMetrics_.neuronActivity << "\n";
            outFile << "  Spike count: " << performanceMetrics_.spikeCount << "\n\n";
        }
        
        outFile << "=== End Debug Information ===\n";
        outFile.close();
        
        NLM_LOG_INFO("Debug information exported successfully");
    } else {
        NLM_LOG_WARNING("Failed to export debug information to: " << filepath);
    }
}

std::string AgentBrain::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Performance and debugging methods
void AgentBrain::recordPerformanceMetrics(double timestep) {
    // Record performance metrics for analysis
    if (performanceMetricsEnabled_) {
        double currentTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        // Record timestep
        recordedTimesteps_.push_back(currentTime);
        
        // Calculate metrics
        if (recordedTimesteps_.size() > 1) {
            double dt = recordedTimesteps_.back() - recordedTimesteps_.front();
            if (dt > 0.0) {
                double stepsPerSecond = (recordedTimesteps_.size() - 1) / dt;
                performanceMetrics_.stepsPerSecond = stepsPerSecond;
                performanceMetrics_.avgStepTime = dt / (recordedTimesteps_.size() - 1);
            }
        }
        
        // Record memory usage (placeholder)
        performanceMetrics_.memoryUsage = recordMemoryUsage();
        
        // Record neuron activity metrics
        performanceMetrics_.neuronActivity = brain_->getFiringNeuronCount();
        performanceMetrics_.spikeCount = brain_->getTotalSpikeCount();
        
        // Log metrics periodically
        if (recordedTimesteps_.size() % 100 == 0) {
            logPerformanceMetrics();
        }
    }
}

void AgentBrain::logPerformanceMetrics() {
    if (performanceMetricsEnabled_) {
        NLM_LOG_INFO("=== Performance Metrics ===");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(performanceMetrics_.stepsPerSecond));
        NLM_LOG_INFO("  Average step time: " + std::to_string(performanceMetrics_.avgStepTime * 1000) + " ms");
        NLM_LOG_INFO("  Memory usage: " + std::to_string(performanceMetrics_.memoryUsage) + " MB");
        NLM_LOG_INFO("  Active neurons: " + std::to_string(performanceMetrics_.neuronActivity));
        NLM_LOG_INFO("  Total spikes: " + std::to_string(performanceMetrics_.spikeCount));
        NLM_LOG_INFO("  Neuromodulation levels - Dopamine: " + std::to_string(dopamineLevel_) + 
                    ", Curiosity: " + std::to_string(curiosityLevel_) + 
                    ", Novelty: " + std::to_string(noveltyLevel_));
        NLM_LOG_INFO("============================");
    }
}

double AgentBrain::recordMemoryUsage() const {
    // Record current memory usage (placeholder implementation)
    // In a real implementation, this would use system calls to get memory usage
    return 0.0;  // Placeholder
}

void AgentBrain::startPerformanceTimer() {
    if (performanceMetricsEnabled_) {
        timerStart_ = std::chrono::high_resolution_clock::now();
    }
}

double AgentBrain::stopPerformanceTimer() {
    if (performanceMetricsEnabled_ && timerStart_ != std::chrono::steady_clock::time_point()) {
        auto timerEnd = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd - timerStart_);
        timerStart_ = std::chrono::steady_clock::time_point();
        return duration.count() / 1000.0;  // Convert to seconds
    }
    return 0.0;
}

void AgentBrain::exportDebugInfo(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting debug information to: " + filepath);
    
    std::ofstream outFile(filepath);
    if (outFile.is_open()) {
        outFile << "=== NLM Debug Information ===\n";
        outFile << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        // Write brain state
        outFile << "Brain State:\n";
        outFile << "  Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
        outFile << "  Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
        outFile << "  Active Neurons: " << brain_->getFiringNeuronCount() << "\n";
        outFile << "  Total Spikes: " << brain_->getTotalSpikeCount() << "\n";
        outFile << "  Average Firing Rate: " << brain_->getAverageFiringRate() << "\n\n";
        
        // Write agent state
        outFile << "Agent State:\n";
        outFile << "  Developmental Stage: " << static_cast<int>(getDevelopmentalStage()) << "\n";
        outFile << "  Dopamine Level: " << getNeuromodulationLevel() << "\n";
        outFile << "  Curiosity Level: " << getCuriosityLevel() << "\n";
        outFile << "  Novelty Level: " << getNoveltyLevel() << "\n";
        outFile << "  Prediction Error: " << getPredictionError() << "\n\n";
        
        // Write performance metrics
        if (performanceMetricsEnabled_) {
            outFile << "Performance Metrics:\n";
            outFile << "  Steps per second: " << performanceMetrics_.stepsPerSecond << "\n";
            outFile << "  Average step time: " << performanceMetrics_.avgStepTime * 1000 << " ms\n";
            outFile << "  Memory usage: " << performanceMetrics_.memoryUsage << " MB\n";
            outFile << "  Neuron activity: " << performanceMetrics_.neuronActivity << "\n";
            outFile << "  Spike count: " << performanceMetrics_.spikeCount << "\n\n";
        }
        
        outFile << "=== End Debug Information ===\n";
        outFile.close();
        
        NLM_LOG_INFO("Debug information exported successfully");
    } else {
        NLM_LOG_WARNING("Failed to export debug information to: " + filepath);
    }
}

std::string AgentBrain::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
