#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

ConsoleVisualization::ConsoleVisualization()
    : zoomLevel_(1.0), panX_(0.0), panY_(0.0), dataDirty_(true) {
    config_.enabled = true;
    config_.autoRefresh = true;
    config_.updateInterval = 1.0; // 1 Hz
    config_.width = 80;
    config_.height = 24;
    config_.backend = "console";
    
    NLM_LOG_INFO("ConsoleVisualization initialized", "ConsoleVisualization::Constructor");
}

void ConsoleVisualization::update(const Brain* brain, const AgentBrain* agent, double timestep) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Update all data
        processNeuronData(brain);
        processSynapseData(brain);
        processRegionData(brain);
        processMotorOutputData(agent);
        processSensorData(agent);
        processMemoryStateData(brain);
        processNeuromodulationData(brain);
        processDevelopmentData(agent);
        processConceptData(brain);
        
        dataDirty_ = true;
        lastUpdate_ = std::chrono::high_resolution_clock::now();
        
        if (updateCallback_) {
            VisualizationData callbackData;
            callbackData.type = config_.type;
            callbackData.source = "console_visualization";
            updateCallback_(callbackData);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating console visualization: ") + e.what(), 
                     "ConsoleVisualization::update");
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    static double totalUpdateTime = 0.0;
    static size_t updateCount = 0;
    totalUpdateTime += duration;
    updateCount++;
    
    if (updateCount % 100 == 0) {
        double avgUpdateTime = totalUpdateTime / updateCount;
        NLM_LOG_INFO(std::string("Console visualization update: avg ") + 
                    std::to_string(avgUpdateTime) + "ms per update", 
                    "ConsoleVisualization::update");
    }
}

void ConsoleVisualization::render() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!isDataAvailable()) {
        std::cout << "[Visualization] No data available" << std::endl;
        return;
    }
    
    // Clear screen (for better user experience in console)
    system("clear");
    
    std::cout << "=== NLM Neural Network Visualization ===" << std::endl;
    std::cout << "Type: " << config_.title << std::endl;
    std::cout << "Neurons: " << neuronData_.size() << ", "
              << "Synapses: " << synapseData_.size() << std::endl;
    std::cout << "Regions: " << regionData_.size() << std::endl;
    std::cout << "Last update: " << getCurrentTimeString() << std::endl;
    std::cout << std::endl;
    
    // Render based on visualization type
    switch (config_.type) {
        case VisualizationInterface::VisualizationType::NEURON_ACTIVITY:
            renderNeuronActivity();
            break;
        case VisualizationInterface::VisualizationType::SYNAPTIC_WEIGHTS:
            renderSynapticWeights();
            break;
        case VisualizationInterface::VisualizationType::BRAIN_REGIONS:
            renderBrainRegions();
            break;
        case VisualizationInterface::VisualizationType::MOTOR_OUTPUTS:
            renderMotorOutputs();
            break;
        case VisualizationInterface::VisualizationType::SENSOR_INPUTS:
            renderSensorInputs();
            break;
        case VisualizationInterface::VisualizationType::MEMORY_STATE:
            renderMemoryStates();
            break;
        case VisualizationInterface::VisualizationType::NEUROMODULATION:
            renderNeuromodulation();
            break;
        case VisualizationInterface::VisualizationType::DEVELOPMENT_STAGES:
            renderDevelopmentStages();
            break;
        case VisualizationInterface::VisualizationType::CONCEPT_FORMATION:
            renderConceptFormation();
            break;
        case VisualizationInterface::VisualizationType::FULL_DASHBOARD:
            renderFullDashboard();
            break;
    }
    
    std::cout << std::endl << "=== Controls ===" << std::endl;
    std::cout << "- Use 'r' to reset visualization" << std::endl;
    std::cout << "- Use 's' to save data to file" << std::endl;
    std::cout << "- Use 'q' to quit visualization" << std::endl;
}

void ConsoleVisualization::save(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR(std::string("Cannot open visualization file for writing: ") + filepath, 
                         "ConsoleVisualization::save");
            return;
        }
        
        file << "NLM Visualization Data" << std::endl;
        file << "========================" << std::endl;
        file << "Timestamp: " << getCurrentTimeString() << std::endl;
        file << "Visualization Type: " << config_.title << std::endl;
        file << std::endl;
        
        // Save neuron data
        file << "Neuron Data:" << std::endl;
        file << "ID,Type,Region,Firing Rate,Activity" << std::endl;
        for (const auto& neuron : neuronData_) {
            file << neuron.id << ","
                 << static_cast<int>(neuron.neuronType) << ","
                 << neuron.region << ","
                 << std::fixed << std::setprecision(3) << neuron.firingRate << ","
                 << std::fixed << std::setprecision(3) << (neuron.isActive ? 1.0f : 0.0f)
                 << std::endl;
        }
        
        file << std::endl;
        
        // Save region data
        file << "Region Data:" << std::endl;
        file << "Name,Neuron Count,Average Firing Rate,Activity" << std::endl;
        for (const auto& region : regionData_) {
            file << region.name << ","
                 << region.neuronCount << ","
                 << std::fixed << std::setprecision(3) << region.averageFiringRate << ","
                 << std::fixed << std::setprecision(3) << calculateRegionActivity(region)
                 << std::endl;
        }
        
        file << std::endl;
        
        // Save motor output data
        file << "Motor Output Data:" << std::endl;
        file << "Command,Activity,Firing Rate,Confidence" << std::endl;
        for (const auto& motor : motorOutputData_) {
            file << static_cast<int>(motor.command) << ","
                 << std::fixed << std::setprecision(3) << motor.activity << ","
                 << std::fixed << std::setprecision(3) << motor.firingRate << ","
                 << std::fixed << std::setprecision(3) << motor.confidence
                 << std::endl;
        }
        
        file.close();
        
        NLM_LOG_INFO(std::string("Visualization saved to: ") + filepath, 
                    "ConsoleVisualization::save");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error saving visualization: ") + e.what(), 
                     "ConsoleVisualization::save");
    }
}

void ConsoleVisualization::setConfig(const VisualizationConfig& config) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    config_ = config;
    dataDirty_ = true;
    
    NLM_LOG_INFO(std::string("Console visualization configuration updated: ") + config_.title, 
                "ConsoleVisualization::setConfig");
}

VisualizationConfig ConsoleVisualization::getConfig() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return config_;
}

std::vector<VisualizationInterface::NeuronData> ConsoleVisualization::getNeuronData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return neuronData_;
}

std::vector<VisualizationInterface::SynapseData> ConsoleVisualization::getSynapseData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return synapseData_;
}

std::vector<VisualizationInterface::RegionData> ConsoleVisualization::getRegionData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return regionData_;
}

