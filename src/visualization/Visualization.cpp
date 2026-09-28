#include "visualization/VisualizationManager.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

namespace {
    // Helper functions
    std::string currentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    std::string escapeJSONString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\" + c; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
}

// TextVisualizer implementation
void TextVisualizer::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    currentStep_ = 0;
    currentTime_ = 0.0;
    history_.clear();
    
    NLM_LOG_INFO("TextVisualizer initialized");
}

void TextVisualizer::update(double timestep, size_t step) {
    currentStep_ = step;
    currentTime_ = timestep * step;
    
    // Create snapshot
    BrainSnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.timestep = timestep;
    snapshot.step = step;
    snapshot.totalSpikeCount = brain_->getTotalSpikeCount();
    snapshot.activeNeuronCount = brain_->getActiveNeuronCount();
    snapshot.averageFiringRate = brain_->getAverageFiringRate();
    
    // Collect neuron data
    neuronMembranePotentials_.clear();
    neuronActivations_.clear();
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                neuronMembranePotentials_.push_back(neuron->getMembranePotential());
                neuronActivations_.push_back(std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) / 20.0f);
            }
        }
    }
    
    snapshot.neuronMembranePotentials = neuronMembranePotentials_;
    snapshot.neuronActivations = neuronActivations_;
    
    // Collect synaptic weights
    synapticWeights_.clear();
    for (const auto& region : brain_->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            synapticWeights_.push_back(syn->getWeight());
        }
    }
    snapshot.synapticWeights = synapticWeights_;
    
    // Collect neuromodulator levels
    neuromodulatorLevels_.clear();
    if (auto* dopamine = brain_->getDopamine()) {
        neuromodulatorLevels_["dopamine"] = dopamine->getLevel();
    }
    if (auto* curiosity = brain_->getCuriosity()) {
        neuromodulatorLevels_["curiosity"] = curiosity->getLevel();
    }
    if (auto* novelty = brain_->getNovelty()) {
        neuromodulatorLevels_["novelty"] = novelty->getLevel();
    }
    if (auto* predictionError = brain_->getPredictionErrorSignal()) {
        neuromodulatorLevels_["prediction_error"] = predictionError->getError();
    }
    
    snapshot.neuromodulatorLevels = neuromodulatorLevels_;
    
    // Get developmental stage
    snapshot.developmentalStage = std::to_string(static_cast<int>(brain_->getDevelopmentalStage()));
    
    history_.push_back(snapshot);
    
    if (debugLevel_ >= DebugLevel::INFO) {
        NLM_LOG_INFO("TextVisualizer updated: step " + std::to_string(step) + 
                    ", spikes: " + std::to_string(snapshot.totalSpikeCount) +
                    ", neurons: " + std::to_string(snapshot.activeNeuronCount));
    }
}

void TextVisualizer::render() {
    if (debugLevel_ >= DebugLevel::DETAILED) {
        std::cout << "=== NLM Brain State ===" << std::endl;
        std::cout << "Step: " << currentStep_ << std::endl;
        std::cout << "Time: " << currentTime_ << "s" << std::endl;
        std::cout << "Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain_->getAverageFiringRate() << std::endl;
        
        if (showNeurons_) {
            std::cout << "\nNeuron Activity:" << std::endl;
            for (size_t i = 0; i < std::min<size_t>(neuronMembranePotentials_.size(), 10); ++i) {
                std::cout << "  Neuron " << i 
                          << " V=" << std::fixed << std::setprecision(2) << neuronMembranePotentials_[i]
                          << " Act=" << neuronActivations_[i] << std::endl;
            }
            if (neuronMembranePotentials_.size() > 10) {
                std::cout << "  ... and " << (neuronMembranePotentials_.size() - 10) << " more neurons" << std::endl;
            }
        }
        
        if (showSynapses_) {
            std::cout << "\nSynaptic Weights:" << std::endl;
            for (size_t i = 0; i < std::min<size_t>(synapticWeights_.size(), 10); ++i) {
                std::cout << "  Synapse " << i << " W=" << std::fixed << std::setprecision(4) << synapticWeights_[i] << std::endl;
            }
            if (synapticWeights_.size() > 10) {
                std::cout << "  ... and " << (synapticWeights_.size() - 10) << " more synapses" << std::endl;
            }
        }
        
        if (showSpikes_) {
            std::cout << "\nRecent Spikes:" << std::endl;
            // This would need actual spike history tracking
            std::cout << "  Spike count: " << brain_->getTotalSpikeCount() << std::endl;
        }
        
        std::cout << "\n=== Network Statistics ===" << std::endl;
        std::cout << "Regions: " << brain_->getRegionCount() << std::endl;
        std::cout << "Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "Excitation/Inhibition Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    }
}

