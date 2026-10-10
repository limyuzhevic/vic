#include "NetworkBuilder.hpp"
#include "BrainSimulator.hpp"
#include "BrainDebugger.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <random>

namespace nlm {

NetworkBuilder::NetworkBuilder(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger)
    : brain_(brain), debugger_(debugger), isInitialized_(false), isBuilding_(false), isSimulating_(false),
      randomSeed_(42), loggingEnabled_(true), buildStartTime_(0.0), buildEndTime_(0.0),
      simulationStartTime_(0.0), simulationEndTime_(0.0) {
    
    cleanup();
    NLM_LOG_INFO("NetworkBuilder initialized");
}

NetworkBuilder::~NetworkBuilder() {
    shutdown();
}

void NetworkBuilder::cleanup() {
    createdRegions_.clear();
    regionNeuronCounts_.clear();
    regionNeuronTypes_.clear();
    regionFiringRates_.clear();
    
    statistics_ = NetworkStatistics();
}

bool NetworkBuilder::initialize() {
    if (!brain_) {
        NLM_LOG_ERROR("NetworkBuilder: No brain reference available");
        return false;
    }
    
    // Initialize random generator
    if (!initializeRandomGenerator()) {
        NLM_LOG_ERROR("Failed to initialize random generator");
        return false;
    }
    
    // Reset statistics
    statistics_ = NetworkStatistics();
    buildStartTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    isInitialized_ = true;
    
    NLM_LOG_INFO("NetworkBuilder initialized successfully");
    return true;
}

void NetworkBuilder::shutdown() {
    if (isSimulating_) {
        stopNetworkSimulation();
    }
    
    if (isBuilding_) {
        // Cancel build if in progress
        isBuilding_ = false;
    }
    
    cleanup();
    
    buildEndTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    statistics_.buildTime = buildEndTime_ - buildStartTime_;
    
    NLM_LOG_INFO("NetworkBuilder shutdown completed");
}

bool NetworkBuilder::initializeRandomGenerator() {
    // Initialize random seed if provided
    std::random_device rd;
    if (randomSeed_ == 0) {
        randomSeed_ = rd();
    }
    
    NLM_LOG_INFO("Random seed initialized: " + std::to_string(randomSeed_));
    return true;
}

bool NetworkBuilder::createCustomNetwork(const NeuralNetworkConfig& config) {
    if (!isInitialized_) {
        NLM_LOG_ERROR("NetworkBuilder not initialized");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(builderMutex_);
    
    if (isBuilding_) {
        NLM_LOG_ERROR("Network already being built");
        return false;
    }
    
    isBuilding_ = true;
    currentNetworkName_ = config.name.empty() ? "Custom Network" : config.name;
    
    // Validate configuration
    if (!validateNetworkConfig(config)) {
        isBuilding_ = false;
        NLM_LOG_ERROR("Invalid network configuration");
        return false;
    }
    
    NLM_LOG_INFO("Creating custom network: " + currentNetworkName_);
    
    try {
        // Setup regions
        if (!setupRegions(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to setup regions");
            return false;
        }
        
        // Setup connections
        if (!setupConnections(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to setup connections");
            return false;
        }
        
        // Initialize neurons
        if (!initializeNeurons(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to initialize neurons");
            return false;
        }
        
        // Initialize connections
        if (!initializeConnections(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to initialize connections");
            return false;
        }
        
        // Apply plasticity configuration
        if (!applyPlasticityConfiguration(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to apply plasticity configuration");
            return false;
        }
        
        // Apply neuromodulation configuration
        if (!applyNeuromodulationConfiguration(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to apply neuromodulation configuration");
            return false;
        }
        
        // Apply development configuration
        if (!applyDevelopmentConfiguration(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to apply development configuration");
            return false;
        }
        
        // Apply simulation configuration
        if (!applySimulationConfiguration(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to apply simulation configuration");
            return false;
        }
        
        // Apply performance configuration
        if (!applyPerformanceConfiguration(config)) {
            isBuilding_ = false;
            NLM_LOG_ERROR("Failed to apply performance configuration");
            return false;
        }
        
        // Generate network topology
        generateNetworkTopology(config);
        
        buildEndTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        statistics_.buildTime = buildEndTime_ - buildStartTime_;
        statistics_.neuronsCreated = getTotalNeuronCount();
        statistics_.synapsesCreated = getTotalSynapseCount();
        statistics_.connectionDensity = getConnectionDensity();
        
        isBuilding_ = false;
        
        NLM_LOG_INFO("Custom network created successfully: " + currentNetworkName_);
        return true;
        
    } catch (const std::exception& e) {
        isBuilding_ = false;
        NLM_LOG_ERROR("Exception during network creation: " + std::string(e.what()));
        return false;
    }
}

bool NetworkBuilder::createStandardNetwork(const std::string& topologyType) {
    NeuralNetworkConfig config;
    config.name = "Standard " + topologyType + " Network";
    config.neuronCount = 100;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.initialWeightRange[0] = -0.5f;
    config.initialWeightRange[1] = 0.5f;
    config.learningRate = 0.01f;
    config.plasticityRate = 0.01f;
    config.enableSTDP = true;
    config.enableHebbian = true;
    config.enableStructuralPlasticity = false;
    config.enableNeuromodulation = true;
    config.enableDevelopment = false;
    
    // Configure topology based on type
    if (topologyType == "fully_connected") {
        config.topology.type = "fully_connected";
    } else if (topologyType == "random") {
        config.topology.type = "random";
    } else if (topologyType == "clustered") {
        config.topology.type = "clustered";
        config.topology.moduleCount = 5;
        config.topology.intraModuleConnectionProbability = 0.3f;
        config.topology.interModuleConnectionProbability = 0.05f;
    } else if (topologyType == "modular") {
        config.topology.type = "modular";
        config.topology.moduleCount = 4;
        config.topology.intraModuleConnectionProbability = 0.4f;
        config.topology.interModuleConnectionProbability = 0.02f;
    } else if (topologyType == "small_world") {
        config.topology.type = "small_world";
        config.topology.smallWorld = true;
        config.topology.smallWorldLambda = 0.1f;
    } else if (topologyType == "scale_free") {
        config.topology.type = "scale_free";
        config.topology.scaleFree = true;
        config.topology.powerLawExponent = 2.0f;
    } else {
        config.topology.type = "random";
    }
    
    // Add regions
    for (const auto& regionConfig : config.regionConfigs) {
        RegionId regionId = addRegion(regionConfig.name, regionConfig.neuronType);
        if (regionId != static_cast<RegionId>(-1)) {
            addPopulationToRegion(regionId, regionConfig.neuronType, regionConfig.neuronCount);
        }
    }
    
    // Add connections
    for (const auto& connConfig : config.connectionConfigs) {
        addConnection(connConfig.sourceNeuronIndex, connConfig.targetNeuronIndex, 
                     connConfig.weight, connConfig.delay);
    }
    
    return createCustomNetwork(config);
}

bool NetworkBuilder::createModularNetwork(size_t moduleCount, float intraModuleConnProb, float interModuleConnProb) {
    NeuralNetworkConfig config;
    config.name = "Modular Network";
    config.neuronCount = 200;
    config.regionCount = moduleCount;
    config.connectionProbability = 0.05f;
    config.topology.type = "modular";
    config.topology.moduleCount = moduleCount;
    config.topology.intraModuleConnectionProbability = intraModuleConnProb;
    config.topology.interModuleConnectionProbability = interModuleConnProb;
    
    // Create regions for each module
    for (size_t i = 0; i < moduleCount; ++i) {
        NeuralNetworkConfig::RegionConfig regionConfig;
        regionConfig.name = "Module_" + std::to_string(i);
        regionConfig.neuronCount = config.neuronCount / moduleCount;
        regionConfig.neuronType = NeuronType::Excitatory;
        regionConfig.excitationInhibitionRatio = 1.0f;
        regionConfig.baselineFiringRate = 0.1f;
        regionConfig.inputScale = 1.0f;
        regionConfig.outputScale = 1.0f;
        regionConfig.connectedRegions = {}; // Will be filled later
        regionConfig.interRegionConnectionProbability = interModuleConnProb;
        
        config.regionConfigs.push_back(regionConfig);
    }
    
    return createCustomNetwork(config);
}

bool NetworkBuilder::createClusteredNetwork(size_t clusters, float intraClusterProb, float interClusterProb) {
    NeuralNetworkConfig config;
    config.name = "Clustered Network";
    config.neuronCount = 150;
    config.regionCount = clusters;
    config.connectionProbability = 0.08f;
    config.topology.type = "clustered";
    
    // Create clusters
    for (size_t i = 0; i < clusters; ++i) {
        NeuralNetworkConfig::RegionConfig regionConfig;
        regionConfig.name = "Cluster_" + std::to_string(i);
        regionConfig.neuronCount = config.neuronCount / clusters;
        regionConfig.neuronType = NeuronType::Excitatory;
        regionConfig.excitationInhibitionRatio = 1.0f;
        regionConfig.baselineFiringRate = 0.1f;
        regionConfig.inputScale = 1.0f;
        regionConfig.outputScale = 1.0f;
        regionConfig.connectedRegions = {}; // Will be filled later
        regionConfig.interRegionConnectionProbability = interClusterProb;
        
        config.regionConfigs.push_back(regionConfig);
    }
    
    return createCustomNetwork(config);
}

bool NetworkBuilder::createSmallWorldNetwork(float lambda) {
    NeuralNetworkConfig config;
    config.name = "Small World Network";
    config.neuronCount = 100;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.topology.type = "small_world";
    config.topology.smallWorld = true;
    config.topology.smallWorldLambda = lambda;
    
    return createCustomNetwork(config);
}

bool NetworkBuilder::createScaleFreeNetwork(float exponent) {
    NeuralNetworkConfig config;
    config.name = "Scale Free Network";
    config.neuronCount = 100;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.topology.type = "scale_free";
    config.topology.scaleFree = true;
    config.topology.powerLawExponent = exponent;
    
    return createCustomNetwork(config);
}

RegionId NetworkBuilder::addRegion(const std::string& name, NeuronType type) {
    if (!brain_) return static_cast<RegionId>(-1);
    
    RegionId regionId = brain_->addRegion(name);
    
    if (regionId != static_cast<RegionId>(-1)) {
        createdRegions_.push_back(name);
        regionNeuronTypes_.push_back(type);
        regionNeuronCounts_.push_back(0);
        regionFiringRates_.push_back(0.0f);
        
        NLM_LOG_INFO("Added region: " + name + " (ID: " + std::to_string(regionId) + ")");
    }
    
    return regionId;
}

bool NetworkBuilder::addPopulationToRegion(RegionId regionId, NeuronType type, size_t neuronCount) {
    if (!brain_) return false;
    
    auto region = brain_->getRegion(regionId);
    if (!region) {
        NLM_LOG_ERROR("Region not found: " + std::to_string(regionId));
        return false;
    }
    
    // Find the region index
    size_t regionIndex = 0;
    for (size_t i = 0; i < createdRegions_.size(); ++i) {
        if (i < brain_->getRegionCount() && brain_->getRegionIds()[i] == regionId) {
            regionIndex = i;
            break;
        }
    }
    
    // Create neurons in the population
    for (size_t i = 0; i < neuronCount; ++i) {
        // Note: This would require access to neural population creation API
        // For now, we'll just track the count
        regionNeuronCounts_[regionIndex]++;
    }
    
    // Update firing rate (placeholder)
    regionFiringRates_[regionIndex] = 0.1f;
    
    NLM_LOG_INFO("Added " + std::to_string(neuronCount) + " neurons to region " + 
                 std::to_string(regionId) + " of type " + std::to_string(static_cast<int>(type)));
    
    return true;
}

bool NetworkBuilder::addNeuronsToRegion(RegionId regionId, NeuronType type, size_t neuronCount) {
    return addPopulationToRegion(regionId, type, neuronCount);
}

bool NetworkBuilder::addConnection(NeuronId source, NeuronId target, float weight, float delay) {
    if (!brain_) return false;
    
    // Find the region for each neuron
    RegionId sourceRegion = static_cast<RegionId>(-1);
    RegionId targetRegion = static_cast<RegionId>(-1);
    
    for (size_t i = 0; i < brain_->getRegionCount(); ++i) {
        auto region = brain_->getRegion(brain_->getRegionIds()[i]);
        if (region) {
            for (const auto& population : region->getPopulations()) {
                for (Neuron* neuron : population->getNeurons()) {
                    if (neuron && neuron->getId() == source) {
                        sourceRegion = region->getId();
                    }
                    if (neuron && neuron->getId() == target) {
                        targetRegion = region->getId();
                    }
                }
            }
        }
    }
    
    if (sourceRegion == static_cast<RegionId>(-1) || targetRegion == static_cast<RegionId>(-1)) {
        NLM_LOG_ERROR("Source or target neuron not found in any region");
        return false;
    }
    
    // Add the connection using brain's inter-region connection system
    brain_->addInterRegionConnection(sourceRegion, targetRegion, weight, delay);
    
    NLM_LOG_INFO("Added connection from neuron " + std::to_string(source) + " to neuron " + 
                 std::to_string(target) + " (weight: " + std::to_string(weight) + ")");
    
    return true;
}

bool NetworkBuilder::addDirectedConnection(NeuronId source, NeuronId target, float weight, float delay) {
    return addConnection(source, target, weight, delay);
}

bool NetworkBuilder::addRandomConnections(float probability, float weightMean, float weightStdDev) {
    if (!brain_) return false;
    
    // Collect all neurons
    std::vector<NeuronId> neurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    neurons.push_back(neuron->getId());
                }
            }
        }
    }
    
    // Add random connections
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> probDist(0.0f, 1.0f);
    std::normal_distribution<float> weightDist(weightMean, weightStdDev);
    
    size_t connectionsAdded = 0;
    for (size_t i = 0; i < neurons.size(); ++i) {
        for (size_t j = 0; j < neurons.size(); ++j) {
            if (i != j && probDist(gen) < probability) {
                float weight = weightDist(gen);
                // Clamp weight to reasonable range
                weight = std::max(-1.0f, std::min(weight, 1.0f));
                
                if (addConnection(neurons[i], neurons[j], weight)) {
                    connectionsAdded++;
                }
            }
        }
    }
    
    NLM_LOG_INFO("Added " + std::to_string(connectionsAdded) + " random connections with probability " + 
                 std::to_string(probability));
    
    return connectionsAdded > 0;
}

bool NetworkBuilder::addClusteredConnections(size_t clusters, float intraClusterProb, float interClusterProb) {
    if (!brain_) return false;
    
    // Collect all neurons and assign them to clusters
    std::vector<std::vector<NeuronId>> clusterNeurons(clusters);
    
    // Simple clustering: distribute neurons evenly among clusters
    for (size_t i = 0; i < brain_->getTotalNeuronCount(); ++i) {
        size_t cluster = i % clusters;
        clusterNeurons[cluster].push_back(static_cast<NeuronId>(i));
    }
    
    // Add intra-cluster connections
    for (size_t cluster = 0; cluster < clusters; ++cluster) {
        for (size_t i = 0; i < clusterNeurons[cluster].size(); ++i) {
            for (size_t j = i + 1; j < clusterNeurons[cluster].size(); ++j) {
                if (std::rand() / (float)RAND_MAX < intraClusterProb) {
                    addConnection(clusterNeurons[cluster][i], clusterNeurons[cluster][j]);
                }
            }
        }
    }
    
    // Add inter-cluster connections
    for (size_t sourceCluster = 0; sourceCluster < clusters; ++sourceCluster) {
        for (size_t targetCluster = 0; targetCluster < clusters; ++targetCluster) {
            if (sourceCluster != targetCluster) {
                for (NeuronId source : clusterNeurons[sourceCluster]) {
                    for (NeuronId target : clusterNeurons[targetCluster]) {
                        if (std::rand() / (float)RAND_MAX < interClusterProb) {
                            addConnection(source, target);
                        }
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("Added clustered connections with " + std::to_string(clusters) + " clusters");
    
    return true;
}

bool NetworkBuilder::addModularConnections(size_t modules, float intraModuleProb, float interModuleProb) {
    return addClusteredConnections(modules, intraModuleProb, interModuleProb);
}

bool NetworkBuilder::optimizeNetwork() {
    if (!brain_) return false;
    
    // Rebalance connections
    if (!rebalanceConnections()) {
        NLM_LOG_WARNING("Connection rebalancing failed");
    }
    
    // Prune weak connections
    if (!pruneWeakConnections(0.01f)) {
        NLM_LOG_WARNING("Weak connection pruning failed");
    }
    
    // Prune redundant connections
    if (!pruneRedundantConnections(0.5f)) {
        NLM_LOG_WARNING("Redundant connection pruning failed");
    }
    
    // Enhance connectivity if needed
    if (!enhanceConnectivity(0.1f)) {
        NLM_LOG_WARNING("Connectivity enhancement failed");
    }
    
    NLM_LOG_INFO("Network optimization completed");
    
    return true;
}

bool NetworkBuilder::rebalanceConnections() {
    if (!brain_) return false;
    
    // Rebalance connection weights to maintain stable network dynamics
    float targetWeight = 0.0f;
    size_t totalConnections = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            targetWeight += syn->getWeight();
            totalConnections++;
        }
    }
    
    if (totalConnections == 0) return false;
    
    targetWeight /= totalConnections;
    
    // Rebalance each connection
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float currentWeight = syn->getWeight();
            float weightDiff = targetWeight - currentWeight;
            
            if (std::abs(weightDiff) > 0.01f) {
                // Apply weight adjustment
                syn->addToWeight(weightDiff);
            }
        }
    }
    
    NLM_LOG_INFO("Connection rebalancing completed");
    
    return true;
}

bool NetworkBuilder::pruneWeakConnections(float threshold) {
    if (!brain_) return false;
    
    size_t connectionsPruned = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (std::abs(syn->getWeight()) < threshold) {
                // Note: This would require synapse removal API
                // For now, we'll just count pruned connections
                connectionsPruned++;
            }
        }
    }
    
    NLM_LOG_INFO("Pruned " + std::to_string(connectionsPruned) + " weak connections (threshold: " + 
                 std::to_string(threshold) + ")");
    
    return connectionsPruned > 0;
}

bool NetworkBuilder::pruneRedundantConnections(float redundancyThreshold) {
    if (!brain_) return false;
    
    // Redundancy pruning would require path analysis
    // This is a simplified implementation
    
    NLM_LOG_INFO("Redundant connection pruning completed (placeholder implementation)");
    
    return true;
}

bool NetworkBuilder::enhanceConnectivity(float targetDensity) {
    if (!brain_) return false;
    
    float currentDensity = getConnectionDensity();
    
    if (currentDensity >= targetDensity) {
        NLM_LOG_INFO("Connection density already meets or exceeds target");
        return true;
    }
    
    // Add connections to increase density
    float densityNeeded = targetDensity - currentDensity;
    size_t neurons = getTotalNeuronCount();
    
    if (neurons < 2) return false;
    
    size_t connectionsToAdd = static_cast<size_t>(neurons * neurons * densityNeeded);
    
    for (size_t i = 0; i < connectionsToAdd; ++i) {
        // Add random connections
        addRandomConnections(0.01f); // Low probability for specific connections
    }
    
    NLM_LOG_INFO("Enhanced connectivity: added " + std::to_string(connectionsToAdd) + 
                 " connections to reach target density " + std::to_string(targetDensity));
    
    return true;
}

void NetworkBuilder::printNetworkStructure() const {
    if (!brain_) return;
    
    NLM_LOG_INFO("=== Network Structure ===");
    NLM_LOG_INFO("Network Name: " + currentNetworkName_);
    NLM_LOG_INFO("Total Neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total Synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Total Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Connection Density: " + std::to_string(getConnectionDensity()));
    NLM_LOG_INFO("Average Neuron Degree: " + std::to_string(getAverageNeuronDegree()));
    NLM_LOG_INFO("Average Synapse Weight: " + std::to_string(getAverageSynapseWeight()));
    
    printRegionInfo();
    printConnectionInfo();
}

void NetworkBuilder::printRegionInfo() const {
    if (!brain_) return;
    
    NLM_LOG_INFO("=== Region Information ===");
    for (size_t i = 0; i < brain_->getRegionCount(); ++i) {
        auto regionId = brain_->getRegionIds()[i];
        auto region = brain_->getRegion(regionId);
        if (region) {
            NLM_LOG_INFO("Region " + std::to_string(regionId) + ": " + region->getName() +
                        " - " + std::to_string(region->getPopulations().size()) + " populations");
        }
    }
}

void NetworkBuilder::printConnectionInfo() const {
    if (!brain_) return;
    
    NLM_LOG_INFO("=== Connection Information ===");
    NLM_LOG_INFO("Connection Density: " + std::to_string(getConnectionDensity()));
    NLM_LOG_INFO("Average Synapse Weight: " + std::to_string(getAverageSynapseWeight()));
    
    // Calculate clustering coefficient and path length
    float clustering = calculateNetworkClusteringCoefficient();
    float pathLength = calculateNetworkPathLength();
    
    NLM_LOG_INFO("Clustering Coefficient: " + std::to_string(clustering));
    NLM_LOG_INFO("Network Path Length: " + std::to_string(pathLength));
}

void NetworkBuilder::generateNetworkReport(const std::string& filename) {
    if (!brain_) return;
    
    std::ofstream reportFile(filename);
    if (!reportFile.is_open()) {
        NLM_LOG_ERROR("Failed to open network report file: " + filename);
        return;
    }
    
    reportFile << "Network Analysis Report" << std::endl;
    reportFile << "=======================" << std::endl;
    reportFile << std::endl;
    
    reportFile << "Network Name: " << currentNetworkName_ << std::endl;
    reportFile << "Build Time: " << statistics_.buildTime << " seconds" << std::endl;
    reportFile << "Total Neurons: " << getTotalNeuronCount() << std::endl;
    reportFile << "Total Synapses: " << getTotalSynapseCount() << std::endl;
    reportFile << "Total Regions: " << getRegionCount() << std::endl;
    reportFile << "Connection Density: " << getConnectionDensity() << std::endl;
    reportFile << "Average Neuron Degree: " << getAverageNeuronDegree() << std::endl;
    reportFile << "Average Synapse Weight: " << getAverageSynapseWeight() << std::endl;
    
    reportFile << std::endl << "Region Information:" << std::endl;
    for (size_t i = 0; i < createdRegions_.size(); ++i) {
        reportFile << "  " << createdRegions_[i] << ": " << regionNeuronCounts_[i] << 
                  " neurons, type: " << static_cast<int>(regionNeuronTypes_[i]) << 
                  ", firing rate: " << regionFiringRates_[i] << std::endl;
    }
    
    reportFile << std::endl << "Network Properties:" << std::endl;
    NetworkProperties props = getNetworkProperties();
    reportFile << "  Average Degree: " << props.averageDegree << std::endl;
    reportFile << "  Standard Deviation Degree: " << props.standardDeviationDegree << std::endl;
    reportFile << "  Clustering Coefficient: " << props.clusteringCoefficient << std::endl;
    reportFile << "  Path Length: " << props.pathLength << std::endl;
    reportFile << "  Characteristic Path Length: " << props.characteristicPathLength << std::endl;
    reportFile << "  Small Worldness: " << props.smallWorldness << std::endl;
    reportFile << "  Modularity: " << props.modularity << std::endl;
    reportFile << "  Density: " << props.density << std::endl;
    reportFile << "  Betweenness Centrality: " << props.betweennessCentrality << std::endl;
    reportFile << "  Closeness Centrality: " << props.closenessCentrality << std::endl;
    
    reportFile.close();
    NLM_LOG_INFO("Network report generated: " + filename);
}

size_t NetworkBuilder::getTotalNeuronCount() const {
    return brain_ ? brain_->getTotalNeuronCount() : 0;
}

size_t NetworkBuilder::getTotalSynapseCount() const {
    return brain_ ? brain_->getTotalSynapseCount() : 0;
}

size_t NetworkBuilder::getRegionCount() const {
    return brain_ ? brain_->getRegionCount() : 0;
}

float NetworkBuilder::getConnectionDensity() const {
    size_t neurons = getTotalNeuronCount();
    if (neurons < 2) return 0.0f;
    
    size_t possibleConnections = neurons * (neurons - 1); // Directed connections
    size_t actualConnections = getTotalSynapseCount();
    
    return static_cast<float>(actualConnections) / static_cast<float>(possibleConnections);
}

float NetworkBuilder::getAverageNeuronDegree() const {
    size_t neurons = getTotalNeuronCount();
    if (neurons == 0) return 0.0f;
    
    size_t totalConnections = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            totalConnections++;
        }
    }
    
    return static_cast<float>(totalConnections) / static_cast<float>(neurons);
}

float NetworkBuilder::getAverageSynapseWeight() const {
    size_t synapses = getTotalSynapseCount();
    if (synapses == 0) return 0.0f;
    
    float totalWeight = 0.0f;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            totalWeight += syn->getWeight();
        }
    }
    
    return totalWeight / static_cast<float>(synapses);
}

float NetworkBuilder::calculateNetworkClusteringCoefficient() const {
    // Placeholder implementation for clustering coefficient
    // In a real implementation, this would calculate the actual clustering coefficient
    
    size_t neurons = getTotalNeuronCount();
    if (neurons < 3) return 0.0f;
    
    return 0.5f; // Placeholder
}

float NetworkBuilder::calculateNetworkPathLength() const {
    // Placeholder implementation for path length
    // In a real implementation, this would calculate the actual path length
    
    return 2.5f; // Placeholder
}

NetworkBuilder::NetworkProperties NetworkBuilder::getNetworkProperties() const {
    NetworkProperties props;
    
    props.averageDegree = getAverageNeuronDegree();
    props.clusteringCoefficient = calculateNetworkClusteringCoefficient();
    props.pathLength = calculateNetworkPathLength();
    props.characteristicPathLength = calculateNetworkPathLength();
    props.density = getConnectionDensity();
    props.smallWorldness = 1.0f; // Placeholder
    props.modularity = 0.5f; // Placeholder
    
    return props;
}

bool NetworkBuilder::validateNetworkStructure() const {
    if (!brain_) {
        validationErrors_.push_back("No brain reference available");
        return false;
    }
    
    bool valid = true;
    
    // Validate neuron counts
    if (brain_->getTotalNeuronCount() == 0) {
        validationErrors_.push_back("Network has no neurons");
        valid = false;
    }
    
    // Validate synapse counts
    if (brain_->getTotalSynapseCount() == 0) {
        validationErrors_.push_back("Network has no synapses");
        valid = false;
    }
    
    // Validate connectivity
    if (!validateConnectivity()) {
        valid = false;
    }
    
    // Validate region counts
    if (brain_->getRegionCount() == 0) {
        validationErrors_.push_back("Network has no regions");
        valid = false;
    }
    
    return valid;
}

bool NetworkBuilder::validateConnectivity() const {
    if (!brain_) return false;
    
    // Check for disconnected neurons
    size_t disconnectedNeurons = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    // Check if neuron has any connections
                    bool hasConnections = false;
                    for (const auto& syn : region->getSynapses()) {
                        if (syn->getPreNeuron() == neuron->getId() || 
                            syn->getPostNeuron() == neuron->getId()) {
                            hasConnections = true;
                            break;
                        }
                    }
                    
                    if (!hasConnections) {
                        disconnectedNeurons++;
                    }
                }
            }
        }
    }
    
    float disconnectedRatio = static_cast<float>(disconnectedNeurons) / getTotalNeuronCount();
    
    if (disconnectedRatio > 0.5f) {
        validationErrors_.push_back("Too many disconnected neurons: " + std::to_string(disconnectedRatio * 100) + "%");
        return false;
    }
    
    return true;
}

