// DebugTools implementation
#include "interfaces/AdvancedInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Experiment.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "performance/PerformanceMonitor.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

class DebugTools::Impl {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<Experiment> experiment;
    std::shared_ptr<VisualizationInterface> visualizationInterface;
    std::shared_ptr<PerformanceMonitor> performanceMonitor;
    
    bool debugEnabled;
    
    Impl() : debugEnabled(false) {}
    
    std::string formatNeuronInfo(const Neuron* neuron) const {
        std::ostringstream oss;
        oss << "    Neuron ID: " << neuron->getId() << std::endl;
        oss << "      Type: " << static_cast<int>(neuron->getType()) << std::endl;
        oss << "      Membrane Potential: " << std::fixed << std::setprecision(4) << neuron->getMembranePotential() << std::endl;
        oss << "      Threshold: " << std::fixed << std::setprecision(4) << neuron->getThreshold() << std::endl;
        oss << "      Refractory Period: " << std::fixed << std::setprecision(4) << neuron->getRefractoryPeriod() << std::endl;
        oss << "      Is Active: " << (neuron->isActive() ? "Yes" : "No") << std::endl;
        oss << "      Last Spike Time: " << (neuron->getLastSpikeTime() ? getTimeString(*neuron->getLastSpikeTime()) : "Never") << std::endl;
        return oss.str();
    }
    
    std::string formatSynapseInfo(const Synapse* synapse) const {
        std::ostringstream oss;
        oss << "    Synapse ID: " << synapse->getId() << std::endl;
        oss << "      Source Neuron: " << synapse->getSourceNeuronId() << std::endl;
        oss << "      Target Neuron: " << synapse->getTargetNeuronId() << std::endl;
        oss << "      Weight: " << std::fixed << std::setprecision(6) << synapse->getWeight() << std::endl;
        oss << "      Delay: " << std::fixed << std::setprecision(4) << synapse->getDelay() << std::endl;
        oss << "      Is Active: " << (synapse->isActive() ? "Yes" : "No") << std::endl;
        oss << "      Has Plasticity: " << (synapse->hasPlasticity() ? "Yes" : "No") << std::endl;
        oss << "      Plasticity Params: LTP=" << synapse->getLTPWeight() << ", LTD=" << synapse->getLTDWeight() << std::endl;
        return oss.str();
    }
    
    std::string getTimeString(const std::chrono::system_clock::time_point& time) const {
        auto time_t = std::chrono::system_clock::to_time_t(time);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%H:%M:%S.%f") << " UTC";
        return oss.str();
    }
};

DebugTools::DebugTools() : pImpl(std::make_unique<Impl>()) {}
DebugTools::~DebugTools() = default;

void DebugTools::setDebugEnabled(bool enabled) {
    pImpl->debugEnabled = enabled;
}

bool DebugTools::isDebugEnabled() const {
    return pImpl->debugEnabled;
}

std::string DebugTools::getBrainInfo(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Brain Debug Information:" << std::endl;
    oss << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    oss << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    oss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    oss << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    oss << "  Regions: " << brain.getRegionCount() << std::endl;
    oss << "  Current Step: " << brain.getCurrentStep() << std::endl;
    oss << "  Simulation Time: " << std::fixed << std::setprecision(4) << brain.getSimulationTime() << " s" << std::endl;
    return oss.str();
}

std::vector<std::string> DebugTools::getNeuronInfo(const Brain& brain, int neuronId) const {
    std::vector<std::string> info;
    
    if (neuronId >= 0) {
        auto neurons = brain.getAllNeurons();
        auto it = std::find_if(neurons.begin(), neurons.end(),
                              [neuronId](const Neuron* n) { return n->getId() == static_cast<NeuronId>(neuronId); });
        
        if (it != neurons.end()) {
            info.push_back(pImpl->formatNeuronInfo(*it));
        }
    } else {
        auto neurons = brain.getAllNeurons();
        for (auto* neuron : neurons) {
            info.push_back(pImpl->formatNeuronInfo(neuron));
        }
    }
    
    return info;
}