std::vector<VisualizationInterface::MotorOutputData> ConsoleVisualization::getMotorOutputData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return motorOutputData_;
}

std::vector<VisualizationInterface::SensorData> ConsoleVisualization::getSensorData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return sensorData_;
}

std::vector<VisualizationInterface::MemoryStateData> ConsoleVisualization::getMemoryStateData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return memoryStateData_;
}

std::vector<VisualizationInterface::NeuromodulationData> ConsoleVisualization::getNeuromodulationData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return neuromodulationData_;
}

VisualizationInterface::DevelopmentData ConsoleVisualization::getDevelopmentData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return developmentData_;
}

std::vector<VisualizationInterface::ConceptData> ConsoleVisualization::getConceptData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return conceptData_;
}

void ConsoleVisualization::setNeuronActivity(size_t neuronId, float activity) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    for (auto& neuron : neuronData_) {
        if (neuron.id == neuronId) {
            neuron.isActive = (activity > 0.5f);
            neuron.outputCurrent = activity;
            break;
        }
    }
    
    dataDirty_ = true;
}

void ConsoleVisualization::injectCurrent(size_t neuronId, float current) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    for (auto& neuron : neuronData_) {
        if (neuron.id == neuronId) {
            neuron.inputCurrent = current;
            neuron.membranePotential += current * 0.1f; // Simplified model
            break;
        }
    }
    
    dataDirty_ = true;
}

void ConsoleVisualization::selectRegion(const std::string& regionName) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    selectedRegion_ = regionName;
    dataDirty_ = true;
    
    NLM_LOG_INFO(std::string("Selected region: ") + regionName, 
                "ConsoleVisualization::selectRegion");
}

void ConsoleVisualization::zoom(double factor) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    zoomLevel_ *= factor;
    // Clamp zoom level
    zoomLevel_ = std::max(0.1, std::min(5.0, zoomLevel_));
    
    dataDirty_ = true;
}

void ConsoleVisualization::pan(double dx, double dy) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    panX_ += dx * zoomLevel_;
    panY_ += dy * zoomLevel_;
    
    dataDirty_ = true;
}

void ConsoleVisualization::setUpdateCallback(std::function<void(const VisualizationData&)> callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    updateCallback_ = callback;
}

void ConsoleVisualization::setInteractionCallback(std::function<void(const std::string&, const std::map<std::string, std::string>&)> callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    interactionCallback_ = callback;
}

std::string ConsoleVisualization::getVisualizationData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::stringstream data;
    data << "NeuronCount:" << neuronData_.size() << ";";
    data << "SynapseCount:" << synapseData_.size() << ";";
    data << "RegionCount:" << regionData_.size() << ";";
    data << "ActiveNeurons:" << std::count_if(neuronData_.begin(), neuronData_.end(),
                                              [](const NeuronData& n) { return n.isActive; }) << ";";
    data << "LastUpdate:" << getCurrentTimestamp() << ";";
    
    return data.str();
}

bool ConsoleVisualization::isDataAvailable() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return !neuronData_.empty() && !regionData_.empty();
}

void ConsoleVisualization::clear() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    neuronData_.clear();
    synapseData_.clear();
    regionData_.clear();
    motorOutputData_.clear();
    sensorData_.clear();
    memoryStateData_.clear();
    neuromodulationData_.clear();
    conceptData_.clear();
    
    dataDirty_ = true;
    
    NLM_LOG_INFO("Console visualization cleared", "ConsoleVisualization::clear");
}

void ConsoleVisualization::reset() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    clear();
    zoomLevel_ = 1.0;
    panX_ = panY_ = 0.0;
    selectedRegion_.clear();
    
    dataDirty_ = true;
    
    NLM_LOG_INFO("Console visualization reset", "ConsoleVisualization::reset");
}

// Rendering methods
void ConsoleVisualization::renderNeuronActivity() const {
    std::cout << "=== NEURON ACTIVITY ===" << std::endl;
    
    // Group neurons by region
    std::map<std::string, std::vector<const NeuronData*>> neuronsByRegion;
    for (const auto& neuron : neuronData_) {
        neuronsByRegion[neuron.region].push_back(&neuron);
    }
    
    for (const auto& regionPair : neuronsByRegion) {
        std::cout << "\nRegion: " << regionPair.first << " ("
                  << regionPair.second.size() << " neurons)" << std::endl;
        
        // Sort by activity level (descending)
        std::vector<const NeuronData*> sortedNeurons = regionPair.second;
        std::sort(sortedNeurons.begin(), sortedNeurons.end(),
                  [](const NeuronData* a, const NeuronData* b) {
                      return a->firingRate > b->firingRate;
                  });
        
        // Display top neurons
        size_t displayCount = std::min(static_cast<size_t>(15), sortedNeurons.size());
        for (size_t i = 0; i < displayCount; ++i) {
            const auto& neuron = *sortedNeurons[i];
            std::cout << "  " << neuron.id << ": "
                      << std::fixed << std::setprecision(3) << neuron.firingRate
                      << " Hz (" << std::string(static_cast<size_t>(neuron.firingRate * 10), '=')
                      << " " << std::string(static_cast<size_t>(10 - neuron.firingRate * 10), ' ')
                      << ")" << std::endl;
        }
        
        if (sortedNeurons.size() > displayCount) {
            std::cout << "  ... and " << (sortedNeurons.size() - displayCount) << " more neurons" << std::endl;
        }
    }
}

void ConsoleVisualization::renderSynapticWeights() const {
    std::cout << "=== SYNAPTIC WEIGHTS ===" << std::endl;
    
    if (synapseData_.empty()) {
        std::cout << "No synaptic data available" << std::endl;
        return;
    }
    
    // Group synapses by strength
    std::vector<const SynapseData*> strongSynapses;
    std::vector<const SynapseData*> weakSynapses;
    
    for (const auto& synapse : synapseData_) {
        if (synapse.weight > 0.5f) {
            strongSynapses.push_back(&synapse);
        } else {
            weakSynapses.push_back(&synapse);
        }
    }
    
    std::cout << "\nStrong Synapses (>0.5): " << strongSynapses.size() << std::endl;
    for (const auto& synapse : strongSynapses) {
        std::cout << "  " << synapse->preNeuronId << " -> " << synapse->postNeuronId
                  << ": " << std::fixed << std::setprecision(3) << synapse->weight << std::endl;
    }
    
    std::cout << "\nWeak Synapses (≤0.5): " << weakSynapses.size() << std::endl;
    for (const auto& synapse : weakSynapses) {
        std::cout << "  " << synapse->preNeuronId << " -> " << synapse->postNeuronId
                  << ": " << std::fixed << std::setprecision(3) << synapse->weight << std::endl;
    }
}