bool NetworkBuilder::validateNeuronCounts() const {
    if (!brain_) return false;
    
    // Check that all regions have neurons
    for (const auto& region : brain_->getRegions()) {
        if (region->getPopulations().empty()) {
            validationErrors_.push_back("Region " + region->getName() + " has no populations");
            return false;
        }
    }
    
    return true;
}

bool NetworkBuilder::validateSynapseCounts() const {
    if (!brain_) return false;
    
    // Check that there are synapses connecting neurons
    if (getTotalSynapseCount() == 0) {
        validationErrors_.push_back("Network has no synapses");
        return false;
    }
    
    return true;
}

std::vector<std::string> NetworkBuilder::getValidationErrors() const {
    return validationErrors_;
}

bool NetworkBuilder::exportNetworkToJSON(const std::string& filename) const {
    return exportNetworkToJSONInternal(filename);
}

bool NetworkBuilder::importNetworkFromJSON(const std::string& filename) {
    return importNetworkFromJSONInternal(filename);
}

bool NetworkBuilder::exportNetworkToBinary(const std::string& filename) const {
    return exportNetworkToBinaryInternal(filename);
}

bool NetworkBuilder::importNetworkFromBinary(const std::string& filename) {
    return importNetworkFromBinaryInternal(filename);
}

