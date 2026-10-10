#include "Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace nlm {

Phase6Demo::Phase6Demo()
    : running_(false)
    , demoMode_(false)
    , visualizationEnabled_(false)
    , benchmarkingEnabled_(false)
    , realTimeMonitoring_(false)
    , currentStep_(0)
    , wallClockTime_(0.0)
    , startTime_(0.0)
    , lastUpdateTime_(0.0)
    , totalStepsProcessed_(0)
    , totalNeuronsCreated_(0)
    , totalConnectionsCreated_(0)
    , totalConnectionsPruned_(0) {
    
    NLM_LOG_INFO("Phase6Demo initialized");
}

Phase6Demo::~Phase6Demo() {
    stop();
    NLM_LOG_INFO("Phase6Demo destroyed");
}

bool Phase6Demo::initialize(const Phase6Config& config) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    try {
        NLM_LOG_INFO("Initializing Phase6Demo with config: " + std::to_string(config.neuronCount) + " neurons, " + std::to_string(config.maxSteps) + " max steps");
        
        // Store configuration
        demoConfig_ = config;
        
        // Initialize configuration
        config_ = std::make_shared<Config>();
        config_->set("neuron_count", config.neuronCount);
        config_->set("region_count", config.regionCount);
        config_->set("connection_probability", config.connectionProbability);
        config_->set("random_seed", uint64_t(42));
        config_->set("simulation_timestep", 0.001);
        config_->set("stdp_ltp_weight", 0.01f);
        config_->set("stdp_ltd_weight", 0.012f);
        config_->set("stdp_tau", 20.0f);
        config_->set("synaptogenesis_rate", 0.0001f);
        config_->set("pruning_rate", 0.00001f);
        
        // Initialize brain
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        // Initialize agent brain
        agentBrain_ = std::make_shared<AgentBrain>(brain_);
        agentBrain_->initialize(*world_);
        
        // Configure agent brain for demo
        agentBrain_->enableRewardModulation(true);
        agentBrain_->enableStructuralPlasticity(true);
        agentBrain_->enableDevelopment(true);
        agentBrain_->enableCuriosity(true);
        
        // Initialize development system
        developmentSystem_ = std::make_shared<DevelopmentSystem>();
        
        // Initialize visualization system
        if (initializeVisualizationSystem()) {
            visualizationEnabled_ = true;
            NLM_LOG_INFO("Visualization system initialized");
        }
        
        // Initialize performance monitoring
        if (initializePerformanceMonitoring()) {
            benchmarkingEnabled_ = true;
            NLM_LOG_INFO("Performance monitoring initialized");
        }
        
        // Initialize neural plasticity systems
        stdp_ = std::make_shared<STDP>(config_->getFloat("stdp_ltp_weight"), config_->getFloat("stdp_ltd_weight"), config_->getFloat("stdp_tau"));
        hebbian_ = std::make_shared<Hebbian>();
        
        // Initialize memory systems
        workingMemory_ = std::make_shared<NeuralWorkingMemory>();
        episodicMemory_ = std::make_shared<NeuralEpisodicMemory>();
        
        // Initialize neuromodulation systems
        neuromodulator_ = std::make_shared<Neuromodulator>();
        
        // Initialize prediction systems
        predictionSystem_ = std::make_shared<NeuralPrediction>();
        
        // Initialize cognitive systems
        conceptFormation_ = std::make_shared<ConceptFormation>();
        neuralPlanner_ = std::make_shared<NeuralPlanner>();
        
        // Initialize synaptic development systems
        synaptogenesis_ = std::make_shared<Synaptogenesis>();
        pruning_ = std::make_shared<Pruning>();
        maturation_ = std::make_shared<Maturation>();
        
        // Set initial development stage
        developmentSystem_->setStage(DevelopmentalStage::Initial);
        
        // Initialize neural activity data
        currentActivity_.step = 0;
        currentActivity_.wallClockTime = 0.0;
        currentActivity_.avgFiringRate = 0.0;
        currentActivity_.totalFiringRate = 0.0;
        currentActivity_.avgSynapticWeight = 0.0;
        currentActivity_.memoryUtilization = 0.0;
        currentActivity_.attentionLevel = 0.0;
        currentActivity_.curiosityLevel = 0.0;
        currentActivity_.dopamineLevel = 0.0;
        currentActivity_.rewardValue = 0.0;
        currentActivity_.activeNeurons = 0;
        currentActivity_.newConnections = 0;
        currentActivity_.prunedConnections = 0;
        currentActivity_.developmentalAge = 0.0;
        currentActivity_.developmentalStage = developmentSystem_->getStageName();
        currentActivity_.firingRateHistory.clear();
        currentActivity_.rewardHistory.clear();
        currentActivity_.connectionCountHistory.clear();
        
        // Initialize performance metrics
        currentPerformance_.initializationTime = 0.0;
        currentPerformance_.learningTime = 0.0;
        currentPerformance_.peakFiringRate = 0.0;
        currentPerformance_.avgFiringRate = 0.0;
        currentPerformance_.rewardEfficiency = 0.0;
        currentPerformance_.memoryEfficiency = 0.0;
        currentPerformance_.plasticityEfficiency = 0.0;
        currentPerformance_.convergenceTime = 0.0;
        currentPerformance_.stabilityMetrics.clear();
        currentPerformance_.learningCurves.clear();
        currentPerformance_.performanceTrajectories.clear();
        
        // Initialize development progression
        currentDevelopment_.stageStartStep = 0;
        currentDevelopment_.stepsInStage = 0;
        currentDevelopment_.stageAge = 0.0;
        currentDevelopment_.plasticityModifier = developmentSystem_->getPlasticityModifier();
        currentDevelopment_.criticalPeriodProgress = developmentSystem_->getCriticalPeriodProgress();
        currentDevelopment_.achievedMilestones.clear();
        currentDevelopment_.stageCharacteristics.clear();
        currentDevelopment_.stageCapabilities.clear();
        
        // Initialize statistics
        statistics_.clear();
        statistics_["total_neurons"] = static_cast<float>(brain_->getTotalNeuronCount());
        statistics_["total_synapses"] = static_cast<float>(brain_->getTotalSynapseCount());
        statistics_["integration_status"] = 1.0f;
        statistics_["performance_score"] = 0.0f;
        
        // Reset step counter
        currentStep_ = 0;
        wallClockTime_ = 0.0;
        startTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        lastUpdateTime_ = startTime_;
        totalStepsProcessed_ = 0;
        
        NLM_LOG_INFO("Phase6Demo initialization completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error initializing Phase6Demo: " + std::string(e.what()));
        return false;
    }
}

