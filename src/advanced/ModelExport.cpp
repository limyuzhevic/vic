#include "advanced/ModelExport.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

bool ModelExporter::exportModel(const std::string& filename, const std::shared_ptr<Brain>& brain) {
    std::cout << "Exporting brain model to: " << filename << std::endl;
    
    try {
        nlohmann::json modelData = brainToJson(brain);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "✗ Failed to open file for writing: " << filename << std::endl;
            return false;
        }
        
        file << modelData.dump(2);
        file.close();
        
        std::cout << "✓ Model exported successfully to: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Error exporting model: " << e.what() << std::endl;
        return false;
    }
}

bool ModelExporter::importModel(const std::string& filename, std::shared_ptr<Brain>& brain) {
    std::cout << "Importing brain model from: " << filename << std::endl;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "✗ Cannot open file: " << filename << std::endl;
            return false;
        }
        
        nlohmann::json modelData = nlohmann::json::parse(file);
        
        brain = jsonToBrain(modelData);
        if (!brain) {
            std::cout << "✗ Failed to create brain from imported data" << std::endl;
            return false;
        }
        
        std::cout << "✓ Model imported successfully from: " << filename << std::endl;
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        std::cout << "✗ Failed to parse model file: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cout << "✗ Error importing model: " << e.what() << std::endl;
        return false;
    }
}

bool ModelExporter::exportConfig(const std::string& filename, const std::shared_ptr<Config>& config) {
    std::cout << "Exporting configuration to: " << filename << std::endl;
    
    try {
        nlohmann::json configData = configToJson(config);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "✗ Failed to open file for writing: " << filename << std::endl;
            return false;
        }
        
        file << configData.dump(2);
        file.close();
        
        std::cout << "✓ Configuration exported successfully to: " << filename << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Error exporting configuration: " << e.what() << std::endl;
        return false;
    }
}

bool ModelExporter::importConfig(const std::string& filename, std::shared_ptr<Config>& config) {
    std::cout << "Importing configuration from: " << filename << std::endl;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "✗ Cannot open file: " << filename << std::endl;
            return false;
        }
        
        nlohmann::json configData = nlohmann::json::parse(file);
        
        config = jsonToConfig(configData);
        if (!config) {
            std::cout << "✗ Failed to create configuration from imported data" << std::endl;
            return false;
        }
        
        std::cout << "✓ Configuration imported successfully from: " << filename << std::endl;
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        std::cout << "✗ Failed to parse configuration file: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cout << "✗ Error importing configuration: " << e.what() << std::endl;
        return false;
    }
}

nlohmann::json ModelExporter::brainToJson(const std::shared_ptr<Brain>& brain) {
    nlohmann::json j;
    
    j["brain_type"] = "NLM_Phase2";
    j["version"] = "2.0";
    j["neuron_count"] = brain->getTotalNeuronCount();
    j["synapse_count"] = brain->getTotalSynapseCount();
    j["region_count"] = brain->getRegionCount();
    j["total_spikes"] = brain->getTotalSpikeCount();
    j["current_step"] = brain->getCurrentStep();
    
    // Export regions
    auto regions = brain->getAllRegions();
    j["regions"] = nlohmann::json::array();
    
    for (const auto& region : regions) {
        nlohmann::json regionJson;
        regionJson["id"] = static_cast<int>(region->getId());
        regionJson["neuron_count"] = region->getNeuronCount();
        
        // Export neurons
        regionJson["neurons"] = nlohmann::json::array();
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < neurons.size(); ++i) {
            nlohmann::json neuronJson;
            neuronJson["id"] = i;
            neuronJson["type"] = static_cast<int>(neurons[i]->getType());
            neuronJson["membrane_potential"] = neurons[i]->getMembranePotential();
            neuronJson["threshold"] = neurons[i]->getThreshold();
            neuronJson["refractory_time"] = neurons[i]->getRefractoryTime();
            neuronJson["injected_current"] = neurons[i]->getInjectedCurrent();
            regionJson["neurons"].push_back(neuronJson);
        }
        
        // Export synapses
        regionJson["synapses"] = nlohmann::json::array();
        auto synapses = region->getSynapses();
        for (size_t i = 0; i < synapses.size(); ++i) {
            nlohmann::json synapseJson;
            synapseJson["id"] = i;
            synapseJson["pre_neuron"] = synapses[i]->getPreNeuronId();
            synapseJson["post_neuron"] = synapses[i]->getPostNeuronId();
            synapseJson["weight"] = synapses[i]->getWeight();
            synapseJson["delay"] = synapses[i]->getDelay();
            synapseJson["enabled"] = synapses[i]->isEnabled();
            regionJson["synapses"].push_back(synapseJson);
        }
        
        j["regions"].push_back(regionJson);
    }
    
    // Export configuration
    auto config = brain->getConfig();
    if (config) {
        j["config"] = configToJson(config);
    }
    
    return j;
}

