#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <map>
#include <set>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    VisualizationConfig config;
    VisualizationMode currentMode;
    std::unordered_set<VisualizationMode> enabledModes;
    
    // Data for visualization
    std::vector<std::vector<double>> networkData;
    std::vector<std::vector<double>> activityData;
    std::vector<std::vector<double>> spikeData;
    std::vector<std::vector<double>> weightData;
    std::vector<std::vector<double>> memoryData;
    std::vector<std::vector<double>> neuromodData;
    
    // Brain system references
    std::shared_ptr<Brain> brain;
    std::shared_ptr<MonitoringSystem> monitoringSystem;
    
    // Interaction state
    int lastMouseX;
    int lastMouseY;
    bool mouseButtonDown;
    std::string hoveredElement;
    
    // Animation and timing
    double lastFrameTime;
    double accumulatedTime;
    int frameCount;
    
    // Statistical data
    std::unordered_map<std::string, double> currentMetrics;
    std::unordered_map<std::string, std::vector<double>> historicalMetrics;
    
    // Performance tracking
    double fps; // Frames per second
    double frameTime; // Time per frame in milliseconds
    double updateTime; // Time for update in milliseconds
    
    // Export state
    std::string currentAnimationPath;
    std::vector<std::string> animationFrames;
    
    Impl() : active(false), updateRate(30.0), currentMode(VisualizationMode::NETWORK_GRAPH)
        , lastMouseX(0), lastMouseY(0), mouseButtonDown(false)
        , lastFrameTime(0.0), accumulatedTime(0.0), frameCount(0), fps(0.0), frameTime(0.0), updateTime(0.0) {
        
        enabledModes.insert(VisualizationMode::NETWORK_GRAPH);
        enabledModes.insert(VisualizationMode::ACTIVITY_HEATMAP);
        enabledModes.insert(VisualizationMode::SPIKE_RASTER);
        
        // Initialize data buffers
        initializeBuffers();
    }
    
    void initializeBuffers() {
        size_t bufferSize = 1000;
        
        networkData.resize(bufferSize, std::vector<double>(5)); // neurons, synapses, eiri, regions, avg_weight
        activityData.resize(bufferSize, std::vector<double>(4)); // avg_rate, active, firing, spikes
        spikeData.resize(bufferSize, std::vector<double>(3)); // count, rate, density
        weightData.resize(bufferSize, std::vector<double>(3)); // avg_weight, std_weight, range
        memoryData.resize(bufferSize, std::vector<double>(8)); // working, episodic, associative, procedural, total_memory
        neuromodData.resize(bufferSize, std::vector<double>(6)); // dopamine, curiosity, novelty, error, level, threshold
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() {
    close();
}

bool VisualizationInterface::initialize(const VisualizationConfig& config) {
    pImpl->config = config;
    pImpl->active = true;
    pImpl->updateRate = config.updateFrequency;
    pImpl->lastFrameTime = 0.0;
    pImpl->accumulatedTime = 0.0;
    pImpl->frameCount = 0;
    
    // Initialize visualization libraries (simplified - would use GLFW/SDL in real implementation)
    NLM_LOG_INFO("Initializing VisualizationInterface with config: "
                 << "window=" << config.windowWidth << "x" << config.windowHeight
                 << ", fps=" << config.updateFrequency
                 << ", antialias=" << (config.enableAntialiasing ? "yes" : "no")
                 << ", 3D=" << (config.enable3D ? "yes" : "no"));
    
    // Initialize enabled modes
    pImpl->enabledModes.clear();
    pImpl->enabledModes.insert(VisualizationMode::NETWORK_GRAPH);
    pImpl->enabledModes.insert(VisualizationMode::ACTIVITY_HEATMAP);
    pImpl->enabledModes.insert(VisualizationMode::SPIKE_RASTER);
    
    NLM_LOG_INFO("VisualizationInterface initialized successfully");
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    double currentTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    pImpl->updateTime = (pImpl->lastFrameTime > 0.0) ? 
        (currentTime - pImpl->lastFrameTime) * 1000.0 : 0.0;
    pImpl->lastFrameTime = currentTime;
    pImpl->accumulatedTime += pImpl->updateTime;
    pImpl->frameCount++;
    
    pImpl->fps = (pImpl->accumulatedTime > 0.0) ? 
        (pImpl->frameCount * 1000.0 / pImpl->accumulatedTime) : 0.0;
    pImpl->frameTime = (pImpl->frameCount > 0) ? 
        pImpl->updateTime / pImpl->frameCount : 0.0;
    
    // Update brain data if available
    if (pImpl->brain) {
        calculateNetworkMetrics(*pImpl->brain);
        calculateActivityMetrics(*pImpl->brain);
        calculateMemoryMetrics(*pImpl->brain);
        calculateNeuromodulationMetrics(*pImpl->brain);
    }
    
    // Update visualization buffers
    updateBuffers();
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Clear screen (simplified - would use OpenGL/Vulkan in real implementation)
    // NLM_LOG_RENDER("Clearing screen");
    
    // Draw grid and background
    drawGrid();
    
    // Draw enabled visualization modes
    for (auto mode : pImpl->enabledModes) {
        switch (mode) {
            case VisualizationMode::NETWORK_GRAPH:
                if (pImpl->brain) drawNetworkGraph(*pImpl->brain);
                break;
            case VisualizationMode::ACTIVITY_HEATMAP:
                if (pImpl->brain) drawActivityHeatmap(*pImpl->brain);
                break;
            case VisualizationMode::SPIKE_RASTER:
                // Draw spike raster plot
                break;
            case VisualizationMode::WEIGHT_MATRIX:
                if (pImpl->brain) drawWeightMatrix(*pImpl->brain);
                break;
            case VisualizationMode::MEMORY_VISUALIZATION:
                if (pImpl->brain) drawMemoryVisualization(*pImpl->brain);
                break;
            case VisualizationMode::NEUROMODULATION_DISPLAY:
                if (pImpl->brain) drawNeuromodulationDisplay(*pImpl->brain);
                break;
            case VisualizationMode::DEVELOPMENT_STAGE:
                if (pImpl->brain) drawDevelopmentVisualization(*pImpl->brain);
                break;
            case VisualizationMode::CHECKPOINT_PROGRESS:
                drawCheckpointVisualization();
                break;
            case VisualizationMode::EXPERIMENT_TRACKING:
                drawExperimentVisualization();
                break;
            default:
                break;
        }
    }
    
    // Draw UI elements
    drawLabels();
    drawColorLegend();
    drawStatisticsPanel();
    drawControlPanel();
    
    // Save frame if recording
    if (!pImpl->currentAnimationPath.empty()) {
        saveAnimationFrame(pImpl->frameCount);
    }
}

void VisualizationInterface::close() {
    if (pImpl->active) {
        // Cleanup visualization resources
        // NLM_LOG_RENDER("Cleaning up visualization resources");
        
        pImpl->active = false;
        pImpl->brain.reset();
        pImpl->monitoringSystem.reset();
        
        // Clear data buffers
        pImpl->networkData.clear();
        pImpl->activityData.clear();
        pImpl->spikeData.clear();
        pImpl->weightData.clear();
        pImpl->memoryData.clear();
        pImpl->neuromData.clear();
        
        NLM_LOG_INFO("VisualizationInterface closed");
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    pImpl->config.updateFrequency = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    pImpl->brain = std::make_shared<Brain>(brain);
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    pImpl->brain = std::make_shared<Brain>(brain);
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Store spike data for raster plot visualization
    size_t bufferSize = pImpl->spikeData.size();
    
    for (size_t i = bufferSize - 1; i > 0; --i) {
        pImpl->spikeData[i] = pImpl->spikeData[i - 1];
    }
    
    pImpl->spikeData[0].resize(3);
    pImpl->spikeData[0][0] = static_cast<double>(spikes.size());
    pImpl->spikeData[0][1] = (spikes.size() / std::max(1.0, pImpl->config.updateFrequency));
    pImpl->spikeData[0][2] = calculateSpikeDensity(spikes);
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    pImpl->brain = std::make_shared<Brain>(brain);
}

void VisualizationInterface::visualizeMemorySystems(const Brain& brain) {
    // Calculate memory system statistics
    double workingMemoryActive = 0.0, episodicMemoryCount = 0.0;
    double associativeMemoryPatterns = 0.0, proceduralMemorySkills = 0.0;
    
    auto* wm = brain.getWorkingMemory();
    auto* em = brain.getEpisodicMemory();
    auto* am = brain.getAssociativeMemory();
    auto* pm = brain.getProceduralMemory();
    
    if (wm) workingMemoryActive = static_cast<double>(wm->getActiveTraces());
    if (em) episodicMemoryCount = static_cast<double>(em->getEpisodeCount());
    if (am) associativeMemoryPatterns = static_cast<double>(am->getPatternCount());
    if (pm) proceduralMemorySkills = static_cast<double>(pm->getSkillCount());
    
    recordTimeSeries("memory_systems", {workingMemoryActive, episodicMemoryCount, associativeMemoryPatterns, proceduralMemorySkills});
}

void VisualizationInterface::visualizeNeuromodulation(const Brain& brain) {
    // Calculate neuromodulator levels
    double dopamine = 0.0, curiosity = 0.0, novelty = 0.0, predictionError = 0.0;
    
    auto* dm = brain.getDopamine();
    auto* cu = brain.getCuriosity();
    auto* nv = brain.getNovelty();
    auto* pe = brain.getPredictionErrorSignal();
    
    if (dm) dopamine = dm->getLevel();
    if (cu) curiosity = cu->getLevel();
    if (nv) novelty = nv->getLevel();
    if (pe) predictionError = pe->getLevel();
    
    recordTimeSeries("neuromodulation", {dopamine, curiosity, novelty, predictionError});
}

void VisualizationInterface::visualizeDevelopmentStage(const Brain& brain) {
    // Calculate development stage visualization data
    double stage = static_cast<double>(brain.getDevelopmentalStage());
    
    recordTimeSeries("development_stage", {stage});
}

void VisualizationInterface::visualizeCheckpointProgress(const std::vector<std::pair<uint64_t, double>>& progress) {
    // Store checkpoint progress for visualization
    // In real implementation, this would render a progress bar or timeline
    recordTimeSeries("checkpoint_progress", {
        static_cast<double>(progress.size()),
        (progress.empty() ? 0.0 : progress.back().second),
        calculateAverageCheckpointSize(progress)
    });
}

void VisualizationInterface::visualizeExperimentProgress(const Experiment& experiment) {
    // Record experiment progress metrics
    recordTimeSeries("experiment_progress", {
        static_cast<double>(experiment.getStepData().size()),
        static_cast<double>(experiment.getMetrics().size()),
        static_cast<double>(experiment.getNotes().size())
    });
}

void VisualizationInterface::visualizePerformanceMetrics(const std::unordered_map<std::string, double>& metrics) {
    // Store performance metrics for visualization
    for (const auto& [name, value] : metrics) {
        recordTimeSeries("performance_" + name, {value});
    }
}

void VisualizationInterface::setMonitoringSystem(std::shared_ptr<MonitoringSystem> monitoring) {
    pImpl->monitoringSystem = monitoring;
    
    if (monitoring && monitoring->getEnableRealTimeUpdates()) {
        NLM_LOG_INFO("MonitoringSystem connected to VisualizationInterface");
    }
}

void VisualizationInterface::setVisualizationMode(VisualizationMode mode) {
    pImpl->currentMode = mode;
}

VisualizationInterface::VisualizationMode VisualizationInterface::getVisualizationMode() const {
    return pImpl->currentMode;
}

void VisualizationInterface::addVisualizationMode(VisualizationMode mode) {
    pImpl->enabledModes.insert(mode);
}

void VisualizationInterface::removeVisualizationMode(VisualizationMode mode) {
    pImpl->enabledModes.erase(mode);
}

bool VisualizationInterface::hasVisualizationMode(VisualizationMode mode) const {
    return pImpl->enabledModes.find(mode) != pImpl->enabledModes.end();
}

void VisualizationInterface::exportVisualization(const std::string& filename) {
    // Export current visualization state
    std::ofstream file(filename + ".txt");
    if (file.is_open()) {
        file << "Visualization export at step " << pImpl->frameCount << "\n";
        file << "Configuration: " << pImpl->config.windowWidth << "x" << pImpl->config.windowHeight << "\n";
        file << "Update rate: " << pImpl->config.updateFrequency << " Hz\n";
        file << "Active modes: ";
        for (auto mode : pImpl->enabledModes) {
            file << static_cast<int>(mode) << " ";
        }
        file << "\n";
        file.close();
        
        NLM_LOG_INFO("Visualization exported to: " + filename + ".txt");
    }
}

void VisualizationInterface::setConfig(const VisualizationConfig& config) {
    pImpl->config = config;
    if (config.updateFrequency > 0.0) {
        setUpdateRate(config.updateFrequency);
    }
}

VisualizationConfig VisualizationInterface::getConfig() const {
    return pImpl->config;
}

std::string VisualizationInterface::generateInsights() const {
    std::stringstream ss;
    ss << "=== VISUALIZATION INSIGHTS ===" << std::endl;
    
    if (pImpl->frameCount < 10) {
        ss << "Insufficient data for insights (need more frames)" << std::endl;
        return ss.str();
    }
    
    // Analyze activity patterns
    if (!pImpl->activityData.empty() && !pImpl->activityData[0].empty()) {
        double avgFiringRate = pImpl->activityData[0][0];
        double firingRateStd = calculateStdDev(pImpl->activityData, 0, 0);
        
        if (avgFiringRate > 50.0 && firingRateStd < 10.0) {
            ss << "Stable high activity detected" << std::endl;
        } else if (avgFiringRate > 20.0 && firingRateStd > 20.0) {
            ss << "Highly variable activity patterns" << std::endl;
        } else if (avgFiringRate < 5.0) {
            ss << "Low activity regime" << std::endl;
        }
    }
    
    // Analyze network stability
    if (!pImpl->networkData.empty() && !pImpl->networkData[0].empty()) {
        double eiriRatio = pImpl->networkData[0][2];
        if (eiriRatio > 2.0) {
            ss << "Strong excitation-inhibition balance" << std::endl;
        } else if (eiriRatio < 0.5) {
            ss << "Imbalanced network (high inhibition)" << std::endl;
        }
    }
    
    // Analyze memory engagement
    if (!pImpl->memoryData.empty() && !pImpl->memoryData[0].empty()) {
        double memoryUsage = pImpl->memoryData[0][0] + pImpl->memoryData[0][1]; // working + episodic
        if (memoryUsage > 50.0) {
            ss << "High memory engagement" << std::endl;
        } else if (memoryUsage < 10.0) {
            ss << "Low memory engagement" << std::endl;
        }
    }
    
    // Analyze neuromodulation state
    if (!pImpl->neuromData.empty() && !pImpl->neuromData[0].empty()) {
        double avgNeuromod = (pImpl->neuromData[0][0] + pImpl->neuromData[0][1]) / 2.0;
        if (avgNeuromod > 0.8) {
            ss << "High neuromodulatory state (exploration)" << std::endl;
        } else if (avgNeuromod < 0.2) {
            ss << "Low neuromodulatory state (exploitation)" << std::endl;
        }
    }
    
    ss << "Performance: " << std::fixed << std::setprecision(2) << pImpl->fps << " FPS" << std::endl;
    ss << "Frame time: " << std::fixed << std::setprecision(2) << pImpl->frameTime << " ms" << std::endl;
    
    return ss.str();
}

std::vector<std::string> VisualizationInterface::detectAnomalies() const {
    std::vector<std::string> anomalies;
    
    // Check for unusual activity patterns
    if (!pImpl->activityData.empty() && pImpl->activityData.size() > 10) {
        double recentAvgRate = calculateMovingAverage(pImpl->activityData, 0, 0, 10);
        double historicAvgRate = calculateMovingAverage(pImpl->activityData, 10, 0, 10);
        
        if (historicAvgRate > 0.0 && std::abs(recentAvgRate - historicAvgRate) / historicAvgRate > 0.5) {
            anomalies.push_back("Significant change in neural activity patterns");
        }
    }
    
    // Check for memory system anomalies
    if (!pImpl->memoryData.empty() && pImpl->memoryData.size() > 5) {
        double recentMemory = pImpl->memoryData[0][0]; // working memory
        double historicMemory = calculateMovingAverage(pImpl->memoryData, 0, 0, 5);
        
        if (historicMemory > 0.0 && std::abs(recentMemory - historicMemory) / historicMemory > 0.7) {
            anomalies.push_back("Memory system instability detected");
        }
    }
    
    // Check for neuromodulation anomalies
    if (!pImpl->neuromData.empty() && pImpl->neuromData.size() > 5) {
        double recentDopamine = pImpl->neuromData[0][0];
        double historicDopamine = calculateMovingAverage(pImpl->neuromData, 0, 0, 5);
        
        if (historicDopamine > 0.0 && std::abs(recentDopamine - historicDopamine) / historicDopamine > 0.8) {
            anomalies.push_back("Neuromodulator level anomaly detected");
        }
    }
    
    // Check for performance anomalies
    if (pImpl->fps < 10.0 && pImpl->frameCount > 50) {
        anomalies.push_back("Poor performance (low FPS)");
    }
    
    if (pImpl->frameTime > 50.0 && pImpl->frameCount > 50) {
        anomalies.push_back("High frame rendering time");
    }
    
    return anomalies;
}

double VisualizationInterface::calculateStabilityScore() const {
    double score = 100.0;
    
    // Penalize for large variations
    if (!pImpl->activityData.empty() && pImpl->activityData.size() > 20) {
        double activityVariation = calculateStdDev(pImpl->activityData, 0, 0);
        double activityMean = calculateMean(pImpl->activityData, 0, 0);
        if (activityMean > 0.0) {
            double activityCoefficient = activityVariation / activityMean;
            score -= std::min(30.0, activityCoefficient * 50.0);
        }
    }
    
    if (!pImpl->networkData.empty() && pImpl->networkData.size() > 20) {
        double networkVariation = calculateStdDev(pImpl->networkData, 0, 2); // E/I ratio variation
        double networkMean = calculateMean(pImpl->networkData, 0, 2);
        if (networkMean > 0.0) {
            double networkCoefficient = networkVariation / networkMean;
            score -= std::min(25.0, networkCoefficient * 50.0);
        }
    }
    
    // Penalize for performance issues
    if (pImpl->fps < 30.0 && pImpl->frameCount > 100) {
        score -= 20.0;
    }
    
    return std::max(0.0, score);
}

void VisualizationInterface::onMouseClick(int x, int y) {
    pImpl->lastMouseX = x;
    pImpl->lastMouseY = y;
    pImpl->mouseButtonDown = true;
    
    // Handle click on visualization elements
    if (pImpl->brain) {
        // Find element at click position
        // In real implementation, this would intersect with drawn elements
        std::string element = findElementAtPosition(x, y);
        if (!element.empty()) {
            pImpl->hoveredElement = element;
            // Trigger visualization of specific element
            if (element.find("neuron") != std::string::npos) {
                // Visualize specific neuron
            } else if (element.find("synapse") != std::string::npos) {
                // Visualize specific synapse
            }
        }
    }
}

void VisualizationInterface::onMouseMove(int x, int y) {
    if (pImpl->mouseButtonDown) {
        int dx = x - pImpl->lastMouseX;
        int dy = y - pImpl->lastMouseY;
        
        // Handle camera rotation/pan
        // In real implementation, this would update view transform
        pImpl->lastMouseX = x;
        pImpl->lastMouseY = y;
    }
}

void VisualizationInterface::onKeyPress(int key, bool pressed) {
    if (!pressed) return;
    
    // Handle keyboard shortcuts
    switch (key) {
        case 'W': case 'w':
            // Toggle wireframe mode
            break;
        case 'S': case 's':
            // Toggle stats display
            break;
        case 'M': case 'm':
            // Toggle memory visualization
            toggleVisualizationMode(VisualizationMode::MEMORY_VISUALIZATION);
            break;
        case 'N': case 'n':
            // Toggle neuromodulation display
            toggleVisualizationMode(VisualizationMode::NEUROMODULATION_DISPLAY);
            break;
        case 'D': case 'd':
            // Toggle development visualization
            toggleVisualizationMode(VisualizationMode::DEVELOPMENT_STAGE);
            break;
        case 'C': case 'c':
            // Toggle checkpoint visualization
            toggleVisualizationMode(VisualizationMode::CHECKPOINT_PROGRESS);
            break;
        case 'E': case 'e':
            // Toggle experiment visualization
            toggleVisualizationMode(VisualizationMode::EXPERIMENT_TRACKING);
            break;
        case 'F': case 'f':
            // Take screenshot
            saveScreenshot("screenshot_" + std::to_string(pImpl->frameCount) + ".png");
            break;
        default:
            break;
    }
}

void VisualizationInterface::onWindowResize(int width, int height) {
    pImpl->config.windowWidth = width;
    pImpl->config.windowHeight = height;
    // In real implementation, would update viewport and projection matrix
}

void VisualizationInterface::drawNetworkGraph(const Brain& brain) {
    // Simplified network graph drawing
    size_t neuronCount = brain.getTotalNeuronCount();
    size_t regionCount = brain.getRegionCount();
    
    // Draw regions
    float regionWidth = static_cast<float>(pImpl->config.windowWidth) / std::max(1.0f, static_cast<float>(regionCount));
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            float x = (i + 0.5f) * regionWidth;
            float y = static_cast<float>(pImpl->config.windowHeight) * 0.7f;
            
            // Draw region circle
            // In real implementation, would use OpenGL drawing commands
            // drawCircle(x, y, regionWidth * 0.3f, getRegionColor(i));
            
            // Draw neurons in region
            size_t neuronsInRegion = region->getTotalNeuronCount();
            float neuronRadius = std::min(regionWidth * 0.15f, neuronsInRegion * 0.005f);
            
            // Calculate firing activity for visualization
            float activity = calculateRegionActivity(region);
            float colorIntensity = 0.3f + activity * 0.7f;
            
            for (size_t j = 0; j < neuronsInRegion; ++j) {
                float angle = static_cast<float>(j) / static_cast<float>(neuronsInRegion) * 2.0f * 3.14159f;
                float nx = x + std::cos(angle) * neuronRadius * 2.0f;
                float ny = y + std::sin(angle) * neuronRadius * 2.0f;
                
                // Draw neuron with color based on type and activity
                // In real implementation:
                // float r = neuronType == NeuronType::Excitatory ? 0.8f : 0.2f;
                // float g = neuronType == NeuronType::Inhibitory ? 0.2f : 0.8f;
                // float b = activity;
                // drawCircle(nx, ny, neuronRadius, r, g, b);
            }
        }
    }
    
    // Draw region labels
    // In real implementation:
    // for (size_t i = 0; i < regionCount; ++i) {
    //     drawText((i + 0.5f) * regionWidth, pImpl->config.windowHeight * 0.85f, "Region_" + std::to_string(i + 1));
    // }
}