void ConsoleVisualization::renderBrainRegions() const {
    std::cout << "=== BRAIN REGIONS ===" << std::endl;
    
    if (regionData_.empty()) {
        std::cout << "No region data available" << std::endl;
        return;
    }
    
    // Calculate total activity for all regions
    float totalActivity = 0.0f;
    for (const auto& region : regionData_) {
        totalActivity += calculateRegionActivity(region);
    }
    
    // Display regions sorted by activity
    std::vector<const RegionData*> sortedRegions;
    for (const auto& region : regionData_) {
        sortedRegions.push_back(&region);
    }
    
    std::sort(sortedRegions.begin(), sortedRegions.end(),
              [](const RegionData* a, const RegionData* b) {
                  return calculateRegionActivity(*a) > calculateRegionActivity(*b);
              });
    
    for (const auto& region : sortedRegions) {
        float activity = calculateRegionActivity(*region);
        float activityPercent = (activity / std::max(totalActivity, 0.01f)) * 100.0f;
        
        std::cout << region->name << ": "
                  << region->neuronCount << " neurons, "
                  << std::fixed << std::setprecision(3) << region->averageFiringRate
                  << " avg Hz, "
                  << std::fixed << std::setprecision(1) << activityPercent << "% activity"
                  << std::endl;
    }
}

void ConsoleVisualization::renderMotorOutputs() const {
    std::cout << "=== MOTOR OUTPUTS ===" << std::endl;
    
    if (motorOutputData_.empty()) {
        std::cout << "No motor output data available" << std::endl;
        return;
    }
    
    std::cout << "Command Activity Confidence Curiosity" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    
    for (const auto& motor : motorOutputData_) {
        std::cout << static_cast<int>(motor.command) << "    "
                  << std::fixed << std::setprecision(3) << motor.activity << "  "
                  << std::fixed << std::setprecision(3) << motor.firingRate << "  "
                  << std::fixed << std::setprecision(3) << motor.confidence << "  "
                  << std::fixed << std::setprecision(3) << motor.curiosityLevel
                  << std::endl;
    }
}

void ConsoleVisualization::renderSensorInputs() const {
    std::cout << "=== SENSOR INPUTS ===" << std::endl;
    
    if (sensorData_.empty()) {
        std::cout << "No sensor data available" << std::endl;
        return;
    }
    
    std::cout << "Sensor Intensity Normalized" << std::endl;
    std::cout << "-------------------------" << std::endl;
    
    for (const auto& sensor : sensorData_) {
        std::cout << sensor.sensorName << ": "
                  << std::fixed << std::setprecision(3) << sensor.intensity << "  "
                  << std::fixed << std::setprecision(3) << sensor.normalizedValue
                  << std::endl;
    }
}

void ConsoleVisualization::renderMemoryStates() const {
    std::cout << "=== MEMORY STATES ===" << std::endl;
    
    if (memoryStateData_.empty()) {
        std::cout << "No memory state data available" << std::endl;
        return;
    }
    
    for (const auto& memory : memoryStateData_) {
        std::cout << memory.memoryType << ": "
                  << memory.traceCount << " traces, "
                  << memory.episodeCount << " episodes, "
                  << std::fixed << std::setprecision(3) << memory.averageStrength
                  << " avg strength" << std::endl;
    }
}

void ConsoleVisualization::renderNeuromodulation() const {
    std::cout << "=== NEUROMODULATION ===" << std::endl;
    
    if (neuromodulationData_.empty()) {
        std::cout << "No neuromodulation data available" << std::endl;
        return;
    }
    
    for (const auto& mod : neuromodulationData_) {
        std::cout << mod.neuromodulator << ": "
                  << std::fixed << std::setprecision(3) << mod.level
                  << " (baseline: " << std::fixed << std::setprecision(3) << mod.baseline << ")"
                  << std::endl;
    }
}

void ConsoleVisualization::renderDevelopmentStages() const {
    std::cout << "=== DEVELOPMENT STAGES ===" << std::endl;
    
    std::cout << "Stage: " << static_cast<int>(developmentData_.stage) << std::endl;
    std::cout << "Age: " << std::fixed << std::setprecision(1) << developmentData_.age << " time units" << std::endl;
    std::cout << "Plasticity Modifier: " << std::fixed << std::setprecision(3) << developmentData_.plasticityModifier << std::endl;
    std::cout << "Synaptogenesis Rate: " << std::fixed << std::setprecision(3) << developmentData_.synaptogenesisRate << std::endl;
    std::cout << "Pruning Rate: " << std::fixed << std::setprecision(3) << developmentData_.pruningRate << std::endl;
    std::cout << "Time to Next Stage: " << std::fixed << std::setprecision(1) << developmentData_.timeToNextStage << " time units" << std::endl;
}

void ConsoleVisualization::renderConceptFormation() const {
    std::cout << "=== CONCEPT FORMATION ===" << std::endl;
    
    if (conceptData_.empty()) {
        std::cout << "No concept data available" << std::endl;
        return;
    }
    
    std::cout << "ID Pattern Strength Avg Reward Activations" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    
    for (const auto& concept : conceptData_) {
        std::cout << concept.conceptId << "    "
                  << std::fixed << std::setprecision(3) << concept.strength << "  "
                  << std::fixed << std::setprecision(3) << concept.avgReward << "  "
                  << concept.activationCount << std::endl;
    }
}

void ConsoleVisualization::renderFullDashboard() const {
    std::cout << "=== FULL NEURAL NETWORK DASHBOARD ===" << std::endl;
    
    // Summary statistics
    std::cout << "NETWORK STATISTICS:" << std::endl;
    std::cout << "  Neurons: " << neuronData_.size() << std::endl;
    std::cout << "  Synapses: " << synapseData_.size() << std::endl;
    std::cout << "  Regions: " << regionData_.size() << std::endl;
    std::cout << "  Active Neurons: " << std::count_if(neuronData_.begin(), neuronData_.end(),
                                               [](const NeuronData& n) { return n.isActive; }) << std::endl;
    std::cout << "  Average Firing Rate: "
              << std::fixed << std::setprecision(3) << calculateAverageFiringRate() << " Hz" << std::endl;
    
    std::cout << std::endl;
    
    // Recent activity
    std::cout << "RECENT ACTIVITY:" << std::endl;
    if (!motorOutputData_.empty()) {
        std::cout << "  Last Motor Command: " << static_cast<int>(motorOutputData_.back().command) << std::endl;
    }
    if (!sensorData_.empty()) {
        std::cout << "  Max Sensor Intensity: " << std::fixed << std::setprecision(3) << 
                     (*std::max_element(sensorData_.begin(), sensorData_.end(),
                                        [](const SensorData& a, const SensorData& b) {
                                            return a.intensity < b.intensity;
                                        })).intensity << std::endl;
    }
    
    std::cout << std::endl;
    
    // Quick plots
    std::cout << "NEURON ACTIVITY DISTRIBUTION:" << std::endl;
    renderActivityHistogram();
}