std::shared_ptr<Brain> ModelExporter::jsonToBrain(const nlohmann::json& j) {
    // Validate format
    if (!j.contains("brain_type") || j["brain_type"] != "NLM_Phase2") {
        std::cout << "✗ Invalid brain model format" << std::endl;
        return nullptr;
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration if present
    if (j.contains("config")) {
        // Parse configuration from JSON
        std::shared_ptr<Config> loadedConfig = jsonToConfig(j["config"]);
        if (loadedConfig) {
            config = loadedConfig;
        }
    }
    
    // Create brain with loaded config
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "✗ Failed to initialize brain from model" << std::endl;
        return nullptr;
    }
    
    // Restore neuron and synapse states from JSON
    if (j.contains("regions")) {
        restoreBrainStateFromJson(brain, j["regions"]);
    }
    
    return brain;
}

void ModelExporter::restoreBrainStateFromJson(const std::shared_ptr<Brain>& brain, const nlohmann::json& regionsJson) {
    std::cout << "Restoring brain state from JSON..." << std::endl;
    
    auto regions = brain->getAllRegions();
    size_t regionIndex = 0;
    
    for (const auto& regionJson : regionsJson) {
        if (regionIndex < regions.size()) {
            auto region = regions[regionIndex];
            
            // Restore neurons
            if (regionJson.contains("neurons")) {
                restoreNeuronsFromJson(region, regionJson["neurons"]);
            }
            
            // Restore synapses
            if (regionJson.contains("synapses")) {
                restoreSynapsesFromJson(region, regionJson["synapses"], regionJson.value("neuron_count", 0));
            }
            
            regionIndex++;
        }
    }
    
    std::cout << "✓ Brain state restored from JSON" << std::endl;
}

void ModelExporter::restoreNeuronsFromJson(const std::shared_ptr<NeuralRegion>& region, const nlohmann::json& neuronsJson) {
    auto neurons = region->getAllNeurons();
    
    for (size_t i = 0; i < neuronsJson.size() && i < neurons.size(); ++i) {
        const auto& neuronJson = neuronsJson[i];
        auto neuron = neurons[i];
        
        // Restore neuron properties
        if (neuronJson.contains("membrane_potential")) {
            neuron->setMembranePotential(neuronJson["membrane_potential"]);
        }
        if (neuronJson.contains("threshold")) {
            neuron->setThreshold(neuronJson["threshold"]);
        }
        if (neuronJson.contains("refractory_time")) {
            neuron->setRefractoryTime(neuronJson["refractory_time"]);
        }
        if (neuronJson.contains("injected_current")) {
            neuron->injectCurrent(neuronJson["injected_current"]);
        }
        
        // Note: In a real implementation, you would also restore spike history,
        // membrane potential history, and other internal state
    }
}

void ModelExporter::restoreSynapsesFromJson(const std::shared_ptr<NeuralRegion>& region, const nlohmann::json& synapsesJson, size_t neuronCount) {
    auto synapses = region->getSynapses();
    
    // Clear existing synapses
    region->clearSynapses();
    
    // Restore synapses from JSON
    for (size_t i = 0; i < synapsesJson.size(); ++i) {
        const auto& synapseJson = synapsesJson[i];
        
        // Validate synapse data
        if (!synapseJson.contains("pre_neuron") || !synapseJson.contains("post_neuron") || 
            !synapseJson.contains("weight") || !synapseJson.contains("delay")) {
            continue;
        }
        
        // Create new synapse
        size_t preNeuronId = synapseJson["pre_neuron"];
        size_t postNeuronId = synapseJson["post_neuron"];
        
        // Find neurons in the region (simplified: assume neurons 0..neuronCount-1 are in this region)
        auto preNeuron = (preNeuronId < neuronCount) ? region->getNeuron(NeuronId(preNeuronId)) : nullptr;
        auto postNeuron = (postNeuronId < neuronCount) ? region->getNeuron(NeuronId(postNeuronId)) : nullptr;
        
        if (preNeuron && postNeuron) {
            // Create synapse
            auto synapse = std::make_shared<Synapse>(preNeuron, postNeuron, synapseJson["delay"]);
            
            // Restore synapse properties
            synapse->setWeight(synapseJson["weight"]);
            if (synapseJson.contains("enabled")) {
                synapse->enablePlasticity(synapseJson["enabled"], 
                                       synapseJson.value("stdp_enabled", true),
                                       synapseJson.value("hebbian_enabled", true));
            } else {
                synapse->enablePlasticity(true);
            }
            
            // Re-add to region
            region->addSynapse(synapse);
        }
    }
    
    // Re-establish region connectivity
    region->rebuildConnectivity();
}

nlohmann::json ModelExporter::configToJson(const std::shared_ptr<Config>& config) {
    nlohmann::json j;
    
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        auto value = config->getOr<std::string>(key, "");
        if (!value.empty()) {
            j[key] = value;
        }
    }
    
    return j;
}

std::shared_ptr<Config> ModelExporter::jsonToConfig(const nlohmann::json& j) {
    auto config = std::make_shared<Config>();
    
    for (auto& [key, value] : j.items()) {
        if (value.is_number()) {
            config->set(key, value.get<double>(), ConfigSource::File);
        } else if (value.is_string()) {
            config->set(key, value.get<std::string>(), ConfigSource::File);
        } else if (value.is_boolean()) {
            config->set(key, value.get<bool>(), ConfigSource::File);
        } else if (value.is_array()) {
            // Handle array values
            std::vector<std::string> arrayVals;
            for (const auto& item : value) {
                arrayVals.push_back(item.dump());
            }
            config->set(key, arrayVals, ConfigSource::File);
        }
    }
    
    return config;
}

} // namespace nlm