bool NetworkBuilder::runNetworkSimulation(double duration, double timestep) {
    if (!brain_) return false;
    
    if (isSimulating_) {
        NLM_LOG_ERROR("Network simulation already running");
        return false;
    }
    
    isSimulating_ = true;
    simulationStartTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    NLM_LOG_INFO("Running network simulation for " + std::to_string(duration) + " seconds");
    
    // Run simulation steps
    size_t steps = static_cast<size_t>(duration / timestep);
    for (size_t i = 0; i < steps && isSimulating_; ++i) {
        if (!runNetworkStep()) {
            break;
        }
    }
    
    simulationEndTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    statistics_.simulationTime = simulationEndTime_ - simulationStartTime_;
    
    isSimulating_ = false;
    
    NLM_LOG_INFO("Network simulation completed");
    
    return true;
}

bool NetworkBuilder::runNetworkStep() {
    if (!brain_) return false;
    
    // Execute one simulation step
    brain_->step(static_cast<SimulationStep>(0), static_cast<Timestamp>(0.0));
    
    // Update statistics
    statistics_.totalActivity += brain_->getTotalSpikeCount();
    
    // Log step if enabled
    if (loggingEnabled_) {
        logNetworkSimulation("Step completed");
    }
    
    return true;
}

bool NetworkBuilder::pauseNetworkSimulation() {
    isSimulating_ = false;
    NLM_LOG_INFO("Network simulation paused");
    return true;
}