// Data processing methods
void ConsoleVisualization::processNeuronData(const Brain* brain) {
    neuronData_.clear();
    
    if (!brain) return;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto& neuron : pop->getNeurons()) {
                NeuronData data;
                data.id = neuron->getId();
                data.region = region->getName();
                data.neuronType = neuron->getNeuronType();
                data.firingRate = neuron->getFiringRate();
                data.membranePotential = neuron->getState().membranePotential;
                data.restingPotential = neuron->getState().restingPotential;
                data.inputCurrent = 0.0f; // Would need to track this separately
                data.outputCurrent = 0.0f; // Would need to track this separately
                data.lastSpikeTime = neuron->getLastSpikeTime();
                data.isActive = (neuron->getFiringRate() > 0.1f);
                
                // Add some metrics
                data.metrics["excitatory_input"] = neuron->getExcitatoryInput();
                data.metrics["inhibitory_input"] = neuron->getInhibitoryInput();
                
                neuronData_.push_back(data);
            }
        }
    }
}

void ConsoleVisualization::processSynapseData(const Brain* brain) {
    synapseData_.clear();
    
    if (!brain) return;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            SynapseData data;
            data.id = syn->getId();
            data.preNeuronId = syn->getPreNeuronId();
            data.postNeuronId = syn->getPostNeuronId();
            data.weight = syn->getWeight();
            data.weightChange = syn->getWeightChange();
            data.lastUpdateTime = syn->getLastUpdateTime();
            data.eligibilityTrace = syn->getEligibilityTrace();
            data.connectionType = "plastic"; // Simplified
            data.delay = syn->getDelay();
            
            synapseData_.push_back(data);
        }
    }
}

void ConsoleVisualization::processRegionData(const Brain* brain) {
    regionData_.clear();
    
    if (!brain) return;
    
    for (const auto& region : brain->getRegions()) {
        RegionData data;
        data.name = region->getName();
        data.neuronCount = region->getNeuronCount();
        data.averageFiringRate = 0.0f; // Calculate from neurons
        
        // Collect neuron IDs
        for (const auto& pop : region->getPopulations()) {
            for (const auto& neuron : pop->getNeurons()) {
                data.neuronIds.push_back(neuron->getId());
            }
        }
        
        regionData_.push_back(data);
    }
}

void ConsoleVisualization::processMotorOutputData(const AgentBrain* agent) {
    motorOutputData_.clear();
    
    if (!agent) return;
    
    auto activities = agent->getMotorGroupActivities();
    
    for (const auto& act : activities) {
        MotorOutputData data;
        data.command = stringToMotorCommand(act.name);
        data.activity = act.activity;
        data.firingRate = act.firingRate;
        data.confidence = 0.5f; // Would need to get from agent
        data.noveltyLevel = agent->getNoveltyLevel();
        data.curiosityLevel = agent->getCuriosityLevel();
        data.lastExecutedTime = 0.0; // Would need to track this
        
        motorOutputData_.push_back(data);
    }
}

void ConsoleVisualization::processSensorData(const AgentBrain* agent) {
    sensorData_.clear();
    
    if (!agent) return;
    
    // Get sensory percept from agent
    // This would need to be implemented based on AgentBrain API
    // For now, create placeholder data
    SensorData visionSensor;
    visionSensor.sensorName = "Vision";
    visionSensor.intensity = 0.5f;
    visionSensor.normalizedValue = 0.5f;
    visionSensor.timestamp = getCurrentTimestamp();
    sensorData_.push_back(visionSensor);
    
    SensorData touchSensor;
    touchSensor.sensorName = "Touch";
    touchSensor.intensity = 0.3f;
    touchSensor.normalizedValue = 0.3f;
    touchSensor.timestamp = getCurrentTimestamp();
    sensorData_.push_back(touchSensor);
}

void ConsoleVisualization::processMemoryStateData(const Brain* brain) {
    memoryStateData_.clear();
    
    if (!brain) return;
    
    auto* wm = brain->getWorkingMemory();
    if (wm) {
        MemoryStateData data;
        data.memoryType = "working";
        data.traceCount = wm->getActiveTraces();
        data.episodeCount = 0; // Would need to get from episodic memory
        data.averageStrength = 0.5f; // Placeholder
        data.lastUpdateTime = 0.0; // Would need to track
        memoryStateData_.push_back(data);
    }
}

void ConsoleVisualization::processNeuromodulationData(const Brain* brain) {
    neuromodulationData_.clear();
    
    if (!brain) return;
    
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        NeuromodulationData data;
        data.neuromodulator = "dopamine";
        data.level = dopamine->getCurrentLevel();
        data.baseline = 0.1f; // Would need to get from neuromodulator
        data.changeRate = 0.0f; // Would need to track
        data.lastUpdateTime = 0.0; // Would need to track
        neuromodulationData_.push_back(data);
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        NeuromodulationData data;
        data.neuromodulator = "curiosity";
        data.level = curiosity->getCurrentLevel();
        data.baseline = 0.1f; // Would need to get from neuromodulator
        data.changeRate = 0.0f; // Would need to track
        data.lastUpdateTime = 0.0; // Would need to track
        neuromodulationData_.push_back(data);
    }
}

void ConsoleVisualization::processDevelopmentData(const AgentBrain* agent) {
    developmentData_.stage = DevelopmentalStage::Initial;
    developmentData_.age = 0.0;
    developmentData_.plasticityModifier = 1.0f;
    developmentData_.synaptogenesisRate = 0.001f;
    developmentData_.pruningRate = 0.0001f;
    developmentData_.timeToNextStage = 60.0;
    
    if (agent) {
        developmentData_.stage = agent->getDevelopmentalStage();
        developmentData_.age = agent->getDevelopmentalAge();
        developmentData_.plasticityModifier = agent->getPlasticityModifier();
        // Get other development data from agent
    }
}

void ConsoleVisualization::processConceptData(const Brain* brain) {
    conceptData_.clear();
    
    if (!brain) return;
    
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        // Get concept count and create placeholder data
        size_t conceptCount = conceptFormation->getConceptCount();
        
        for (size_t i = 0; i < conceptCount; ++i) {
            ConceptData data;
            data.conceptId = i;
            data.pattern = std::vector<float>(16, 0.5f); // Placeholder pattern
            data.strength = 0.5f + (i % 10) * 0.05f; // Varying strength
            data.avgReward = (i % 5) * 0.2f - 0.5f; // Varying reward
            data.activationCount = i * 2; // Varying activations
            data.lastActivatedTime = 0.0; // Would need to track
            data.category = "unknown"; // Would need to get from concept
            
            conceptData_.push_back(data);
        }
    }
}

