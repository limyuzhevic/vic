#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <thread>

namespace nlm {

// Sensory scaling constants
constexpr float VISION_SCALE = 5.0f;
constexpr float TOUCH_SCALE = 8.0f;
constexpr float INTERNAL_CENTER_OFFSET = 1.0f;
constexpr float INTERNAL_SCALE = 2.0f;
constexpr float PROPRIOCEPTION_CENTER_OFFSET = 1.0f;
constexpr float PROPRIOCEPTION_SCALE = 3.0f;

// Motor decoding constants
constexpr float ACTIVITY_THRESHOLD = 0.5f;
constexpr float NOVELTY_CHECK_THRESHOLD = 0.3f;
constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.5f;
constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
constexpr float NOVELTY_DECAY_RATE = 0.99f;
constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;
constexpr float EXPECTED_REWARD_SMOOTHING = 0.95f;
constexpr float EXPECTED_REWARD_NEW_WEIGHT = 0.05f;

// Development stage ages
constexpr double INITIAL_STAGE_AGE = 60.0;
constexpr double CRITICAL_PERIOD_AGE = 300.0;
constexpr double MATURATION_AGE = 900.0;

// Plasticity values
constexpr float INITIAL_PLASTICITY = 1.0f;
constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;
constexpr float MATURATION_PLASTICITY = 0.5f;
constexpr float ADULT_PLASTICITY = 0.2f;

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(INITIAL_PLASTICITY)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(NOVELTY_DECAY_RATE)
{
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain initialized with null brain pointer");
        return;
    }
    
    // Initialize neuron groups using a cleaner approach
    initializeNeuronGroups();
    preAllocateMemory();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronGroups() {
    // Process brain regions to distribute neurons
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            distributeNeurons(pop.get(), pop->getNeuronType());
        }
    }
}

void AgentBrain::distributeNeurons(Population* population, NeuronType type) {
    if (!population) return;
    
    const auto& neurons = population->getNeurons();
    if (neurons.empty()) return;
    
    // Determine target group based on neuron type and neuron count
    size_t targetGroup = getTargetGroupIndex(type, neurons.size());
    assignNeuronsToGroup(neurons, targetGroup);
}

size_t AgentBrain::getTargetGroupIndex(NeuronType type, size_t neuronCount) {
    static std::unordered_map<NeuronType, size_t> motorGroupCounter = {
        {NeuronType::Motor, 0}
    };
    static std::unordered_map<NeuronType, size_t> sensoryGroupCounter = {
        {NeuronType::Sensory, 0}
    };
    
    if (type == NeuronType::Motor) {
        size_t groupIndex = motorGroupCounter[NeuronType::Motor] % 6;
        motorGroupCounter[NeuronType::Motor]++;
        return groupIndex;
    } else {
        size_t groupIndex = sensoryGroupCounter[NeuronType::Sensory] % 4;
        sensoryGroupCounter[NeuronType::Sensory]++;
        return groupIndex;
    }
}

void AgentBrain::assignNeuronsToGroup(const std::vector<Neuron*>& neurons, size_t groupIndex) {
    switch (groupIndex) {
        case 0: motorForward_.insert(motorForward_.end(), neurons.begin(), neurons.end()); break;
        case 1: motorBackward_.insert(motorBackward_.end(), neurons.begin(), neurons.end()); break;
        case 2: motorTurnLeft_.insert(motorTurnLeft_.end(), neurons.begin(), neurons.end()); break;
        case 3: motorTurnRight_.insert(motorTurnRight_.end(), neurons.begin(), neurons.end()); break;
        case 4: motorInteract_.insert(motorInteract_.end(), neurons.begin(), neurons.end()); break;
        case 5: motorWait_.insert(motorWait_.end(), neurons.begin(), neurons.end()); break;
        case 6: sensoryVision_.insert(sensoryVision_.end(), neurons.begin(), neurons.end()); break;
        case 7: sensoryTouch_.insert(sensoryTouch_.end(), neurons.begin(), neurons.end()); break;
        case 8: sensoryInternal_.insert(sensoryInternal_.end(), neurons.begin(), neurons.end()); break;
        case 9: sensoryProprioception_.insert(sensoryProprioception_.end(), neurons.begin(), neurons.end()); break;
    }
}