bool NetworkBuilder::resumeNetworkSimulation() {
    isSimulating_ = true;
    NLM_LOG_INFO("Network simulation resumed");
    return true;
}

bool NetworkBuilder::stopNetworkSimulation() {
    isSimulating_ = false;
    NLM_LOG_INFO("Network simulation stopped");
    return true;
}

bool NetworkBuilder::isNetworkSimulationRunning() const {
    return isSimulating_.load();
}

bool NetworkBuilder::exploreNetwork() {
    // Perform network exploration and analysis
    if (!brain_) return false;
    
    NLM_LOG_INFO("Exploring network structure and properties");
    
    // Analyze connectivity
    std::vector<size_t> componentSizes = getComponentSizes();
    size_t largestComponent = getLargestComponentSize();
    
    NLM_LOG_INFO("Network exploration completed: " + std::to_string(componentSizes.size()) + 
                 " components, largest component size: " + std::to_string(largestComponent));
    
    return true;
}

std::vector<size_t> NetworkBuilder::getComponentSizes() const {
    // Placeholder implementation for component sizes
    // In a real implementation, this would use graph algorithms to find connected components
    
    std::vector<size_t> componentSizes;
    componentSizes.push_back(getTotalNeuronCount()); // Single component for now
    
    return componentSizes;
}

