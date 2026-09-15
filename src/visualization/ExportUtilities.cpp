#include "ExportUtilities.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>

namespace nlm {

ExportUtilities::ExportUtilities() {}
ExportUtilities::~ExportUtilities() {}

bool ExportUtilities::writeText(const std::string& content, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return true;
}

bool ExportUtilities::writeJSON(const std::string& content, const std::string& filepath) {
    return writeText(content, filepath);
}

bool ExportUtilities::writeCSV(const std::string& content, const std::string& filepath) {
    return writeText(content, filepath);
}

bool ExportUtilities::writeBinary(const std::vector<char>& data, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(data.data(), data.size());
    file.close();
    return true;
}

bool ExportUtilities::exportSpikeData(const std::vector<DetailedSpikeEvent>& spikes,
                                     const std::string& filepath,
                                     ExportFormat format) {
    std::stringstream ss;
    
    switch (format) {
        case Text:
            ss << "Spike Events\n";
            ss << "============\n\n";
            ss << "Total spikes: " << spikes.size() << "\n\n";
            ss << "Format: time, neuron_id, step, region_id, population_id\n";
            
            for (const auto& spike : spikes) {
                ss << spike.timestamp << ", " << spike.source.index() << 
                   ", " << spike.step << ", " << spike.regionId.index() << 
                   ", " << spike.populationId.index() << "\n";
            }
            break;
            
        case JSON:
            ss << "{\n  \"spikes\": [\n";
            for (size_t i = 0; i < spikes.size(); ++i) {
                const auto& spike = spikes[i];
                ss << "    {\"timestamp\": " << spike.timestamp << ", \"neuron_id\": " << spike.source.index() << 
                   ", \"step\": " << spike.step << ", \"region_id\": " << spike.regionId.index() << 
                   ", \"population_id\": " << spike.populationId.index();
                if (i < spikes.size() - 1) ss << ",";
                ss << "\n";
            }
            ss << "  ]\n}";
            break;
            
        case CSV:
            ss << "timestamp,neuron_id,step,region_id,population_id\n";
            for (const auto& spike : spikes) {
                ss << spike.timestamp << "," << spike.source.index() << 
                   "," << spike.step << "," << spike.regionId.index() << 
                   "," << spike.populationId.index() << "\n";
            }
            break;
            
        default:
            return false;
    }
    
    return writeText(ss.str(), filepath);
}

bool ExportUtilities::exportWeightData(const std::vector<std::vector<float>>& matrix,
                                       const std::string& filepath,
                                       ExportFormat format,
                                       const std::vector<std::string>& rowLabels,
                                       const std::vector<std::string>& colLabels) {
    std::stringstream ss;
    
    switch (format) {
        case Text:
            ss << "Weight Matrix\n";
            ss << "==============\n\n";
            ss << "Size: " << matrix.size() << " x " << (matrix.empty() ? 0 : matrix[0].size()) << "\n\n";
            
            ss << "  ";
            if (rowLabels.empty()) {
                for (size_t j = 0; j < matrix[0].size(); ++j) {
                    ss << "C" << j << " ";
                }
            } else {
                for (size_t j = 0; j < matrix[0].size(); ++j) {
                    ss << colLabels[j] << " ";
                }
            }
            ss << "\n";
            
            for (size_t i = 0; i < matrix.size(); ++i) {
                if (!rowLabels.empty()) {
                    ss << rowLabels[i] << ": ";
                } else {
                    ss << "R" << i << ": ";
                }
                
                for (size_t j = 0; j < matrix[i].size(); ++j) {
                    ss << matrix[i][j] << " ";
                }
                ss << "\n";
            }
            break;
            
        case JSON:
            ss << "{\n  \"matrix\": [\n";
            for (size_t i = 0; i < matrix.size(); ++i) {
                ss << "    [\n";
                for (size_t j = 0; j < matrix[i].size(); ++j) {
                    ss << "      " << matrix[i][j];
                    if (j < matrix[i].size() - 1) ss << ", ";
                    ss << "\n";
                }
                ss << "    ]";
                if (i < matrix.size() - 1) ss << ", ";
                ss << "\n";
            }
            ss << "  ],\n  \"row_labels\": [";
            for (size_t i = 0; i < rowLabels.size(); ++i) {
                ss << "\"" << rowLabels[i] << "\"";
                if (i < rowLabels.size() - 1) ss << ", ";
            }
            ss << "],\n  \"col_labels\": [";
            for (size_t j = 0; j < colLabels.size(); ++j) {
                ss << "\"" << colLabels[j] << "\"";
                if (j < colLabels.size() - 1) ss << ", ";
            }
            ss << "]\n}";
            break;
            
        case CSV:
            ss << "row,col,value\n";
            for (size_t i = 0; i < matrix.size(); ++i) {
                for (size_t j = 0; j < matrix[i].size(); ++j) {
                    std::string rowLabel = (i < rowLabels.size() ? rowLabels[i] : "R" + std::to_string(i));
                    std::string colLabel = (j < colLabels.size() ? colLabels[j] : "C" + std::to_string(j));
                    ss << rowLabel << "," << colLabel << "," << matrix[i][j] << "\n";
                }
            }
            break;
            
        default:
            return false;
    }
    
    return writeText(ss.str(), filepath);
}

bool ExportUtilities::exportActivityData(const std::vector<double>& firingRates,
                                         const std::vector<double>& membranePotentials,
                                         const std::string& filepath,
                                         ExportFormat format) {
    std::stringstream ss;
    
    switch (format) {
        case Text:
            ss << "Activity Data\n";
            ss << "==============\n\n";
            ss << "Time Points: " << firingRates.size() << "\n\n";
            ss << "Format: time, firing_rate, membrane_potential\n";
            
            for (size_t i = 0; i < firingRates.size(); ++i) {
                ss << i << ", " << firingRates[i] << ", " << membranePotentials[i] << "\n";
            }
            break;
            
        case JSON:
            ss << "{\n  \"time_series\": [\n";
            for (size_t i = 0; i < firingRates.size(); ++i) {
                ss << "    {\"time\": " << i << ", \"firing_rate\": " << firingRates[i] << 
                   ", \"membrane_potential\": " << membranePotentials[i];
                if (i < firingRates.size() - 1) ss << ", ";
                ss << "\n";
            }
            ss << "  ]\n}";
            break;
            
        case CSV:
            ss << "time,firing_rate,membrane_potential\n";
            for (size_t i = 0; i < firingRates.size(); ++i) {
                ss << i << "," << firingRates[i] << "," << membranePotentials[i] << "\n";
            }
            break;
            
        default:
            return false;
    }
    
    return writeText(ss.str(), filepath);
}

bool ExportUtilities::exportNetworkData(const Brain& brain,
                                       const std::string& filepath,
                                       ExportFormat format) {
    std::stringstream ss;
    
    switch (format) {
        case Text:
            ss << "Network Data\n";
            ss << "============\n\n";
            ss << "Regions: " << brain.getRegionCount() << "\n";
            ss << "Neurons: " << brain.getTotalNeuronCount() << "\n";
            ss << "Synapses: " << brain.getTotalSynapseCount() << "\n";
            ss << "Firing Rate: " << brain.getAverageFiringRate() << " Hz\n\n";
            
            ss << "Region Details:\n";
            for (size_t i = 0; i < brain.getRegionCount(); ++i) {
                auto region = brain.getRegion(brain.getRegionIds()[i]);
                if (region) {
                    ss << "  Region " << region->getId().index() << " (" << region->getName() << "):\n";
                    ss << "    Neurons: " << region->getTotalNeuronCount() << "\n";
                    ss << "    Synapses: " << region->getSynapseCount() << "\n";
                    ss << "    Active: " << region->getActiveNeuronCount() << "\n";
                    ss << "    Firing: " << region->getFiringNeuronCount() << "\n";
                }
            }
            break;
            
        case JSON:
            ss << "{\n  \"brain_info\": {\n";
            ss << "    \"regions\": " << brain.getRegionCount() << ",\n";
            ss << "    \"neurons\": " << brain.getTotalNeuronCount() << ",\n";
            ss << "    \"synapses\": " << brain.getTotalSynapseCount() << ",\n";
            ss << "    \"firing_rate\": " << brain.getAverageFiringRate() << "\n";
            ss << "  },\n";
            
            ss << "  \"regions\": [\n";
            for (size_t i = 0; i < brain.getRegionCount(); ++i) {
                auto region = brain.getRegion(brain.getRegionIds()[i]);
                if (region) {
                    ss << "    {\"id\": " << region->getId().index() << ", \"name\": \"" << region->getName() << "\", \"neurons\": " << region->getTotalNeuronCount() << ", \"synapses\": " << region->getSynapseCount() << "}";
                    if (i < brain.getRegionCount() - 1) ss << ",";
                    ss << "\n";
                }
            }
            ss << "  ]\n}";
            break;
            
        case CSV:
            ss << "region_id,region_name,neuron_count,synapse_count,active_count,firing_count\n";
            for (size_t i = 0; i < brain.getRegionCount(); ++i) {
                auto region = brain.getRegion(brain.getRegionIds()[i]);
                if (region) {
                    ss << region->getId().index() << ", " << region->getName() << ", " << region->getTotalNeuronCount() << ", " << region->getSynapseCount() << ", " << region->getActiveNeuronCount() << ", " << region->getFiringNeuronCount() << "\n";
                }
            }
            break;
            
        default:
            return false;
    }
    
    return writeText(ss.str(), filepath);
}

bool ExportUtilities::exportDevelopmentData(const VisualizationSystem::DevelopmentStats& stats,
                                           const std::string& filepath,
                                           ExportFormat format) {
    std::stringstream ss;
    
    switch (format) {
        case Text:
            ss << "Development Data\n";
            ss << "================\n\n";
            ss << "Current Stage: " << static_cast<int>(stats.currentStage) << "\n";
            ss << "Stage Progress: " << stats.stageProgress << "\n";
            ss << "Neurons Created: " << stats.neuronsCreated << "\n";
            ss << "Synapses Created: " << stats.synapsesCreated << "\n";
            ss << "Synapses Eliminated: " << stats.synapsesEliminated << "\n";
            ss << "Total Plasticity Change: " << stats.totalPlasticityChange << "\n\n";
            
            ss << "Synaptogenesis Rate: " << stats.synaptogenesisRate << "\n";
            ss << "Pruning Rate: " << stats.pruningRate << "\n\n";
            
            ss << "Stage Progression:\n";
            for (size_t i = 0; i < stats.stageProgression.size(); ++i) {
                ss << "  Time " << i << ": " << stats.stageProgression[i] << "\n";
            }
            
            ss << "\nPlasticity History:\n";
            for (size_t i = 0; i < stats.plasticityHistory.size(); ++i) {
                ss << "  Time " << i << ": " << stats.plasticityHistory[i] << "\n";
            }
            
            ss << "\nConnectivity History:\n";
            for (size_t i = 0; i < stats.connectivityHistory.size(); ++i) {
                ss << "  Time " << i << ": " << stats.connectivityHistory[i] << "\n";
            }
            break;
            
        case JSON:
            ss << "{\n  \"current_stage\": " << static_cast<int>(stats.currentStage) << ",\n";
            ss << "  \"stage_progress\": " << stats.stageProgress << ",\n";
            ss << "  \"neurons_created\": " << stats.neuronsCreated << ",\n";
            ss << "  \"synapses_created\": " << stats.synapsesCreated << ",\n";
            ss << "  \"synapses_eliminated\": " << stats.synapsesEliminated << ",\n";
            ss << "  \"total_plasticity_change\": " << stats.totalPlasticityChange << ",\n";
            ss << "  \"synaptogenesis_rate\": " << stats.synaptogenesisRate << ",\n";
            ss << "  \"pruning_rate\": " << stats.pruningRate << ",\n";
            
            ss << "  \"stage_progression\": [";
            for (size_t i = 0; i < stats.stageProgression.size(); ++i) {
                ss << stats.stageProgression[i];
                if (i < stats.stageProgression.size() - 1) ss << ", ";
            }
            ss << "],\n";
            
            ss << "  \"plasticity_history\": [";
            for (size_t i = 0; i < stats.plasticityHistory.size(); ++i) {
                ss << stats.plasticityHistory[i];
                if (i < stats.plasticityHistory.size() - 1) ss << ", ";
            }
            ss << "],\n";
            
            ss << "  \"connectivity_history\": [";
            for (size_t i = 0; i < stats.connectivityHistory.size(); ++i) {
                ss << stats.connectivityHistory[i];
                if (i < stats.connectivityHistory.size() - 1) ss << ", ";
            }
            ss << "]\n}";
            break;
            
        case CSV:
            ss << "metric,value\n";
            ss << "current_stage," << static_cast<int>(stats.currentStage) << "\n";
            ss << "stage_progress," << stats.stageProgress << "\n";
            ss << "neurons_created," << stats.neuronsCreated << "\n";
            ss << "synapses_created," << stats.synapsesCreated << "\n";
            ss << "synapses_eliminated," << stats.synapsesEliminated << "\n";
            ss << "total_plasticity_change," << stats.totalPlasticityChange << "\n";
            ss << "synaptogenesis_rate," << stats.synaptogenesisRate << "\n";
            ss << "pruning_rate," << stats.pruningRate << "\n";
            
            for (size_t i = 0; i < stats.stageProgression.size(); ++i) {
                ss << "stage_progression_" << i << "," << stats.stageProgression[i] << "\n";
            }
            for (size_t i = 0; i < stats.plasticityHistory.size(); ++i) {
                ss << "plasticity_history_" << i << ",\" << stats.plasticityHistory[i] << "\n";
            }
            for (size_t i = 0; i < stats.connectivityHistory.size(); ++i) {
                ss << "connectivity_history_" << i << "," << stats.connectivityHistory[i] << "\n";
            }
            break;
            
        default:
            return false;
    }
    
    return writeText(ss.str(), filepath);
}

bool ExportUtilities::exportConfig(const std::map<std::string, std::string>& config,
                                   const std::string& filepath) {
    std::stringstream ss;
    ss << "# Configuration file\n# Generated by NLM Visualization System\n\n";
    
    for (const auto& pair : config) {
        ss << pair.first << " = " << pair.second << "\n";
    }
    
    return writeText(ss.str(), filepath);
}

bool ExportUtilities::importConfig(const std::string& filepath,
                                  std::map<std::string, std::string>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Find first '=' to split key and value
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        config[key] = value;
    }
    
    file.close();
    return true;
}

} // namespace nlm