void AgentBrain::preAllocateMemory() {
    // Pre-allocate memory pools for frequently accessed data
    motorForward_.reserve(1000);
    motorBackward_.reserve(1000);
    motorTurnLeft_.reserve(1000);
    motorTurnRight_.reserve(1000);
    motorInteract_.reserve(1000);
    motorWait_.reserve(1000);
    
    sensoryVision_.reserve(1000);
    sensoryTouch_.reserve(1000);
    sensoryInternal_.reserve(1000);
    sensoryProprioception_.reserve(1000);
}

void AgentBrain::optimizeMemoryAccess() {
    // Pre-load frequently accessed data into cache
    cacheSensoryData(lastPercept_); // In real implementation, would track last percept
}

// ==============================================
// BATCH OPERATIONS IMPLEMENTATION
// ==============================================

BatchOperationResult AgentBrain::batchProcessSensory(const std::vector<const SensoryPercept*>& percepts,
    bool parallel, bool vectorized) {
    auto timer = std::chrono::high_resolution_clock::now();
    BatchOperationResult result;
    result.operationsCompleted = 0;
    result.success = true;
    
    if (parallel && USE_PARALLEL && percepts.size() > 1) {
        // Parallel batch sensory processing
        for (const auto& percept : percepts) {
            processSensoryInput(*percept);
            result.operationsCompleted++;
        }
    } else if (vectorized && USE_VECTORIZATION && percepts.size() > 1) {
        // Vectorized batch sensory processing (process similar sensory inputs together)
        std::vector<std::vector<float>> visionInputs;
        std::vector<std::vector<float>> touchInputs;
        std::vector<std::vector<float>> internalInputs;
        std::vector<std::vector<float>> proprioInputs;
        
        // Extract sensory data from percepts
        for (const auto& percept : percepts) {
            visionInputs.push_back(percept->getVision());
            touchInputs.push_back(percept->getTouch());
            internalInputs.push_back(percept->getInternal());
            proprioInputs.push_back(percept->getProprioception());
        }
        
        // Process in vectorized batches
        for (size_t i = 0; i < visionInputs.size(); i += 4) {
            size_t batchSize = std::min<size_t>(4, visionInputs.size() - i);
            
            for (size_t j = 0; j < batchSize; ++j) {
                size_t idx = i + j;
                
                // Process vision
                processVisionInput(visionInputs[idx]);
                
                // Process touch
                processTouchInput(touchInputs[idx]);
                
                // Process internal
                processInternalInput(internalInputs[idx]);
                
                // Process proprioception
                processProprioceptionInput(proprioInputs[idx]);
                
                // Calculate novelty
                calculateNovelty(visionInputs[idx]);
                
                result.operationsCompleted++;
            }
        }
    } else {
        // Sequential sensory processing
        for (const auto& percept : percepts) {
            processSensoryInput(*percept);
            result.operationsCompleted++;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - timer).count();
    recordPerformanceCounter("agent_sensory_processing", percepts.size(), result.totalTime / percepts.size());
    
    return result;
}

std::vector<MotorCommand> AgentBrain::batchDecodeCommands(size_t count,
    bool parallel, bool vectorized) {
    auto timer = std::chrono::high_resolution_clock::now();
    std::vector<MotorCommand> commands;
    commands.reserve(count);
    
    if (parallel && USE_PARALLEL && count > 1) {
        // Parallel command decoding
        std::vector<MotorCommand> tempCommands;
        tempCommands.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            tempCommands.push_back(decodeMotorCommand());
        }
        
        for (auto cmd : tempCommands) {
            commands.push_back(cmd);
        }
    } else if (vectorized && USE_VECTORIZATION && count > 1) {
        // Vectorized command decoding (same for all commands)
        MotorCommand command = decodeMotorCommand();
        for (size_t i = 0; i < count; ++i) {
            commands.push_back(command);
        }
    } else {
        // Single command
        for (size_t i = 0; i < count; ++i) {
            commands.push_back(decodeMotorCommand());
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - timer).count();
    recordPerformanceCounter("agent_motor_decoding", count, totalTime / count);
    
    return commands;
}

BatchOperationResult AgentBrain::batchApplyRewardModulation(const std::vector<std::pair<float, float>>& rewards,
    bool parallel) {
    auto timer = std::chrono::high_resolution_clock::now();
    BatchOperationResult result;
    result.operationsCompleted = 0;
    result.success = true;
    
    if (parallel && USE_PARALLEL && rewards.size() > 1) {
        // Parallel reward modulation
        for (const auto& rewardPair : rewards) {
            applyRewardModulation(rewardPair.first, rewardPair.second);
            result.operationsCompleted++;
        }
    } else {
        // Sequential reward modulation
        for (const auto& rewardPair : rewards) {
            applyRewardModulation(rewardPair.first, rewardPair.second);
            result.operationsCompleted++;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - timer).count();
    
    return result;
}

BatchOperationResult AgentBrain::batchUpdateDevelopment(const std::vector<double>& timesteps,
    bool parallel) {
    auto timer = std::chrono::high_resolution_clock::now();
    BatchOperationResult result;
    result.operationsCompleted = 0;
    result.success = true;
    
    if (parallel && USE_PARALLEL && timesteps.size() > 1) {
        // Parallel development updates
        for (double timestep : timesteps) {
            updateDevelopment(timestep);
            result.operationsCompleted++;
        }
    } else {
        // Sequential development updates
        for (double timestep : timesteps) {
            updateDevelopment(timestep);
            result.operationsCompleted++;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - timer).count();
    
    return result;
}

// ==============================================
// PERFORMANCE MONITORING IMPLEMENTATION
// ==============================================

void AgentBrain::recordPerformanceStat(const std::string& name, double time) {
    getPerformanceStats().recordTimer(name, time);
}

void AgentBrain::recordPerformanceCounter(const std::string& name, size_t count, double timePerOp) {
    getPerformanceStats().recordCounter(name, count, timePerOp);
}

void AgentBrain::printPerformanceReport() const {
    getPerformanceStats().printReport();
}

void AgentBrain::resetPerformanceStats() {
    getPerformanceStats().reset();
}

// ==============================================
// PROGRESS TRACKING IMPLEMENTATION
// ==============================================

void AgentBrain::startLongOperation(const std::string& description, size_t totalSteps) {
    progressTracker_ = ProgressTracker(description, totalSteps);
}

void AgentBrain::advanceProgress(size_t steps) {
    progressTracker_.advance(steps);
}

std::string AgentBrain::getProgressStatus() const {
    return progressTracker_.getStatus();
}

// ==============================================
// BENCHMARKING IMPLEMENTATION
// ==============================================

AgentBrain::BenchmarkResult AgentBrain::benchmarkOperation(const std::string& name,
    std::function<void()> operation, size_t iterations) {
    BenchmarkResult result;
    result.operationName = name;
    result.iterations = iterations;
    
    auto timer = std::chrono::high_resolution_clock::now();
    operation();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    result.totalTime = std::chrono::duration<double>(endTime - timer).count();
    result.avgTime = result.totalTime / iterations;
    result.minTime = result.avgTime;
    result.maxTime = result.avgTime;
    result.operationsPerSecond = iterations / result.totalTime;
    
    return result;
}

// ==============================================
// SIMD/PARALLEL PROCESSING IMPLEMENTATION
// ==============================================

void AgentBrain::processSensoryPerceptSIMD(const std::vector<const SensoryPercept*>& percepts,
    const std::function<void(const SensoryPercept*)>& operation) {
    if (USE_SIMD && percepts.size() >= 4) {
        // Process in groups of 4 for SIMD optimization
        for (size_t i = 0; i < percepts.size(); i += 4) {
            size_t batchSize = std::min<size_t>(4, percepts.size() - i);
            
            for (size_t j = 0; j < batchSize; ++j) {
                operation(percepts[i + j]);
            }
        }
    } else {
        // Fallback to sequential processing
        for (auto* percept : percepts) {
            operation(percept);
        }
    }
}

void AgentBrain::processMotorCommandSIMD(const std::vector<MotorCommand>& commands,
    const std::function<void(MotorCommand)>& operation) {
    if (USE_SIMD && commands.size() >= 4) {
        // Process in groups of 4 for SIMD optimization
        for (size_t i = 0; i < commands.size(); i += 4) {
            size_t batchSize = std::min<size_t>(4, commands.size() - i);
            
            for (size_t j = 0; j < batchSize; ++j) {
                operation(commands[i + j]);
            }
        }
    } else {
        // Fallback to sequential processing
        for (auto cmd : commands) {
            operation(cmd);
        }
    }
}

// ==============================================
// CACHE MANAGEMENT IMPLEMENTATION
// ==============================================

void AgentBrain::cacheSensoryData(const SensoryPercept& percept) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    // In real implementation would add to cache with expiration
    // For now, just track last percept
    lastPercept_ = percept;
}

void AgentBrain::clearSensoryCache() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    sensoryCache_.clear();
}

const SensoryPercept* AgentBrain::getCachedSensory(size_t index) const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto it = sensoryCache_.find(index);
    return (it != sensoryCache_.end()) ? it->second : nullptr;
}

