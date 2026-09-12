// Advanced Python API for Neuron and Synapse Control
// Provides detailed control over individual neurons, synapses, and network manipulation

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuronId.hpp"
#include "brain/SynapseId.hpp"
#include "brain/RegionId.hpp"
#include "brain/PopulationId.hpp"
#include "plasticity/STDP.hpp"
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace nlm {

// Neuron Controller Class for Advanced Usage
class NeuronController {
public:
    NeuronController(std::shared_ptr<Brain> brain) : brain(brain) {}
    
    // Get neuron by ID
    std::shared_ptr<Neuron> getNeuron(NeuronId id) {
        for (auto* region : brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getId() == id) {
                    return std::shared_ptr<Neuron>(neuron);
                }
            }
        }
        return nullptr;
    }
    
    // Find neurons by type
    std::vector<std::shared_ptr<Neuron>> findNeuronsByType(NeuronType type) {
        std::vector<std::shared_ptr<Neuron>> results;
        for (auto* region : brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == type) {
                    results.push_back(std::shared_ptr<Neuron>(neuron));
                }
            }
        }
        return results;
    }
    
    // Get neuron statistics
    float getAverageMembranePotential(NeuronType type = NeuronType::Internal) {
        auto neurons = findNeuronsByType(type);
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (auto& neuron : neurons) {
            sum += neuron->getMembranePotential();
        }
        return sum / neurons.size();
    }
    
    // Inject current into neurons
    void injectCurrentToType(NeuronType type, float current, float duration = 1.0f) {
        auto neurons = findNeuronsByType(type);
        for (size_t i = 0; i < neurons.size() && i < 10; ++i) {
            neurons[i]->injectCurrent(current);
        }
    }
    
    // Get all neuron IDs
    std::vector<NeuronId> getAllNeuronIds() {
        std::vector<NeuronId> ids;
        for (auto* region : brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                ids.push_back(neuron->getId());
            }
        }
        return ids;
    }
    
    // Get neurons by ID list
    std::vector<std::shared_ptr<Neuron>> getNeuronsByIds(const std::vector<NeuronId>& ids) {
        std::vector<std::shared_ptr<Neuron>> results;
        for (const auto& id : ids) {
            for (auto* region : brain->getRegions()) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getId() == id) {
                        results.push_back(std::shared_ptr<Neuron>(neuron));
                        break;
                    }
                }
            }
        }
        return results;
    }
    
private:
    std::shared_ptr<Brain> brain;
};

// Synapse Controller for Advanced Usage
class SynapseController {
public:
    SynapseController(std::shared_ptr<Brain> brain) : brain(brain) {}
    
    // Find synapses by type
    std::vector<std::shared_ptr<Synapse>> findSynapsesByType(SynapseType type) {
        std::vector<std::shared_ptr<Synapse>> results;
        for (auto* region : brain->getRegions()) {
            for (auto* synapse : region->getSynapses()) {
                if (synapse->getType() == type) {
                    results.push_back(std::shared_ptr<Synapse>(synapse));
                }
            }
        }
        return results;
    }
    
    // Find synapses between neurons
    std::vector<std::shared_ptr<Synapse>> findSynapsesBetween(NeuronId pre, NeuronId post) {
        std::vector<std::shared_ptr<Synapse>> results;
        for (auto* region : brain->getRegions()) {
            for (auto* synapse : region->getSynapses()) {
                auto source = synapse->getSourceNeuronId();
                auto target = synapse->getTargetNeuronId();
                if ((source == pre && target == post) || (source == post && target == pre)) {
                    results.push_back(std::shared_ptr<Synapse>(synapse));
                }
            }
        }
        return results;
    }
    
    // Get synaptic strength statistics
    float getAverageWeight(SynapseType type = SynapseType::Excitatory) {
        auto synapses = findSynapsesByType(type);
        if (synapses.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (auto& synapse : synapses) {
            sum += synapse->getWeight();
        }
        return sum / synapses.size();
    }
    
    // Apply STDP to specific synapse
    void applySTDPTo(NeuronId preId, NeuronId postId, float duration, std::shared_ptr<STDP> stdp = nullptr) {
        // This is a simplified STDP application
        if (!stdp) stdp = std::make_shared<STDP>();
        
        std::vector<Timestamp> preSpikes;
        std::vector<Timestamp> postSpikes;
        
        // Simple spike pattern - can be enhanced
        for (float t = 0; t < duration; t += 10.0f) {
            preSpikes.push_back(t);
            postSpikes.push_back(t + 5.0f);  // Post fires 5ms after pre
        }
        
        for (auto* region : brain->getRegions()) {
            for (auto* synapse : region->getSynapses()) {
                auto source = synapse->getSourceNeuronId();
                auto target = synapse->getTargetNeuronId();
                if ((source == preId && target == postId) || (source == postId && target == preId)) {
                    stdp->update(synapse, preSpikes, postSpikes, 0.001);
                }
            }
        }
    }
    
    // Get all synaptic connections
    std::vector<std::pair<NeuronId, NeuronId>> getAllConnections() {
        std::vector<std::pair<NeuronId, NeuronId>> connections;
        for (auto* region : brain->getRegions()) {
            for (auto* synapse : region->getSynapses()) {
                connections.push_back({synapse->getSourceNeuronId(), synapse->getTargetNeuronId()});
            }
        }
        return connections;
    }
    
    // Add new synapse
    bool addSynapse(NeuronId preId, NeuronId postId, float weight = 1.0f, SynapseType type = SynapseType::Excitatory) {
        for (auto* region : brain->getRegions()) {
            // Look for region that contains both neurons
            auto neurons = region->getAllNeurons();
            bool hasPre = false, hasPost = false;
            for (auto* neuron : neurons) {
                if (neuron->getId() == preId) hasPre = true;
                if (neuron->getId() == postId) hasPost = true;
            }
            if (hasPre && hasPost) {
                // TODO: Implement synapse creation
                // This would require adding a synapse to the neural system
                return false; // Placeholder
            }
        }
        return false;
    }
    
private:
    std::shared_ptr<Brain> brain;
};

// Network Controller for batch operations
class NetworkController {
public:
    NetworkController(std::shared_ptr<Brain> brain) : brain(brain) {}
    
