// Synapse loading test file for SynapseLoader
#include "SynapseLoading.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace nlm {

void testSynapseLoader_basicFunctionality() {
    std::cout << "Testing SynapseLoader basic functionality..." << std::endl;
    
    // Create a simple brain with some neurons
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Create synapse checkpoint data
    SynapseCheckpointData checkpointData;
    
    // Add some test synapses
    checkpointData.sourceNeuron.push_back(0);  // Neuron ID 0
    checkpointData.destinationNeuron.push_back(1);  // Neuron ID 1
    checkpointData.weight.push_back(0.5f);
    checkpointData.delay.push_back(2);
    checkpointData.synapseType.push_back(static_cast<uint8_t>(SynapseType::Excitatory));
    checkpointData.plasticityFlags.push_back(0x03);  // Hebbian + STDP
    checkpointData.eligibilityTrace.push_back(0.1f);
    checkpointData.efficacy.push_back(1.0f);
    checkpointData.shortTermDepression.push_back(1.0f);
    checkpointData.shortTermFacilitation.push_back(0.0f);
    
    // Load synapses using SynapseLoader
    bool success = SynapseLoader::loadSynapses(checkpointData, &brain);
    
    assert(success == true);
    assert(SynapseLoader::getLoadingState().synapsesLoaded == 1);
    assert(SynapseLoader::getLoadingState().synapsesCreated == 1);
    
    std::cout << "✓ SynapseLoader basic functionality test passed" << std::endl;
}

void testSynapseValidation() {
    std::cout << "Testing synapse validation..." << std::endl;
    
    // Test self-connection validation
    SynapseCheckpointData badData;
    badData.sourceNeuron.push_back(0);
    badData.destinationNeuron.push_back(0);  // Self-connection
    badData.weight.push_back(0.5f);
    badData.delay.push_back(2);
    badData.synapseType.push_back(static_cast<uint8_t>(SynapseType::Excitatory));
    badData.plasticityFlags.push_back(0x03);
    badData.eligibilityTrace.push_back(0.1f);
    
    bool valid = validateSingleSynapseData(badData, 0);
    assert(valid == false);  // Should reject self-connections
    
    // Test weight bounds validation
    SynapseCheckpointData badData2;
    badData2.sourceNeuron.push_back(0);
    badData2.destinationNeuron.push_back(1);
    badData2.weight.push_back(2.0f);  // Weight > 1.0
    badData2.delay.push_back(2);
    badData2.synapseType.push_back(static_cast<uint8_t>(SynapseType::Excitatory));
    badData2.plasticityFlags.push_back(0x03);
    badData2.eligibilityTrace.push_back(0.1f);
    
    valid = validateSingleSynapseData(badData2, 0);
    assert(valid == false);  // Should reject weight > 1.0
    
    // Test valid synapse
    SynapseCheckpointData goodData;
    goodData.sourceNeuron.push_back(0);
    goodData.destinationNeuron.push_back(1);
    goodData.weight.push_back(0.3f);
    goodData.delay.push_back(2);
    goodData.synapseType.push_back(static_cast<uint8_t>(SynapseType::Excitatory));
    goodData.plasticityFlags.push_back(0x03);
    goodData.eligibilityTrace.push_back(0.1f);
    
    valid = validateSingleSynapseData(goodData, 0);
    assert(valid == true);  // Should accept valid synapse
    
    std::cout << "✓ Synapse validation test passed" << std::endl;
}

void testSynapseLookupTables() {
    std::cout << "Testing synapse lookup tables..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Verify lookup tables are initially empty
    assert(SynapseLoader::getLoadingState().synapsesLoaded == 0);
    
    // Setup lookup tables
    SynapseLoader::setupSynapseLookupTables(&brain);
    
    // Create test checkpoint data
    SynapseCheckpointData checkpointData;
    checkpointData.sourceNeuron.push_back(0);
    checkpointData.destinationNeuron.push_back(1);
    checkpointData.weight.push_back(0.5f);
    checkpointData.delay.push_back(2);
    checkpointData.synapseType.push_back(static_cast<uint8_t>(SynapseType::Excitatory));
    checkpointData.plasticityFlags.push_back(0x03);
    checkpointData.eligibilityTrace.push_back(0.1f);
    
    // Load synapses
    bool success = SynapseLoader::loadSynapses(checkpointData, &brain);
    assert(success == true);
    
    // Verify loading statistics
    auto state = SynapseLoader::getLoadingState();
    assert(state.synapsesLoaded == 1);
    assert(state.synapsesCreated == 1);
    
    std::cout << "✓ Synapse lookup tables test passed" << std::endl;
}

} // namespace nlm