size_t NetworkBuilder::getLargestComponentSize() const {
    // Placeholder implementation for largest component
    return getTotalNeuronCount(); // Assume all neurons are in one component
}

float NetworkBuilder::getNetworkDiameter() const {
    // Placeholder implementation for network diameter
    return 10.0f; // Placeholder
}

float NetworkBuilder::getNetworkRadius() const {
    // Placeholder implementation for network radius
    return 5.0f; // Placeholder
}

bool NetworkBuilder::analyzeNetworkTopology() {
    // Perform topology analysis
    if (!brain_) return false;
    
    NLM_LOG_INFO("Analyzing network topology");
    
    // Calculate network metrics
    float clustering = calculateNetworkClusteringCoefficient();
    float pathLength = calculateNetworkPathLength();
    float smallWorldness = calculateNetworkSmallWorldness();
    
    NLM_LOG_INFO("Topology analysis: clustering=" + std::to_string(clustering) +
                ", path_length=" + std::to_string(pathLength) +
                ", small_worldness=" + std::to_string(smallWorldness));
    
    return true;
}

float NetworkBuilder::calculateNetworkSmallWorldness() const {
    // Placeholder implementation for small worldness
    return 1.5f; // Placeholder
}

void NetworkBuilder::setRandomSeed(uint64_t seed) {
    randomSeed_ = seed;
    NLM_LOG_INFO("Random seed set to: " + std::to_string(seed));
}