void VisualizationInterface::drawActivityHeatmap(const Brain& brain) {
    // Draw activity heatmap overlay
    size_t neuronCount = brain.getTotalNeuronCount();
    
    if (neuronCount == 0) return;
    
    // Create heatmap grid
    int gridSize = 50;
    std::vector<std::vector<float>> heatmap(gridSize, std::vector<float>(gridSize, 0.0f));
    
    // Fill heatmap with neural activity data
    for (size_t i = 0; i < neuronCount; ++i) {
        // In real implementation, would get neuron position and activity
        // float x = neuron->getPosition().x;
        // float y = neuron->getPosition().y;
        // float activity = neuron->getCurrentActivity();
        // 
        // int gridX = static_cast<int>(x / (pImpl->config.windowWidth / gridSize));
        // int gridY = static_cast<int>((1.0f - y / pImpl->config.windowHeight) * gridSize);
        // 
        // if (gridX >= 0 && gridX < gridSize && gridY >= 0 && gridY < gridSize) {
        //     heatmap[gridY][gridX] = std::max(heatmap[gridY][gridX], activity);
        // }
    }
    
    // Draw heatmap as colored rectangles
    // In real implementation:
    // for (int y = 0; y < gridSize; ++y) {
    //     for (int x = 0; x < gridSize; ++x) {
    //         float intensity = heatmap[y][gridSize - 1 - y]; // Invert Y axis
    //         drawRectangle(x * (pImpl->config.windowWidth / gridSize),
    //                      y * (pImpl->config.windowHeight / gridSize),
    //                      pImpl->config.windowWidth / gridSize,
    //                      pImpl->config.windowHeight / gridSize,
    //                      getHeatmapColor(intensity));
    //     }
    // }
}

