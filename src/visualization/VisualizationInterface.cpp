#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cmath>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    VisualizationBackend backend;
    VisualizationFormat format;
    std::string outputFile;
    bool enableColor;
    bool enableAnimations;
    size_t maxHistoryPoints;
    std::map<std::string, std::string> parameters;
    
    // Performance tracking
    double lastFrameTime;
    size_t totalFramesRendered;
    
    // Visualization data
    std::vector<std::function<void()>> batchUpdates;
    std::function<void()> customRenderer;
    
    // Output buffers
    std::stringstream textBuffer;
    std::ofstream fileOutput;
    
    // Timing and animation
    double animationTime;
    double animationFrame;
    
    Impl() : active(false), updateRate(30.0), backend(VisualizationBackend::ASCII_CONSOLE),
             format(VisualizationFormat::ASCII_ART), enableColor(true), 
             enableAnimations(false), maxHistoryPoints(1000), 
             lastFrameTime(0.0), totalFramesRendered(0), animationTime(0.0), 
             animationFrame(0.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() {
    close();
}

bool VisualizationInterface::initialize(VisualizationBackend backend) {
    pImpl->backend = backend;
    pImpl->active = true;
    
    // Initialize output based on backend
    switch (pImpl->backend) {
        case VisualizationBackend::ASCII_CONSOLE:
            pImpl->enableColor = true;
            pImpl->format = VisualizationFormat::ASCII_ART;
            break;
            
        case VisualizationBackend::FILE_OUTPUT:
            if (!pImpl->outputFile.empty()) {
                pImpl->fileOutput.open(pImpl->outputFile);
                if (!pImpl->fileOutput.is_open()) {
                    NLM_LOG_ERROR("Failed to open visualization output file: " + pImpl->outputFile);
                    return false;
                }
            }
            break;
            
        case VisualizationBackend::WEB_SOCKET:
            // Would initialize web socket connection
            break;
            
        case VisualizationBackend::DISPLAY_WINDOW:
            // Would initialize display window (e.g., GLFW, SDL)
            break;
            
        case VisualizationBackend::NONE:
            pImpl->active = false;
            break;
    }
    
    NLM_LOG_INFO("Visualization interface initialized with backend: " + std::to_string(static_cast<int>(pImpl->backend)));
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    double startTime = time.time();
    
    // Process batch updates
    if (!pImpl->batchUpdates.empty()) {
        processBatchUpdates();
    }
    
    // Update animation time
    if (pImpl->enableAnimations) {
        pImpl->animationTime += 1.0 / pImpl->updateRate;
        pImpl->animationFrame = std::fmod(pImpl->animationTime, 360.0);
    }
    
    pImpl->lastFrameTime = time.time() - startTime;
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Clear output buffer
    pImpl->textBuffer.str("");
    pImpl->textBuffer.clear();
    
    // Render based on format
    switch (pImpl->format) {
        case VisualizationFormat::TEXT:
            renderText();
            break;
            
        case VisualizationFormat::ASCII_ART:
            renderAsciiArt();
            break;
            
        case VisualizationFormat::JSON:
            renderJson();
            break;
            
        case VisualizationFormat::CSV:
            renderCsv();
            break;
            
        case VisualizationFormat::HTML:
            renderHtml();
            break;
            
        case VisualizationFormat::SVG:
            renderSvg();
            break;
            
        case VisualizationFormat::PNG:
            // Would render PNG image
            renderText(); // Placeholder
            break;
            
        case VisualizationFormat::MATLAB:
            renderMatlab();
            break;
            
        default:
            renderText();
            break;
    }
    
    // Output to configured destination
    switch (pImpl->backend) {
        case VisualizationBackend::ASCII_CONSOLE:
            std::cout << pImpl->textBuffer.str();
            break;
            
        case VisualizationBackend::FILE_OUTPUT:
            if (pImpl->fileOutput.is_open()) {
                pImpl->fileOutput << pImpl->textBuffer.str();
            }
            break;
            
        case VisualizationBackend::WEB_SOCKET:
            // Would send data via web socket
            break;
            
        case VisualizationBackend::DISPLAY_WINDOW:
            // Would update display window
            break;
            
        case VisualizationBackend::NONE:
            break;
    }
    
    pImpl->totalFramesRendered++;
}

void VisualizationInterface::close() {
    if (pImpl->fileOutput.is_open()) {
        pImpl->fileOutput.close();
    }
    
    pImpl->active = false;
    pImpl->batchUpdates.clear();
    pImpl->customRenderer = nullptr;
    
    NLM_LOG_INFO("Visualization interface closed");
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setOutputFormat(VisualizationFormat format) {
    pImpl->format = format;
}

VisualizationFormat VisualizationInterface::getOutputFormat() const {
    return pImpl->format;
}

void VisualizationInterface::setOutputFile(const std::string& filepath) {
    pImpl->outputFile = filepath;
    if (pImpl->backend == VisualizationBackend::FILE_OUTPUT && pImpl->fileOutput.is_open()) {
        pImpl->fileOutput.close();
    }
    if (pImpl->backend == VisualizationBackend::FILE_OUTPUT) {
        pImpl->fileOutput.open(filepath);
    }
}

std::string VisualizationInterface::getOutputFile() const {
    return pImpl->outputFile;
}

void VisualizationInterface::setConfig(const VisualizationConfig& config) {
    pImpl->backend = config.backend;
    pImpl->format = config.format;
    pImpl->outputFile = config.outputFile;
    pImpl->updateRate = config.updateRate;
    pImpl->enableColor = config.enableColor;
    pImpl->enableAnimations = config.enableAnimations;
    pImpl->maxHistoryPoints = config.maxHistoryPoints;
    pImpl->parameters = config.parameters;
    
    // Reinitialize if active
    if (pImpl->active) {
        close();
        initialize(pImpl->backend);
    }
}

VisualizationInterface::VisualizationConfig VisualizationInterface::getConfig() const {
    VisualizationConfig config;
    config.backend = pImpl->backend;
    config.format = pImpl->format;
    config.outputFile = pImpl->outputFile;
    config.updateRate = pImpl->updateRate;
    config.enableColor = pImpl->enableColor;
    config.enableAnimations = pImpl->enableAnimations;
    config.maxHistoryPoints = pImpl->maxHistoryPoints;
    config.parameters = pImpl->parameters;
    return config;
}

double VisualizationInterface::getLastFrameTime() const {
    return pImpl->lastFrameTime;
}

size_t VisualizationInterface::getTotalFramesRendered() const {
    return pImpl->totalFramesRendered;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    addBatchUpdate([&brain, this]() {
        // Network visualization logic
        pImpl->textBuffer << "=== Network Visualization ===\n";
        pImpl->textBuffer << "Neurons: " << brain.getNeuronCount() << "\n";
        pImpl->textBuffer << "Synapses: " << brain.getSynapseCount() << "\n";
        pImpl->textBuffer << "Regions: " << brain.getRegionCount() << "\n";
        pImpl->textBuffer << "\n";
    });
}

void VisualizationInterface::visualizeActivity(const Brain& brain, 
                                              bool showNeurons,
                                              bool showConnections,
                                              bool showSpikes) {
    addBatchUpdate([&brain, this, showNeurons, showConnections, showSpikes]() {
        pImpl->textBuffer << "=== Activity Visualization ===\n";
        
        if (showNeurons) {
            pImpl->textBuffer << "Active Neurons: ";
            int activeCount = 0;
            for (size_t i = 0; i < brain.getNeuronCount(); ++i) {
                const Neuron* neuron = brain.getNeuron(i);
                if (neuron && neuron->checkThreshold()) {
                    activeCount++;
                }
            }
            pImpl->textBuffer << activeCount << "\n";
        }
        
        if (showConnections) {
            pImpl->textBuffer << "Total Connections: ";
            size_t totalConnections = 0;
            for (size_t i = 0; i < brain.getSynapseCount(); ++i) {
                const Synapse* synapse = brain.getSynapse(i);
                if (synapse) {
                    totalConnections++;
                }
            }
            pImpl->textBuffer << totalConnections << "\n";
        }
        
        if (showSpikes) {
            pImpl->textBuffer << "Recent Spikes: " << brain.getTotalSpikeCount() << "\n";
        }
        
        pImpl->textBuffer << "\n";
    });
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes,
                                            bool showStatistics,
                                            double timeWindow) {
    addBatchUpdate([spikes, this, showStatistics, timeWindow]() {
        pImpl->textBuffer << "=== Spike Raster Plot ===\n";
        
        if (showStatistics) {
            pImpl->textBuffer << "Statistics:\n";
            pImpl->textBuffer << "  Total spikes: " << spikes.size() << "\n";
            
            if (!spikes.empty()) {
                double maxTime = spikes.back().timestamp;
                double minTime = spikes.front().timestamp;
                pImpl->textBuffer << "  Time window: " << (maxTime - minTime) << "ms\n";
                
                double rate = (spikes.size() / (maxTime - minTime)) * 1000.0;
                pImpl->textBuffer << "  Firing rate: " << rate << " Hz\n";
                
                // Simple raster plot for recent spikes
                pImpl->textBuffer << "  Recent spikes (last " << timeWindow << "ms):\n";
                size_t recentCount = 0;
                for (const auto& spike : spikes) {
                    if (spike.timestamp >= maxTime - timeWindow) {
                        pImpl->textBuffer << "    " << spike.neuronId << " @ " << spike.timestamp << "ms\n";
                        recentCount++;
                    }
                }
                pImpl->textBuffer << "    (showing " << recentCount << " spikes)\n";
            }
        }
        
        pImpl->textBuffer << "\n";
    });
}

void VisualizationInterface::visualizeWeights(const Brain& brain,
                                            bool showPositive,
                                            bool showNegative,
                                            double minWeight,
                                            double maxWeight) {
    addBatchUpdate([&brain, this, showPositive, showNegative, minWeight, maxWeight]() {
        pImpl->textBuffer << "=== Weight Matrix Visualization ===\n";
        
        std::vector<float> positiveWeights;
        std::vector<float> negativeWeights;
        
        for (size_t i = 0; i < brain.getSynapseCount(); ++i) {
            const Synapse* synapse = brain.getSynapse(i);
            if (synapse) {
                float weight = synapse->getWeight();
                if (weight > 0 && showPositive) {
                    positiveWeights.push_back(weight);
                } else if (weight < 0 && showNegative) {
                    negativeWeights.push_back(weight);
                }
            }
        }
        
        if (!positiveWeights.empty()) {
            pImpl->textBuffer << "Positive weights (" << positiveWeights.size() << "):\n";
            pImpl->textBuffer << "  Min: " << *std::min_element(positiveWeights.begin(), positiveWeights.end()) << "\n";
            pImpl->textBuffer << "  Max: " << *std::max_element(positiveWeights.begin(), positiveWeights.end()) << "\n";
            pImpl->textBuffer << "  Mean: " << std::accumulate(positiveWeights.begin(), positiveWeights.end(), 0.0) / positiveWeights.size() << "\n";
            
            // Simple histogram (text-based)
            pImpl->textBuffer << "  Histogram (normalized):\n";
            for (float weight : positiveWeights) {
                int barWidth = static_cast<int>((weight - minWeight) / (maxWeight - minWeight) * 50.0);
                pImpl->textBuffer << "    " << std::string(barWidth, '=') << " " << weight << "\n";
            }
        }
        
        if (!negativeWeights.empty()) {
            pImpl->textBuffer << "\nNegative weights (" << negativeWeights.size() << "):\n";
            pImpl->textBuffer << "  Min: " << *std::min_element(negativeWeights.begin(), negativeWeights.end()) << "\n";
            pImpl->textBuffer << "  Max: " << *std::max_element(negativeWeights.begin(), negativeWeights.end()) << "\n";
            pImpl->textBuffer << "  Mean: " << std::accumulate(negativeWeights.begin(), negativeWeights.end(), 0.0) / negativeWeights.size() << "\n";
        }
        
        pImpl->textBuffer << "\n";
    });
}

void VisualizationInterface::visualizeBrainLayers(const Brain& brain) {
    addBatchUpdate([&brain, this]() {
        pImpl->textBuffer << "=== Brain Layers Visualization ===\n";
        
        for (size_t regionId = 0; regionId < brain.getRegionCount(); ++regionId) {
            pImpl->textBuffer << "Region " << regionId << ":\n";
            
            const NeuralRegion* region = brain.getRegion(regionId);
            if (region) {
                pImpl->textBuffer << "  Neurons: " << region->getNeuronCount() << "\n";
                pImpl->textBuffer << "  Population count: " << region->getPopulationCount() << "\n";
                
                for (size_t popId = 0; popId < region->getPopulationCount(); ++popId) {
                    const NeuralPopulation* population = region->getPopulation(popId);
                    if (population) {
                        pImpl->textBuffer << "    Population " << popId << ": " << population->getNeuronCount() << " neurons\n";
                    }
                }
            }
            
            pImpl->textBuffer << "\n";
        }
        
        pImpl->textBuffer << "\n";
    });
}

void VisualizationInterface::visualizeActivityHeatmap(const Brain& brain,
                                                     const std::string& metric) {
    addBatchUpdate([&brain, this, metric]() {
        pImpl->textBuffer << "=== Activity Heatmap ===\n";
        pImpl->textBuffer << "Metric: " << metric << "\n";
        
        // Simple heatmap representation
        if (pImpl->enableColor) {
            pImpl->textBuffer << "(Color heatmap not available in text mode)\n";
        } else {
            pImpl->textBuffer << "(Text-based heatmap)\n";
        }
        
        // Placeholder for heatmap data
        pImpl->textBuffer << "Heatmap data would be computed here based on " << metric << "\n";
        pImpl->textBuffer << "This would show neuron activity levels in a 2D representation\n";
        
        pImpl->textBuffer << "\n";
    });
}

void VisualizationInterface::visualizeTimeSeries(const Brain& brain,
                                                const std::string& seriesName,
                                                size_t duration) {
    addBatchUpdate([&brain, this, seriesName, duration]() {
        pImpl->textBuffer << "=== Time Series Visualization ===\n";
        pImpl->textBuffer << "Series: " << seriesName << "\n";
        pImpl->textBuffer << "Duration: " << duration << " steps\n";
        
        // Placeholder for time series data
        pImpl->textBuffer << "Time series data would be plotted here\n";
        pImpl->textBuffer << "This would show " << seriesName << " over time\n";
        
        pImpl->textBuffer << "\n";
    });
}

void VisualizationInterface::setCustomRenderer(std::function<void()> renderer) {
    pImpl->customRenderer = renderer;
}

void VisualizationInterface::addBatchUpdate(std::function<void()> updateFunc) {
    pImpl->batchUpdates.push_back(updateFunc);
}

void VisualizationInterface::processBatchUpdates() {
    for (auto& update : pImpl->batchUpdates) {
        try {
            update();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Batch update error: " + std::string(e.what()));
        }
    }
    pImpl->batchUpdates.clear();
}

void VisualizationInterface::exportVisualization(const std::string& filepath) {
    // Export current visualization to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << pImpl->textBuffer.str();
        file.close();
        NLM_LOG_INFO("Visualization exported to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to export visualization to: " + filepath);
    }
}

// Helper rendering methods
void VisualizationInterface::renderText() {
    pImpl->textBuffer << "=== NLM Neural Simulation Visualization ===\n";
    pImpl->textBuffer << "Backend: " << static_cast<int>(pImpl->backend) << "\n";
    pImpl->textBuffer << "Format: " << static_cast<int>(pImpl->format) << "\n";
    pImpl->textBuffer << "Update Rate: " << pImpl->updateRate << " Hz\n";
    pImpl->textBuffer << "Frame Time: " << pImpl->lastFrameTime << " ms\n";
    pImpl->textBuffer << "Total Frames: " << pImpl->totalFramesRendered << "\n";
    
    if (pImpl->enableColor) {
        pImpl->textBuffer << "Color Output: Enabled\n";
    }
    
    if (pImpl->enableAnimations) {
        pImpl->textBuffer << "Animation Time: " << pImpl->animationTime << " s\n";
    }
    
    pImpl->textBuffer << "\n";
}

void VisualizationInterface::renderAsciiArt() {
    renderText();
    
    // Simple ASCII art representation
    pImpl->textBuffer << "  o     o   \n";
    pImpl->textBuffer << "   |     |   \n";
    pImpl->textBuffer << "  / \___/ \  \n";
    pImpl->textBuffer << " | |     | | \n";
    pImpl->textBuffer << " |_|_____|_| \n";
    pImpl->textBuffer << "  ( )     ( ) \n";
    pImpl->textBuffer << "==============\n\n";
}

void VisualizationInterface::renderJson() {
    // Convert current visualization state to JSON
    // (Would use a JSON library in real implementation)
    pImpl->textBuffer << "{\n";
    pImpl->textBuffer << "  \"visualization_state\": {\n";
    pImpl->textBuffer << "    \"backend\": \"" << static_cast<int>(pImpl->backend) << "\",\n";
    pImpl->textBuffer << "    \"format\": \"" << static_cast<int>(pImpl->format) << "\",\n";
    pImpl->textBuffer << "    \"update_rate\": " << pImpl->updateRate << ",\n";
    pImpl->textBuffer << "    \"active\": " << (pImpl->active ? "true" : "false") << ",\n";
    pImpl->textBuffer << "    \"frame_count\": " << pImpl->totalFramesRendered << "\n";
    pImpl->textBuffer << "  }\n";
    pImpl->textBuffer << "}\n";
}

void VisualizationInterface::renderCsv() {
    pImpl->textBuffer << "backend,format,update_rate,active,frame_count,last_frame_time\n";
    pImpl->textBuffer << static_cast<int>(pImpl->backend) << ",";
    pImpl->textBuffer << static_cast<int>(pImpl->format) << ",";
    pImpl->textBuffer << pImpl->updateRate << ",";
    pImpl->textBuffer << (pImpl->active ? "true" : "false") << ",";
    pImpl->textBuffer << pImpl->totalFramesRendered << ",";
    pImpl->textBuffer << pImpl->lastFrameTime << "\n";
}

void VisualizationInterface::renderHtml() {
    pImpl->textBuffer << "<!DOCTYPE html>\n";
    pImpl->textBuffer << "<html><head><title>NLM Visualization</title></head><body>\n";
    pImpl->textBuffer << "<h1>NLM Neural Simulation Visualization</h1>\n";
    pImpl->textBuffer << "<p>Backend: " << static_cast<int>(pImpl->backend) << "</p>\n";
    pImpl->textBuffer << "<p>Format: " << static_cast<int>(pImpl->format) << "</p>\n";
    pImpl->textBuffer << "<p>Update Rate: " << pImpl->updateRate << " Hz</p>\n";
    pImpl->textBuffer << "<p>Active: " << (pImpl->active ? "true" : "false") << "</p>\n";
    pImpl->textBuffer << "<p>Frame Count: " << pImpl->totalFramesRendered << "</p>\n";
    pImpl->textBuffer << "</body></html>\n";
}

void VisualizationInterface::renderSvg() {
    pImpl->textBuffer << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    pImpl->textBuffer << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";
    pImpl->textBuffer << "  <rect width=\"800\" height=\"600\" fill=\"#f0f0f0\"/>\n";
    pImpl->textBuffer << "  <text x=\"10\" y=\"20\" font-family=\"Arial\" font-size=\"16\" fill=\"#000000\">";
    pImpl->textBuffer << "NLM Neural Simulation Visualization</text>\n";
    pImpl->textBuffer << "  <text x=\"10\" y=\"40\" font-family=\"Arial\" font-size=\"12\" fill=\"#000000\">";
    pImpl->textBuffer << "Backend: " << static_cast<int>(pImpl->backend) << "</text>\n";
    pImpl->textBuffer << "</svg>\n";
}

void VisualizationInterface::renderMatlab() {
    pImpl->textBuffer << "% NLM Neural Simulation Visualization Data\n";
    pImpl->textBuffer << "% Generated at: " << time.time() << "\n\n";
    pImpl->textBuffer << "backend = " << static_cast<int>(pImpl->backend) << ";\n";
    pImpl->textBuffer << "format = " << static_cast<int>(pImpl->format) << ";\n";
    pImpl->textBuffer << "update_rate = " << pImpl->updateRate << ";\n";
    pImpl->textBuffer << "active = " << (pImpl->active ? "1" : "0") << ";\n";
    pImpl->textBuffer << "frame_count = " << pImpl->totalFramesRendered << ";\n";
    pImpl->textBuffer << "last_frame_time = " << pImpl->lastFrameTime << ";\n";
}

} // namespace nlm

