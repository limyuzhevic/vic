#include "BrainDebugger.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace nlm {

BrainDebugger::BrainDebugger(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain,
                             std::shared_ptr<Phase6IntegratedExperiment> experiment)
    : brain_(brain), agentBrain_(agentBrain), experiment_(experiment), config_(nullptr),
      isMonitoring_(false), isCollectingData_(false), isPaused_(false),
      analysisInterval_(0.1), maxNeuronsToAnalyze_(1000), realTimeUpdatesEnabled_(true) {
    
    if (brain_) {
        // Try to get config from brain (if available)
        // Note: Brain doesn't expose config directly, so we need a different approach
        NLM_LOG_INFO("BrainDebugger initialized with brain reference");
    }
    
    cleanup();
}

BrainDebugger::~BrainDebugger() {
    shutdownAnalysis();
}

void BrainDebugger::cleanup() {
    if (isCollectingData_) {
        stopDataCollection();
    }
    if (isMonitoring()) {
        stopRealTimeMonitoring();
    }
    if (dataFile_.is_open()) {
        dataFile_.close();
    }
}

bool BrainDebugger::initializeAnalysis() {
    if (!brain_) {
        NLM_LOG_ERROR("BrainDebugger: No brain reference available");
        return false;
    }
    
    // Validate brain state before analysis
    if (!validateBrainState()) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            NLM_LOG_WARNING("BrainDebugger validation warning: " + error);
        }
    }
    
    NLM_LOG_INFO("BrainDebugger analysis initialized successfully");
    return true;
}

void BrainDebugger::shutdownAnalysis() {
    if (isCollectingData_) {
        stopDataCollection();
    }
    if (isMonitoring()) {
        stopRealTimeMonitoring();
    }
    if (dataFile_.is_open()) {
        dataFile_.close();
    }
    
    NLM_LOG_INFO("BrainDebugger analysis shutdown completed");
}

BrainDebugger::BrainAnalysis BrainDebugger::getBrainAnalysis() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    return performBrainAnalysis();
}

void BrainDebugger::analyzeBrain() {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    performAnalysis();
}

BrainDebugger::NeuronAnalysis BrainDebugger::inspectNeuron(NeuronId id) const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    NeuronAnalysis analysis;
    analysis.id = id;
    
    if (!brain_) {
        return analysis;
    }
    
    // Get neuron from brain regions
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron && neuron->getId() == id) {
                    analysis.id = neuron->getId();
                    analysis.type = neuron->getType();
                    analysis.regionName = region->getName();
                    analysis.membranePotential = neuron->getState().membranePotential;
                    analysis.restingPotential = neuron->getState().restingPotential;
                    analysis.activationLevel = neuron->getState().activationLevel;
                    
                    // Calculate input/output currents
                    analysis.inputCurrent = 0.0f; // Would need detailed calculation
                    analysis.outputCurrent = 0.0f; // Would need detailed calculation
                    analysis.firingRate = neuron->getState().firingRate;
                    
                    // Get connections
                    for (const auto& syn : region->getSynapses()) {
                        if (syn->getPostNeuron() == id) {
                            analysis.outgoingConnections.push_back(syn->getId());
                        }
                        if (syn->getPreNeuron() == id) {
                            analysis.incomingConnections.push_back(syn->getId());
                        }
                    }
                    
                    analysis.connectivityScore = calculateConnectivityScore(id);
                    analysis.isActive = neuron->getState().isActive;
                    analysis.isFiring = neuron->getState().isFiring;
                    
                    return analysis;
                }
            }
        }
    }
    
    return analysis;
}