void VisualizationInterface::drawSpikeRaster(const std::vector<SpikeEvent>& spikes) {
    if (spikes.empty()) return;
    
    // Find time range
    Timestamp minTime = spikes.front().timestamp;
    Timestamp maxTime = spikes.back().timestamp;
    
    // Draw spike raster plot
    // In real implementation:
    // int plotHeight = pImpl->config.windowHeight / 3;
    // int plotY = pImpl->config.windowHeight - plotHeight;
    // 
    // drawRectangle(0, plotY, pImpl->config.windowWidth, plotHeight, 0.1f, 0.1f, 0.1f, 0.8f); // Background
    // 
    // for (const auto& spike : spikes) {
    //     float normalizedTime = (spike.timestamp - minTime) / (maxTime - minTime);
    //     int x = static_cast<int>(normalizedTime * pImpl->config.windowWidth);
    //     
    //     // Draw spike as vertical line
    //     drawLine(x, plotY, x, plotY + plotHeight, 0.0f, 1.0f, 0.0f, 0.8f);
    // }
    
    // Draw axes
    // drawText(0, plotY - 10, "Time");
    // drawText(0, plotY - 30, "Step: " + std::to_string(spikes.back().step));
}

void VisualizationInterface::drawWeightMatrix(const Brain& brain) {
    // Draw synaptic weight matrix visualization
    size_t neuronCount = brain.getTotalNeuronCount();
    
    if (neuronCount == 0) return;
    
    // Simplified weight matrix visualization
    // In real implementation, this would draw a heat map of connection weights
    
    int matrixSize = static_cast<int>(std::sqrt(neuronCount)) + 1;
    int cellSize = std::max(2, pImpl->config.windowWidth / matrixSize);
    
    // Draw sub-sampled weight matrix
    // for (int i = 0; i < matrixSize && i * matrixSize < static_cast<int>(neuronCount); ++i) {
    //     for (int j = 0; j < matrixSize && j * matrixSize < static_cast<int>(neuronCount); ++j) {
    //         size_t idx1 = i * matrixSize;
    //         size_t idx2 = j * matrixSize;
    //         
    //         // Get average weight between neuron groups
    //         // float weight = calculateAverageWeightBetweenRegions(idx1, idx2);
    //         float normalizedWeight = (weight + 1.0f) * 0.5f; // Map from [-1, 1] to [0, 1]
    //         
    //         // Draw cell
    //         int x = i * cellSize;
    //         int y = j * cellSize;
    //         
    //         float r = normalizedWeight;
    //         float g = 1.0f - normalizedWeight;
    //         float b = 0.5f;
    //         
    //         drawRectangle(x, y, cellSize, cellSize, r, g, b, 0.9f);
    //     }
    // }
}

