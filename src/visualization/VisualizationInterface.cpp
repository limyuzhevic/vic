// Visualization Interface
// Provides basic visualization capabilities for NLM brain state
// Supports text-based output, plotting, and data export

#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace nlm {

VisualizationInterface::VisualizationInterface(std::shared_ptr<Brain> brain, OutputFormat format)
    : brain_(brain), format_(format), width_(80), height_(40) {
}

VisualizationInterface::~VisualizationInterface() = default;

void VisualizationInterface::setDimensions(int width, int height) {
    width_ = std::max(40, width);
    height_ = std::max(20, height);
}

void VisualizationInterface::setOutputFormat(OutputFormat format) {
    format_ = format;
}

void VisualizationInterface::renderBrainState() {
    switch (format_) {
        case OutputFormat::Text:
            renderTextMode();
            break;
        case OutputFormat::CSV:
            renderCSVMode();
            break;
        case OutputFormat::JSON:
            renderJSONMode();
            break;
        case OutputFormat::HTML:
            renderHTMLMode();
            break;
        case OutputFormat::Plot:
            renderPlotMode();
            break;
        case OutputFormat::ASCII:
            renderASCIIArt();
            break;
    }
}

void VisualizationInterface::renderTextMode() {
    std::cout << "=== NLM Brain State Visualization ===" << std::endl;
    std::cout << std::endl;
    
    // Basic statistics
    std::cout << "Statistics:" << std::endl;
    std::cout << "  Regions: " << brain_->getRegionCount() << std::endl;
    std::cout << "  Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    std::cout << "  Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons (this step): " << brain_->getFiringNeuronCount() << std::endl;
    std::cout << "  Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
    std::cout << "  E/I Ratio: " << std::setprecision(3) << brain_->getExcitationInhibitionRatio() << std::endl;
    std::cout << std::endl;
    
    // Regional statistics
    std::cout << "Regional Statistics:" << std::endl;
    for (const auto& region : brain_->getRegions()) {
        std::cout << "  Region " << region->getId().index() << " (" << region->getName() << "):" << std::endl;
        std::cout << "    Neurons: " << region->getTotalNeuronCount() << std::endl;
        std::cout << "    Synapses: " << region->getSynapseCount() << std::endl;
        std::cout << "    Average Weight: " << std::setprecision(4) << region->getAverageSynapticWeight() << std::endl;
        std::cout << "    Average Firing Rate: " << std::setprecision(3) << region->getAverageFiringRate() << std::endl;
    }
    std::cout << std::endl;
    
    // Activity levels
    renderActivityLevels();
    
    // Plasticity information
    renderPlasticityInfo();
}

void VisualizationInterface::renderCSVMode() {
    std::cout << "timestamp,region_id,neuron_count,synapse_count,avg_weight,avg_firing_rate" << std::endl;
    
    size_t timestamp = brain_->getTotalSpikeCount();  // Use spike count as timestamp
    for (const auto& region : brain_->getRegions()) {
        std::cout << timestamp << ","
                  << region->getId().index() << ","
                  << region->getTotalNeuronCount() << ","
                  << region->getSynapseCount() << ","
                  << std::setprecision(6) << region->getAverageSynapticWeight() << ","
                  << std::setprecision(6) << region->getAverageFiringRate()
                  << std::endl;
    }
}

void VisualizationInterface::renderJSONMode() {
    std::cout << "{" << std::endl;
    std::cout << "  \"brain_state\": {" << std::endl;
    std::cout << "    \"timestamp\": " << brain_->getTotalSpikeCount() << "," << std::endl;
    std::cout << "    \"regions\": [" << std::endl;
    
    bool firstRegion = true;
    for (const auto& region : brain_->getRegions()) {
        if (!firstRegion) std::cout << "," << std::endl;
        firstRegion = false;
        
        std::cout << "      {" << std::endl;
        std::cout << "        \"id\": " << region->getId().index() << "," << std::endl;
        std::cout << "        \"name\": \"" << region->getName() << "\"," << std::endl;
        std::cout << "        \"neuron_count\": " << region->getTotalNeuronCount() << "," << std::endl;
        std::cout << "        \"synapse_count\": " << region->getSynapseCount() << "," << std::endl;
        std::cout << "        \"avg_weight\": " << std::setprecision(6) << region->getAverageSynapticWeight() << "," << std::endl;
        std::cout << "        \"avg_firing_rate\": " << std::setprecision(6) << region->getAverageFiringRate() << std::endl;
        std::cout << "      }";
    }
    
    std::cout << std::endl << "    ]," << std::endl;
    std::cout << "    \"global_stats\": {" << std::endl;
    std::cout << "      \"total_spikes\": " << brain_->getTotalSpikeCount() << "," << std::endl;
    std::cout << "      \"active_neurons\": " << brain_->getActiveNeuronCount() << "," << std::endl;
    std::cout << "      \"e_i_ratio\": " << std::setprecision(3) << brain_->getExcitationInhibitionRatio() << std::endl;
    std::cout << "    }" << std::endl;
    std::cout << "  }" << std::endl;
    std::cout << "}" << std::endl;
}