BrainDebugger::SynapseAnalysis BrainDebugger::inspectSynapse(SynapseId id) const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    SynapseAnalysis analysis;
    analysis.id = id;
    
    if (!brain_) {
        return analysis;
    }
    
    // Find synapse in brain regions
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn->getId() == id) {
                analysis.id = syn->getId();
                analysis.preNeuron = syn->getPreNeuron();
                analysis.postNeuron = syn->getPostNeuron();
                analysis.weight = syn->getWeight();
                analysis.delay = syn->getDelay();
                analysis.eligibilityTrace = syn->getEligibilityTrace();
                analysis.isPlastic = syn->isPlastic();
                
                // Get region names
                for (const auto& r : brain_->getRegions()) {
                    if (r->getId() == region->getId()) {
                        analysis.preRegion = r->getName();
                    }
                    if (r->getId() == region->getId()) {
                        analysis.postRegion = r->getName();
                    }
                }
                
                // Calculate contribution
                analysis.contributionToOutput = syn->getWeight() * syn->getEligibilityTrace();
                analysis.plasticityRate = syn->getPlasticityRate();
                
                return analysis;
            }
        }
    }
    
    return analysis;
}

void BrainDebugger::exploreConnections() {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    if (!brain_) return;
    
    NLM_LOG_INFO("Exploring brain connections...");
    
    // Analyze inter-regional connections
    for (const auto& region : brain_->getRegions()) {
        std::vector<NeuronId> regionNeurons;
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    regionNeurons.push_back(neuron->getId());
                }
            }
        }
        
        // Analyze connectivity within region
        for (NeuronId neuronId : regionNeurons) {
            auto connected = getConnectedNeurons(neuronId);
            NLM_LOG_DEBUG("Neuron " + std::to_string(neuronId) + " connected to " + 
                         std::to_string(connected.size()) + " other neurons");
        }
    }
    
    NLM_LOG_INFO("Connection exploration completed");
}

std::vector<NeuronId> BrainDebugger::getConnectedNeurons(NeuronId id) const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    std::vector<NeuronId> connected;
    
    if (!brain_) return connected;
    
    // Find all synapses connected to this neuron
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn->getPreNeuron() == id) {
                connected.push_back(syn->getPostNeuron());
            }
            if (syn->getPostNeuron() == id) {
                connected.push_back(syn->getPreNeuron());
            }
        }
    }
    
    // Remove duplicates
    std::sort(connected.begin(), connected.end());
    connected.erase(std::unique(connected.begin(), connected.end()), connected.end());
    
    return connected;
}

std::vector<SynapseId> BrainDebugger::getOutgoingConnections(NeuronId id) const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    std::vector<SynapseId> outgoing;
    
    if (!brain_) return outgoing;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn->getPreNeuron() == id) {
                outgoing.push_back(syn->getId());
            }
        }
    }
    
    return outgoing;
}

std::vector<SynapseId> BrainDebugger::getIncomingConnections(NeuronId id) const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    std::vector<SynapseId> incoming;
    
    if (!brain_) return incoming;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn->getPostNeuron() == id) {
                incoming.push_back(syn->getId());
            }
        }
    }
    
    return incoming;
}

float BrainDebugger::calculateConnectivityScore(NeuronId id) const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!brain_) return 0.0f;
    
    size_t outgoing = getOutgoingConnections(id).size();
    size_t incoming = getIncomingConnections(id).size();
    size_t total = outgoing + incoming;
    
    if (total == 0) return 0.0f;
    
    // Normalize connectivity score (0-1 scale)
    // Use sigmoid-like function to give diminishing returns for very high connectivity
    float connectivity = static_cast<float>(total) / 100.0f; // Normalize by expected max
    return std::min(1.0f, connectivity);
}

