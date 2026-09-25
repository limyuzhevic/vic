// Enhanced Visualization Interface Implementation
// 
// This file provides the implementation for the EnhancedVisualizationInterface
// with comprehensive visualization capabilities for the NLM brain simulator.

#include "visualization/EnhancedVisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

EnhancedVisualizationInterface::EnhancedVisualizationInterface() {
    // Initialize with default configuration
    active_ = false;
    currentTime_ = 0.0;
    currentFrame_ = 0;
    
    // Set up backends
    textBackend_ = std::make_unique<TextBackend>();
    svgBackend_ = std::make_unique<SVGBackend>();
    jsonBackend_ = std::make_unique<JSONBackend>();
    htmlBackend_ = std::make_unique<HTMLBackend>();
    
    // Initialize configuration
    VisualizationConfig defaultConfig;
    configure(defaultConfig);
    
    NLM_LOG_INFO("Enhanced visualization interface initialized");
}

EnhancedVisualizationInterface::~EnhancedVisualizationInterface() {
    cleanupBackends();
}

void EnhancedVisualizationInterface::configure(const VisualizationConfig& config) {
    config_ = config;
    
    // Configure backends based on configuration
    if (config_.enabled) {
        // Initialize data collections based on enabled visualizations
        if (config_.visualizeNetwork) {
            networkData_.regions.clear();
            networkData_.adjacencyMatrix.clear();
        }
        
        if (config_.visualizeActivity) {
            activityData_.firingRates.clear();
            activityData_.membranePotentials.clear();
            activityData_.timeSeriesData.clear();
        }
        
        if (config_.visualizeSpikes) {
            spikeData_.spikeEvents.clear();
            spikeData_.spikeRasters.clear();
        }
        
        if (config_.visualizeWeights) {
            weightData_.weightMatrix.clear();
            weightData_.connectionTypes.clear();
        }
        
        if (config_.visualizeMemory) {
            memoryData_.workingMemoryTraces.clear();
            memoryData_.episodicMemoryStrengths.clear();
            memoryData_.conceptRepresentations.clear();
        }
        
        if (config_.visualizeNeuromod) {
            neuromodData_.dopamineLevels.clear();
            neuromodData_.curiosityLevels.clear();
            neuromodData_.predictionErrorLevels.clear();
        }
        
        if (config_.visualizeDevelopment) {
            developmentData_.plasticityRates.clear();
            developmentData_.developmentalStages.clear();
        }
    }
    
    NLM_LOG_INFO("Enhanced visualization interface configured");
}

VisualizationConfig& EnhancedVisualizationInterface::getConfig() {
    return config_;
}

const VisualizationConfig& EnhancedVisualizationInterface::getConfig() const {
    return config_;
}

bool EnhancedVisualizationInterface::initialize() {
    if (config_.enabled) {
        // Initialize based on backend type
        if (config_.backend == "text") {
            textBackend_->initialize();
        } else if (config_.backend == "svg") {
            svgBackend_->initialize();
        } else if (config_.backend == "json") {
            jsonBackend_->initialize();
        } else if (config_.backend == "html") {
            htmlBackend_->initialize();
        }
        
        active_ = true;
        NLM_LOG_INFO("Enhanced visualization interface initialized successfully");
        return true;
    }
    
    NLM_LOG_WARN("Enhanced visualization interface disabled (config.enabled = false)");
    return false;
}

void EnhancedVisualizationInterface::update() {
    if (!active_) return;
    
    // Update time
    currentTime_ += 1.0 / config_.updateRate;
    
    // Collect and process data
    processData();
    
    // Render current frame if in real-time mode
    if (config_.realTime) {
        render();
    }
}

void EnhancedVisualizationInterface::render() {
    if (!active_) return;
    
    // Clear previous renders
    clearPreviousRenders();
    
    // Render based on backend type
    if (config_.backend == "text") {
        renderToTextBackend();
    } else if (config_.backend == "svg") {
        renderToSVGBackend();
    } else if (config_.backend == "json") {
        renderToJSONBackend();
    } else if (config_.backend == "html") {
        renderToHTMLBackend();
    }
}

void EnhancedVisualizationInterface::close() {
    if (active_) {
        if (config_.backend == "text") {
            textBackend_->close();
        } else if (config_.backend == "svg") {
            svgBackend_->close();
        } else if (config_.backend == "json") {
            jsonBackend_->close();
        } else if (config_.backend == "html") {
            htmlBackend_->close();
        }
        
        active_ = false;
        NLM_LOG_INFO("Enhanced visualization interface closed");
    }
}

void EnhancedVisualizationInterface::setActive(bool active) {
    if (active == active_) return;
    
    if (active) {
        if (initialize()) {
            active_ = true;
        }
    } else {
        close();
    }
}

bool EnhancedVisualizationInterface::isActive() const {
    return active_;
}

void EnhancedVisualizationInterface::setUpdateRate(double hz) {
    if (hz <= 0.0) {
        NLM_LOG_WARN("Update rate must be positive");
        return;
    }
    
    config_.updateRate = hz;
    
    // Update backends
    if (config_.backend == "text") {
        textBackend_->setUpdateRate(hz);
    } else if (config_.backend == "svg") {
        svgBackend_->setUpdateRate(hz);
    } else if (config_.backend == "json") {
        jsonBackend_->setUpdateRate(hz);
    } else if (config_.backend == "html") {
        htmlBackend_->setUpdateRate(hz);
    }
}

double EnhancedVisualizationInterface::getUpdateRate() const {
    return config_.updateRate;
}

