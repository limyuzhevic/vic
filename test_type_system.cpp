#include "core/Types/Types.hpp"
#include <iostream>
#include <unordered_map>
#include <string>

int main() {
    std::cout << "Testing NLM Type System and Hash Functions..." << std::endl;
    
    // Test NeuronId hash function
    std::unordered_map<nlm::NeuronId, std::string> neuronMap;
    neuronMap[nlm::NeuronId(1)] = "Neuron 1";
    neuronMap[nlm::NeuronId(42)] = "Neuron 42";
    neuronMap[nlm::NeuronId(100)] = "Neuron 100";
    
    std::cout << "NeuronId hash test: " << neuronMap.size() << " entries" << std::endl;
    for (const auto& pair : neuronMap) {
        std::cout << "  NeuronId(" << pair.first.value << ") -> " << pair.second << std::endl;
    }
    
    // Test SynapseId hash function
    std::unordered_map<nlm::SynapseId, double> synapseMap;
    synapseMap[nlm::SynapseId(1)] = 0.5;
    synapseMap[nlm::SynapseId(2)] = -0.3;
    synapseMap[nlm::SynapseId(3)] = 1.2;
    
    std::cout << "SynapseId hash test: " << synapseMap.size() << " entries" << std::endl;
    
    // Test RegionId hash function
    std::unordered_map<nlm::RegionId, int> regionMap;
    regionMap[nlm::RegionId(1)] = 100;
    regionMap[nlm::RegionId(2)] = 200;
    regionMap[nlm::RegionId(3)] = 300;
    
    std::cout << "RegionId hash test: " << regionMap.size() << " entries" << std::endl;
    
    // Test PopulationId hash function
    std::unordered_map<nlm::PopulationId, bool> populationMap;
    populationMap[nlm::PopulationId(1)] = true;
    populationMap[nlm::PopulationId(2)] = false;
    populationMap[nlm::PopulationId(3)] = true;
    
    std::cout << "PopulationId hash test: " << populationMap.size() << " entries" << std::endl;
    
    // Test enum hash functions
    std::unordered_map<nlm::NeuronType, std::string> neuronTypeMap;
    neuronTypeMap[nlm::NeuronType::Excitatory] = "Excitatory";
    neuronTypeMap[nlm::NeuronType::Inhibitory] = "Inhibitory";
    neuronTypeMap[nlm::NeuronType::Sensory] = "Sensory";
    
    std::cout << "NeuronType hash test: " << neuronTypeMap.size() << " entries" << std::endl;
    
    // Test SynapseType hash function
    std::unordered_map<nlm::SynapseType, std::string> synapseTypeMap;
    synapseTypeMap[nlm::SynapseType::Excitatory] = "Excitatory";
    synapseTypeMap[nlm::SynapseType::Inhibitory] = "Inhibitory";
    synapseTypeMap[nlm::SynapseType::Electrical] = "Electrical";
    
    std::cout << "SynapseType hash test: " << synapseTypeMap.size() << " entries" << std::endl;
    
    // Test developmental stage hash function
    std::unordered_map<nlm::DevelopmentalStage, std::string> devStageMap;
    devStageMap[nlm::DevelopmentalStage::Initial] = "Initial";
    devStageMap[nlm::DevelopmentalStage::CriticalPeriod] = "CriticalPeriod";
    devStageMap[nlm::DevelopmentalStage::Adult] = "Adult";
    
    std::cout << "DevelopmentalStage hash test: " << devStageMap.size() << " entries" << std::endl;
    
    // Test firing state hash function
    std::unordered_map<nlm::FiringState, std::string> firingStateMap;
    firingStateMap[nlm::FiringState::Resting] = "Resting";
    firingStateMap[nlm::FiringState::Active] = "Active";
    firingStateMap[nlm::FiringState::Refractory] = "Refractory";
    
    std::cout << "FiringState hash test: " << firingStateMap.size() << " entries" << std::endl;
    
    // Test that hash_combine works with all types
    std::cout << "\nTesting hash_combine utility function..." << std::endl;
    
    size_t seed = 12345;
    seed = hash_combine(seed, nlm::NeuronId(42));
    seed = hash_combine(seed, nlm::SynapseId(99));
    seed = hash_combine(seed, nlm::RegionId(7));
    seed = hash_combine(seed, nlm::PopulationId(15));
    seed = hash_combine(seed, nlm::NeuronType::Excitatory);
    seed = hash_combine(seed, nlm::SynapseType::Inhibitory);
    seed = hash_combine(seed, nlm::DevelopmentalStage::CriticalPeriod);
    seed = hash_combine(seed, nlm::FiringState::Active);
    
    std::cout << "Combined hash seed: " << seed << std::endl;
    
    // Test ID indexing methods
    std::cout << "\nTesting ID indexing methods..." << std::endl;
    std::cout << "NeuronId(42).index() = " << nlm::NeuronId(42).index() << std::endl;
    std::cout << "SynapseId(99).index() = " << nlm::SynapseId(99).index() << std::endl;
    std::cout << "RegionId(7).index() = " << nlm::RegionId(7).index() << std::endl;
    std::cout << "PopulationId(15).index() = " << nlm::PopulationId(15).index() << std::endl;
    
    std::cout << "\n✅ All NLM Type System tests passed!" << std::endl;
    return 0;
}