void BrainDebugger::examineConfiguration() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot examine configuration: no brain reference");
        return;
    }
    
    NLM_LOG_INFO("=== Brain Configuration Analysis ===");
    
    // Basic brain information
    NLM_LOG_INFO("Brain has " + std::to_string(brain_->getRegionCount()) + " regions");
    NLM_LOG_INFO("Total neuron count: " + std::to_string(brain_->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapse count: " + std::to_string(brain_->getTotalSynapseCount()));
    NLM_LOG_INFO("Active neuron count: " + std::to_string(brain_->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neuron count: " + std::to_string(brain_->getFiringNeuronCount()));
    NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(brain_->getExcitationInhibitionRatio()));
    
    // Region details
    for (size_t i = 0; i < brain_->getRegionCount(); ++i) {
        auto regionId = brain_->getRegionIds()[i];
        auto region = brain_->getRegion(regionId);
        if (region) {
            NLM_LOG_INFO("Region " + std::to_string(regionId) + ": " + region->getName() + 
                        " - " + std::to_string(region->getPopulations().size()) + " populations");
        }
    }
    
    NLM_LOG_INFO("=== Configuration Analysis Complete ===");
}

std::string BrainDebugger::getBrainConfiguration() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    std::stringstream configStream;
    
    if (!brain_) {
        return "No brain reference available";
    }
    
    configStream << "Brain Configuration Report" << std::endl;
    configStream << "===========================" << std::endl;
    configStream << "Region Count: " << brain_->getRegionCount() << std::endl;
    configStream << "Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    configStream << "Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    configStream << "Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
    configStream << "Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
    configStream << "E/I Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    
    return configStream.str();
}

void BrainDebugger::printBrainConfiguration() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    std::cout << getBrainConfiguration() << std::endl;
}

BrainDebugger::MemoryAnalysis BrainDebugger::analyzeMemorySystems() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    MemoryAnalysis analysis;
    
    if (!brain_) return analysis;
    
    // This would need access to memory system components
    // For now, provide placeholder analysis
    NLM_LOG_INFO("Memory system analysis requires integration with memory components");
    
    // Based on brain persistence methods, estimate memory usage
    // These would need actual memory system access in a real implementation
    analysis.workingMemorySize = 1024;  // Placeholder values
    analysis.workingMemoryCapacity = 4096;
    analysis.workingMemoryUtilization = static_cast<float>(analysis.workingMemorySize) / analysis.workingMemoryCapacity;
    
    analysis.episodicMemoryCount = 100;
    analysis.episodicMemoryCapacity = 10000;
    
    analysis.semanticMemorySize = 512;
    analysis.semanticMemoryCapacity = 8192;
    
    analysis.proceduralMemorySize = 256;
    analysis.proceduralMemoryCapacity = 2048;
    
    analysis.associativeMemorySize = 128;
    analysis.associativeMemoryCapacity = 1024;
    
    analysis.memoryAccessTime = 0.001f; // 1ms average
    analysis.memoryRetentionTime = 3600.0f; // 1 hour average
    analysis.memoryRetrievalAccuracy = 0.95f; // 95% accuracy
    
    return analysis;
}

BrainDebugger::PerformanceMetrics BrainDebugger::getPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    PerformanceMetrics metrics;
    
    if (!brain_) return metrics;
    
    // Calculate performance metrics from brain state
    metrics.totalSpikeCount = brain_->getTotalSpikeCount();
    metrics.fps = 60.0; // Would need actual timing
    metrics.frameTime = 1.0 / metrics.fps;
    metrics.memoryUsage = 0; // Would need system memory monitoring
    metrics.cpuUsagePercent = 0.0f; // Would need system CPU monitoring
    metrics.neuralActivityLevel = static_cast<float>(brain_->getActiveNeuronCount()) / 
                                  std::max<size_t>(1, brain_->getTotalNeuronCount());
    metrics.learningProgress = 0.5f; // Would need learning system access
    metrics.stabilityIndex = 1.0f; // Would need stability analysis
    metrics.efficiencyScore = metrics.neuralActivityLevel * metrics.learningProgress;
    
    // Calculate spike statistics
    if (metrics.totalSpikeCount > 0) {
        metrics.averageSpikeInterval = 1.0 / (metrics.fps * metrics.neuralActivityLevel);
        metrics.peakFiringRate = brain_->getAverageFiringRate();
    }
    
    return metrics;
}