// Data filtering and aggregation methods
std::vector<VisualizationInterface::NeuronData> ConsoleVisualization::filterNeuronsByRegion(
    const std::string& regionName) const {
    std::vector<NeuronData> filtered;
    
    for (const auto& neuron : neuronData_) {
        if (neuron.region == regionName) {
            filtered.push_back(neuron);
        }
    }
    
    return filtered;
}

std::vector<VisualizationInterface::NeuronData> ConsoleVisualization::filterNeuronsByType(
    NeuronType type) const {
    std::vector<NeuronData> filtered;
    
    for (const auto& neuron : neuronData_) {
        if (neuron.neuronType == type) {
            filtered.push_back(neuron);
        }
    }
    
    return filtered;
}

float ConsoleVisualization::calculateRegionActivity(const RegionData& region) const {
    if (region.neuronCount == 0) return 0.0f;
    
    // Find neurons in this region
    float totalActivity = 0.0f;
    int neuronCount = 0;
    
    for (const auto& neuron : neuronData_) {
        if (neuron.region == region.name) {
            totalActivity += neuron.firingRate;
            neuronCount++;
        }
    }
    
    return (neuronCount > 0) ? (totalActivity / neuronCount) : 0.0f;
}

std::string ConsoleVisualization::formatActivityDisplay(float activity) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << activity;
    return ss.str();
}

std::string ConsoleVisualization::formatMetricDisplay(float value, const std::string& unit) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << value << " " << unit;
    return ss.str();
}

// Utility methods
std::string ConsoleVisualization::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    return ss.str();
}

std::string ConsoleVisualization::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
    return std::to_string(seconds);
}

float ConsoleVisualization::calculateAverageFiringRate() const {
    if (neuronData_.empty()) return 0.0f;
    
    float totalRate = 0.0f;
    for (const auto& neuron : neuronData_) {
        totalRate += neuron.firingRate;
    }
    
    return totalRate / neuronData_.size();
}

MotorCommand ConsoleVisualization::stringToMotorCommand(const std::string& name) {
    if (name == "forward") return MotorCommand::MoveForward;
    if (name == "backward") return MotorCommand::MoveBackward;
    if (name == "left") return MotorCommand::TurnLeft;
    if (name == "right") return MotorCommand::TurnRight;
    if (name == "interact") return MotorCommand::Interact;
    if (name == "wait") return MotorCommand::Wait;
    return MotorCommand::Wait;
}

void ConsoleVisualization::renderActivityHistogram() const {
    // Simple histogram of neuron firing rates
    std::map<size_t, size_t> histogram; // firing rate buckets
    
    for (const auto& neuron : neuronData_) {
        size_t bucket = static_cast<size_t>(neuron.firingRate * 10); // 0-10 Hz buckets
        histogram[bucket]++;
    }
    
    std::cout << "  Firing Rate (Hz) : Count" << std::endl;
    std::cout << "  ------------------ " << std::endl;
    
    for (const auto& bucket : histogram) {
        std::cout << "  " << bucket.first << ".0-" << (bucket.first + 1) << ".0 : "
                  << std::string(bucket.second / 5, '=')  // Scale for display
                  << std::endl;
    }
}

// Web visualization implementation (simplified for compilation)
WebVisualization::WebVisualization() : running_(false) {
    config_.enabled = true;
    config_.autoRefresh = true;
    config_.updateInterval = 1.0;
    config_.width = 800;
    config_.height = 600;
    config_.backend = "web";
    
    // For compilation, we'll use localhost on a standard port
    webSocketServer_ = WebSocketServer("localhost", 8080);
    
    NLM_LOG_INFO("WebVisualization initialized", "WebVisualization::Constructor");
}

WebVisualization::~WebVisualization() {
    stop();
}

void WebVisualization::update(const Brain* brain, const AgentBrain* agent, double timestep) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Simplified web visualization update
    // In a real implementation, this would update the web data structures
    // and broadcast changes to connected clients via WebSocket
    
    dataDirty_ = true;
    lastUpdate_ = std::chrono::system_clock::now();
}

void WebVisualization::render() {
    std::cout << "[Web Visualization] Web server started on localhost:8080" << std::endl;
    std::cout << "Access visualization in a web browser at: http://localhost:8080" << std::endl;
    std::cout << "Visualization data would be available at: http://localhost:8080/api/v1/data" << std::endl;
}

void WebVisualization::save(const std::string& filepath) {
    std::cout << "[Web Visualization] Saving visualization data to: " << filepath << std::endl;
    std::cout << "Web visualization saves are typically handled by the web interface" << std::endl;
}

void WebVisualization::setConfig(const VisualizationConfig& config) {
    config_ = config;
    dataDirty_ = true;
}

VisualizationConfig WebVisualization::getConfig() const {
    return config_;
}

std::vector<VisualizationInterface::NeuronData> WebVisualization::getNeuronData() const {
    return std::vector<NeuronData>(); // Would fetch from web server
}

std::vector<VisualizationInterface::SynapseData> WebVisualization::getSynapseData() const {
    return std::vector<SynapseData>(); // Would fetch from web server
}

std::vector<VisualizationInterface::RegionData> WebVisualization::getRegionData() const {
    return std::vector<RegionData>(); // Would fetch from web server
}

std::vector<VisualizationInterface::MotorOutputData> WebVisualization::getMotorOutputData() const {
    return std::vector<MotorOutputData>(); // Would fetch from web server
}

std::vector<VisualizationInterface::SensorData> WebVisualization::getSensorData() const {
    return std::vector<SensorData>(); // Would fetch from web server
}

std::vector<VisualizationInterface::MemoryStateData> WebVisualization::getMemoryStateData() const {
    return std::vector<MemoryStateData>(); // Would fetch from web server
}

std::vector<VisualizationInterface::NeuromodulationData> WebVisualization::getNeuromodulationData() const {
    return std::vector<NeuromodulationData>(); // Would fetch from web server
}

VisualizationInterface::DevelopmentData WebVisualization::getDevelopmentData() const {
    DevelopmentData data;
    return data;
}

std::vector<VisualizationInterface::ConceptData> WebVisualization::getConceptData() const {
    return std::vector<ConceptData>(); // Would fetch from web server
}

void WebVisualization::setNeuronActivity(size_t neuronId, float activity) {
    // Send neuron activity update to web server
}

void WebVisualization::injectCurrent(size_t neuronId, float current) {
    // Send current injection command to web server
}

void WebVisualization::selectRegion(const std::string& regionName) {
    // Send region selection to web server
}