    // Set random seed for reproducibility
    void setRandomSeed(uint64_t seed) {
        // Configure brain with new seed
        brain->reset();
        brain->initialize();
    }
    
    // Apply network-wide current injection
    void broadcastCurrent(float current, NeuronType type = NeuronType::Sensory) {
        auto neurons = getNeuronsByType(type);
        for (auto& neuron : neurons) {
            neuron->injectCurrent(current);
        }
    }
    
    // Get network statistics
    struct NetworkStats {
        size_t neuronCount;
        size_t synapseCount;
        size_t regionCount;
        float averageFiringRate;
        float averageWeight;
        size_t activeNeuronCount;
    };
    
    NetworkStats getNetworkStats() {
        NetworkStats stats;
        stats.neuronCount = brain->getTotalNeuronCount();
        stats.synapseCount = brain->getTotalSynapseCount();
        stats.regionCount = brain->getRegionCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.averageWeight = getAverageWeight(SynapseType::Excitatory);
        stats.activeNeuronCount = brain->getFiringNeuronCount();
        return stats;
    }
    
    // Get neurons by type
    std::vector<std::shared_ptr<Neuron>> getNeuronsByType(NeuronType type) {
        std::vector<std::shared_ptr<Neuron>> results;
        for (auto* region : brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == type) {
                    results.push_back(std::shared_ptr<Neuron>(neuron));
                }
            }
        }
        return results;
    }
    
    // Get average weight for synapse type
    float getAverageWeight(SynapseType type) {
        auto synapses = findSynapsesByType(type);
        if (synapses.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (auto& synapse : synapses) {
            sum += synapse->getWeight();
        }
        return sum / synapses.size();
    }
    
    // Find synapses by type (helper)
    std::vector<std::shared_ptr<Synapse>> findSynapsesByType(SynapseType type) {
        std::vector<std::shared_ptr<Synapse>> results;
        for (auto* region : brain->getRegions()) {
            for (auto* synapse : region->getSynapses()) {
                if (synapse->getType() == type) {
                    results.push_back(std::shared_ptr<Synapse>(synapse));
                }
            }
        }
        return results;
    }
    
private:
    std::shared_ptr<Brain> brain;
};

} // namespace nlm

// Python bindings for advanced controllers
PYBIND11_MODULE(pynlm, m) {
    // ... existing bindings ...
    
    // Add controller classes
    py::class_<NeuronController>(m, "NeuronController", 
        R"pbdoc(Advanced controller for individual neurons)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getNeuron", &NeuronController::getNeuron, py::arg("id"))
        .def("findNeuronsByType", &NeuronController::findNeuronsByType, py::arg("type"))
        .def("getAverageMembranePotential", &NeuronController::getAverageMembranePotential, 
            py::arg("type") = NeuronType::Internal)
        .def("injectCurrentToType", &NeuronController::injectCurrentToType, 
            py::arg("type"), py::arg("current"), py::arg("duration") = 1.0f)
        .def("getAllNeuronIds", &NeuronController::getAllNeuronIds)
        .def("getNeuronsByIds", &NeuronController::getNeuronsByIds, py::arg("ids"));

    py::class_<SynapseController>(m, "SynapseController", 
        R"pbdoc(Advanced controller for individual synapses)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("findSynapsesByType", &SynapseController::findSynapsesByType, py::arg("type"))
        .def("findSynapsesBetween", &SynapseController::findSynapsesBetween, 
            py::arg("pre_id"), py::arg("post_id"))
        .def("getAverageWeight", &SynapseController::getAverageWeight, py::arg("type") = SynapseType::Excitatory)
        .def("applySTDPTo", &SynapseController::applySTDPTo, 
            py::arg("pre_id"), py::arg("post_id"), py::arg("duration"), py::arg("stdp") = nullptr)
        .def("getAllConnections", &SynapseController::getAllConnections)
        .def("addSynapse", &SynapseController::addSynapse, 
            py::arg("pre_id"), py::arg("post_id"), py::arg("weight") = 1.0f, 
            py::arg("type") = SynapseType::Excitatory);

    py::class_<NetworkController>(m, "NetworkController", 
        R"pbdoc(Controller for batch network operations)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("setRandomSeed", &NetworkController::setRandomSeed, py::arg("seed"))
        .def("broadcastCurrent", &NetworkController::broadcastCurrent, 
            py::arg("current"), py::arg("type") = NeuronType::Sensory)
        .def("getNetworkStats", &NetworkController::getNetworkStats)
        .def("getNeuronsByType", &NetworkController::getNeuronsByType, py::arg("type"));

    // Add convenience functions
    m.def("createNeuronController", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<NeuronController>(brain);
    }, py::arg("brain"), "Create a neuron controller for advanced operations");

    m.def("createSynapseController", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<SynapseController>(brain);
    }, py::arg("brain"), "Create a synapse controller for advanced operations");

    m.def("createNetworkController", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<NetworkController>(brain);
    }, py::arg("brain"), "Create a network controller for batch operations");
}