void VisualizationInterface::drawMemoryVisualization(const Brain& brain) {
    // Draw memory system state visualization
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    auto* proceduralMemory = brain.getProceduralMemory();
    
    // Create memory system bar chart
    int chartWidth = pImpl->config.windowWidth / 4;
    int chartHeight = 200;
    int chartY = pImpl->config.windowHeight - chartHeight - 50;
    
    std::vector<std::pair<std::string, double>> memoryStats;
    
    if (workingMemory) {
        memoryStats.push_back({"Working", static_cast<double>(workingMemory->getActiveTraces()) / workingMemory->getCapacity()});
    }
    if (episodicMemory) {
        memoryStats.push_back({"Episodic", static_cast<double>(episodicMemory->getEpisodeCount()) / episodicMemory->getMaxEpisodes()});
    }
    if (associativeMemory) {
        memoryStats.push_back({"Associative", static_cast<double>(associativeMemory->getPatternCount()) / 1000.0}); // Normalized
    }
    if (proceduralMemory) {
        memoryStats.push_back({"Procedural", static_cast<double>(proceduralMemory->getSkillCount()) / 100.0}); // Normalized
    }
    
    // Draw memory bars
    // for (size_t i = 0; i < memoryStats.size(); ++i) {
    //     float barWidth = static_cast<float>(chartWidth / memoryStats.size());
    //     float barHeight = memoryStats[i].second * (chartHeight - 20);
    //     float x = static_cast<float>(i * chartWidth);
    //     float y = static_cast<float>(chartY + chartHeight - barHeight);
    //     
    //     // Color based on memory system type
    //     float r, g, b;
    //     if (memoryStats[i].first == "Working") { r = 0.2f; g = 0.6f; b = 1.0f; }
    //     else if (memoryStats[i].first == "Episodic") { r = 1.0f; g = 0.6f; b = 0.2f; }
    //     else if (memoryStats[i].first == "Associative") { r = 0.6f; g = 1.0f; b = 0.2f; }
    //     else { r = 0.8f; g = 0.2f; b = 0.8f; }
    //     
    //     drawRectangle(x, y, barWidth, barHeight, r, g, b, 0.8f);
    //     
    //     // Draw label
    //     drawText(x + barWidth / 2, chartY + chartHeight + 15, memoryStats[i].first);
    // }
}