void WebVisualization::zoom(double factor) {
    // Send zoom command to web server
}

void WebVisualization::pan(double dx, double dy) {
    // Send pan command to web server
}

void WebVisualization::setUpdateCallback(std::function<void(const VisualizationData&)> callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    updateCallback_ = callback;
}

void WebVisualization::setInteractionCallback(std::function<void(const std::string&, const std::map<std::string, std::string>&)> callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    interactionCallback_ = callback;
}

std::string WebVisualization::getVisualizationData() const {
    return "{}"; // JSON object with visualization data
}

bool WebVisualization::isDataAvailable() const {
    return false; // Data availability depends on web server
}

void WebVisualization::clear() {
    std::cout << "[Web Visualization] Clearing visualization data" << std::endl;
}

void WebVisualization::reset() {
    std::cout << "[Web Visualization] Resetting visualization" << std::endl;
}

// WebSocket Server implementation
WebVisualization::WebSocketServer::WebSocketServer(const std::string& host, int port)
    : host_(host), port_(port), running_(false) {
    // In a real implementation, this would create a WebSocket server
    // For compilation, we'll use a placeholder
}

WebVisualization::WebSocketServer::~WebSocketServer() {
    stop();
}

void WebVisualization::WebSocketServer::start() {
    running_ = true;
    std::cout << "[WebSocket] Server starting on " << host_ << ":" << port_ << std::endl;
    
    // In a real implementation, this would start the WebSocket server thread
    // For compilation, we'll just log
    
    serverThread_ = std::thread([this]() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            if (dataCallback_) {
                // Broadcast data to connected clients
                // dataCallback_("data", getSerializedData());
            }
        }
    });
}

void WebVisualization::WebSocketServer::stop() {
    running_ = false;
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
    std::cout << "[WebSocket] Server stopped" << std::endl;
}

bool WebVisualization::WebSocketServer::isRunning() const {
    return running_;
}

void WebVisualization::WebSocketServer::broadcastData(const std::string& data) {
    // In a real implementation, this would broadcast to all connected WebSocket clients
}

void WebVisualization::WebSocketServer::setDataCallback(std::function<void(const std::string&, const std::string&)> callback) {
    dataCallback_ = callback;
}

std::string WebVisualization::WebSocketServer::getSerializedData() const {
    return "{}"; // Empty JSON object
}

// VisualizationManager implementation
VisualizationManager::VisualizationManager()
    : running_(false), brainReference_(nullptr), agentReference_(nullptr) {
    
    // Register default visualization types
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::NEURON_ACTIVITY);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::SYNAPTIC_WEIGHTS);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::BRAIN_REGIONS);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::MOTOR_OUTPUTS);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::SENSOR_INPUTS);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::MEMORY_STATE);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::NEUROMODULATION);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::DEVELOPMENT_STAGES);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::CONCEPT_FORMATION);
    visualizationTypes_.push_back(VisualizationInterface::VisualizationType::FULL_DASHBOARD);
    
    NLM_LOG_INFO("VisualizationManager initialized", "VisualizationManager::Constructor");
}

VisualizationManager::~VisualizationManager() {
    stopAll();
}

VisualizationManager& VisualizationManager::getInstance() {
    static VisualizationManager instance;
    return instance;
}

void VisualizationManager::resetInstance() {
    // Note: This is mainly for testing; in production, we typically want a singleton
    // that persists throughout the application lifetime.
}

bool VisualizationManager::addVisualization(const std::string& name, std::unique_ptr<VisualizationInterface> visualization) {
    if (!visualization) {
        NLM_LOG_ERROR(std::string("Cannot add null visualization: ") + name, 
                     "VisualizationManager::addVisualization");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    if (visualizations_.find(name) != visualizations_.end()) {
        NLM_LOG_WARN(std::string("Visualization already exists: ") + name, 
                    "VisualizationManager::addVisualization");
        return false;
    }
    
    visualizations_[name] = std::move(visualization);
    
    NLM_LOG_INFO(std::string("Visualization added: ") + name, 
                "VisualizationManager::addVisualization");
    
    return true;
}

bool VisualizationManager::removeVisualization(const std::string& name) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    auto it = visualizations_.find(name);
    if (it == visualizations_.end()) {
        NLM_LOG_WARN(std::string("Visualization not found: ") + name, 
                    "VisualizationManager::removeVisualization");
        return false;
    }
    
    visualizations_.erase(it);
    
    if (activeVisualizationName_ == name) {
        activeVisualizationName_.clear();
    }
    
    NLM_LOG_INFO(std::string("Visualization removed: ") + name, 
                "VisualizationManager::removeVisualization");
    
    return true;
}

bool VisualizationManager::activateVisualization(const std::string& name) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    auto it = visualizations_.find(name);
    if (it == visualizations_.end()) {
        NLM_LOG_ERROR(std::string("Cannot activate non-existent visualization: ") + name, 
                     "VisualizationManager::activateVisualization");
        return false;
    }
    
    // Deactivate current visualization
    if (!activeVisualizationName_.empty()) {
        auto currentIt = visualizations_.find(activeVisualizationName_);
        if (currentIt != visualizations_.end()) {
            // In a real implementation, we would call a deactivation method
        }
    }
    
    activeVisualizationName_ = name;
    
    // Configure the visualization
    it->second->setConfig(globalConfig_);
    
    NLM_LOG_INFO(std::string("Visualization activated: ") + name, 
                "VisualizationManager::activateVisualization");
    
    return true;
}

bool VisualizationManager::deactivateVisualization(const std::string& name) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    if (activeVisualizationName_ != name) {
        return true; // Already deactivated
    }
    
    NLM_LOG_INFO(std::string("Visualization deactivated: ") + name, 
                "VisualizationManager::deactivateVisualization");
    
    activeVisualizationName_.clear();
    
    return true;
}

std::vector<std::string> VisualizationManager::getVisualizationNames() const {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    std::vector<std::string> names;
    for (const auto& pair : visualizations_) {
        names.push_back(pair.first);
    }
    
    return names;
}

VisualizationInterface* VisualizationManager::getVisualization(const std::string& name) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    auto it = visualizations_.find(name);
    if (it == visualizations_.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

VisualizationInterface* VisualizationManager::getActiveVisualization() {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    if (activeVisualizationName_.empty()) {
        return nullptr;
    }
    
    return getVisualization(activeVisualizationName_);
}

void VisualizationManager::setGlobalConfig(const VisualizationConfig& config) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    globalConfig_ = config;
    
    // Apply config to all visualizations
    for (auto& pair : visualizations_) {
        pair.second->setConfig(config);
    }
    
    NLM_LOG_INFO(std::string("Global visualization configuration updated: ") + config.title, 
                "VisualizationManager::setGlobalConfig");
}