void EnhancedVisualizationInterface::collectData(const Brain& brain, double timestamp) {
    currentTime_ = timestamp;
    
    // Collect network data
    if (config_.visualizeNetwork) {
        updateNetworkData(brain);
    }
    
    // Collect activity data
    if (config_.visualizeActivity) {
        updateActivityData(brain);
    }
    
    // Collect spike data
    if (config_.visualizeSpikes) {
        updateSpikeData(brain);
    }
    
    // Collect weight data
    if (config_.visualizeWeights) {
        updateWeightData(brain);
    }
    
    // Collect memory data
    if (config_.visualizeMemory) {
        updateMemoryData(brain);
    }
    
    // Collect neuromodulation data
    if (config_.visualizeNeuromod) {
        updateNeuromodData(brain);
    }
    
    // Collect development data
    if (config_.visualizeDevelopment) {
        updateDevelopmentData(brain);
    }
}

void EnhancedVisualizationInterface::processData() {
    // Process collected data
    if (!activityData_.timeSeriesData.empty()) {
        // Update time series
        updateTimeSeries();
    }
    
    // Filter and aggregate data based on configuration
    filterAndAggregateData();
}

void EnhancedVisualizationInterface::renderTextOutput(const std::string& filename) {
    if (!active_) {
        NLM_LOG_WARN("Cannot render to text: visualization not active");
        return;
    }
    
    // Render to text backend
    textBackend_->render(networkData_);
    textBackend_->render(activityData_);
    textBackend_->render(spikeData_);
    textBackend_->render(weightData_);
    textBackend_->render(memoryData_);
    textBackend_->render(neuromodData_);
    textBackend_->render(developmentData_);
    
    // Write to file
    std::string content = textBackend_->getContent();
    writeTextFile(filename, content);
    
    NLM_LOG_INFO("Text visualization rendered to " + filename);
}

void EnhancedVisualizationInterface::renderSVGOutput(const std::string& filename) {
    if (!active_) {
        NLM_LOG_WARN("Cannot render to SVG: visualization not active");
        return;
    }
    
    // Render to SVG backend
    svgBackend_->render(networkData_);
    svgBackend_->render(activityData_);
    svgBackend_->render(spikeData_);
    svgBackend_->render(weightData_);
    svgBackend_->render(memoryData_);
    svgBackend_->render(neuromodData_);
    svgBackend_->render(developmentData_);
    
    // Write to file
    std::string content = svgBackend_->getContent();
    writeSVGFile(filename, content);
    
    NLM_LOG_INFO("SVG visualization rendered to " + filename);
}

void EnhancedVisualizationInterface::renderJSONOutput(const std::string& filename) {
    if (!active_) {
        NLM_LOG_WARN("Cannot render to JSON: visualization not active");
        return;
    }
    
    // Render to JSON backend
    jsonBackend_->render(networkData_);
    jsonBackend_->render(activityData_);
    jsonBackend_->render(spikeData_);
    jsonBackend_->render(weightData_);
    jsonBackend_->render(memoryData_);
    jsonBackend_->render(neuromodData_);
    jsonBackend_->render(developmentData_);
    
    // Write to file
    std::string content = jsonBackend_->getContent();
    writeJSONFile(filename, content);
    
    NLM_LOG_INFO("JSON visualization rendered to " + filename);
}

void EnhancedVisualizationInterface::renderHTMLOutput(const std::string& filename) {
    if (!active_) {
        NLM_LOG_WARN("Cannot render to HTML: visualization not active");
        return;
    }
    
    // Render to HTML backend
    htmlBackend_->render(networkData_);
    htmlBackend_->render(activityData_);
    htmlBackend_->render(spikeData_);
    htmlBackend_->render(weightData_);
    htmlBackend_->render(memoryData_);
    htmlBackend_->render(neuromodData_);
    htmlBackend_->render(developmentData_);
    
    // Write to file
    std::string content = htmlBackend_->getContent();
    writeHTMLFile(filename, content);
    
    NLM_LOG_INFO("HTML visualization rendered to " + filename);
}

void EnhancedVisualizationInterface::generateAnimation(const std::string& filename, double duration) {
    if (!active_) {
        NLM_LOG_WARN("Cannot generate animation: visualization not active");
        return;
    }
    
    // Calculate number of frames
    size_t numFrames = static_cast<size_t>(duration * config_.updateRate);
    
    // Generate frames
    generateFrames(filename + "_frames", numFrames);
    
    // Combine frames into animation
    combineFramesToAnimation(filename, numFrames);
    
    NLM_LOG_INFO("Animation generated: " + filename);
}

void EnhancedVisualizationInterface::generateFrames(const std::string& directory, size_t numFrames) {
    if (!active_) return;
    
    for (size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
        // Advance time
        currentTime_ += 1.0 / config_.updateRate;
        
        // Render frame
        renderFrame(frameIndex, directory + "_frame_" + std::to_string(frameIndex) + "." + config_.outputFormat);
        
        // Small delay to maintain frame rate
        if (config_.frameRate > 0.0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0 / config_.frameRate)));
        }
    }
}

void EnhancedVisualizationInterface::renderFrame(size_t frameIndex, const std::string& filename) {
    // Set current frame
    currentFrame_ = frameIndex;
    
    // Render to backend
    if (config_.backend == "text") {
        renderToTextBackend();
        writeTextFile(filename, textBackend_->getContent());
    } else if (config_.backend == "svg") {
        renderToSVGBackend();
        writeSVGFile(filename, svgBackend_->getContent());
    } else if (config_.backend == "json") {
        renderToJSONBackend();
        writeJSONFile(filename, jsonBackend_->getContent());
    } else if (config_.backend == "html") {
        renderToHTMLBackend();
        writeHTMLFile(filename, htmlBackend_->getContent());
    }
}