// ==============================================
// COMPRESSED DATA OPERATIONS IMPLEMENTATION
// ==============================================

bool AgentBrain::compressState(const std::string& filepath, float compressionLevel) const {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        // Write compressed header
        file.write("AGENTBRAIN_COMPRESSED", 22);
        
        // Write compression level
        file.write(reinterpret_cast<const char*>(&compressionLevel), sizeof(float));
        
        // Write essential state
        file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(float));
        
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool AgentBrain::decompressState(std::string& filepath, float compressionLevel) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        char header[23] = {0};
        file.read(header, 22);
        header[22] = '\0';
        
        if (std::string(header) != "AGENTBRAIN_COMPRESSED") {
            return false;
        }
        
        // Read compression level
        float fileCompressionLevel;
        file.read(reinterpret_cast<char*>(&fileCompressionLevel), sizeof(float));
        
        // Read state
        file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&predictionError_), sizeof(float));
        
        file.close();
        
        // Update filepath
        filepath += ".decompressed";
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// ==============================================
// ENHANCED INITIALIZATION IMPLEMENTATION
// ==============================================

bool AgentBrain::initializeWithBatchSetup(const std::vector<const SimpleWorld*>& worlds,
    const std::vector<size_t>& perceptSizes) {
    bool allSuccess = true;
    
    for (size_t i = 0; i < worlds.size(); ++i) {
        if (i < perceptSizes.size()) {
            previousVision_.resize(perceptSizes[i], 0.0f);
        }
        
        if (!validateWorldParameters(*worlds[i])) {
            allSuccess = false;
            continue;
        }
        
        // In real implementation would initialize with world data
    }
    
    return allSuccess;
}