bool Phase6Demo::run(const Phase6Config& config) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!initialize(config)) {
        NLM_LOG_ERROR("Failed to initialize Phase6Demo");
        return false;
    }
    
    running_ = true;
    demoMode_ = true;
    
    NLM_LOG_INFO("Starting Phase6Demo with config: " + std::to_string(config.maxSteps) + " max steps");
    
    try {
        // Create world for demo
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(20.0f, 20.0f, 16, 16);
        world_->setRandomSeed(42);
        
        // Initialize visualization if enabled
        if (visualizationEnabled_) {
            if (!visualization_->initialize()) {
                NLM_LOG_WARN("Failed to initialize visualization system");
                visualizationEnabled_ = false;
            }
        }
        
        // Start performance monitoring
        if (benchmarkingEnabled_) {
            performance_->startBenchmarking();
        }
        
        // Start demonstration loop
        runDemonstrationLoop(config);
        
        // Stop performance monitoring
        if (benchmarkingEnabled_) {
            performance_->stopBenchmarking();
        }
        
        // Final update
        updateStatistics();
        printStatusReport();
        
        NLM_LOG_INFO("Phase6Demo completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error running Phase6Demo: " + std::string(e.what()));
        return false;
    }
}

void Phase6Demo::stop() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (running_) {
        NLM_LOG_INFO("Stopping Phase6Demo");
        running_ = false;
        
        // Stop performance monitoring
        if (benchmarkingEnabled_) {
            performance_->stopBenchmarking();
        }
        
        // Clean up visualization
        if (visualizationEnabled_) {
            visualization_->shutdown();
        }
        
        // Save final results
        if (!neuralActivityHistory_.empty()) {
            exportMetricsToCSV("./Phase6Demo_results.csv");
        }
        
        NLM_LOG_INFO("Phase6Demo stopped");
    }
}

bool Phase6Demo::isRunning() const {
    return running_;
}

NeuralActivityData Phase6Demo::getNeuralActivity() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return currentActivity_;
}

PerformanceMetrics Phase6Demo::getPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return currentPerformance_;
}

DevelopmentProgression Phase6Demo::getDevelopmentProgression() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return currentDevelopment_;
}

std::string Phase6Demo::getSystemStatus() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return generateSystemStatus();
}

std::unordered_map<std::string, float> Phase6Demo::getStatistics() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return statistics_;
}