void EnhancedVisualizationInterface::updateNetworkData(const Brain& brain) {
    networkData_.regions.clear();
    networkData_.adjacencyMatrix.clear();
    
    // Get regions from brain
    auto regionIds = brain.getRegionIds();
    
    for (const auto& regionId : regionIds) {
        auto* region = brain.getRegion(regionId);
        if (region) {
            std::string regionName = region->getName();
            networkData_.regions.push_back(regionName);
            
            // Get neuron and connection information
            auto neurons = region->getAllNeurons();
            auto connections = region->getSynapses();
            
            // Build adjacency matrix (simplified)
            std::vector<double> row(neurons.size(), 0.0);
            for (size_t i = 0; i < neurons.size(); ++i) {
                for (size_t j = 0; j < neurons.size(); ++j) {
                    if (i != j) {
                        // Check if there's a connection
                        bool connected = false;
                        for (const auto& syn : connections) {
                            if (syn->getSourceNeuron() == neurons[i]->getId() &&
                                syn->getDestinationNeuron() == neurons[j]->getId()) {
                                connected = true;
                                break;
                            }
                        }
                        row[j] = connected ? 1.0 : 0.0;
                    }
                }
            }
            
            networkData_.adjacencyMatrix.push_back(row);
        }
    }
    
    // Add region properties
    for (const auto& regionName : networkData_.regions) {
        networkData_.properties[regionName] = {
            "neuron_count": networkData_.adjacencyMatrix.size(),
            "connection_density": calculateConnectionDensity(networkData_.adjacencyMatrix)
        };
    }
}

void EnhancedVisualizationInterface::updateActivityData(const Brain& brain) {
    activityData_.firingRates.clear();
    activityData_.membranePotentials.clear();
    activityData_.timeSeriesData.clear();
    
    // Get total neuron count
    size_t totalNeurons = brain.getTotalNeuronCount();
    
    // Calculate firing rates for each population type
    activityData_.firingRates.resize(static_cast<size_t>(NeuronType::Count), 0.0f);
    
    // Get populations and calculate activity
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            auto neurons = pop->getNeurons();
            size_t activeNeurons = 0;
            double totalPotential = 0.0;
            
            for (const auto* neuron : neurons) {
                if (neuron->isFiring()) {
                    activeNeurons++;
                }
                totalPotential += neuron->getMembranePotential();
            }
            
            double firingRate = static_cast<double>(activeNeurons) / neurons.size();
            activityData_.firingRates[static_cast<size_t>(pop->getNeuronType())] = firingRate;
            
            double averagePotential = totalPotential / neurons.size();
            activityData_.membranePotentials.push_back(averagePotential);
        }
    }
    
    // Add time series data
    ActivityDataPoint point;
    point.timestamp = currentTime_;
    point.firingRates = activityData_.firingRates;
    point.averageMembranePotential = std::accumulate(activityData_.membranePotentials.begin(),
                                                     activityData_.membranePotentials.end(), 0.0) / activityData_.membranePotentials.size();
    
    activityData_.timeSeriesData.push_back(point);
    
    // Limit history size
    if (activityData_.timeSeriesData.size() > 100) {
        activityData_.timeSeriesData.erase(activityData_.timeSeriesData.begin());
    }
}

void EnhancedVisualizationInterface::updateSpikeData(const Brain& brain) {
    spikeData_.spikeEvents.clear();
    spikeData_.spikeRasters.clear();
    
    // Collect spike events from brain
    auto regions = brain.getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (const auto* neuron : neurons) {
            if (neuron->getLastSpikeTime() > 0.0) {
                spikeData_.spikeEvents.push_back(std::make_pair(neuron->getLastSpikeTime(), neuron->getId().index()));
            }
        }
    }
    
    // Create spike raster data
    double timeWindow = 1000.0;  // 1 second window
    for (const auto& spikeEvent : spikeData_.spikeEvents) {
        double time = spikeEvent.first;
        int neuronId = spikeEvent.second;
        
        // Add to raster
        spikeData_.spikeRasters.push_back({time, static_cast<double>(neuronId)});
    }
    
    // Calculate statistics
    spikeData_.timeWindow = timeWindow;
}

void EnhancedVisualizationInterface::updateWeightData(const Brain& brain) {
    weightData_.weightMatrix.clear();
    weightData_.connectionTypes.clear();
    
    // Get all synapses
    for (const auto& region : brain.getRegions()) {
        auto synapses = region->getSynapses();
        
        // Create weight matrix (simplified)
        std::vector<double> row(synapses.size(), 0.0);
        std::vector<std::string> connTypes;
        
        for (size_t i = 0; i < synapses.size(); ++i) {
            row[i] = synapses[i]->getWeight();
            connTypes.push_back(synapseToString(synapses[i]->getType()));
        }
        
        weightData_.weightMatrix.push_back(row);
        weightData_.connectionTypes.push_back(connTypes);
    }
    
    // Calculate weight statistics
    weightData_.weightRangeMin = 1.0f;
    weightData_.weightRangeMax = -1.0f;
    
    for (const auto& row : weightData_.weightMatrix) {
        for (double weight : row) {
            weightData_.weightRangeMin = std::min(weightData_.weightRangeMin, weight);
            weightData_.weightRangeMax = std::max(weightData_.weightRangeMax, weight);
        }
    }
}

void EnhancedVisualizationInterface::updateMemoryData(const Brain& brain) {
    memoryData_.workingMemoryTraces.clear();
    memoryData_.episodicMemoryStrengths.clear();
    memoryData_.conceptRepresentations.clear();
    
    // Get working memory traces
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Get active traces (simplified)
        auto traces = workingMemory->getActiveTraces();
        for (const auto& trace : traces) {
            memoryData_.workingMemoryTraces.push_back(trace.strength);
        }
    }
    
    // Get episodic memory episodes
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        // Get recent episodes (simplified)
        auto episodes = episodicMemory->getRecentEpisodes(10);
        for (const auto* episode : episodes) {
            memoryData_.episodicMemoryStrengths.push_back(episode->reward);
        }
    }
    
    // Get concept representations
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        // Get concepts (simplified)
        auto concepts = conceptFormation->getConcepts();
        for (const auto& concept : concepts) {
            memoryData_.conceptRepresentations.push_back(concept.name);
        }
    }
}