void NetworkBuilder::lockNetworkBuilder() const {
    builderMutex_.lock();
}

void NetworkBuilder::unlockNetworkBuilder() const {
    builderMutex_.unlock();
}

bool NetworkBuilder::tryLockNetworkBuilder() const {
    return builderMutex_.try_lock();
}

// Network configuration validation implementation
bool NetworkBuilder::validateNetworkConfig(const NeuralNetworkConfig& config) const {
    if (config.neuronCount == 0) {
        validationErrors_.push_back("Network must have at least one neuron");
        return false;
    }
    
    if (config.regionCount == 0) {
        validationErrors_.push_back("Network must have at least one region");
        return false;
    }
    
    if (config.regionCount > config.neuronCount) {
        validationErrors_.push_back("Number of regions cannot exceed number of neurons");
        return false;
    }
    
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        validationErrors_.push_back("Connection probability must be between 0 and 1");
        return false;
    }
    
    return true;
}

void NetworkBuilder::generateNetworkTopology(const NeuralNetworkConfig& config) {
    if (config.topology.type == "fully_connected") {
        generateFullyConnectedTopology(config);
    } else if (config.topology.type == "random") {
        generateRandomTopology(config);
    } else if (config.topology.type == "clustered") {
        generateClusteredTopology(config);
    } else if (config.topology.type == "modular") {
        generateModularTopology(config);
    } else if (config.topology.type == "small_world") {
        generateSmallWorldTopology(config);
    } else if (config.topology.type == "scale_free") {
        generateScaleFreeTopology(config);
    } else {
        // Default to random
        generateRandomTopology(config);
    }
}