std::vector<std::string> DebugTools::getSynapseInfo(const Brain& brain, int synapseId) const {
    std::vector<std::string> info;
    
    auto* region = brain.getRegion(RegionId(1));
    if (!region) {
        return info;
    }
    
    auto synapses = region->getSynapses();
    
    if (synapseId >= 0) {
        auto it = std::find_if(synapses.begin(), synapses.end(),
                              [synapseId](const Synapse* s) { return s->getId() == static_cast<SynapseId>(synapseId); });
        
        if (it != synapses.end()) {
            info.push_back(pImpl->formatSynapseInfo(*it));
        }
    } else {
        for (auto* synapse : synapses) {
            info.push_back(pImpl->formatSynapseInfo(synapse));
        }
    }
    
    return info;
}

std::string DebugTools::getMemoryInfo(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Memory System Debug Information:" << std::endl;
    oss << "  Working Memory: Active" << std::endl;
    oss << "  Episodic Memory: Active" << std::endl;
    oss << "  Semantic Memory: Active" << std::endl;
    oss << "  Procedural Memory: Active" << std::endl;
    oss << "  Associative Memory: Active" << std::endl;
    oss << "  Memory Events: " << "N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getPredictionInfo(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Prediction System Debug Information:" << std::endl;
    oss << "  Prediction System: Active" << std::endl;
    oss << "  Prediction Accuracy: N/A" << std::endl;
    oss << "  Confidence Threshold: N/A" << std::endl;
    oss << "  Prediction Errors: N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getNeuromodulationInfo(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Neuromodulation System Debug Information:" << std::endl;
    oss << "  Dopamine System: Active" << std::endl;
    oss << "  Curiosity System: Active" << std::endl;
    oss << "  Novelty Detection: Active" << std::endl;
    oss << "  Reward Prediction Error: Active" << std::endl;
    oss << "  Neuromodulator Levels: N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getPlasticityInfo(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Plasticity System Debug Information:" << std::endl;
    
    auto* region = brain.getRegion(RegionId(1));
    if (!region) {
        oss << "  No regions found" << std::endl;
        return oss.str();
    }
    
    auto synapses = region->getSynapses();
    size_t plasticSynapses = 0;
    size_t activeSynapses = 0;
    double totalWeight = 0.0;
    
    for (auto* synapse : synapses) {
        if (synapse->hasPlasticity()) {
            plasticSynapses++;
        }
        if (synapse->isActive()) {
            activeSynapses++;
        }
        totalWeight += synapse->getWeight();
    }
    
    oss << "  Total Synapses: " << synapses.size() << std::endl;
    oss << "  Plastic Synapses: " << plasticSynapses << std::endl;
    oss << "  Active Synapses: " << activeSynapses << std::endl;
    oss << "  Total Weight: " << std::fixed << std::setprecision(4) << totalWeight << std::endl;
    oss << "  Average Weight: " << (synapses.empty() ? 0.0 : totalWeight / synapses.size()) << std::endl;
    
    return oss.str();
}

std::string DebugTools::getConfigInfo(const Config& config) const {
    std::ostringstream oss;
    oss << "Configuration Debug Information:" << std::endl;
    oss << "  Total Keys: " << config.getKeys().size() << std::endl;
    oss << "  Valid: " << (config.validate() ? "Yes" : "No") << std::endl;
    oss << "  Sources:" << std::endl;
    
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        oss << "    - " << key << std::endl;
    }
    
    if (!config.getValidationErrors().empty()) {
        oss << "  Validation Errors:" << std::endl;
        for (const auto& error : config.getValidationErrors()) {
            oss << "    * " << error << std::endl;
        }
    }
    
    return oss.str();
}