void EnhancedVisualizationInterface::updateNeuromodData(const Brain& brain) {
    neuromodData_.dopamineLevels.clear();
    neuromodData_.curiosityLevels.clear();
    neuromodData_.predictionErrorLevels.clear();
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        neuromodData_.dopamineLevels.push_back(dopamine->getLevel());
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        neuromodData_.curiosityLevels.push_back(curiosity->getLevel());
    }
    
    auto* predictionError = brain.getPredictionErrorSignal();
    if (predictionError) {
        neuromodData_.predictionErrorLevels.push_back(predictionError->getLevel());
    }
    
    // Calculate exploration drive
    neuromodData_.explorationDrive = calculateExplorationDrive(neuromodData_);
}

void EnhancedVisualizationInterface::updateDevelopmentData(const Brain& brain) {
    developmentData_.plasticityRates.clear();
    developmentData_.developmentalStages.clear();
    
    // Get development system
    auto* development = brain.getDevelopmentSystem();
    if (development) {
        // Get developmental stage
        auto stage = brain.getDevelopmentalStage();
        developmentData_.developmentalStages.push_back(developmentStageToString(stage));
        
        // Get plasticity rates (simplified)
        developmentData_.plasticityRates.push_back(1.0f / development->getAge());
    }
    
    // Get maturation progress
    developmentData_.maturationProgress = 1.0f - developmentData_.plasticityRates.back();
}

// Backend implementations

void EnhancedVisualizationInterface::TextBackend::initialize() {
    // Initialize text backend
}

void EnhancedVisualizationInterface::TextBackend::setUpdateRate(double hz) {
    updateRate_ = hz;
}

void EnhancedVisualizationInterface::TextBackend::close() {
    content_.clear();
}

void EnhancedVisualizationInterface::TextBackend::render(const NetworkData& network) {
    std::stringstream ss;
    ss << "=== Network Visualization ===\n";
    ss << "Regions: " << network.regions.size() << "\n";
    ss << "Adjacency Matrix:\n";
    for (const auto& row : network.adjacencyMatrix) {
        for (double val : row) {
            ss << (val > 0.5 ? "1 " : "0 ");
        }
        ss << "\n";
    }
    ss << "\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::TextBackend::render(const ActivityData& activity) {
    std::stringstream ss;
    ss << "=== Activity Visualization ===\n";
    ss << "Firing Rates by Type:\n";
    for (size_t i = 0; i < static_cast<size_t>(NeuronType::Count); ++i) {
        ss << neuronTypeToString(static_cast<NeuronType>(i)) << ": " << activity.firingRates[i] << "\n";
    }
    ss << "\nAverage Membrane Potential: " << std::accumulate(activity.membranePotentials.begin(),
                                                             activity.membranePotentials.end(), 0.0) / activity.membranePotentials.size() << "\n";
    ss << "\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::TextBackend::render(const SpikeData& spikes) {
    std::stringstream ss;
    ss << "=== Spike Visualization ===\n";
    ss << "Total Spikes: " << spikes.spikeEvents.size() << "\n";
    ss << "Time Window: " << spikes.timeWindow << " ms\n";
    ss << "Spike Raster:\n";
    for (const auto& spike : spikes.spikeRasters) {
        ss << "  Time: " << spike.first << " ms, Neuron: " << spike.second << "\n";
    }
    ss << "\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::TextBackend::render(const WeightData& weights) {
    std::stringstream ss;
    ss << "=== Weight Visualization ===\n";
    ss << "Weight Range: [" << weights.weightRangeMin << ", " << weights.weightRangeMax << "]\n";
    ss << "Weight Matrix:\n";
    for (const auto& row : weights.weightMatrix) {
        for (double weight : row) {
            ss << weight << " ";
        }
        ss << "\n";
    }
    ss << "\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::TextBackend::render(const MemoryData& memory) {
    std::stringstream ss;
    ss << "=== Memory Visualization ===\n";
    ss << "Working Memory Traces: " << memory.workingMemoryTraces.size() << "\n";
    ss << "Episodic Memory Episodes: " << memory.episodicMemoryStrengths.size() << "\n";
    ss << "Concept Representations: " << memory.conceptRepresentations.size() << "\n";
    ss << "Consolidation Level: " << memory.consolidationLevel << "\n";
    ss << "\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::TextBackend::render(const NeuromodData& neuromod) {
    std::stringstream ss;
    ss << "=== Neuromodulation Visualization ===\n";
    ss << "Dopamine Levels: " << (neuromod.dopamineLevels.empty() ? "N/A" : std::to_string(neuromod.dopamineLevels.back())) << "\n";
    ss << "Curiosity Levels: " << (neuromod.curiosityLevels.empty() ? "N/A" : std::to_string(neuromod.curiosityLevels.back())) << "\n";
    ss << "Prediction Error Levels: " << (neuromod.predictionErrorLevels.empty() ? "N/A" : std::to_string(neuromod.predictionErrorLevels.back())) << "\n";
    ss << "Exploration Drive: " << neuromod.explorationDrive << "\n";
    ss << "\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::TextBackend::render(const DevelopmentData& development) {
    std::stringstream ss;
    ss << "=== Development Visualization ===\n";
    ss << "Developmental Stage: " << development.developmentalStages.back() << "\n";
    ss << "Plasticity Rates: " << development.plasticityRates.size() << "\n";
    ss << "Maturation Progress: " << development.maturationProgress << "\n";
    ss << "\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

std::string EnhancedVisualizationInterface::TextBackend::getContent() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return content_;
}