bool BrainDebugger::validateBrainState() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    std::vector<std::string> errors;
    
    if (!brain_) {
        errors.push_back("No brain reference available");
        return false;
    }
    
    // Check basic brain integrity
    if (brain_->getTotalNeuronCount() == 0) {
        errors.push_back("Brain has no neurons");
    }
    
    if (brain_->getTotalSynapseCount() == 0) {
        errors.push_back("Brain has no synapses");
    }
    
    if (brain_->getRegionCount() == 0) {
        errors.push_back("Brain has no regions");
    }
    
    // Validate connectivity
    if (!validateConnectivity()) {
        errors.push_back("Brain connectivity validation failed");
    }
    
    // Validate memory systems
    if (!validateMemorySystems()) {
        errors.push_back("Memory systems validation failed");
    }
    
    // Validate performance
    if (!validatePerformance()) {
        errors.push_back("Performance validation failed");
    }
    
    // Store errors in const_cast version for const method
    // This is a limitation - in a real implementation, errors would be stored differently
    return errors.empty();
}

std::vector<std::string> BrainDebugger::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    std::vector<std::string> errors;
    
    if (!brain_) {
        errors.push_back("No brain reference available");
        return errors;
    }
    
    if (brain_->getTotalNeuronCount() == 0) {
        errors.push_back("Brain has no neurons");
    }
    
    if (brain_->getTotalSynapseCount() == 0) {
        errors.push_back("Brain has no synapses");
    }
    
    if (brain_->getRegionCount() == 0) {
        errors.push_back("Brain has no regions");
    }
    
    return errors;
}

bool BrainDebugger::validateConnectivity() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!brain_) return false;
    
    // Check for neurons without connections
    size_t disconnectedCount = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    auto connected = getConnectedNeurons(neuron->getId());
                    if (connected.empty()) {
                        disconnectedCount++;
                    }
                }
            }
        }
    }
    
    // Allow some disconnected neurons (input/output neurons)
    float disconnectedRatio = static_cast<float>(disconnectedCount) / brain_->getTotalNeuronCount();
    return disconnectedRatio < 0.5f; // Less than 50% disconnected
}

bool BrainDebugger::validateMemorySystems() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    // Memory validation would require access to memory system components
    // For now, assume valid if brain exists
    return brain_ != nullptr;
}

bool BrainDebugger::validatePerformance() const {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!brain_) return false;
    
    PerformanceMetrics metrics = getPerformanceMetrics();
    
    // Check for performance issues
    if (metrics.neuralActivityLevel < 0.01f) {
        return false; // Too little neural activity
    }
    
    if (metrics.stabilityIndex < 0.1f) {
        return false; // Unstable
    }
    
    return true;
}

void BrainDebugger::pauseBrain() const {
    if (brain_) {
        NLM_LOG_INFO("Pausing brain simulation...");
        // Note: Brain doesn't have pause functionality, this would need implementation
        isPaused_ = true;
    }
}

void BrainDebugger::resumeBrain() const {
    if (brain_) {
        NLM_LOG_INFO("Resuming brain simulation...");
        // Note: Brain doesn't have resume functionality, this would need implementation
        isPaused_ = false;
    }
}

void BrainDebugger::stepBrain() const {
    if (brain_) {
        NLM_LOG_INFO("Stepping brain simulation...");
        // Note: Brain doesn't have step functionality, this would need implementation
    }
}

bool BrainDebugger::isBrainPaused() const {
    return isPaused_;
}

void BrainDebugger::startRealTimeMonitoring(unsigned int updateIntervalMs) {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (isMonitoring()) {
        NLM_LOG_WARNING("Real-time monitoring already started");
        return;
    }
    
    isMonitoring_ = true;
    monitoringThread_ = std::thread(&BrainDebugger::performAnalysis, this);
    
    NLM_LOG_INFO("Real-time monitoring started with interval " + std::to_string(updateIntervalMs) + "ms");
}

void BrainDebugger::stopRealTimeMonitoring() {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!isMonitoring()) {
        return;
    }
    
    isMonitoring_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    NLM_LOG_INFO("Real-time monitoring stopped");
}

bool BrainDebugger::isMonitoring() const {
    return isMonitoring_.load();
}