void VisualizationInterface::drawNeuromodulationDisplay(const Brain& brain) {
    // Draw neuromodulator level visualization
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    // Create circular gauge displays
    int gaugeRadius = 80;
    int startX = 100;
    int startY = pImpl->config.windowHeight - 150;
    
    std::vector<std::pair<std::string, double>> neuromodulators = {
        {"Dopamine", dopamine ? dopamine->getLevel() : 0.0},
        {"Curiosity", curiosity ? curiosity->getLevel() : 0.0},
        {"Novelty", novelty ? novelty->getLevel() : 0.0},
        {"Error", predictionError ? predictionError->getLevel() : 0.0}
    };
    
    // Draw gauges
    for (size_t i = 0; i < neuromodulators.size(); ++i) {
        int centerX = startX + i * (gaugeRadius * 2.5);
        int centerY = startY + gaugeRadius;
        
        // Draw gauge circle
        // drawCircle(centerX, centerY, gaugeRadius, 0.1f, 0.1f, 0.1f, 0.5f);
        
        // Draw value arc
        double angle = neuromodulators[i].second * 3.14159f; // 0 to pi
        // drawArc(centerX, centerY, gaugeRadius * 0.7f, gaugeRadius * 0.7f,
        //         3.14159f, 3.14159f - angle, 0.2f, 1.0f, 0.0f);
        
        // Draw value text
        // drawText(centerX - 15, centerY + gaugeRadius + 15, neuromodulators[i].first);
        // drawText(centerX - 20, centerY + 5, std::to_string(static_cast<int>(neuromodulators[i].second * 100)) + "%");
    }
}