// SVG Backend implementation (simplified)
void EnhancedVisualizationInterface::SVGBackend::initialize() {
    // Initialize SVG backend
}

void EnhancedVisualizationInterface::SVGBackend::setUpdateRate(double hz) {
    updateRate_ = hz;
}

void EnhancedVisualizationInterface::SVGBackend::close() {
    content_.clear();
}

void EnhancedVisualizationInterface::SVGBackend::render(const NetworkData& network) {
    // Generate SVG for network visualization
    std::stringstream ss;
    ss << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    ss << "  <title>Network Visualization</title>\n";
    ss << "  <!-- Network content would go here -->\n";
    ss << "</svg>\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::SVGBackend::render(const ActivityData& activity) {
    // Generate SVG for activity visualization
    std::stringstream ss;
    ss << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    ss << "  <title>Activity Visualization</title>\n";
    ss << "  <!-- Activity content would go here -->\n";
    ss << "</svg>\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::SVGBackend::render(const SpikeData& spikes) {
    // Generate SVG for spike visualization
    std::stringstream ss;
    ss << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    ss << "  <title>Spike Visualization</title>\n";
    ss << "  <!-- Spike content would go here -->\n";
    ss << "</svg>\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::SVGBackend::render(const WeightData& weights) {
    // Generate SVG for weight visualization
    std::stringstream ss;
    ss << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    ss << "  <title>Weight Visualization</title>\n";
    ss << "  <!-- Weight content would go here -->\n";
    ss << "</svg>\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::SVGBackend::render(const MemoryData& memory) {
    // Generate SVG for memory visualization
    std::stringstream ss;
    ss << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    ss << "  <title>Memory Visualization</title>\n";
    ss << "  <!-- Memory content would go here -->\n";
    ss << "</svg>\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::SVGBackend::render(const NeuromodData& neuromod) {
    // Generate SVG for neuromodulation visualization
    std::stringstream ss;
    ss << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    ss << "  <title>Neuromodulation Visualization</title>\n";
    ss << "  <!-- Neuromodulation content would go here -->\n";
    ss << "</svg>\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::SVGBackend::render(const DevelopmentData& development) {
    // Generate SVG for development visualization
    std::stringstream ss;
    ss << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    ss << "  <title>Development Visualization</title>\n";
    ss << "  <!-- Development content would go here -->\n";
    ss << "</svg>\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

std::string EnhancedVisualizationInterface::SVGBackend::getContent() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return content_;
}

// JSON Backend implementation (simplified)
void EnhancedVisualizationInterface::JSONBackend::initialize() {
    // Initialize JSON backend
}

void EnhancedVisualizationInterface::JSONBackend::setUpdateRate(double hz) {
    updateRate_ = hz;
}

void EnhancedVisualizationInterface::JSONBackend::close() {
    content_.clear();
}

void EnhancedVisualizationInterface::JSONBackend::render(const NetworkData& network) {
    // Generate JSON for network visualization
    std::stringstream ss;
    ss << "{\"network\": {\"regions\": [";
    for (size_t i = 0; i < network.regions.size(); ++i) {
        ss << "\"" << network.regions[i] << "\"";
        if (i < network.regions.size() - 1) ss << ", ";
    }
    ss << "]}}";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ = ss.str();
}

void EnhancedVisualizationInterface::JSONBackend::render(const ActivityData& activity) {
    // Generate JSON for activity visualization
    std::stringstream ss;
    ss << "{\"activity\": {\"firingRates\": [";
    for (size_t i = 0; i < activity.firingRates.size(); ++i) {
        ss << activity.firingRates[i];
        if (i < activity.firingRates.size() - 1) ss << ", ";
    }
    ss << "]}}";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ = ss.str();
}

void EnhancedVisualizationInterface::JSONBackend::render(const SpikeData& spikes) {
    // Generate JSON for spike visualization
    std::stringstream ss;
    ss << "{\"spikes\": {\"events\": [";
    for (size_t i = 0; i < spikes.spikeEvents.size(); ++i) {
        ss << "{\"time\": " << spikes.spikeEvents[i].first << ", \"neuron\": " << spikes.spikeEvents[i].second << "}";
        if (i < spikes.spikeEvents.size() - 1) ss << ", ";
    }
    ss << "]}}";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ = ss.str();
}

void EnhancedVisualizationInterface::JSONBackend::render(const WeightData& weights) {
    // Generate JSON for weight visualization
    std::stringstream ss;
    ss << "{\"weights\": {\"matrix\": [";
    for (size_t i = 0; i < weights.weightMatrix.size(); ++i) {
        ss << "[";
        for (size_t j = 0; j < weights.weightMatrix[i].size(); ++j) {
            ss << weights.weightMatrix[i][j];
            if (j < weights.weightMatrix[i].size() - 1) ss << ", ";
        }
        ss << "]";
        if (i < weights.weightMatrix.size() - 1) ss << ", ";
    }
    ss << "]}}";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ = ss.str();
}

void EnhancedVisualizationInterface::JSONBackend::render(const MemoryData& memory) {
    // Generate JSON for memory visualization
    std::stringstream ss;
    ss << "{\"memory\": {\"traces\": [";
    for (size_t i = 0; i < memory.workingMemoryTraces.size(); ++i) {
        ss << memory.workingMemoryTraces[i];
        if (i < memory.workingMemoryTraces.size() - 1) ss << ", ";
    }
    ss << "]}}";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ = ss.str();
}