void VisualizationInterface::renderHTMLMode() {
    std::cout << "<!DOCTYPE html>" << std::endl;
    std::cout << "<html>" << std::endl;
    std::cout << "<head>" << std::endl;
    std::cout << "<title>NLM Brain State Visualization</title>" << std::endl;
    std::cout << "<style>" << std::endl;
    std::cout << "body { font-family: monospace; margin: 20px; }" << std::endl;
    std::cout << ".stat { background-color: #f0f0f0; padding: 10px; margin: 5px; }" << std::endl;
    std::cout << ".region { border: 1px solid #ccc; padding: 15px; margin: 10px; }" << std::endl;
    std::cout << "</style>" << std::endl;
    std::cout << "</head>" << std::endl;
    std::cout << "<body>" << std::endl;
    std::cout << "<h1>NLM Brain State Visualization</h1>" << std::endl;
    std::cout << "<div class=\"stat\">" << std::endl;
    std::cout << "<h2>Global Statistics</h2>" << std::endl;
    std::cout << "<p>Regions: " << brain_->getRegionCount() << "</p>" << std::endl;
    std::cout << "<p>Total Neurons: " << brain_->getTotalNeuronCount() << "</p>" << std::endl;
    std::cout << "<p>Total Synapses: " << brain_->getTotalSynapseCount() << "</p>" << std::endl;
    std::cout << "<p>Active Neurons: " << brain_->getActiveNeuronCount() << "</p>" << std::endl;
    std::cout << "<p>Firing Neurons: " << brain_->getFiringNeuronCount() << "</p>" << std::endl;
    std::cout << "<p>Total Spikes: " << brain_->getTotalSpikeCount() << "</p>" << std::endl;
    std::cout << "</div>" << std::endl;
    
    std::cout << "<div>" << std::endl;
    for (const auto& region : brain_->getRegions()) {
        std::cout << "<div class=\"region\">" << std::endl;
        std::cout << "<h3>Region " << region->getId().index() << " (" << region->getName() << ")</h3>" << std::endl;
        std::cout << "<p>Neurons: " << region->getTotalNeuronCount() << "</p>" << std::endl;
        std::cout << "<p>Synapses: " << region->getSynapseCount() << "</p>" << std::endl;
        std::cout << "<p>Average Weight: " << std::setprecision(6) << region->getAverageSynapticWeight() << "</p>" << std::endl;
        std::cout << "<p>Average Firing Rate: " << std::setprecision(6) << region->getAverageFiringRate() << "</p>" << std::endl;
        std::cout << "</div>" << std::endl;
    }
    
    std::cout << "</div>" << std::endl;
    std::cout << "</body>" << std::endl;
    std::cout << "</html>" << std::endl;
}

void VisualizationInterface::renderPlotMode() {
    std::cout << "=== NLM Brain State Plot ===" << std::endl;
    std::cout << "Plotting neural activity and connectivity..." << std::endl;
    std::cout << "(This would generate visual plots in a GUI environment)" << std::endl;
    std::cout << std::endl;
    
    // Create simple ASCII-based activity bars
    renderActivityBars();
    
    // Show connectivity heatmap
    renderConnectivityHeatmap();
}

void VisualizationInterface::renderASCIIArt() {
    std::cout << "=== NLM Brain ASCII Art ===" << std::endl;
    std::cout << std::endl;
    
    // Simple representation
    std::cout << "     NEURAL NETWORK STATE" << std::endl;
    std::cout << "     " << std::string(50, '=') << std::endl;
    
    // Draw a simple neuron representation
    drawNeuron(20, 10, "Input", 0.3f);
    drawNeuron(40, 10, "Hidden", 0.7f);
    drawNeuron(60, 10, "Output", 0.5f);
    
    // Draw connections
    drawConnection(25, 12, 35, 12, 0.6f);
    drawConnection(35, 12, 45, 12, 0.8f);
    drawConnection(45, 12, 55, 12, 0.4f);
    
    std::cout << std::endl;
    std::cout << "Key: Green=High Activity, Red=Low Activity, Blue=Connections" << std::endl;
}

void VisualizationInterface::renderActivityLevels() {
    std::cout << "Activity Levels:" << std::endl;
    
    size_t totalNeurons = brain_->getTotalNeuronCount();
    if (totalNeurons == 0) {
        std::cout << "  No neurons to display" << std::endl;
        return;
    }
    
    // Calculate activity percentages
    float activePercent = (static_cast<float>(brain_->getActiveNeuronCount()) / totalNeurons) * 100.0f;
    float firingPercent = (static_cast<float>(brain_->getFiringNeuronCount()) / totalNeurons) * 100.0f;
    
    // Draw activity bars
    std::cout << "  Active Neurons: [";
    int activeBars = static_cast<int>(activePercent * width_ / 100.0f);
    for (int i = 0; i < activeBars; ++i) std::cout << "#";
    for (int i = activeBars; i < 20; ++i) std::cout << " ";
    std::cout << "] " << std::fixed << std::setprecision(1) << activePercent << "%" << std::endl;
    
    std::cout << "  Firing Neurons:  [";
    int firingBars = static_cast<int>(firingPercent * width_ / 100.0f);
    for (int i = 0; i < firingBars; ++i) std::cout << "@";
    for (int i = firingBars; i < 20; ++i) std::cout << " ";
    std::cout << "] " << std::fixed << std::setprecision(1) << firingPercent << "%" << std::endl;
}