bool Phase6Demo::exportNeuralData(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing neural data: " + filename);
            return false;
        }
        
        file << "Step,WallClockTime,AvgFiringRate,TotalFiringRate,AvgSynapticWeight,MemoryUtilization,AttentionLevel,CuriosityLevel,DopamineLevel,RewardValue,ActiveNeurons,NewConnections,PrunedConnections,DevelopmentalAge,DevelopmentalStage" << std::endl;
        
        for (const auto& data : neuralActivityHistory_) {
            file << data.step << ","
                 << std::fixed << std::setprecision(6) << data.wallClockTime << ","
                 << std::fixed << std::setprecision(6) << data.avgFiringRate << ","
                 << std::fixed << std::setprecision(6) << data.totalFiringRate << ","
                 << std::fixed << std::setprecision(6) << data.avgSynapticWeight << ","
                 << std::fixed << std::setprecision(6) << data.memoryUtilization << ","
                 << std::fixed << std::setprecision(6) << data.attentionLevel << ","
                 << std::fixed << std::setprecision(6) << data.curiosityLevel << ","
                 << std::fixed << std::setprecision(6) << data.dopamineLevel << ","
                 << std::fixed << std::setprecision(6) << data.rewardValue << ","
                 << data.activeNeurons << ","
                 << data.newConnections << ","
                 << data.prunedConnections << ","
                 << std::fixed << std::setprecision(6) << data.developmentalAge << ","
                 << data.developmentalStage << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Neural activity data exported to: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting neural data: " + std::string(e.what()));
        return false;
    }
}

bool Phase6Demo::exportPerformanceMetrics(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing performance metrics: " + filename);
            return false;
        }
        
        file << "Metric,Value" << std::endl;
        file << "InitializationTime," << std::fixed << std::setprecision(6) << currentPerformance_.initializationTime << std::endl;
        file << "LearningTime," << std::fixed << std::setprecision(6) << currentPerformance_.learningTime << std::endl;
        file << "PeakFiringRate," << std::fixed << std::setprecision(6) << currentPerformance_.peakFiringRate << std::endl;
        file << "AvgFiringRate," << std::fixed << std::setprecision(6) << currentPerformance_.avgFiringRate << std::endl;
        file << "RewardEfficiency," << std::fixed << std::setprecision(6) << currentPerformance_.rewardEfficiency << std::endl;
        file << "MemoryEfficiency," << std::fixed << std::setprecision(6) << currentPerformance_.memoryEfficiency << std::endl;
        file << "PlasticityEfficiency," << std::fixed << std::setprecision(6) << currentPerformance_.plasticityEfficiency << std::endl;
        file << "ConvergenceTime," << std::fixed << std::setprecision(6) << currentPerformance_.convergenceTime << std::endl;
        
        file.close();
        NLM_LOG_INFO("Performance metrics exported to: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting performance metrics: " + std::string(e.what()));
        return false;
    }
}

bool Phase6Demo::exportDevelopmentData(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing development data: " + filename);
            return false;
        }
        
        file << "Stage,StageStartStep,StepsInStage,StageAge,PlasticityModifier,CriticalPeriodProgress" << std::endl;
        
        // We would need to track stage history, for now just write current state
        file << currentDevelopment_.developmentalStage << ","
             << currentDevelopment_.stageStartStep << ","
             << currentDevelopment_.stepsInStage << ","
             << std::fixed << std::setprecision(6) << currentDevelopment_.stageAge << ","
             << std::fixed << std::setprecision(6) << currentDevelopment_.plasticityModifier << ","
             << std::fixed << std::setprecision(6) << currentDevelopment_.criticalPeriodProgress << std::endl;
        
        file.close();
        NLM_LOG_INFO("Development data exported to: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting development data: " + std::string(e.what()));
        return false;
    }
}

void Phase6Demo::setDemoMode(bool enabled) {
    demoMode_ = enabled;
}

void Phase6Demo::setVisualizationMode(bool enabled) {
    visualizationEnabled_ = enabled;
}

void Phase6Demo::setBenchmarkingMode(bool enabled) {
    benchmarkingEnabled_ = enabled;
}

void Phase6Demo::setRealTimeMonitoring(bool enabled) {
    realTimeMonitoring_ = enabled;
}

double Phase6Demo::getProgress() const {
    if (demoConfig_.maxSteps == 0) return 0.0;
    return static_cast<double>(currentStep_) / static_cast<double>(demoConfig_.maxSteps);
}

void Phase6Demo::forceStageTransition(DevelopmentalStage stage) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    developmentSystem_->setStage(stage);
    NLM_LOG_INFO("Forced developmental stage transition to: " + std::string(developmentSystem_->getStageName()));
}