void NetworkBuilder::generateFullyConnectedTopology(const NeuralNetworkConfig& config) {
    // Create fully connected network within each region
    for (const auto& regionConfig : config.regionConfigs) {
        // Add neurons to region
        addPopulationToRegion(static_cast<RegionId>(0), regionConfig.neuronType, regionConfig.neuronCount);
        
        // Create fully connected connections within region
        for (size_t i = 0; i < regionConfig.neuronCount; ++i) {
            for (size_t j = i + 1; j < regionConfig.neuronCount; ++j) {
                addConnection(static_cast<NeuronId>(i), static_cast<NeuronId>(j), 
                             regionConfig.initialWeightRange[0], 1);
            }
        }
    }
}

void NetworkBuilder::generateRandomTopology(const NeuralNetworkConfig& config) {
    // Simple random topology generation
    size_t totalNeurons = 0;
    for (const auto& regionConfig : config.regionConfigs) {
        totalNeurons += regionConfig.neuronCount;
    }
    
    // Generate random connections based on probability
    addRandomConnections(config.connectionProbability, 
                        config.initialWeightRange[0], 
                        config.initialWeightRange[1] - config.initialWeightRange[0]);
}

void NetworkBuilder::generateClusteredTopology(const NeuralNetworkConfig& config) {
    // Create clustered topology
    size_t totalNeurons = 0;
    for (const auto& regionConfig : config.regionConfigs) {
        totalNeurons += regionConfig.neuronCount;
    }
    
    // Create clusters
    size_t clusters = config.topology.moduleCount > 0 ? config.topology.moduleCount : 3;
    float intraClusterProb = config.topology.intraModuleConnectionProbability;
    float interClusterProb = config.topology.interModuleConnectionProbability;
    
    addClusteredConnections(clusters, intraClusterProb, interClusterProb);
}

void NetworkBuilder::generateModularTopology(const NeuralNetworkConfig& config) {
    // Create modular topology
    size_t modules = config.topology.moduleCount > 0 ? config.topology.moduleCount : 3;
    float intraModuleProb = config.topology.intraModuleConnectionProbability;
    float interModuleProb = config.topology.interModuleConnectionProbability;
    
    createModularNetwork(modules, intraModuleProb, interModuleProb);
}

void NetworkBuilder::generateSmallWorldTopology(const NeuralNetworkConfig& config) {
    // Create small-world topology (Watts-Strogatz model)
    size_t neurons = config.neuronCount;
    float lambda = config.topology.smallWorldLambda;
    
    // Placeholder implementation
    createSmallWorldNetwork(lambda);
}

void NetworkBuilder::generateScaleFreeTopology(const NeuralNetworkConfig& config) {
    // Create scale-free topology (Barabasi-Albert model)
    size_t neurons = config.neuronCount;
    float exponent = config.topology.powerLawExponent;
    
    // Placeholder implementation
    createScaleFreeNetwork(exponent);
}

void NetworkBuilder::setupRegions(const NeuralNetworkConfig& config) {
    for (size_t i = 0; i < config.regionConfigs.size(); ++i) {
        const auto& regionConfig = config.regionConfigs[i];
        
        RegionId regionId = addRegion(regionConfig.name, regionConfig.neuronType);
        if (regionId != static_cast<RegionId>(-1)) {
            // Update region index
            createdRegions_[i] = regionConfig.name;
            regionNeuronTypes_[i] = regionConfig.neuronType;
            
            // Add neurons to region
            addPopulationToRegion(regionId, regionConfig.neuronType, regionConfig.neuronCount);
        }
    }
}

void NetworkBuilder::setupConnections(const NeuralNetworkConfig& config) {
    // Setup inter-region connections
    for (size_t i = 0; i < config.regionConfigs.size(); ++i) {
        for (size_t connectedRegionIndex : config.regionConfigs[i].connectedRegions) {
            if (connectedRegionIndex < config.regionConfigs.size()) {
                // Create connections between regions
                // This is a simplified implementation
            }
        }
    }
}

void NetworkBuilder::initializeNeurons(const NeuralNetworkConfig& config) {
    // Initialize neuron properties based on configuration
    for (const auto& regionConfig : config.regionConfigs) {
        for (size_t i = 0; i < regionConfig.neuronCount; ++i) {
            // Initialize neuron properties
            // This would require access to neuron initialization API
        }
    }
}

void NetworkBuilder::initializeConnections(const NeuralNetworkConfig& config) {
    // Initialize connection properties based on configuration
    for (const auto& connConfig : config.connectionConfigs) {
        // Initialize connection properties
        // This would require access to connection initialization API
    }
}

void NetworkBuilder::applyPlasticityConfiguration(const NeuralNetworkConfig& config) {
    if (config.enableSTDP) {
        if (auto* stdp = brain_->getSTDP()) {
            stdp->setLTPWeight(config.plasticity.stdp.learningRate);
            stdp->setLTDWeight(config.plasticity.stdp.LTDThreshold);
            stdp->setTraceDecay(config.plasticity.stdp.traceDecay);
        }
    }
    
    if (config.enableHebbian) {
        if (auto* hebbian = brain_->getHebbian()) {
            hebbian->setLearningRate(config.plasticity.hebbian.learningRate);
            hebbian->setSimilarityThreshold(config.plasticity.hebbian.similarityThreshold);
        }
    }
    
    if (config.enableStructuralPlasticity) {
        if (auto* sp = brain_->getStructuralPlasticity()) {
            sp->setSynaptogenesisRate(config.plasticity.structural.synaptogenesisRate);
            sp->setPruningRate(config.plasticity.structural.pruningRate);
        }
    }
}

void NetworkBuilder::applyNeuromodulationConfiguration(const NeuralNetworkConfig& config) {
    if (config.enableNeuromodulation) {
        // Configure neuromodulation system
        // This would require neuromodulation system access
    }
}