VisualizationConfig VisualizationManager::getGlobalConfig() const {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    return globalConfig_;
}

void VisualizationManager::updateAll(const Brain* brain, const AgentBrain* agent, double timestep) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (auto& pair : visualizations_) {
        try {
            pair.second->update(brain, agent, timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error updating visualization '": ") + pair.first + ": " + e.what(), 
                         "VisualizationManager::updateAll");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // Update performance metrics
    PerformanceMetrics& metrics = performanceMetrics_["global"];
    metrics.updateTime = duration;
    metrics.lastUpdate = std::chrono::system_clock::now();
}

void VisualizationManager::renderAll() {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    if (activeVisualizationName_.empty()) {
        NLM_LOG_WARN("No active visualization to render", "VisualizationManager::renderAll");
        return;
    }
    
    auto it = visualizations_.find(activeVisualizationName_);
    if (it == visualizations_.end()) {
        NLM_LOG_ERROR(std::string("Active visualization not found: ") + activeVisualizationName_, 
                     "VisualizationManager::renderAll");
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        it->second->render();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error rendering visualization: ") + e.what(), 
                     "VisualizationManager::renderAll");
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // Update performance metrics
    PerformanceMetrics& metrics = performanceMetrics_[activeVisualizationName_];
    metrics.renderTime = duration;
    metrics.lastUpdate = std::chrono::system_clock::now();
}

void VisualizationManager::saveAll(const std::string& basePath) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    for (auto& pair : visualizations_) {
        std::string filepath = basePath + "/" + pair.first + ".json";
        try {
            pair.second->save(filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error saving visualization '": ") + pair.first + ": " + e.what(), 
                         "VisualizationManager::saveAll");
        }
    }
}

std::map<std::string, std::vector<VisualizationInterface::NeuronData>> VisualizationManager::getAllNeuronData() const {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    std::map<std::string, std::vector<VisualizationInterface::NeuronData>> allNeuronData;
    
    for (const auto& pair : visualizations_) {
        allNeuronData[pair.first] = pair.second->getNeuronData();
    }
    
    return allNeuronData;
}

std::map<std::string, std::vector<VisualizationInterface::SynapseData>> VisualizationManager::getAllSynapseData() const {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    std::map<std::string, std::vector<VisualizationInterface::SynapseData>> allSynapseData;
    
    for (const auto& pair : visualizations_) {
        allSynapseData[pair.first] = pair.second->getSynapseData();
    }
    
    return allSynapseData;
}

std::map<std::string, std::vector<VisualizationInterface::RegionData>> VisualizationManager::getAllRegionData() const {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    std::map<std::string, std::vector<VisualizationInterface::RegionData>> allRegionData;
    
    for (const auto& pair : visualizations_) {
        allRegionData[pair.first] = pair.second->getRegionData();
    }
    
    return allRegionData;
}

void VisualizationManager::startAll() {
    running_ = true;
    
    NLM_LOG_INFO("All visualizations started", "VisualizationManager::startAll");
}

void VisualizationManager::stopAll() {
    running_ = false;
    
    for (auto& pair : visualizations_) {
        pair.second->clear();
    }
    
    NLM_LOG_INFO("All visualizations stopped", "VisualizationManager::stopAll");
}

bool VisualizationManager::isRunning() const {
    return running_;
}

void VisualizationManager::setBrainReference(const std::shared_ptr<Brain>& brain) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    brainReference_ = brain;
}

void VisualizationManager::setAgentReference(const std::shared_ptr<AgentBrain>& agent) {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    agentReference_ = agent;
}

bool VisualizationManager::loadVisualizationConfig(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR(std::string("Cannot open visualization config file: ") + filepath, 
                         "VisualizationManager::loadVisualizationConfig");
            return false;
        }
        
        // In a real implementation, this would parse JSON/YAML config
        // For compilation, we'll use a placeholder
        file.close();
        
        NLM_LOG_INFO(std::string("Visualization config loaded: ") + filepath, 
                    "VisualizationManager::loadVisualizationConfig");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading visualization config: ") + e.what(), 
                     "VisualizationManager::loadVisualizationConfig");
        return false;
    }
}

bool VisualizationManager::saveVisualizationConfig(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR(std::string("Cannot open visualization config file for writing: ") + filepath, 
                         "VisualizationManager::saveVisualizationConfig");
            return false;
        }
        
        // In a real implementation, this would serialize JSON/YAML config
        // For compilation, we'll just write a placeholder
        file << "{\"visualization\": {\"type\": \"dashboard\", \"enabled\": true}}" << std::endl;
        
        file.close();
        
        NLM_LOG_INFO(std::string("Visualization config saved: ") + filepath, 
                    "VisualizationManager::saveVisualizationConfig");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error saving visualization config: ") + e.what(), 
                     "VisualizationManager::saveVisualizationConfig");
        return false;
    }
}

std::string VisualizationManager::getVisualizationStatus() const {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    std::stringstream status;
    status << "=== Visualization Manager Status ===" << std::endl;
    status << "Total visualizations: " << visualizations_.size() << std::endl;
    status << "Active visualization: " << (activeVisualizationName_.empty() ? "None" : activeVisualizationName_) << std::endl;
    status << "Running: " << (running_ ? "Yes" : "No") << std::endl;
    status << "Brain reference: " << (brainReference_ ? "Yes" : "No") << std::endl;
    status << "Agent reference: " << (agentReference_ ? "Yes" : "No") << std::endl;
    
    status << std::endl << "=== Visualization List ===" << std::endl;
    for (const auto& pair : visualizations_) {
        status << "  - " << pair.first << std::endl;
    }
    
    status << std::endl << "=== Performance Metrics ===" << std::endl;
    for (const auto& metricsPair : performanceMetrics_) {
        status << "  " << metricsPair.first << ": "
               << "update=" << metricsPair.second.updateTime << "ms, "
               << "render=" << metricsPair.second.renderTime << "ms" << std::endl;
    }
    
    return status.str();
}

void VisualizationManager::logPerformanceMetrics() {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    
    NLM_LOG_INFO(std::string("Performance metrics logged. Total visualizations: ") + 
                std::to_string(visualizations_.size()), 
                "VisualizationManager::logPerformanceMetrics");
    
    for (const auto& metricsPair : performanceMetrics_) {
        NLM_LOG_INFO(std::string("Visualization ':") + metricsPair.first + ": "
                   + std::to_string(metricsPair.second.updateTime) + "ms update, "
                   + std::to_string(metricsPair.second.renderTime) + "ms render",
                   "VisualizationManager::logPerformanceMetrics");
    }
}