void VisualizationInterface::drawDevelopmentVisualization(const Brain& brain) {
    // Draw development stage visualization
    DevelopmentalStage stage = brain.getDevelopmentalStage();
    
    // Create developmental stage indicator
    int indicatorSize = 200;
    int indicatorX = pImpl->config.windowWidth - indicatorSize - 50;
    int indicatorY = pImpl->config.windowHeight - indicatorSize - 50;
    
    // Draw development stage circle with segmented regions
    std::vector<std::pair<DevelopmentalStage, std::string>> stages = {
        {DevelopmentalStage::Initial, "Initial"},
        {DevelopmentalStage::CriticalPeriod, "Critical"},
        {DevelopmentalStage::Maturation, "Maturation"},
        {DevelopmentalStage::Adult, "Adult"},
        {DevelopmentalStage::Aging, "Aging"}
    };
    
    float angleStep = 2.0f * 3.14159f / stages.size();
    float currentAngle = 0.0f;
    
    for (size_t i = 0; i < stages.size(); ++i) {
        float endAngle = currentAngle + angleStep;
        bool isActive = stages[i].first == stage;
        
        // Draw segment
        // float r1 = gaugeRadius * 0.8f;
        // float r2 = gaugeRadius * 1.0f;
        // drawSector(indicatorX + indicatorSize / 2, indicatorY + indicatorSize / 2,
        //           r1, r2, currentAngle, endAngle, getStageColor(stages[i].first));
        
        // Draw label
        float labelX = indicatorX + indicatorSize / 2 + std::cos(currentAngle + angleStep / 2) * (indicatorSize * 0.6f);
        float labelY = indicatorY + indicatorSize / 2 + std::sin(currentAngle + angleStep / 2) * (indicatorSize * 0.6f);
        
        // drawText(labelX - 20, labelY - 5, stages[i].second);
        
        currentAngle = endAngle;
    }
    
    // Draw center text
    // drawText(indicatorX + indicatorSize / 2 - 30, indicatorY + indicatorSize / 2 + 10,
    //          "Stage: " + std::to_string(static_cast<int>(stage)));
}

void VisualizationInterface::drawCheckpointVisualization() {
    // Draw checkpoint progress visualization
    int chartWidth = pImpl->config.windowWidth - 100;
    int chartHeight = 100;
    int chartX = 50;
    int chartY = pImpl->config.windowHeight - chartHeight - 50;
    
    // Draw progress bar background
    // drawRectangle(chartX, chartY, chartWidth, chartHeight, 0.2f, 0.2f, 0.2f, 0.8f);
    
    // Draw progress fill (simplified)
    // float progress = calculateCheckpointProgress();
    // drawRectangle(chartX, chartY, chartWidth * progress, chartHeight,
    //             0.2f, 0.8f, 0.2f, 0.9f);
    
    // Draw text
    // drawText(chartX + 10, chartY + chartHeight / 2 + 5, "Checkpoints: " + std::to_string(getCheckpointCount()));
    // drawText(chartX + 10, chartY + chartHeight / 2 + 20, "Progress: " + std::to_string(static_cast<int>(progress * 100)) + "%");
}