void EnhancedVisualizationInterface::JSONBackend::render(const NeuromodData& neuromod) {
    // Generate JSON for neuromodulation visualization
    std::stringstream ss;
    ss << "{\"neuromod\": {\"dopamine\": [";
    for (size_t i = 0; i < neuromod.dopamineLevels.size(); ++i) {
        ss << neuromod.dopamineLevels[i];
        if (i < neuromod.dopamineLevels.size() - 1) ss << ", ";
    }
    ss << "], \"curiosity\": [";
    for (size_t i = 0; i < neuromod.curiosityLevels.size(); ++i) {
        ss << neuromod.curiosityLevels[i];
        if (i < neuromod.curiosityLevels.size() - 1) ss << ", ";
    }
    ss << "]}}";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ = ss.str();
}

void EnhancedVisualizationInterface::JSONBackend::render(const DevelopmentData& development) {
    // Generate JSON for development visualization
    std::stringstream ss;
    ss << "{\"development\": {\"stages\": [";
    for (size_t i = 0; i < development.developmentalStages.size(); ++i) {
        ss << "\"" << development.developmentalStages[i] << "\"";
        if (i < development.developmentalStages.size() - 1) ss << ", ";
    }
    ss << "], \"plasticityRates\": [";
    for (size_t i = 0; i < development.plasticityRates.size(); ++i) {
        ss << development.plasticityRates[i];
        if (i < development.plasticityRates.size() - 1) ss << ", ";
    }
    ss << "]}}";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ = ss.str();
}

std::string EnhancedVisualizationInterface::JSONBackend::getContent() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return content_;
}

// HTML Backend implementation (simplified)
void EnhancedVisualizationInterface::HTMLBackend::initialize() {
    // Initialize HTML backend
}

void EnhancedVisualizationInterface::HTMLBackend::setUpdateRate(double hz) {
    updateRate_ = hz;
}

void EnhancedVisualizationInterface::HTMLBackend::close() {
    content_.clear();
}