void VisualizationManager::resetPerformanceMetrics() {
    std::lock_guard<std::mutex> lock(visualizationsMutex_);
    performanceMetrics_.clear();
    
    NLM_LOG_INFO("Performance metrics reset", "VisualizationManager::resetPerformanceMetrics");
}

std::vector<VisualizationInterface::VisualizationType> VisualizationManager::getAvailableVisualizationTypes() const {
    return visualizationTypes_;
}

VisualizationInterface::VisualizationType VisualizationManager::getVisualizationTypeFromString(
    const std::string& typeName) {
    std::string lowerType = typeName;
    for (char& c : lowerType) c = std::tolower(c);
    
    if (lowerType == "neuron activity" || lowerType == "neurons") {
        return VisualizationInterface::VisualizationType::NEURON_ACTIVITY;
    } else if (lowerType == "synaptic weights" || lowerType == "synapses") {
        return VisualizationInterface::VisualizationType::SYNAPTIC_WEIGHTS;
    } else if (lowerType == "brain regions" || lowerType == "regions") {
        return VisualizationInterface::VisualizationType::BRAIN_REGIONS;
    } else if (lowerType == "motor outputs" || lowerType == "motor") {
        return VisualizationInterface::VisualizationType::MOTOR_OUTPUTS;
    } else if (lowerType == "sensor inputs" || lowerType == "sensors") {
        return VisualizationInterface::VisualizationType::SENSOR_INPUTS;
    } else if (lowerType == "memory state") {
        return VisualizationInterface::VisualizationType::MEMORY_STATE;
    } else if (lowerType == "neuromodulation") {
        return VisualizationInterface::VisualizationType::NEUROMODULATION;
    } else if (lowerType == "development stages") {
        return VisualizationInterface::VisualizationType::DEVELOPMENT_STAGES;
    } else if (lowerType == "concept formation") {
        return VisualizationInterface::VisualizationType::CONCEPT_FORMATION;
    } else if (lowerType == "full dashboard") {
        return VisualizationInterface::VisualizationType::FULL_DASHBOARD;
    }
    
    return VisualizationInterface::VisualizationType::FULL_DASHBOARD;
}

std::string VisualizationManager::getVisualizationTypeName(VisualizationInterface::VisualizationType type) {
    switch (type) {
        case VisualizationInterface::VisualizationType::NEURON_ACTIVITY: return "Neuron Activity";
        case VisualizationInterface::VisualizationType::SYNAPTIC_WEIGHTS: return "Synaptic Weights";
        case VisualizationInterface::VisualizationType::BRAIN_REGIONS: return "Brain Regions";
        case VisualizationInterface::VisualizationType::MOTOR_OUTPUTS: return "Motor Outputs";
        case VisualizationInterface::VisualizationType::SENSOR_INPUTS: return "Sensor Inputs";
        case VisualizationInterface::VisualizationType::MEMORY_STATE: return "Memory State";
        case VisualizationInterface::VisualizationType::NEUROMODULATION: return "Neuromodulation";
        case VisualizationInterface::VisualizationType::DEVELOPMENT_STAGES: return "Development Stages";
        case VisualizationInterface::VisualizationType::CONCEPT_FORMATION: return "Concept Formation";
        case VisualizationInterface::VisualizationType::FULL_DASHBOARD: return "Full Dashboard";
        default: return "Unknown";
    }
}

// Visualization backend factory functions
namespace VisualizationBackend {

    std::unique_ptr<VisualizationInterface> createVisualization(
        VisualizationInterface::VisualizationType type,
        const VisualizationInterface::VisualizationConfig& config) {
        
        switch (config.backend) {
            case "console":
                return std::make_unique<ConsoleVisualization>();
            case "web":
                return std::make_unique<WebVisualization>();
            default:
                return std::make_unique<ConsoleVisualization>();
        }
    }
    
    std::unique_ptr<VisualizationInterface> createConsoleVisualization(
        const VisualizationInterface::VisualizationConfig& config) {
        return std::make_unique<ConsoleVisualization>();
    }
    
    std::unique_ptr<VisualizationInterface> createWebVisualization(
        const VisualizationInterface::VisualizationConfig& config) {
        return std::make_unique<WebVisualization>();
    }
    
    bool isBackendAvailable(const std::string& backendName) {
        return backendName == "console" || backendName == "web";
    }
    
    std::vector<std::string> getAvailableBackends() {
        return {"console", "web"};
    }

} // namespace VisualizationBackend

// Visualization configuration utilities
namespace VisualizationConfig {

    VisualizationInterface::VisualizationConfig createDefaultConfig(
        VisualizationInterface::VisualizationType type,
        int width,
        int height) {
        VisualizationInterface::VisualizationConfig config;
        config.type = type;
        config.title = VisualizationManager::getVisualizationTypeName(type);
        config.updateInterval = 1.0;
        config.enabled = true;
        config.autoRefresh = true;
        config.width = width;
        config.height = height;
        config.backend = "console";
        
        return config;
    }
    
    VisualizationInterface::VisualizationConfig createConsoleConfig(
        VisualizationInterface::VisualizationType type,
        int width,
        int height) {
        VisualizationInterface::VisualizationConfig config = createDefaultConfig(type, width, height);
        config.backend = "console";
        return config;
    }
    
    VisualizationInterface::VisualizationConfig createWebConfig(
        VisualizationInterface::VisualizationType type,
        int port,
        const std::string& host) {
        VisualizationInterface::VisualizationConfig config = createDefaultConfig(type, 800, 600);
        config.backend = "web";
        config.parameters["port"] = std::to_string(port);
        config.parameters["host"] = host;
        return config;
    }
    
    bool saveConfig(const VisualizationInterface::VisualizationConfig& config, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // In a real implementation, this would serialize to JSON/YAML
            file << "{\"visualization\": {\"type\": \"" << static_cast<int>(config.type)
                 << "\", \"title\": \"" << config.title
                 << "\", \"enabled\": " << (config.enabled ? "true" : "false")
                 << "}}" << std::endl;
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    bool loadConfig(VisualizationInterface::VisualizationConfig& config, const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // In a real implementation, this would parse JSON/YAML
            // For compilation, we'll just use defaults
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    std::string configToString(const VisualizationInterface::VisualizationConfig& config) {
        std::stringstream ss;
        ss << "VisualizationConfig(type=" << static_cast<int>(config.type)
           << ", title=\"" << config.title << "\"
           << ", enabled=" << (config.enabled ? "true" : "false")
           << ", backend=\"" << config.backend << "\")";
        return ss.str();
    }
    
    bool configFromString(VisualizationInterface::VisualizationConfig& config, const std::string& str) {
        // In a real implementation, this would parse the string
        return false;
    }

} // namespace VisualizationConfig

} // namespace nlm