void TextVisualizer::saveToFile(const std::string& filename, VisualizationFormat format) {
    switch (format) {
        case VisualizationFormat::TEXT:
            saveText(filename);
            break;
        case VisualizationFormat::CSV:
            CSVExporter::exportBrainState(brain_, filename);
            break;
        case VisualizationFormat::JSON:
            JSONExporter::exportBrainState(brain_, filename);
            break;
        default:
            NLM_LOG_WARNING("Unsupported visualization format for file: " + filename);
            break;
    }
}

void TextVisualizer::setDebugLevel(DebugLevel level) {
    debugLevel_ = level;
}

DebugLevel TextVisualizer::getDebugLevel() const {
    return debugLevel_;
}

void TextVisualizer::saveText(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open text visualization file: " + filename);
        return;
    }

    file << "NLM Brain Visualization" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    file << "Generated: " << currentTimestamp() << std::endl;
    file << "Step: " << currentStep_ << std::endl;
    file << "Time: " << currentTime_ << "s" << std::endl;
    file << std::endl;
    
    file << "Network Statistics:" << std::endl;
    file << "  Regions: " << brain_->getRegionCount() << std::endl;
    file << "  Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    file << "  Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    file << "  Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
    file << "  Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
    file << "  Average Firing Rate: " << brain_->getAverageFiringRate() << std::endl;
    file << "  E/I Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    file << std::endl;
    
    file << "Neuron Data:" << std::endl;
    file << "NeuronID,MembranePotential,Activation" << std::endl;
    for (size_t i = 0; i < neuronMembranePotentials_.size(); ++i) {
        file << i << "," 
             << std::fixed << std::setprecision(4) << neuronMembranePotentials_[i] << "," 
             << neuronActivations_[i] << std::endl;
    }
    file << std::endl;
    
    file << "Synapse Data:" << std::endl;
    file << "SynapseID,SourceNeuron,TargetNeuron,Weight,Delay" << std::endl;
    // This would need actual synapse data access
    
    file.close();
    NLM_LOG_INFO("Text visualization saved to: " + filename);
}

// CSVExporter implementation
int CSVExporter::precision_ = 4;

bool CSVExporter::exportBrainState(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open CSV file: " + filename);
        return false;
    }

    file << "step,spikes,active_neurons,firing_rate,neuron_count,synapse_count,e_i_ratio" << std::endl;
    
    for (size_t step = 0; step < brain->getTotalSpikeCount(); step++) {
        file << step << ","
             << brain->getTotalSpikeCount() << ","
             << brain->getActiveNeuronCount() << ","
             << brain->getAverageFiringRate() << ","
             << brain->getTotalNeuronCount() << ","
             << brain->getTotalSynapseCount() << ","
             << brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Brain state exported to CSV: " + filename);
    return true;
}

bool CSVExporter::exportSimulationResults(const std::vector<BrainSnapshot>& history, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open CSV simulation results file: " + filename);
        return false;
    }

    file << "step,time,spikes,active_neurons,firing_rate,neuron_count,synapse_count" << std::endl;
    
    for (const auto& snapshot : history) {
        file << snapshot.step << ","
             << snapshot.timestep * snapshot.step << ","
             << snapshot.totalSpikeCount << ","
             << snapshot.activeNeuronCount << ","
             << snapshot.averageFiringRate << ","
             << snapshot.neuronMembranePotentials.size() << ","
             << snapshot.synapticWeights.size() << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Simulation results exported to CSV: " + filename);
    return true;
}