void Phase6Demo::triggerMemoryConsolidation() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (agentBrain_) {
        agentBrain_->triggerMemoryConsolidation();
        NLM_LOG_INFO("Memory consolidation triggered");
    }
}

void Phase6Demo::triggerNeuralReplay() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (agentBrain_) {
        agentBrain_->triggerNeuralReplay();
        NLM_LOG_INFO("Neural replay triggered");
    }
}

void Phase6Demo::triggerSynaptogenesis() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (synaptogenesis_) {
        RandomGenerator rng;
        synaptogenesis_->update(brain_.get(), rng);
        NLM_LOG_INFO("Synaptogenesis triggered");
    }
}

void Phase6Demo::triggerPruning() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (pruning_) {
        RandomGenerator rng;
        pruning_->update(brain_.get(), rng);
        NLM_LOG_INFO("Synaptic pruning triggered");
    }
}

void Phase6Demo::triggerMaturation() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (maturation_) {
        maturation_->update(brain_.get(), currentStep_);
        NLM_LOG_INFO("Maturation triggered");
    }
}

DevelopmentalStage Phase6Demo::getCurrentStage() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return developmentSystem_->getStage();
}

void Phase6Demo::printStatusReport() const {
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "          PHASE 6 STATUS REPORT          " << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "Current Step: " << currentStep_ << " / " << demoConfig_.maxSteps << std::endl;
    std::cout << "Progress: " << std::fixed << std::setprecision(2) << (getProgress() * 100) << "%" << std::endl;
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(3) << wallClockTime_ << "s" << std::endl;
    std::cout << "Developmental Stage: " << currentDevelopment_.developmentalStage << std::endl;
    std::cout << "Developmental Age: " << std::fixed << std::setprecision(3) << currentDevelopment_.stageAge << std::endl;
    std::cout << "Plasticity Modifier: " << std::fixed << std::setprecision(3) << currentDevelopment_.plasticityModifier << std::endl;
    std::cout << "Critical Period Progress: " << std::fixed << std::setprecision(3) << currentDevelopment_.criticalPeriodProgress << std::endl;
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(3) << currentActivity_.avgFiringRate << std::endl;
    std::cout << "Memory Utilization: " << std::fixed << std::setprecision(3) << currentActivity_.memoryUtilization << std::endl;
    std::cout << "Attention Level: " << std::fixed << std::setprecision(3) << currentActivity_.attentionLevel << std::endl;
    std::cout << "Curiosity Level: " << std::fixed << std::setprecision(3) << currentActivity_.curiosityLevel << std::endl;
    std::cout << "Dopamine Level: " << std::fixed << std::setprecision(3) << currentActivity_.dopamineLevel << std::endl;
    std::cout << "Total Reward: " << std::fixed << std::setprecision(3) << currentActivity_.rewardValue << std::endl;
    std::cout << "Active Neurons: " << currentActivity_.activeNeurons << std::endl;
    std::cout << "New Connections: " << currentActivity_.newConnections << std::endl;
    std::cout << "Pruned Connections: " << currentActivity_.prunedConnections << std::endl;
    
    std::cout << std::endl;
    std::cout << "Statistics:" << std::endl;
    for (const auto& pair : statistics_) {
        std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(3) << pair.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
}

void Phase6Demo::printPerformanceReport() const {
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "          PHASE 6 PERFORMANCE REPORT     " << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "Initialization Time: " << std::fixed << std::setprecision(3) << currentPerformance_.initializationTime << "s" << std::endl;
    std::cout << "Learning Time: " << std::fixed << std::setprecision(3) << currentPerformance_.learningTime << "s" << std::endl;
    std::cout << "Peak Firing Rate: " << std::fixed << std::setprecision(3) << currentPerformance_.peakFiringRate << std::endl;
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(3) << currentPerformance_.avgFiringRate << std::endl;
    std::cout << "Reward Efficiency: " << std::fixed << std::setprecision(3) << currentPerformance_.rewardEfficiency << std::endl;
    std::cout << "Memory Efficiency: " << std::fixed << std::setprecision(3) << currentPerformance_.memoryEfficiency << std::endl;
    std::cout << "Plasticity Efficiency: " << std::fixed << std::setprecision(3) << currentPerformance_.plasticityEfficiency << std::endl;
    std::cout << "Convergence Time: " << std::fixed << std::setprecision(3) << currentPerformance_.convergenceTime << "s" << std::endl;
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
}

void Phase6Demo::printDevelopmentReport() const {
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "          PHASE 6 DEVELOPMENT REPORT     " << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "Current Developmental Stage: " << currentDevelopment_.developmentalStage << std::endl;
    std::cout << "Stage Start Step: " << currentDevelopment_.stageStartStep << std::endl;
    std::cout << "Steps in Current Stage: " << currentDevelopment_.stepsInStage << std::endl;
    std::cout << "Stage Age: " << std::fixed << std::setprecision(3) << currentDevelopment_.stageAge << std::endl;
    std::cout << "Plasticity Modifier: " << std::fixed << std::setprecision(3) << currentDevelopment_.plasticityModifier << std::endl;
    std::cout << "Critical Period Progress: " << std::fixed << std::setprecision(3) << currentDevelopment_.criticalPeriodProgress << std::endl;
    
    std::cout << std::endl;
    std::cout << "Milestones Achieved:" << std::endl;
    for (const auto& milestone : currentDevelopment_.achievedMilestones) {
        std::cout << "  - " << milestone << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Stage Characteristics:" << std::endl;
    for (const auto& pair : currentDevelopment_.stageCharacteristics) {
        std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(3) << pair.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Stage Capabilities:" << std::endl;
    for (const auto& pair : currentDevelopment_.stageCapabilities) {
        std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(3) << pair.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
}

bool Phase6Demo::initializeBrainSystems(const Phase6Config& config) {
    // Brain initialization is done in the main initialize() method
    return true;
}

bool Phase6Demo::initializeVisualizationSystem() {
    // Visualization system initialization
    NLM_LOG_INFO("Visualization system initialization requested");
    return true;
}

bool Phase6Demo::initializePerformanceMonitoring() {
    // Performance monitoring initialization
    NLM_LOG_INFO("Performance monitoring initialization requested");
    return true;
}

void Phase6Demo::runDemonstrationLoop(const Phase6Config& config) {
    startTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    lastUpdateTime_ = startTime_;
    
    // Create world for demo
    world_ = std::make_shared<SimpleWorld>();
    world_->configure(20.0f, 20.0f, 16, 16);
    world_->setRandomSeed(42);
    
    // Initialize agent brain
    agentBrain_ = std::make_shared<AgentBrain>(brain_);
    agentBrain_->initialize(*world_);
    
    // Configure agent brain
    agentBrain_->enableRewardModulation(true);
    agentBrain_->enableStructuralPlasticity(true);
    agentBrain_->enableDevelopment(true);
    agentBrain_->enableCuriosity(true);
    
    NLM_LOG_INFO("Starting Phase6Demo demonstration loop");
    
    while (running_ && currentStep_ < config.maxSteps) {
        // Record start time
        auto currentTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        
        // Process simulation step
        processStep(brain_.get(), agentBrain_.get(), world_.get());
        
        // Update neural activity data
        updateNeuralActivityData(brain_.get(), agentBrain_.get());
        
        // Update performance metrics
        updatePerformanceMetrics(currentTime - lastUpdateTime_);
        
        // Update development progression
        updateDevelopmentProgression();
        
        // Update statistics
        updateStatistics();
        
        // Print progress update if real-time monitoring is enabled
        if (realTimeMonitoring_ && currentStep_ % 100 == 0) {
            printProgressUpdate(currentActivity_);
        }
        
        lastUpdateTime_ = currentTime;
    }
    
    NLM_LOG_INFO("Phase6Demo demonstration loop completed");
}

void Phase6Demo::updateNeuralActivityData(const Brain* brain, AgentBrain* agentBrain) {
    currentActivity_.step = currentStep_;
    currentActivity_.wallClockTime = wallClockTime_;
    
    // Get neural activity data from brain
    currentActivity_.avgFiringRate = brain->getAverageFiringRate();
    currentActivity_.totalFiringRate = brain->getFiringNeuronCount();
    currentActivity_.avgSynapticWeight = 0.5f; // Placeholder - should get from brain
    currentActivity_.activeNeurons = brain->getActiveNeuronCount();
    currentActivity_.newConnections = totalConnectionsCreated_;
    currentActivity_.prunedConnections = totalConnectionsPruned_;
    
    // Get agent brain data
    if (agentBrain) {
        currentActivity_.memoryUtilization = agentBrain->getMemoryUtilization();
        currentActivity_.attentionLevel = agentBrain->getAttentionLevel();
        currentActivity_.curiosityLevel = agentBrain->getCuriosityLevel();
        currentActivity_.dopamineLevel = agentBrain->getNeuromodulationLevel();
        currentActivity_.rewardValue = agentBrain->getTotalReward();
    }
    
    currentActivity_.developmentalAge = developmentSystem_->getDevelopmentalAge();
    currentActivity_.developmentalStage = developmentSystem_->getStageName();
    
    // Store in history
    neuralActivityHistory_.push_back(currentActivity_);
    
    // Limit history size to prevent memory issues
    if (neuralActivityHistory_.size() > 1000) {
        neuralActivityHistory_.erase(neuralActivityHistory_.begin());
    }
}

void Phase6Demo::updatePerformanceMetrics(double timestep) {
    currentPerformance_.learningTime = wallClockTime_;
    
    // Update peak firing rate
    if (currentActivity_.avgFiringRate > currentPerformance_.peakFiringRate) {
        currentPerformance_.peakFiringRate = currentActivity_.avgFiringRate;
    }
    
    // Update average firing rate (running average)
    static double firingRateSum = 0.0;
    static size_t firingRateCount = 0;
    
    firingRateSum += currentActivity_.avgFiringRate;
    firingRateCount++;
    currentPerformance_.avgFiringRate = firingRateSum / static_cast<double>(firingRateCount);
    
    // Calculate efficiency metrics
    if (currentPerformance_.avgFiringRate > 0) {
        currentPerformance_.rewardEfficiency = currentActivity_.rewardValue / currentPerformance_.avgFiringRate;
        currentPerformance_.memoryEfficiency = currentActivity_.memoryUtilization / static_cast<double>(brain_->getTotalNeuronCount());
        currentPerformance_.plasticityEfficiency = (currentActivity_.newConnections + currentActivity_.prunedConnections) / timestep;
    }
    
    // Calculate convergence time (simplified)
    if (currentStep_ > 100) {
        currentPerformance_.convergenceTime = wallClockTime_ * 0.5;
    }
    
    // Add to history
    performanceHistory_.push_back(currentPerformance_);
    
    // Limit history size
    if (performanceHistory_.size() > 100) {
        performanceHistory_.erase(performanceHistory_.begin());
    }
}

void Phase6Demo::updateDevelopmentProgression() {
    currentDevelopment_.stageStartStep = developmentSystem_->getStage() == DevelopmentalStage::Initial ? 0 : currentDevelopment_.stageStartStep;
    currentDevelopment_.stepsInStage = currentStep_ - currentDevelopment_.stageStartStep;
    currentDevelopment_.stageAge = developmentSystem_->getDevelopmentalAge();
    currentDevelopment_.plasticityModifier = developmentSystem_->getPlasticityModifier();
    currentDevelopment_.criticalPeriodProgress = developmentSystem_->getCriticalPeriodProgress();
    
    // Update developmental stage
    currentDevelopment_.developmentalStage = developmentSystem_->getStageName();
    
    // Store in history
    developmentHistory_.push_back(currentDevelopment_);
    
    // Limit history size
    if (developmentHistory_.size() > 100) {
        developmentHistory_.erase(developmentHistory_.begin());
    }
}

void Phase6Demo::updateStatistics() {
    statistics_["total_neurons"] = static_cast<float>(brain_->getTotalNeuronCount());
    statistics_["total_synapses"] = static_cast<float>(brain_->getTotalSynapseCount());
    statistics_["active_neurons"] = static_cast<float>(currentActivity_.activeNeurons);
    statistics_["avg_firing_rate"] = currentActivity_.avgFiringRate;
    statistics_["total_reward"] = currentActivity_.rewardValue;
    statistics_["memory_utilization"] = currentActivity_.memoryUtilization;
    statistics_["attention_level"] = currentActivity_.attentionLevel;
    statistics_["curiosity_level"] = currentActivity_.curiosityLevel;
    statistics_["dopamine_level"] = currentActivity_.dopamineLevel;
    statistics_["new_connections"] = static_cast<float>(currentActivity_.newConnections);
    statistics_["pruned_connections"] = static_cast<float>(currentActivity_.prunedConnections);
    statistics_["developmental_age"] = currentActivity_.developmentalAge;
    
    // Calculate performance score
    double performanceScore = 0.0;
    performanceScore += std::min(1.0, currentActivity_.avgFiringRate / 50.0);  // Normalized firing rate
    performanceScore += std::min(1.0, currentActivity_.rewardValue / 100.0);  // Normalized reward
    performanceScore += std::min(1.0, currentActivity_.memoryUtilization);     // Normalized memory utilization
    performanceScore += std::min(1.0, currentDevelopment_.plasticityModifier); // Normalized plasticity
    
    statistics_["performance_score"] = static_cast<float>(performanceScore / 4.0);
}

void Phase6Demo::processStep(const Brain* brain, AgentBrain* agentBrain, SimpleWorld* world) {
    // Process neural step
    processNeuralStep(brain_);
    
    // Process development step
    processDevelopmentStep(brain_.get(), developmentSystem_.get());
    
    // Process memory step
    processMemoryStep(brain_.get(), agentBrain_);
    
    // Process neuromodulation step
    processNeuromodulationStep(agentBrain_);
    
    // Process prediction step
    processPredictionStep(brain_.get(), agentBrain_);
    
    // Process cognition step
    processCognitionStep(brain_.get(), agentBrain_);
    
    // Process action step
    processActionStep(agentBrain_, world);
    
    // Update step counter and time
    currentStep_++;
    wallClockTime_ += 0.001; // Assuming 1kHz simulation rate
    totalStepsProcessed_++;
}

void Phase6Demo::processNeuralStep(Brain* brain) {
    if (brain) {
        brain->step(currentStep_);
    }
}

void Phase6Demo::processDevelopmentStep(Brain* brain, DevelopmentSystem* devSystem) {
    if (devSystem && brain) {
        RandomGenerator rng;
        devSystem->update(brain, rng, 0.001);
    }
}

void Phase6Demo::processMemoryStep(Brain* brain, AgentBrain* agentBrain) {
    if (brain && agentBrain) {
        // Process memory updates
        workingMemory_->update(brain, 0.001);
        episodicMemory_->update(brain, currentStep_);
        
        // Update agent brain memory
        agentBrain->updateMemory(0.001);
    }
}

void Phase6Demo::processNeuromodulationStep(AgentBrain* agentBrain) {
    if (agentBrain) {
        // Process neuromodulation updates
        neuromodulator_->update(agentBrain, 0.001);
        
        // Update agent brain neuromodulation
        agentBrain->updateNeuromodulation(0.001);
    }
}

void Phase6Demo::processPredictionStep(Brain* brain, AgentBrain* agentBrain) {
    if (brain && agentBrain) {
        // Process prediction updates
        predictionSystem_->update(brain, 0.001);
        
        // Update agent brain predictions
        agentBrain->updatePredictions(0.001);
    }
}

void Phase6Demo::processCognitionStep(Brain* brain, AgentBrain* agentBrain) {
    if (brain && agentBrain) {
        // Process cognitive updates
        conceptFormation_->update(brain, 0.001);
        neuralPlanner_->update(brain, agentBrain, 0.001);
        
        // Update agent brain cognition
        agentBrain->updateCognition(0.001);
    }
}

void Phase6Demo::processActionStep(AgentBrain* agentBrain, SimpleWorld* world) {
    if (agentBrain && world) {
        // Get motor command from agent brain
        MotorCommand cmd = agentBrain->decodeMotorCommand();
        
        // Apply motor command to world
        ActionResult result = world->applyMotorCommand(cmd, world->getSimulationTime());
        
        // Apply reward modulation
        agentBrain->applyRewardModulation(result.reward, 0.0f);
        
        // Update development
        agentBrain->updateDevelopment(0.001);
    }
}

void Phase6Demo::generateNeuralVisualizationData(const Brain* brain, NeuralActivityData& data) {
    // Generate visualization data for neural activity
    data.firingRateHistory.push_back(data.avgFiringRate);
    data.rewardHistory.push_back(data.rewardValue);
    data.connectionCountHistory.push_back(data.activeNeurons);
    
    // Limit history size
    if (data.firingRateHistory.size() > 100) {
        data.firingRateHistory.erase(data.firingRateHistory.begin());
        data.rewardHistory.erase(data.rewardHistory.begin());
        data.connectionCountHistory.erase(data.connectionCountHistory.begin());
    }
}

std::string Phase6Demo::generateSystemStatus() const {
    std::ostringstream status;
    status << "Phase6Demo Status:" << std::endl;
    status << "  Running: " << (running_ ? "Yes" : "No") << std::endl;
    status << "  Demo Mode: " << (demoMode_ ? "Yes" : "No") << std::endl;
    status << "  Visualization: " << (visualizationEnabled_ ? "Yes" : "No") << std::endl;
    status << "  Benchmarking: " << (benchmarkingEnabled_ ? "Yes" : "No") << std::endl;
    status << "  Real-time Monitoring: " << (realTimeMonitoring_ ? "Yes" : "No") << std::endl;
    status << "  Progress: " << std::fixed << std::setprecision(2) << (getProgress() * 100) << "%" << std::endl;
    status << "  Current Step: " << currentStep_ << " / " << demoConfig_.maxSteps << std::endl;
    status << "  Current Stage: " << currentDevelopment_.developmentalStage << std::endl;
    
    return status.str();
}

bool Phase6Demo::saveCheckpoint(const std::string& filename) {
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Save simulation state
        file.write(reinterpret_cast<const char*>(&currentStep_), sizeof(SimulationStep));
        file.write(reinterpret_cast<const char*>(&wallClockTime_), sizeof(double));
        file.write(reinterpret_cast<const char*>(&currentActivity_), sizeof(NeuralActivityData));
        
        file.close();
        NLM_LOG_INFO("Checkpoint saved to: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool Phase6Demo::loadCheckpoint(const std::string& filename) {
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Load simulation state
        file.read(reinterpret_cast<char*>(&currentStep_), sizeof(SimulationStep));
        file.read(reinterpret_cast<char*>(&wallClockTime_), sizeof(double));
        file.read(reinterpret_cast<char*>(&currentActivity_), sizeof(NeuralActivityData));
        
        file.close();
        NLM_LOG_INFO("Checkpoint loaded from: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading checkpoint: " + std::string(e.what()));
        return false;
    }
}

bool Phase6Demo::exportMetricsToCSV(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Step,WallClockTime,AvgFiringRate,TotalReward,MemoryUtilization,AttentionLevel,CuriosityLevel,DopamineLevel,NewConnections,PrunedConnections,DevelopmentalAge,DevelopmentalStage" << std::endl;
        
        for (const auto& data : neuralActivityHistory_) {
            file << data.step << ","
                 << std::fixed << std::setprecision(6) << data.wallClockTime << ","
                 << std::fixed << std::setprecision(6) << data.avgFiringRate << ","
                 << std::fixed << std::setprecision(6) << data.rewardValue << ","
                 << std::fixed << std::setprecision(6) << data.memoryUtilization << ","
                 << std::fixed << std::setprecision(6) << data.attentionLevel << ","
                 << std::fixed << std::setprecision(6) << data.curiosityLevel << ","
                 << std::fixed << std::setprecision(6) << data.dopamineLevel << ","
                 << data.newConnections << ","
                 << data.prunedConnections << ","
                 << std::fixed << std::setprecision(6) << data.developmentalAge << ","
                 << data.developmentalStage << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Metrics exported to CSV: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting metrics to CSV: " + std::string(e.what()));
        return false;
    }
}

void Phase6Demo::printProgressUpdate(const NeuralActivityData& activity) const {
    std::cout << "Step: " << activity.step << ", "
              << "Progress: " << std::fixed << std::setprecision(2) << (getProgress() * 100) << "%, "
              << "Firing Rate: " << std::fixed << std::setprecision(3) << activity.avgFiringRate << ", "
              << "Reward: " << std::fixed << std::setprecision(3) << activity.rewardValue << ", "
              << "Stage: " << activity.developmentalStage << std::endl;
}

void Phase6Demo::printStageProgression(const DevelopmentProgression& devProg) const {
    std::cout << "Development Stage Progression:" << std::endl;
    std::cout << "  Stage: " << devProg.developmentalStage << std::endl;
    std::cout << "  Stage Age: " << std::fixed << std::setprecision(3) << devProg.stageAge << std::endl;
    std::cout << "  Plasticity Modifier: " << std::fixed << std::setprecision(3) << devProg.plasticityModifier << std::endl;
    std::cout << "  Critical Period Progress: " << std::fixed << std::setprecision(3) << devProg.criticalPeriodProgress << std::endl;
}

void Phase6Demo::printLearningSummary() const {
    std::cout << "Learning Summary:" << std::endl;
    std::cout << "  Total Steps Processed: " << totalStepsProcessed_ << std::endl;
    std::cout << "  Total Neurons Created: " << totalNeuronsCreated_ << std::endl;
    std::cout << "  Total Connections Created: " << totalConnectionsCreated_ << std::endl;
    std::cout << "  Total Connections Pruned: " << totalConnectionsPruned_ << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << currentPerformance_.avgFiringRate << std::endl;
    std::cout << "  Peak Firing Rate: " << std::fixed << std::setprecision(3) << currentPerformance_.peakFiringRate << std::endl;
    std::cout << "  Reward Efficiency: " << std::fixed << std::setprecision(3) << currentPerformance_.rewardEfficiency << std::endl;
}

} // namespace nlm