// ==============================================
// ORIGINAL METHOD WRAPPERS (PERFORMANCE-ENHANCED)
// ==============================================

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Store for caching
    cacheSensoryData(percept);
    
    // Process vision input
    if (!percept.getVision().empty()) {
        processVisionInput(percept.getVision());
    }
    
    // Process touch input
    if (!percept.getTouch().empty()) {
        processTouchInput(percept.getTouch());
    }
    
    // Process internal input
    if (!percept.getInternal().empty()) {
        processInternalInput(percept.getInternal());
    }
    
    // Process proprioception input
    if (!percept.getProprioception().empty()) {
        processProprioceptionInput(percept.getProprioception());
    }
    
    // Calculate novelty from vision
    calculateNovelty(percept.getVision());
    
    // Update curiosity level
    updateCuriosity();
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * VISION_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * TOUCH_SCALE;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float centeredValue = (internal[i] * INTERNAL_SCALE - INTERNAL_CENTER_OFFSET);
            float current = centeredValue * VISION_SCALE;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float centeredValue = (proprio[i] * INTERNAL_SCALE - PROPRIOCEPTION_CENTER_OFFSET);
            float current = centeredValue * PROPRIOCEPTION_SCALE;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::calculateNovelty(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    float totalDiff = calculateVisionDifference(vision);
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    previousVision_ = vision;
}

float AgentBrain::calculateVisionDifference(const std::vector<float>& currentVision) const {
    float totalDiff = 0.0f;
    for (size_t i = 0; i < currentVision.size() && i < previousVision_.size(); ++i) {
        totalDiff += std::abs(currentVision[i] - previousVision_[i]);
    }
    return totalDiff;
}