bool CSVExporter::exportNeurons(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open neurons CSV file: " + filename);
        return false;
    }

    file << "neuron_id,region,type,membrane_potential,activation,firing_state" << std::endl;
    
    // This would need actual neuron data access
    // For now, just write header
    
    file.close();
    return true;
}

bool CSVExporter::exportSynapses(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open synapses CSV file: " + filename);
        return false;
    }

    file << "synapse_id,source_neuron,target_neuron,weight,delay,type,plasticity_flags" << std::endl;
    
    // This would need actual synapse data access
    
    file.close();
    return true;
}

bool CSVExporter::exportSpikes(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open spikes CSV file: " + filename);
        return false;
    }

    file << "spike_time,neuron_id,region,type" << std::endl;
    
    // This would need actual spike history tracking
    
    file.close();
    return true;
}

std::ofstream CSVExporter::createOutputFile(const std::string& filename) {
    return std::ofstream(filename);
}

// JSONExporter implementation
bool JSONExporter::prettyPrint_ = false;

bool JSONExporter::exportBrainState(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file: " + filename);
        return false;
    }

    std::string jsonData = brainStateToJSON(brain);
    file << jsonData;
    file.close();
    
    NLM_LOG_INFO("Brain state exported to JSON: " + filename);
    return true;
}

bool JSONExporter::exportNetworkTopology(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON topology file: " + filename);
        return false;
    }

    std::string jsonData = networkTopologyToJSON(brain);
    file << jsonData;
    file.close();
    
    NLM_LOG_INFO("Network topology exported to JSON: " + filename);
    return true;
}

bool JSONExporter::exportSimulationData(const std::vector<BrainSnapshot>& history, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON simulation data file: " + filename);
        return false;
    }

    // Create JSON array of snapshots
    std::string jsonData = "[";
    for (size_t i = 0; i < history.size(); ++i) {
        if (i > 0) jsonData += ",";
        // Convert snapshot to JSON (simplified)
        jsonData += "{\"step\": " + std::to_string(history[i].step) +
                   ",\"time\": " + std::to_string(history[i].timestep * history[i].step) +
                   ",\"spikes\": " + std::to_string(history[i].totalSpikeCount) + "}";
    }
    jsonData += "]";

    file << jsonData;
    file.close();
    
    NLM_LOG_INFO("Simulation data exported to JSON: " + filename);
    return true;
}

bool JSONExporter::exportNeuromodulation(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON neuromodulation file: " + filename);
        return false;
    }

    std::string jsonData = "{\"neuromodulators\": [{\"name\": \"dopamine\", \"level\": ";
    if (auto* dopamine = brain->getDopamine()) {
        jsonData += std::to_string(dopamine->getLevel());
    } else {
        jsonData += "0";
    }
    jsonData += "}, {\"name\": \"curiosity\", \"level\": ";
    if (auto* curiosity = brain->getCuriosity()) {
        jsonData += std::to_string(curiosity->getLevel());
    } else {
        jsonData += "0";
    }
    jsonData += "}]}";

    file << jsonData;
    file.close();
    
    NLM_LOG_INFO("Neuromodulation data exported to JSON: " + filename);
    return true;
}