void VisualizationInterface::drawExperimentVisualization() {
    // Draw experiment progress visualization
    if (!pImpl->brain) return;
    
    // Calculate experiment statistics
    size_t totalNeurons = pImpl->brain->getTotalNeuronCount();
    size_t totalSynapses = pImpl->brain->getTotalSynapseCount();
    float avgFiringRate = pImpl->brain->getAverageFiringRate();
    size_t totalSpikes = pImpl->brain->getTotalSpikeCount();
    
    // Draw experiment metrics display
    int panelWidth = 300;
    int panelHeight = 200;
    int panelX = pImpl->config.windowWidth - panelWidth - 20;
    int panelY = 20;
    
    // Draw panel background
    // drawRectangle(panelX, panelY, panelWidth, panelHeight, 0.1f, 0.1f, 0.2f, 0.8f);
    
    // Draw metrics (simplified)
    // drawText(panelX + 10, panelY + 30, "EXPERIMENT METRICS");
    // drawText(panelX + 10, panelY + 60, "Neurons: " + std::to_string(totalNeurons));
    // drawText(panelX + 10, panelY + 90, "Synapses: " + std::to_string(totalSynapses));
    // drawText(panelX + 10, panelY + 120, "Firing Rate: " + std::to_string(avgFiringRate));
    // drawText(panelX + 10, panelY + 150, "Total Spikes: " + std::to_string(totalSpikes));
}

void VisualizationInterface::drawGrid() {
    // Draw grid lines (simplified)
    if (!pImpl->config.showGrid) return;
    
    // Draw horizontal lines
    // for (int y = 0; y <= pImpl->config.windowHeight; y += 50) {
    //     drawLine(0, y, pImpl->config.windowWidth, y, 0.3f, 0.3f, 0.3f, 0.3f);
    // }
    
    // Draw vertical lines
    // for (int x = 0; x <= pImpl->config.windowWidth; x += 50) {
    //     drawLine(x, 0, x, pImpl->config.windowHeight, 0.3f, 0.3f, 0.3f, 0.3f);
    // }
}

void VisualizationInterface::drawLabels() {
    if (!pImpl->config.showLabels) return;
    
    // Draw basic labels (simplified)
    // drawText(10, 20, "NLM Brain Visualization");
    // drawText(10, 40, "FPS: " + std::to_string(static_cast<int>(pImpl->fps)));
    // drawText(10, 60, "Update Rate: " + std::to_string(pImpl->updateRate) + " Hz");
}

void VisualizationInterface::drawColorLegend() {
    // Draw color legend (simplified)
    int legendX = 20;
    int legendY = pImpl->config.windowHeight - 150;
    int boxSize = 20;
    
    // Draw legend background
    // drawRectangle(legendX, legendY, 200, 130, 0.0f, 0.0f, 0.0f, 0.7f);
    
    // Draw color boxes and labels
    // drawText(legendX + 5, legendY + 15, "Neuron Types:");
    // drawRectangle(legendX + 5, legendY + 30, boxSize, boxSize, 0.2f, 0.6f, 1.0f, 0.8f); // Excitatory
    // drawText(legendX + 15, legendY + 45, "Excitatory");
    // drawRectangle(legendX + 5, legendY + 60, boxSize, boxSize, 1.0f, 0.2f, 0.2f, 0.8f); // Inhibitory
    // drawText(legendX + 15, legendY + 75, "Inhibitory");
    // drawRectangle(legendX + 5, legendY + 90, boxSize, boxSize, 0.6f, 1.0f, 0.2f, 0.8f); // Internal
    // drawText(legendX + 15, legendY + 105, "Internal");
}

void VisualizationInterface::drawStatisticsPanel() {
    // Draw statistics panel (simplified)
    int panelX = pImpl->config.windowWidth - 250;
    int panelY = pImpl->config.windowHeight - 250;
    int panelWidth = 230;
    int panelHeight = 220;
    
    // Draw panel background
    // drawRectangle(panelX, panelY, panelWidth, panelHeight, 0.0f, 0.0f, 0.0f, 0.7f);
    
    // Draw statistics text
    // drawText(panelX + 10, panelY + 20, "STATISTICS");
    // drawText(panelX + 10, panelY + 40, "Step: " + std::to_string(pImpl->frameCount));
    // drawText(panelX + 10, panelY + 60, "FPS: " + std::to_string(static_cast<int>(pImpl->fps)));
    // drawText(panelX + 10, panelY + 80, "Frame Time: " + std::to_string(static_cast<int>(pImpl->frameTime)) + " ms");
    // drawText(panelX + 10, panelY + 100, "Update Time: " + std::to_string(static_cast<int>(pImpl->updateTime)) + " ms");
    // drawText(panelX + 10, panelY + 120, "Memory Score: " + std::to_string(static_cast<int>(calculateStabilityScore())));
    
    // Draw anomalies if any
    std::vector<std::string> anomalies = detectAnomalies();
    if (!anomalies.empty()) {
        drawText(panelX + 10, panelY + 140, "ANOMALIES:");
        for (size_t i = 0; i < anomalies.size() && i < 3; ++i) {
            drawText(panelX + 20, panelY + 160 + i * 15, "- " + anomalies[i]);
        }
    }
}

void VisualizationInterface::drawControlPanel() {
    // Draw control panel (simplified)
    int panelX = 20;
    int panelY = pImpl->config.windowHeight - 150;
    int buttonHeight = 25;
    
    // Draw panel background
    // drawRectangle(panelX, panelY, 200, 130, 0.0f, 0.0f, 0.0f, 0.7f);
    
    // Draw control buttons (simplified)
    // drawText(panelX + 10, panelY + 20, "CONTROLS:");
    // drawRectangle(panelX + 10, panelY + 40, 180, buttonHeight, 0.3f, 0.6f, 1.0f, 0.8f);
    // drawText(panelX + 50, panelY + 55, "Space: Pause/Resume");
    
    // drawRectangle(panelX + 10, panelY + 75, 180, buttonHeight, 0.6f, 1.0f, 0.2f, 0.8f);
    // drawText(panelX + 50, panelY + 90, "R: Reset View");
    
    // drawRectangle(panelX + 10, panelY + 110, 180, buttonHeight, 0.8f, 0.2f, 0.2f, 0.8f);
    // drawText(panelX + 50, panelY + 125, "Q: Quit");
}