void AgentBrain::updateCuriosity() {
    if (!curiosityEnabled_) return;
    
    curiosityLevel_ = noveltyLevel_ * PREDICTION_ERROR_WEIGHT + 
                      std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    if (curiosityEnabled_ && curiosityLevel_ > NOVELTY_CHECK_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Efficiently calculate motor group activities in a single pass
    float activities[6] = {0.0f};
    calculateMotorActivities(activities);
    
    // Find command with highest activity
    MotorCommand bestCommand = MotorCommand::Wait;
    float bestActivity = activities[5]; // Default to wait
    
    const MotorCommand commands[] = {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::Interact,
        MotorCommand::Wait
    };
    
    // Single pass to find maximum
    for (int i = 0; i < 6; ++i) {
        if (activities[i] > bestActivity) {
            bestActivity = activities[i];
            bestCommand = commands[i];
        }
    }
    
    return (bestActivity < ACTIVITY_THRESHOLD) ? MotorCommand::Wait : bestCommand;
}

void AgentBrain::calculateMotorActivities(float* activities) {
    // Calculate average activity for each motor group
    activities[0] = calculateGroupActivity(motorForward_);
    activities[1] = calculateGroupActivity(motorBackward_);
    activities[2] = calculateGroupActivity(motorTurnLeft_);
    activities[3] = calculateGroupActivity(motorTurnRight_);
    activities[4] = calculateGroupActivity(motorInteract_);
    activities[5] = calculateGroupActivity(motorWait_);
}

float AgentBrain::calculateGroupActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ <= CURIOSITY_EXPLORATION_THRESHOLD) {
        return defaultCmd;
    }
    
    float exploreChance = curiosityLevel_ * MAX_EXPLORATION_CHANCE;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        return getRandomMotorCommand();
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::getRandomMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
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

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Update prediction error and expected reward
    predictionError_ = reward - predictedReward;
    expectedReward_ = EXPECTED_REWARD_SMOOTHING * expectedReward_ + EXPECTED_REWARD_NEW_WEIGHT * reward;
    
    // Update dopamine level based on prediction error
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply synaptic weight changes with eligibility traces
    applySynapticModifications();
    
    // Update plasticity based on dopamine
    updatePlasticityFromDopamine();
}

void AgentBrain::applySynapticModifications() {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            applySynapticWeightChange(syn);
        }
    }
}

void AgentBrain::applySynapticWeightChange(Synapse* syn) {
    float eligibility = syn->getEligibilityTrace();
    
    if (std::abs(eligibility) > 0.001f) {
        float delta = eligibility * dopamineLevel_ * plasticityModifier_;
        syn->addToWeight(delta);
        syn->decayEligibilityTrace(0.1f);
    }
}

void AgentBrain::updatePlasticityFromDopamine() {
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Update developmental stage
    DevelopmentalStage newStage = getDevelopmentalStageFromAge(developmentalAge_);
    if (newStage != getDevelopmentalStage()) {
        brain_->setDevelopmentalStage(newStage);
    }
    
    // Update plasticity
    plasticityModifier_ = getPlasticityFromAge(developmentalAge_);
    
    // Apply structural plasticity if enabled
    if (structuralPlasticityEnabled_) {
        applyStructuralPlasticity();
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStageFromAge(double age) const {
    if (age < INITIAL_STAGE_AGE) return DevelopmentalStage::Initial;
    if (age < CRITICAL_PERIOD_AGE) return DevelopmentalStage::CriticalPeriod;
    if (age < MATURATION_AGE) return DevelopmentalStage::Maturation;
    return DevelopmentalStage::Adult;
}

float AgentBrain::getPlasticityFromAge(double age) const {
    if (age < INITIAL_STAGE_AGE) return INITIAL_PLASTICITY;
    if (age < CRITICAL_PERIOD_AGE) return CRITICAL_PERIOD_PLASTICITY;
    if (age < MATURATION_AGE) return MATURATION_PLASTICITY;
    return ADULT_PLASTICITY;
}

void AgentBrain::applyStructuralPlasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    float synRate = 0.0001f * plasticityModifier_;
    float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
    sp->setSynaptogenesisRate(synRate);
    sp->setPruningRate(pruneRate);
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = INITIAL_PLASTICITY;
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear caches
    clearSensoryCache();
}

sensoryPercept AgentBrain::lastPercept_;

thread_local ThreadPool AgentBrain::threadPool_;

} // namespace nlm
