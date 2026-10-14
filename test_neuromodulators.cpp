#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Acetylcholine.hpp"
#include "src/neuromodulation/Norepinephrine.hpp"
#include "src/neuromodulation/Serotonin.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Neuromodulator Implementation..." << std::endl;
    
    // Test Acetylcholine
    nlm::Acetylcholine ach;
    std::cout << "Acetylcholine created: " << ach.getName() << std::endl;
    ach.setLevel(0.5f);
    std::cout << "Acetylcholine level: " << ach.getLevel() << std::endl;
    ach.enhanceAttention(0.8f);
    ach.enhanceMemoryEncoding(0.7f);
    std::cout << "Acetylcholine attention modulation: " << ach.getLevel() * 1.5f << std::endl;
    
    // Test Norepinephrine
    nlm::Norepinephrine ne;
    std::cout << "\nNorepinephrine created: " << ne.getName() << std::endl;
    ne.setLevel(0.3f);
    std::cout << "Norepinephrine level: " << ne.getLevel() << std::endl;
    ne.enhanceSignalDetection(0.6f);
    ne.enhanceNoveltyEncoding(0.4f);
    std::cout << "Norepinephrine signal detection: " << ne.getLevel() * 0.5f << std::endl;
    
    // Test Serotonin
    nlm::Serotonin serotonin;
    std::cout << "\nSerotonin created: " << serotonin.getName() << std::endl;
    serotonin.setLevel(0.7f);
    std::cout << "Serotonin level: " << serotonin.getLevel() << std::endl;
    serotonin.modulateBehavioralFlexibility(0.5f);
    serotonin.modulateMood(0.6f);
    std::cout << "Serotonin behavioral flexibility: " << serotonin.getLevel() * 0.5f << std::endl;
    
    // Test Dopamine (existing)
    nlm::Dopamine dopamine;
    std::cout << "\nDopamine created: " << dopamine.getName() << std::endl;
    dopamine.setLevel(0.8f);
    std::cout << "Dopamine level: " << dopamine.getLevel() << std::endl;
    dopamine.signalReward(0.5f);
    std::cout << "Dopamine after reward: " << dopamine.getLevel() << std::endl;
    
    std::cout << "\nAll neuromodulator tests passed!" << std::endl;
    return 0;
}