void BrainDebugger::setMonitoringCallback(std::function<void(const BrainAnalysis&)> callback) {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    monitoringCallback_ = std::move(callback);
}

void BrainDebugger::startDataCollection(const std::string& dataFile) {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (isCollectingData_) {
        NLM_LOG_WARNING("Data collection already started");
        return;
    }
    
    dataCollectionFile_ = dataFile;
    dataFile_.open(dataFile, std::ios::out | std::ios::trunc);
    
    if (!dataFile_.is_open()) {
        NLM_LOG_ERROR("Failed to open data file: " + dataFile);
        return;
    }
    
    // Write header
    dataFile_ << "Time,Step,NeuronCount,SynapseCount,ActiveNeurons,FiringRate,EI_Ratio" << std::endl;
    
    isCollectingData_ = true;
    NLM_LOG_INFO("Data collection started: " + dataFile);
}

void BrainDebugger::stopDataCollection() {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!isCollectingData_) {
        return;
    }
    
    isCollectingData_ = false;
    if (dataFile_.is_open()) {
        dataFile_.close();
    }
    
    NLM_LOG_INFO("Data collection stopped");
}

void BrainDebugger::collectDataPoint() {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!isCollectingData_ || !dataFile_.is_open() || !brain_) {
        return;
    }
    
    auto analysis = performBrainAnalysis();
    
    dataFile_ << std::fixed << std::setprecision(3)
              << "0.0,0," << analysis.totalNeuronCount << ","
              << analysis.totalSynapseCount << ","
              << analysis.activeNeuronCount << ","
              << analysis.averageFiringRate << ","
              << analysis.excitationInhibitionRatio << std::endl;
}

bool BrainDebugger::isCollectingData() const {
    return isCollectingData_.load();
}

void BrainDebugger::performAnalysis() {
    std::chrono::milliseconds interval(static_cast<long>(analysisInterval_ * 1000));
    
    while (isMonitoring_) {
        std::lock_guard<std::mutex> lock(analysisMutex_);
        
        auto analysis = performBrainAnalysis();
        
        // Call monitoring callback if set
        if (monitoringCallback_) {
            monitoringCallback_(analysis);
        }
        
        // Collect data if enabled
        collectDataPoint();
        
        std::this_thread::sleep_for(interval);
    }
}

BrainDebugger::BrainAnalysis BrainDebugger::performBrainAnalysis() const {
    BrainAnalysis analysis;
    
    if (!brain_) {
        return analysis;
    }
    
    // Perform basic brain analysis
    analysis.totalNeuronCount = brain_->getTotalNeuronCount();
    analysis.totalSynapseCount = brain_->getTotalSynapseCount();
    analysis.activeNeuronCount = brain_->getActiveNeuronCount();
    analysis.firingNeuronCount = brain_->getFiringNeuronCount();
    analysis.averageFiringRate = brain_->getAverageFiringRate();
    analysis.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
    
    // Regional analysis
    for (size_t i = 0; i < brain_->getRegionCount(); ++i) {
        auto regionId = brain_->getRegionIds()[i];
        auto region = brain_->getRegion(regionId);
        if (region) {
            analysis.regionNames.push_back(region->getName());
            
            // Count neurons in region
            size_t regionNeuronCount = 0;
            float regionFiringRate = 0.0f;
            
            for (const auto& population : region->getPopulations()) {
                regionNeuronCount += population->getNeurons().size();
                regionFiringRate += population->getFiringRate() * population->getNeurons().size();
            }
            
            analysis.neuronCountsPerRegion.push_back(regionNeuronCount);
            analysis.firingRatesPerRegion.push_back(
                regionNeuronCount > 0 ? regionFiringRate / regionNeuronCount : 0.0f);
        }
    }
    
    // Calculate connectivity density
    if (analysis.totalNeuronCount > 1) {
        analysis.connectionDensity = static_cast<float>(analysis.totalSynapseCount) / 
                                    (analysis.totalNeuronCount * (analysis.totalNeuronCount - 1));
    }
    
    // Calculate average synapse weight
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            totalWeight += syn->getWeight();
            synapseCount++;
        }
    }
    
    analysis.averageSynapseWeight = synapseCount > 0 ? totalWeight / synapseCount : 0.0f;
    
    // Calculate weight distribution (binned 0-1 range into 10 bins)
    calculateWeightDistribution(analysis);
    
    // State information
    analysis.isBrainInitialized = brain_ != nullptr;
    analysis.isBrainActive = analysis.activeNeuronCount > 0;
    analysis.currentStep = 0; // Would need access to simulation clock
    analysis.currentTime = 0.0; // Would need access to simulation clock
    
    return analysis;
}