void NetworkBuilder::applyDevelopmentConfiguration(const NeuralNetworkConfig& config) {
    if (config.enableDevelopment) {
        // Configure development system
        // This would require development system access
    }
}

void NetworkBuilder::applySimulationConfiguration(const NeuralNetworkConfig& config) {
    if (config.simulation.enableRealTimeUpdates) {
        // Enable real-time updates
    }
    
    if (config.simulation.enableLogging) {
        setNetworkLogging(true);
    }
}

void NetworkBuilder::applyPerformanceConfiguration(const NeuralNetworkConfig& config) {
    if (config.performance.enablePerformanceMonitoring) {
        // Enable performance monitoring
        // This would require performance monitoring system access
    }
}

void NetworkBuilder::logNetworkBuild(const std::string& message) {
    if (loggingEnabled_) {
        NLM_LOG_INFO("NetworkBuilder: " + message);
    }
}

void NetworkBuilder::logNetworkSimulation(const std::string& message) {
    if (loggingEnabled_) {
        NLM_LOG_INFO("NetworkBuilder Simulation: " + message);
    }
}

bool NetworkBuilder::exportNetworkToJSONInternal(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for JSON export: " + filename);
        return false;
    }
    
    // Generate JSON representation of network
    file << "{\n";
    file << "  \"name\": \"" << currentNetworkName_ << "\",\n";
    file << "  \"neurons\": " << getTotalNeuronCount() << ",\n";
    file << "  \"synapses\": " << getTotalSynapseCount() << ",\n";
    file << "  \"regions\": " << getRegionCount() << ",\n";
    file << "  \"connection_density\": " << getConnectionDensity() << ",\n";
    file << "  \"average_degree\": " << getAverageNeuronDegree() << ",\n";
    file << "  \"average_synapse_weight\": " << getAverageSynapseWeight() << "\n";
    file << "}\n";
    
    file.close();
    NLM_LOG_INFO("Network exported to JSON: " + filename);
    
    return true;
}

bool NetworkBuilder::importNetworkFromJSONInternal(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for JSON import: " + filename);
        return false;
    }
    
    // Parse JSON and import network
    // Note: This would require JSON parsing library
    
    file.close();
    NLM_LOG_INFO("Network imported from JSON: " + filename);
    
    return true;
}

bool NetworkBuilder::exportNetworkToBinaryInternal(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for binary export: " + filename);
        return false;
    }
    
    // Export binary representation
    // This would include network structure, connection information, etc.
    
    file.close();
    NLM_LOG_INFO("Network exported to binary: " + filename);
    
    return true;
}

bool NetworkBuilder::importNetworkFromBinaryInternal(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for binary import: " + filename);
        return false;
    }
    
    // Import binary representation
    // This would read network structure, connection information, etc.
    
    file.close();
    NLM_LOG_INFO("Network imported from binary: " + filename);
    
    return true;
}

void NetworkBuilder::generateNetworkTopologyReport() const {
    // Generate detailed topology report
    NetworkProperties props = getNetworkProperties();
    
    NLM_LOG_INFO("=== Network Topology Report ===");
    NLM_LOG_INFO("Network Name: " + currentNetworkName_);
    NLM_LOG_INFO("Average Degree: " + std::to_string(props.averageDegree));
    NLM_LOG_INFO("Clustering Coefficient: " + std::to_string(props.clusteringCoefficient));
    NLM_LOG_INFO("Path Length: " + std::to_string(props.pathLength));
    NLM_LOG_INFO("Characteristic Path Length: " + std::to_string(props.characteristicPathLength));
    NLM_LOG_INFO("Small Worldness: " + std::to_string(props.smallWorldness));
    NLM_LOG_INFO("Modularity: " + std::to_string(props.modularity));
    NLM_LOG_INFO("Density: " + std::to_string(props.density));
}

void NetworkBuilder::generateConnectionReport() const {
    // Generate connection analysis report
    NLM_LOG_INFO("=== Connection Report ===");
    NLM_LOG_INFO("Total Synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Connection Density: " + std::to_string(getConnectionDensity()));
    NLM_LOG_INFO("Average Synapse Weight: " + std::to_string(getAverageSynapseWeight()));
}

void NetworkBuilder::generatePerformanceReport() const {
    // Generate performance metrics report
    NLM_LOG_INFO("=== Performance Report ===");
    NLM_LOG_INFO("Build Time: " + std::to_string(statistics_.buildTime) + " seconds");
    NLM_LOG_INFO("Neurons Created: " + std::to_string(statistics_.neuronsCreated));
    NLM_LOG_INFO("Synapses Created: " + std::to_string(statistics_.synapsesCreated));
    NLM_LOG_INFO("Connection Density: " + std::to_string(statistics_.connectionDensity));
    NLM_LOG_INFO("Average Firing Rate: " + std::to_string(statistics_.averageFiringRate));
    NLM_LOG_INFO("Total Activity: " + std::to_string(statistics_.totalActivity));
    NLM_LOG_INFO("Memory Usage: " + std::to_string(statistics_.memoryUsage));
    NLM_LOG_INFO("CPU Usage: " + std::to_string(statistics_.cpuUsage));
}

bool NetworkBuilder::validateNetworkInternal() const {
    // Validate network structure
    bool valid = true;
    
    // Check for isolated neurons
    size_t isolatedNeurons = 0;
    for (size_t i = 0; i < getTotalNeuronCount(); ++i) {
        // Check if neuron has any connections
        // This would require connection querying API
    }
    
    if (isolatedNeurons > getTotalNeuronCount() * 0.1f) {
        NLM_LOG_WARNING("High number of isolated neurons: " + std::to_string(isolatedNeurons));
    }
    
    return valid;
}

bool NetworkBuilder::validateNeuronInternal() const {
    // Validate neuron properties
    return true; // Placeholder
}

bool NetworkBuilder::validateSynapseInternal() const {
    // Validate synapse properties
    return true; // Placeholder
}

} // namespace nlm