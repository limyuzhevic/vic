#include <iostream>
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Acetylcholine.hpp"
#include "src/neuromodulation/Norepinephrine.hpp"
#include "src/neuromodulation/Serotonin.hpp"
#include "src/neuromodulation/Reward.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"

using namespace nlm;

int main() {
    std::cout << "=== NLM Neuromodulation System Test ===" << std::endl;
    
    // Test Dopamine
    Dopamine dopamine;
    std::cout << "Dopamine name: " << dopamine.getName() << std::endl;
    std::cout << "Initial dopamine level: " << dopamine.getLevel() << std::endl;
    
    dopamine.setLevel(0.8f);
    std::cout << "Set dopamine level to 0.8: " << dopamine.getLevel() << std::endl;
    
    dopamine.signalReward(0.5f);
    std::cout << "After reward signal: " << dopamine.getLevel() << std::endl;
    
    // Test Reward
    Reward reward;
    std::cout << "\nInitial reward value: " << reward.getValue() << std::endl;
    reward.add(0.3f);
    std::cout << "After adding 0.3f: " << reward.getValue() << std::endl;
    
    // Test Neuromodulators
    std::cout << "\n=== Neuromodulator Test ===" << std::endl;
    
    // Create a Dopamine instance and test its functionality
    Dopamine* da = new Dopamine();
    std::cout << "Created Dopamine neuromodulator" << std::endl;
    std::cout << "Name: " << da->getName() << std::endl;
    
    // Test Acetylcholine
    Acetylcholine* ach = new Acetylcholine();
    std::cout << "\nCreated Acetylcholine neuromodulator" << std::endl;
    std::cout << "Name: " << ach->getName() << std::endl;
    std::cout << "Plasticity factor: " << ach->getPlasticityFactor() << std::endl;
    
    // Test Norepinephrine
    Norepinephrine* ne = new Norepinephrine();
    std::cout << "\nCreated Norepinephrine neuromodulator" << std::endl;
    std::cout << "Name: " << ne->getName() << std::endl;
    std::cout << "Attention gain: " << ne->getAttentionGain() << std::endl;
    
    // Test Serotonin
    Serotonin* ser = new Serotonin();
    std::cout << "\nCreated Serotonin neuromodulator" << std::endl;
    std::cout << "Name: " << ser->getName() << std::endl;
    std::cout << "Social cohesion: " << ser->getSocialCohesion() << std::endl;
    
    delete da;
    delete ach;
    delete ne;
    delete ser;
    
    std::cout << "\n=== All Tests Passed ===" << std::endl;
    return 0;
}