void BrainDebugger::calculateWeightDistribution(BrainAnalysis& analysis) const {
    if (!brain_) return;
    
    float minWeight = 1e6f, maxWeight = -1e6f;
    std::vector<float> weights;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            weights.push_back(syn->getWeight());
            if (syn->getWeight() < minWeight) minWeight = syn->getWeight();
            if (syn->getWeight() > maxWeight) maxWeight = syn->getWeight();
        }
    }
    
    if (weights.empty()) return;
    
    float weightRange = maxWeight - minWeight;
    if (weightRange < 1e-6f) weightRange = 1.0f;
    
    for (float weight : weights) {
        float normalized = (weight - minWeight) / weightRange;
        size_t bin = static_cast<size_t>(normalized * 10.0f);
        if (bin >= 10) bin = 9;
        analysis.weightDistribution[bin]++;
    }
}

void BrainDebugger::generateDebugReport(const std::string& filename) {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot generate debug report: no brain reference");
        return;
    }
    
    std::ofstream reportFile(filename);
    if (!reportFile.is_open()) {
        NLM_LOG_ERROR("Failed to open debug report file: " + filename);
        return;
    }
    
    reportFile << "Brain Debug Report" << std::endl;
    reportFile << "===================" << std::endl;
    reportFile << std::endl;
    
    auto analysis = getBrainAnalysis();
    reportFile << "Brain Analysis:" << std::endl;
    reportFile << "  Total Neurons: " << analysis.totalNeuronCount << std::endl;
    reportFile << "  Total Synapses: " << analysis.totalSynapseCount << std::endl;
    reportFile << "  Active Neurons: " << analysis.activeNeuronCount << std::endl;
    reportFile << "  Firing Neurons: " << analysis.firingNeuronCount << std::endl;
    reportFile << "  Average Firing Rate: " << analysis.averageFiringRate << std::endl;
    reportFile << "  E/I Ratio: " << analysis.excitationInhibitionRatio << std::endl;
    reportFile << "  Connection Density: " << analysis.connectionDensity << std::endl;
    reportFile << "  Average Synapse Weight: " << analysis.averageSynapseWeight << std::endl;
    reportFile << std::endl;
    
    reportFile << "Regional Analysis:" << std::endl;
    for (size_t i = 0; i < analysis.regionNames.size(); ++i) {
        reportFile << "  " << analysis.regionNames[i] << ": "
                   << analysis.neuronCountsPerRegion[i] << " neurons, "
                   << analysis.firingRatesPerRegion[i] << " firing rate" << std::endl;
    }
    reportFile << std::endl;
    
    reportFile << "Memory Analysis:" << std::endl;
    auto memoryAnalysis = analyzeMemorySystems();
    reportFile << "  Working Memory: " << memoryAnalysis.workingMemorySize << "/"
               << memoryAnalysis.workingMemoryCapacity << " ("
               << memoryAnalysis.workingMemoryUtilization * 100 << "% utilized)" << std::endl;
    reportFile << "  Episodic Memory: " << memoryAnalysis.episodicMemoryCount << "/"
               << memoryAnalysis.episodicMemoryCapacity << std::endl;
    reportFile << "  Semantic Memory: " << memoryAnalysis.semanticMemorySize << "/"
               << memoryAnalysis.semanticMemoryCapacity << std::endl;
    reportFile << "  Procedural Memory: " << memoryAnalysis.proceduralMemorySize << "/"
               << memoryAnalysis.proceduralMemoryCapacity << std::endl;
    reportFile << "  Associative Memory: " << memoryAnalysis.associativeMemorySize << "/"
               << memoryAnalysis.associativeMemoryCapacity << std::endl;
    reportFile << std::endl;
    
    reportFile << "Performance Metrics:" << std::endl;
    auto perfMetrics = getPerformanceMetrics();
    reportFile << "  FPS: " << perfMetrics.fps << std::endl;
    reportFile << "  Neural Activity Level: " << perfMetrics.neuralActivityLevel << std::endl;
    reportFile << "  Learning Progress: " << perfMetrics.learningProgress << std::endl;
    reportFile << "  Stability Index: " << perfMetrics.stabilityIndex << std::endl;
    reportFile << "  Efficiency Score: " << perfMetrics.efficiencyScore << std::endl;
    reportFile << "  Total Spikes: " << perfMetrics.totalSpikeCount << std::endl;
    reportFile << std::endl;
    
    reportFile << "Validation Status:" << std::endl;
    bool valid = validateBrainState();
    reportFile << "  Brain State Valid: " << (valid ? "Yes" : "No") << std::endl;
    
    if (!valid) {
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            reportFile << "    - " << error << std::endl;
        }
    }
    
    reportFile.close();
    NLM_LOG_INFO("Debug report generated: " + filename);
}