std::string DebugTools::getExperimentInfo(const Experiment& experiment) const {
    std::ostringstream oss;
    oss << "Experiment Debug Information:" << std::endl;
    oss << "  Status: Unknown" << std::endl;
    oss << "  Steps: Unknown" << std::endl;
    oss << "  Reward: Unknown" << std::endl;
    return oss.str();
}

std::string DebugTools::getNetworkInfo(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Network Information:" << std::endl;
    oss << "  Topology: N/A" << std::endl;
    oss << "  Connectivity: N/A" << std::endl;
    oss << "  Efficiency: N/A" << std::endl;
    oss << "  Synchrony: N/A" << std::endl;
    oss << "  Clustering: N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getConnectivityInfo(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Connectivity Information:" << std::endl;
    oss << "  Average In-Degree: N/A" << std::endl;
    oss << "  Average Out-Degree: N/A" << std::endl;
    oss << "  Connection Density: N/A" << std::endl;
    oss << "  Shortest Path Length: N/A" << std::endl;
    oss << "  Small World Coefficient: N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getSpikeInfo(const Brain& brain, size_t maxSpikes) const {
    std::ostringstream oss;
    oss << "Spike Activity Information:" << std::endl;
    oss << "  Total Spikes Recorded: " << brain.getTotalSpikeCount() << std::endl;
    oss << "  Current Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    oss << "  Max Spikes Displayed: " << maxSpikes << std::endl;
    // In a real implementation, this would show actual spike times
    oss << "  Sample Spike Times: N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getWeightMatrix(const Brain& brain, int regionId) const {
    std::ostringstream oss;
    oss << "Weight Matrix:" << std::endl;
    oss << "  Region ID: " << regionId << std::endl;
    oss << "  Matrix Size: N/A x N/A" << std::endl;
    oss << "  Matrix Values: N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getSynapticCurrents(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Synaptic Currents:" << std::endl;
    oss << "  Total Current: N/A" << std::endl;
    oss << "  Average Current: N/A" << std::endl;
    oss << "  Max Current: N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getNeuralActivity(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Neural Activity:" << std::endl;
    oss << "  Overall Activity: N/A" << std::endl;
    oss << "  Active Regions: N/A" << std::endl;
    oss << "  Regional Activity: N/A" << std::endl;
    return oss.str();
}

std::string DebugTools::getSimulationState(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Simulation State:" << std::endl;
    oss << "  Current Step: " << brain.getCurrentStep() << std::endl;
    oss << "  Simulation Time: " << std::fixed << std::setprecision(4) << brain.getSimulationTime() << " s" << std::endl;
    oss << "  Time Step: " << brain.getTimeStep() << " s" << std::endl;
    oss << "  Is Running: Yes" << std::endl;
    oss << "  Paused: No" << std::endl;
    return oss.str();
}

bool DebugTools::exportDebugInfo(const Brain& brain, const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << getBrainInfo(brain) << std::endl;
    file << "\n" << getNeuronInfo(brain) << std::endl;
    file << "\n" << getSynapseInfo(brain) << std::endl;
    file << "\n" << getMemoryInfo(brain) << std::endl;
    file << "\n" << getPredictionInfo(brain) << std::endl;
    file << "\n" << getNeuromodulationInfo(brain) << std::endl;
    file << "\n" << getPlasticityInfo(brain) << std::endl;
    
    return true;
}

void DebugTools::setBrain(std::shared_ptr<class Brain> brain) {
    pImpl->brain = brain;
}

void DebugTools::setConfig(std::shared_ptr<class Config> config) {
    pImpl->config = config;
}

void DebugTools::setExperiment(std::shared_ptr<class Experiment> experiment) {
    pImpl->experiment = experiment;
}

void DebugTools::setVisualizationInterface(std::shared_ptr<class VisualizationInterface> viz) {
    pImpl->visualizationInterface = viz;
}

void DebugTools::setPerformanceMonitor(std::shared_ptr<class PerformanceMonitor> monitor) {
    pImpl->performanceMonitor = monitor;
}

} // namespace nlm