void EnhancedVisualizationInterface::HTMLBackend::render(const NetworkData& network) {
    // Generate HTML for network visualization
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head><title>Network Visualization</title></head>\n<body>";
    ss << "<h1>Network Visualization</h1>";
    ss << "<p>Regions: ";
    for (size_t i = 0; i < network.regions.size(); ++i) {
        ss << network.regions[i];
        if (i < network.regions.size() - 1) ss << ", ";
    }
    ss << "</p>";
    ss << "</body>\n</html>";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::HTMLBackend::render(const ActivityData& activity) {
    // Generate HTML for activity visualization
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head><title>Activity Visualization</title></head>\n<body>";
    ss << "<h1>Activity Visualization</h1>";
    ss << "<h2>Firing Rates</h2><ul>";
    for (size_t i = 0; i < static_cast<size_t>(NeuronType::Count); ++i) {
        ss << "<li>" << neuronTypeToString(static_cast<NeuronType>(i)) << ": " << activity.firingRates[i] << "</li>";
    }
    ss << "</ul></body>\n</html>";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::HTMLBackend::render(const SpikeData& spikes) {
    // Generate HTML for spike visualization
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head><title>Spike Visualization</title></head>\n<body>";
    ss << "<h1>Spike Visualization</h1>";
    ss << "<p>Total Spikes: " << spikes.spikeEvents.size() << "</p>";
    ss << "<p>Time Window: " << spikes.timeWindow << " ms</p>";
    ss << "</body>\n</html>";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::HTMLBackend::render(const WeightData& weights) {
    // Generate HTML for weight visualization
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head><title>Weight Visualization</title></head>\n<body>";
    ss << "<h1>Weight Visualization</h1>";
    ss << "<p>Weight Range: [" << weights.weightRangeMin << ", " << weights.weightRangeMax << "]</p>";
    ss << "</body>\n</html>";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::HTMLBackend::render(const MemoryData& memory) {
    // Generate HTML for memory visualization
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head><title>Memory Visualization</title></head>\n<body>";
    ss << "<h1>Memory Visualization</h1>";
    ss << "<p>Working Memory Traces: " << memory.workingMemoryTraces.size() << "</p>";
    ss << "<p>Episodic Memory Episodes: " << memory.episodicMemoryStrengths.size() << "</p>";
    ss << "<p>Consolidation Level: " << memory.consolidationLevel << "</p>";
    ss << "</body>\n</html>";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::HTMLBackend::render(const NeuromodData& neuromod) {
    // Generate HTML for neuromodulation visualization
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head><title>Neuromodulation Visualization</title></head>\n<body>";
    ss << "<h1>Neuromodulation Visualization</h1>";
    ss << "<p>Dopamine: " << (neuromod.dopamineLevels.empty() ? "N/A" : std::to_string(neuromod.dopamineLevels.back())) << "</p>";
    ss << "<p>Curiosity: " << (neuromod.curiosityLevels.empty() ? "N/A" : std::to_string(neuromod.curiosityLevels.back())) << "</p>";
    ss << "<p>Exploration Drive: " << neuromod.explorationDrive << "</p>";
    ss << "</body>\n</html>";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

void EnhancedVisualizationInterface::HTMLBackend::render(const DevelopmentData& development) {
    // Generate HTML for development visualization
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n<head><title>Development Visualization</title></head>\n<body>";
    ss << "<h1>Development Visualization</h1>";
    ss << "<p>Developmental Stage: " << development.developmentalStages.back() << "</p>";
    ss << "<p>Maturation Progress: " << development.maturationProgress << "</p>";
    ss << "</body>\n</html>";
    
    std::lock_guard<std::mutex> lock(mutex_);
    content_ += ss.str();
}

std::string EnhancedVisualizationInterface::HTMLBackend::getContent() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return content_;
}

// Utility functions
std::string EnhancedVisualizationInterface::generateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string EnhancedVisualizationInterface::escapeHTML(const std::string& text) const {
    std::string escaped = text;
    size_t pos = 0;
    while ((pos = escaped.find('&', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&amp;");
        pos += 5;
    }
    pos = 0;
    while ((pos = escaped.find('<', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = escaped.find('>', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&gt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&quot;");
        pos += 6;
    }
    pos = 0;
    while ((pos = escaped.find("'", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&apos;");
        pos += 6;
    }
    return escaped;
}

std::string EnhancedVisualizationInterface::formatNumber(double value, int precision) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::string EnhancedVisualizationInterface::getColorForValue(double value, const std::string& scheme) const {
    if (scheme == "heatmap") {
        // Heatmap: blue -> green -> red
        if (value < 0.5) {
            double t = value / 0.5;
            return "rgb(" + std::to_string(static_cast<int>(255 * t)) + ", " +
                   std::to_string(static_cast<int>(255 * (1 - t))) + ", 0)";
        } else {
            double t = (value - 0.5) / 0.5;
            return "rgb(0, " + std::to_string(static_cast<int>(255 * (1 - t))) + ", " +
                   std::to_string(static_cast<int>(255 * t)) + ")";
        }
    } else if (scheme == "categorical") {
        // Simple categorical colors
        int index = static_cast<int>(value * 10.0) % 10;
        const char* colors[] = {"#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF",
                               "#00FFFF", "#800000", "#008000", "#000080", "#808080"};
        return colors[index];
    } else {
        // Default: grayscale
        int intensity = static_cast<int>(value * 255.0);
        return "rgb(" + std::to_string(intensity) + ", " +
               std::to_string(intensity) + ", " + std::to_string(intensity) + ")";
    }
}

// Data filtering and aggregation
double EnhancedVisualizationInterface::calculateConnectionDensity(const std::vector<std::vector<double>>& matrix) {
    if (matrix.empty()) return 0.0;
    
    double totalConnections = 0.0;
    for (const auto& row : matrix) {
        for (double value : row) {
            if (value > 0.5) totalConnections++;
        }
    }
    
    double possibleConnections = matrix.size() * (matrix.size() - 1);
    return (possibleConnections > 0) ? totalConnections / possibleConnections : 0.0;
}

std::string EnhancedVisualizationInterface::neuronTypeToString(NeuronType type) const {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Internal: return "Internal";
        default: return "Unknown";
    }
}

std::string EnhancedVisualizationInterface::developmentStageToString(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

std::string EnhancedVisualizationInterface::synapseTypeToString(SynapseType type) const {
    switch (type) {
        case SynapseType::Excitatory: return "Excitatory";
        case SynapseType::Inhibitory: return "Inhibitory";
        case SynapseType::Modulatory: return "Modulatory";
        default: return "Unknown";
    }
}

// Time series and animation
void EnhancedVisualizationInterface::updateTimeSeries() {
    // Update time series data
    double currentTime = currentTime_;
    
    // Calculate statistics
    double avgFiringRate = 0.0;
    if (!activityData_.timeSeriesData.empty()) {
        avgFiringRate = activityData_.timeSeriesData.back().averageMembranePotential;
    }
    
    timeSeries_.push_back(avgFiringRate);
    
    // Limit history size
    if (timeSeries_.size() > 1000) {
        timeSeries_.erase(timeSeries_.begin());
    }
}

void EnhancedVisualizationInterface::filterAndAggregateData() {
    // Filter data based on configuration
    if (config_.timeWindow > 0.0) {
        double startTime = currentTime_ - config_.timeWindow;
        double endTime = currentTime_;
        
        // Filter activity data
        ActivityData filteredActivity;
        for (const auto& point : activityData_.timeSeriesData) {
            if (point.timestamp >= startTime && point.timestamp <= endTime) {
                filteredActivity.timeSeriesData.push_back(point);
            }
        }
        
        // Update activity data
        activityData_ = filteredActivity;
    }
}

// Statistics and metrics
double EnhancedVisualizationInterface::getAverageFiringRate() const {
    if (activityData_.firingRates.empty()) return 0.0;
    
    double sum = 0.0;
    for (double rate : activityData_.firingRates) {
        sum += rate;
    }
    return sum / activityData_.firingRates.size();
}

double EnhancedVisualizationInterface::getMaxFiringRate() const {
    if (activityData_.firingRates.empty()) return 0.0;
    
    double maxRate = 0.0;
    for (double rate : activityData_.firingRates) {
        if (rate > maxRate) maxRate = rate;
    }
    return maxRate;
}

size_t EnhancedVisualizationInterface::getActiveNeuronCount() const {
    // This would be calculated from spike data or activity data
    // For now, return a simplified estimate
    return static_cast<size_t>(getAverageFiringRate() * 1000.0);
}

size_t EnhancedVisualizationInterface::getTotalSpikeCount() const {
    return static_cast<size_t>(spikeData_.spikeEvents.size());
}

double EnhancedVisualizationInterface::getExcitementInhibitionRatio() const {
    if (weightData_.weightMatrix.empty()) return 1.0;
    
    double totalExcitatory = 0.0;
    double totalInhibitory = 0.0;
    
    for (const auto& row : weightData_.weightMatrix) {
        for (double weight : row) {
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0 ? std::numeric_limits<double>::infinity() : 0.0;
}

double EnhancedVisualizationInterface::getDevelopmentProgress() const {
    return developmentData_.maturationProgress;
}

double EnhancedVisualizationInterface::getLearningProgress() const {
    // Calculate learning progress based on weight changes
    if (weightData_.weightMatrix.empty()) return 0.0;
    
    // This is a simplified calculation
    double currentWeights = 0.0;
    double averageWeights = 0.0;
    
    for (const auto& row : weightData_.weightMatrix) {
        for (double weight : row) {
            currentWeights += std::abs(weight);
        }
    }
    
    if (weightData_.weightMatrix.size() > 0) {
        averageWeights = currentWeights / (weightData_.weightMatrix.size() * weightData_.weightMatrix[0].size());
    }
    
    return std::min(1.0, averageWeights / 1.0);
}

bool EnhancedVisualizationInterface::isDataCollected() const {
    return !networkData_.regions.empty() || !activityData_.firingRates.empty() ||
           !spikeData_.spikeEvents.empty() || !weightData_.weightMatrix.empty();
}

double EnhancedVisualizationInterface::getProgress() const {
    if (isDataCollected()) {
        return 1.0;
    }
    return 0.0;
}

std::string EnhancedVisualizationInterface::getStatusMessage() const {
    if (!active_) {
        return "Visualization not active";
    }
    
    if (!isDataCollected()) {
        return "Collecting data...";
    }
    
    return "Visualization running";
}

void EnhancedVisualizationInterface::clearPreviousRenders() {
    // Clear previous backend data
    if (config_.backend == "text") {
        textBackend_->clear();
    } else if (config_.backend == "svg") {
        svgBackend_->clear();
    } else if (config_.backend == "json") {
        jsonBackend_->clear();
    } else if (config_.backend == "html") {
        htmlBackend_->clear();
    }
}

void EnhancedVisualizationInterface::renderToTextBackend() {
    textBackend_->render(networkData_);
    textBackend_->render(activityData_);
    textBackend_->render(spikeData_);
    textBackend_->render(weightData_);
    textBackend_->render(memoryData_);
    textBackend_->render(neuromodData_);
    textBackend_->render(developmentData_);
}

void EnhancedVisualizationInterface::renderToSVGBackend() {
    svgBackend_->render(networkData_);
    svgBackend_->render(activityData_);
    svgBackend_->render(spikeData_);
    svgBackend_->render(weightData_);
    svgBackend_->render(memoryData_);
    svgBackend_->render(neuromodData_);
    svgBackend_->render(developmentData_);
}

void EnhancedVisualizationInterface::renderToJSONBackend() {
    jsonBackend_->render(networkData_);
    jsonBackend_->render(activityData_);
    jsonBackend_->render(spikeData_);
    jsonBackend_->render(weightData_);
    jsonBackend_->render(memoryData_);
    jsonBackend_->render(neuromodData_);
    jsonBackend_->render(developmentData_);
}

void EnhancedVisualizationInterface::renderToHTMLBackend() {
    htmlBackend_->render(networkData_);
    htmlBackend_->render(activityData_);
    htmlBackend_->render(spikeData_);
    htmlBackend_->render(weightData_);
    htmlBackend_->render(memoryData_);
    htmlBackend_->render(neuromodData_);
    htmlBackend_->render(developmentData_);
}

void EnhancedVisualizationInterface::writeTextFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

void EnhancedVisualizationInterface::writeSVGFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

void EnhancedVisualizationInterface::writeJSONFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

void EnhancedVisualizationInterface::writeHTMLFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

double EnhancedVisualizationInterface::calculateExplorationDrive(const NeuromodData& neuromod) {
    if (neuromod.curiosityLevels.empty()) return 0.0;
    
    double curiosity = neuromod.curiosityLevels.back();
    double dopamine = neuromod.dopamineLevels.empty() ? 0.0 : neuromod.dopamineLevels.back();
    
    return curiosity * (1.0 + dopamine);
}

void EnhancedVisualizationInterface::cleanupBackends() {
    if (textBackend_) {
        textBackend_->close();
        textBackend_.reset();
    }
    
    if (svgBackend_) {
        svgBackend_->close();
        svgBackend_.reset();
    }
    
    if (jsonBackend_) {
        jsonBackend_->close();
        jsonBackend_.reset();
    }
    
    if (htmlBackend_) {
        htmlBackend_->close();
        htmlBackend_.reset();
    }
}

// Generate statistical summary
std::string EnhancedVisualizationInterface::generateStatisticalSummary() const {
    std::stringstream ss;
    ss << "=== NLM Visualization Statistical Summary ===\n\n";
    
    ss << "Network Statistics:\n";
    ss << "  Regions: " << networkData_.regions.size() << "\n";
    ss << "  Total Neurons: " << networkData_.adjacencyMatrix.size() << "\n";
    ss << "  Connection Density: " << calculateConnectionDensity(networkData_.adjacencyMatrix) << "\n\n";
    
    ss << "Activity Statistics:\n";
    ss << "  Average Firing Rate: " << getAverageFiringRate() << "\n";
    ss << "  Maximum Firing Rate: " << getMaxFiringRate() << "\n";
    ss << "  Active Neurons: " << getActiveNeuronCount() << "\n";
    ss << "  Total Spikes: " << getTotalSpikeCount() << "\n\n";
    
    ss << "Plasticity Statistics:\n";
    ss << "  E/I Ratio: " << getExcitementInhibitionRatio() << "\n";
    ss << "  Learning Progress: " << getLearningProgress() << "\n\n";
    
    ss << "Development Statistics:\n";
    ss << "  Development Stage: " << developmentData_.developmentalStages.back() << "\n";
    ss << "  Development Progress: " << getDevelopmentProgress() << "\n\n";
    
    ss << "Visualization Status:\n";
    ss << "  Active: " << (active_ ? "Yes" : "No") << "\n";
    ss << "  Data Collected: " << (isDataCollected() ? "Yes" : "No") << "\n";
    ss << "  Progress: " << getProgress() * 100.0 << "%\n";
    ss << "  Status: " << getStatusMessage() << "\n";
    
    return ss.str();
}

} // namespace nlm