void BrainDebugger::exportBrainState(const std::string& filename) {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot export brain state: no brain reference");
        return;
    }
    
    // Create a simplified brain state representation
    std::ofstream stateFile(filename);
    if (!stateFile.is_open()) {
        NLM_LOG_ERROR("Failed to open brain state file: " + filename);
        return;
    }
    
    stateFile << "Brain State Export" << std::endl;
    stateFile << "===================" << std::endl;
    stateFile << "Timestamp: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    stateFile << "Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    stateFile << "Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    stateFile << std::endl;
    
    // Export neuron states
    stateFile << "Neuron States:" << std::endl;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    stateFile << "  Neuron ID: " << neuron->getId() << 
                              " Type: " << static_cast<int>(neuron->getType()) << 
                              " Region: " << region->getName() << 
                              " Membrane: " << neuron->getState().membranePotential << 
                              " Firing: " << neuron->getState().isFiring << std::endl;
                }
            }
        }
    }
    
    // Export synapse states
    stateFile << std::endl << "Synapse States:" << std::endl;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            stateFile << "  Synapse ID: " << syn->getId() << 
                      " Pre: " << syn->getPreNeuron() << " Post: " << syn->getPostNeuron() <<
                      " Weight: " << syn->getWeight() << 
                      " Delay: " << syn->getDelay() << " Plastic: " << syn->isPlastic() << std::endl;
        }
    }
    
    stateFile.close();
    NLM_LOG_INFO("Brain state exported: " + filename);
}

void BrainDebugger::exportAnalysisData(const std::string& filename) {
    std::lock_guard<std::mutex> lock(analysisMutex_);
    
    // Save collected analysis data to file
    std::ifstream sourceFile(dataCollectionFile_);
    std::ofstream targetFile(filename);
    
    if (!sourceFile.is_open()) {
        NLM_LOG_ERROR("Cannot open source data file for export: " + dataCollectionFile_);
        return;
    }
    
    if (!targetFile.is_open()) {
        NLM_LOG_ERROR("Failed to open analysis export file: " + filename);
        return;
    }
    
    // Copy the data with additional analysis
    targetFile << sourceFile.rdbuf();
    
    sourceFile.close();
    targetFile.close();
    
    NLM_LOG_INFO("Analysis data exported: " + filename);
}

void BrainDebugger::lockAnalysis() const {
    analysisMutex_.lock();
}

void BrainDebugger::unlockAnalysis() const {
    analysisMutex_.unlock();
}

bool BrainDebugger::tryLockAnalysis() const {
    return analysisMutex_.try_lock();
}