#include "neuromodulation/Neuromodulator.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

int main() {
    std::cout << "Testing ACh and NE neuromodulator implementations..." << std::endl;
    
    // Test ACh
    Acetylcholine ach;
    assert(std::string(ach.getName()) == "ACh");
    assert(ach.getLevel() == 0.0f);
    assert(ach.getAttentionLevel() == 0.0f);
    assert(ach.getPlasticityFactor() == 0.5f);
    
    ach.setLevel(0.5f);
    assert(ach.getLevel() == 0.5f);
    assert(ach.getAttentionLevel() == 0.0f);
    
    ach.signalAttention(0.8f);
    assert(ach.getAttentionLevel() == 0.8f);
    
    ach.update(0.001f);
    assert(ach.getLevel() == 0.5f);
    assert(ach.getAttentionLevel() < 0.8f);
    
    std::cout << "    ACh implementation: PASSED" << std::endl;
    
    // Test NE
    Norepinephrine ne;
    assert(std::string(ne.getName()) == "NE");
    assert(ne.getLevel() == 0.0f);
    assert(ne.getArousalLevel() == 0.0f);
    assert(ne.getAlertnessLevel() == 0.0f);
    assert(ne.getPlasticityFactor() == 0.5f);
    
    ne.setLevel(0.7f);
    assert(ne.getLevel() == 0.7f);
    
    ne.signalNovelty(0.9f);
    assert(ne.getArousalLevel() == 0.9f);
    assert(ne.getAlertnessLevel() == 0.72f);
    
    ne.signalAlertness(0.5f);
    assert(ne.getAlertnessLevel() == 1.22f);
    
    ne.update(0.001f);
    assert(ne.getLevel() == 0.7f);
    assert(ne.getArousalLevel() < 0.9f);
    assert(ne.getAlertnessLevel() < 1.22f);
    
    std::cout << "    NE implementation: PASSED" << std::endl;
    
    // Test ACh and NE integration
    assert(ach.getPlasticityFactor() == 0.75f);
    assert(ne.getPlasticityFactor() == 0.85f);
    
    std::cout << "\nAll tests passed! ACh and NE neuromodulation systems implemented successfully." << std::endl;
    return 0;
}