void VisualizationInterface::renderPlasticityInfo() {
    std::cout << "Plasticity Information:" << std::endl;
    
    auto* stdp = brain_->getSTDP();
    auto* hebbian = brain_->getHebbian();
    auto* plasticity = brain_->getStructuralPlasticity();
    
    if (stdp) {
        std::cout << "  STDP: Enabled" << std::endl;
        std::cout << "    LTP Weight: " << std::setprecision(4) << stdp->getLTPWeight() << std::endl;
        std::cout << "    LTD Weight: " << std::setprecision(4) << stdp->getLTDWeight() << std::endl;
        std::cout << "    Tau: " << std::setprecision(2) << stdp->getTau() << std::endl;
    }
    
    if (hebbian) {
        std::cout << "  Hebbian: Enabled" << std::endl;
    }
    
    if (plasticity) {
        std::cout << "  Structural Plasticity: Enabled" << std::endl;
        std::cout << "    Synaptogenesis Rate: " << std::setprecision(6) << plasticity->getSynaptogenesisRate() << std::endl;
        std::cout << "    Pruning Rate: " << std::setprecision(6) << plasticity->getPruningRate() << std::endl;
    }
}

void VisualizationInterface::renderActivityBars() {
    std::cout << "Activity Bar Chart:" << std::endl;
    
    // Create simple bar representation
    for (int i = 0; i < 5; ++i) {
        float activity = static_cast<float>(rand()) / RAND_MAX * 0.8f + 0.1f;
        int barHeight = static_cast<int>(activity * 10);
        std::cout << "  Layer " << i << ": ";
        for (int j = 0; j < barHeight; ++j) std::cout << "*";
        std::cout << " (" << std::fixed << std::setprecision(2) << activity << ")" << std::endl;
    }
}

void VisualizationInterface::renderConnectivityHeatmap() {
    std::cout << "Connectivity Heatmap (simplified):" << std::endl;
    
    // Create a simple 10x10 grid to represent connectivity
    std::vector<std::vector<float>> heatmap(10, std::vector<float>(10, 0.0f));
    
    // Populate with random-like connectivity
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (i != j) {
                heatmap[i][j] = static_cast<float>(rand()) / RAND_MAX * 0.3f;
            }
        }
    }
    
    // Print heatmap
    for (int j = 9; j >= 0; --j) {
        std::cout << "  ";
        for (int i = 0; i < 10; ++i) {
            float value = heatmap[i][j];
            if (value > 0.7f) std::cout << "##";
            else if (value > 0.4f) std::cout << "##";
            else if (value > 0.1f) std::cout << "::";
            else std::cout << "  ";
        }
        std::cout << "  ";
        if (j == 9) std::cout << "(Source)";
        if (j == 0) std::cout << "(Target)";
        std::cout << std::endl;
    }
}

void VisualizationInterface::drawNeuron(int x, int y, const std::string& label, float activity) {
    // Draw neuron
    std::cout << "  ";
    for (int j = y - 2; j <= y + 2; ++j) {
        std::cout << "  ";
        for (int i = x - 8; i <= x + 8; ++i) {
            float dist = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2));
            if (dist <= 8.0f) {
                float intensity = 1.0f - (dist / 8.0f);
                float finalIntensity = intensity * activity;
                
                if (finalIntensity > 0.8f) {
                    std::cout << "█";  // Bright
                } else if (finalIntensity > 0.4f) {
                    std::cout << "▒";  // Medium
                } else if (finalIntensity > 0.2f) {
                    std::cout << "░";  // Dim
                } else {
                    std::cout << "·";  // Very dim
                }
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
    
    // Draw label
    std::cout << "  ";
    for (size_t i = 0; i < label.length(); ++i) {
        std::cout << label[i];
    }
    std::cout << std::endl;
}

void VisualizationInterface::drawConnection(int x1, int y1, int x2, int y2, float strength) {
    // Simple ASCII line drawing
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = std::max(std::abs(dx), std::abs(dy));
    
    if (steps == 0) return;
    
    float x = x1, y = y1;
    for (int i = 0; i < steps; ++i) {
        if (strength > 0.7f) {
            std::cout << "#";
        } else if (strength > 0.4f) {
            std::cout << "+";
        } else if (strength > 0.1f) {
            std::cout << ".";
        } else {
            std::cout << " ";
        }
        
        x += static_cast<float>(dx) / steps;
        y += static_cast<float>(dy) / steps;
    }
    std::cout << std::endl;
}

} // namespace nlm