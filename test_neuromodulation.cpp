#include <iostream>
#include "src/neuromodulation/Neuromodulator.hpp"

int main() {
    std::cout << "Testing Neuromodulation Integration..." << std::endl;
    
    // Test if we can include the neuromodulation headers
    nlm::Dopamine* dopamine = new nlm::Dopamine();
    std::cout << "Dopamine: " << dopamine->getName() << ", level: " << dopamine->getLevel() << std::endl;
    
    delete dopamine;
    
    nlm::Acetylcholine* acetylcholine = new nlm::Acetylcholine();
    std::cout << "Acetylcholine: " << acetylcholine->getName() << ", level: " << acetylcholine->getLevel() << std::endl;
    
    delete acetylcholine;
    
    nlm::Norepinephrine* norepinephrine = new nlm::Norepinephrine();
    std::cout << "Norepinephrine: " << norepinephrine->getName() << ", level: " << norepinephrine->getLevel() << std::endl;
    
    delete norepinephrine;
    
    nlm::Serotonin* serotonin = new nlm::Serotonin();
    std::cout << "Serotonin: " << serotonin->getName() << ", level: " << serotonin->getLevel() << std::endl;
    
    delete serotonin;
    
    std::cout << "All neuromodulation systems created successfully!" << std::endl;
    return 0;
}