std::string JSONExporter::brainStateToJSON(std::shared_ptr<Brain> brain) {
    std::stringstream ss;
    if (prettyPrint_) {
        ss << "{\n";
    } else {
        ss << "{";
    }

    ss << "\"brain\": {",
        "\"step\": " + std::to_string(brain->getTotalSpikeCount()) +
        ",\"neurons\": " + std::to_string(brain->getTotalNeuronCount()) +
        ",\"synapses\": " + std::to_string(brain->getTotalSynapseCount()) +
        ",\"spikes\": " + std::to_string(brain->getTotalSpikeCount()) +
        ",\"firing_rate\": " + std::to_string(brain->getAverageFiringRate()) +
        ",\"e_i_ratio\": " + std::to_string(brain->getExcitationInhibitionRatio());

    if (prettyPrint_) {
        ss << ",\n  \"regions\": [";
    } else {
        ss << ",\"regions\":[";
    }

    auto regions = brain->getRegions();
    for (size_t i = 0; i < regions.size(); ++i) {
        if (i > 0) {
            if (prettyPrint_) ss << ",\n   ";
            else ss << ",";
        }
        if (prettyPrint_) {
            ss << "{\n    \"id\": " + std::to_string(regions[i]->getId().index()) +
               ",\"name\": \" + escapeJSONString(regions[i]->getName()) +
               ",\"neurons\": " + std::to_string(regions[i]->getTotalNeuronCount()) +
               ",\"synapses\": " + std::to_string(regions[i]->getSynapseCount());
        } else {
            ss << "{\"id\": " + std::to_string(regions[i]->getId().index()) +
               ",\"name\":\"" + escapeJSONString(regions[i]->getName()) + "\","
               "\"neurons\":" + std::to_string(regions[i]->getTotalNeuronCount()) +
               ",\"synapses\":" + std::to_string(regions[i]->getSynapseCount());
        }
    }

    if (prettyPrint_) {
        ss << "\n]";
    } else {
        ss << "]";
    }

    ss << "}";
    return ss.str();
}

std::string JSONExporter::networkTopologyToJSON(std::shared_ptr<Brain> brain) {
    std::stringstream ss;
    ss << "{\"network\": {\"nodes\": [], \"edges\": []}}";  // Simplified
    return ss.str();
}

// GraphVisualizer implementation
std::string GraphVisualizer::graphLayout_ = "spring";

bool GraphVisualizer::exportGraphML(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open GraphML file: " + filename);
        return false;
    }

    std::string graphML = graphMLToString(brain);
    file << graphML;
    file.close();
    
    NLM_LOG_INFO("Graph exported to GraphML: " + filename);
    return true;
}

bool GraphVisualizer::exportD3Data(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open D3 data file: " + filename);
        return false;
    }

    std::string d3Data = d3DataToString(brain);
    file << d3Data;
    file.close();
    
    NLM_LOG_INFO("Graph data exported for D3: " + filename);
    return true;
}

bool GraphVisualizer::exportSVG(std::shared_ptr<Brain> brain, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open SVG file: " + filename);
        return false;
    }

    std::string svgData = svgToString(brain);
    file << svgData;
    file.close();
    
    NLM_LOG_INFO("Graph exported to SVG: " + filename);
    return true;
}

std::string GraphVisualizer::graphMLToString(std::shared_ptr<Brain> brain) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.1/graphml.xsd\">\n";
    ss << "  <key id=\"d0\" for=\"node\" attr.name=\"label\" attr.type=\"string\"/>\n";
    ss << "  <key id=\"d1\" for=\"edge\" attr.name=\"weight\" attr.type=\"double\"/>\n";
    ss << "  <graph id=\"G\" edgedefault=\"undirected\">\n";
    
    // Add nodes (neurons)
    ss << "    <node id=\"n0\">\n      <data key=\"d0\">Neuron 0</data>\n    </node>\n";
    
    // Add edges (synapses)
    ss << "    <edge id=\"e0\" source=\"n0\" target=\"n1\">\n      <data key=\"d1\">0.5</data>\n    </edge>\n";
    
    ss << "  </graph>\n";
    ss << "</graphml>\n";
    
    return ss.str();
}

std::string GraphVisualizer::d3DataToString(std::shared_ptr<Brain> brain) {
    std::stringstream ss;
    ss << "{\"nodes\": [{\"id\": \"n0\", \"label\": \"Neuron 0\"}], \"edges\": [{\"source\": \"n0\", \"target\": \"n1\", \"weight\": 0.5}]}";
    return ss.str();
}

std::string GraphVisualizer::svgToString(std::shared_ptr<Brain> brain) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";
    ss << "  <circle cx=\"50\" cy=\"50\" r=\"20\" fill=\"lightblue\"/>\n";
    ss << "  <text x=\"50\" y=\"55\" text-anchor=\"middle\">Neuron 0</text>\n";
    ss << "</svg>\n";
    return ss.str();
}

} // namespace nlm