void VisualizationInterface::saveScreenshot(const std::string& filename) {
    // Save screenshot (simplified - would capture framebuffer in real implementation)
    NLM_LOG_INFO("Screenshot saved: " + filename);
}

void VisualizationInterface::saveAnimationFrame(int frame) {
    // Save animation frame (simplified)
    std::string frameFile = pImpl->currentAnimationPath + "/frame_" + 
        std::to_string(frame) + ".png";
    
    NLM_LOG_INFO("Animation frame saved: " + frameFile);
}

void VisualizationInterface::toggleVisualizationMode(VisualizationMode mode) {
    if (pImpl->enabledModes.find(mode) != pImpl->enabledModes.end()) {
        pImpl->enabledModes.erase(mode);
        NLM_LOG_INFO("Disabled visualization mode: " + std::to_string(static_cast<int>(mode)));
    } else {
        pImpl->enabledModes.insert(mode);
        NLM_LOG_INFO("Enabled visualization mode: " + std::to_string(static_cast<int>(mode)));
    }
}

std::string VisualizationInterface::findElementAtPosition(int x, int y) {
    // Find element at screen position (simplified)
    // In real implementation, would check collision with drawn elements
    return "";
}

double VisualizationInterface::calculateRegionActivity(const NeuralRegion* region) const {
    if (!region) return 0.0;
    
    double totalActivity = 0.0;
    size_t neuronCount = region->getTotalNeuronCount();
    
    for (const auto* neuron : region->getAllNeurons()) {
        totalActivity += neuron->getCurrentActivity();
    }
    
    return (neuronCount > 0) ? totalActivity / neuronCount : 0.0;
}

float VisualizationInterface::getStageColor(DevelopmentalStage stage) const {
    // Get color for developmental stage
    switch (stage) {
        case DevelopmentalStage::Initial:   return 0.2f; // Dark blue
        case DevelopmentalStage::CriticalPeriod: return 0.8f; // Bright blue
        case DevelopmentalStage::Maturation: return 0.6f; // Medium blue
        case DevelopmentalStage::Adult:     return 0.4f; // Light blue
        case DevelopmentalStage::Aging:     return 0.1f; // Pale blue
        default: return 0.0f;
    }
}

double VisualizationInterface::calculateSpikeDensity(const std::vector<SpikeEvent>& spikes) const {
    if (spikes.size() < 2) return 0.0;
    
    double totalTime = spikes.back().timestamp - spikes.front().timestamp;
    if (totalTime <= 0.0) return 0.0;
    
    return static_cast<double>(spikes.size()) / totalTime;
}

double VisualizationInterface::calculateStdDev(const std::vector<std::vector<double>>& data, 
                                              size_t row, size_t col) const {
    if (data.empty() || row >= data.size() || col >= data[0].size()) return 0.0;
    
    double sum = 0.0, sumSquares = 0.0;
    size_t count = 0;
    
    for (const auto& rowData : data) {
        if (col < rowData.size()) {
            sum += rowData[col];
            sumSquares += rowData[col] * rowData[col];
            count++;
        }
    }
    
    if (count < 2) return 0.0;
    
    double mean = sum / count;
    double variance = (sumSquares / count) - (mean * mean);
    
    return (variance > 0.0) ? std::sqrt(variance) : 0.0;
}

double VisualizationInterface::calculateMean(const std::vector<std::vector<double>>& data, 
                                            size_t startRow, size_t col) const {
    if (data.empty() || startRow >= data.size() || col >= data[0].size()) return 0.0;
    
    double sum = 0.0;
    size_t count = 0;
    
    for (size_t i = startRow; i < data.size() && i < startRow + 10; ++i) {
        if (col < data[i].size()) {
            sum += data[i][col];
            count++;
        }
    }
    
    return (count > 0) ? sum / count : 0.0;
}

double VisualizationInterface::calculateMovingAverage(const std::vector<std::vector<double>>& data, 
                                                     size_t startRow, size_t col, 
                                                     size_t windowSize) const {
    if (data.empty() || startRow >= data.size() || col >= data[0].size()) return 0.0;
    
    double sum = 0.0;
    size_t count = 0;
    
    for (size_t i = startRow; i < startRow + windowSize && i < data.size(); ++i) {
        if (col < data[i].size()) {
            sum += data[i][col];
            count++;
        }
    }
    
    return (count > 0) ? sum / count : 0.0;
}

void VisualizationInterface::recordTimeSeries(const std::string& name, const std::vector<double>& values) {
    if (!values.empty()) {
        pImpl->currentMetrics[name] = values.back();
        pImpl->historicalMetrics[name].insert(pImpl->historicalMetrics[name].end(), 
                                               values.begin(), values.end());
        
        // Keep only recent data
        if (pImpl->historicalMetrics[name].size() > 1000) {
            pImpl->historicalMetrics[name].erase(
                pImpl->historicalMetrics[name].begin(),
                pImpl->historicalMetrics[name].begin() + 
                (pImpl->historicalMetrics[name].size() - 1000)
            );
        }
    }
}

double VisualizationInterface::calculateCheckpointProgress() const {
    // Calculate checkpoint progress (simplified)
    return 0.5; // Placeholder
}

size_t VisualizationInterface::getCheckpointCount() const {
    // Get checkpoint count (simplified)
    return 0; // Placeholder
}

} // namespace nlm
