#include "neuromodulation/Neuromodulator.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

using namespace nlm;

int main() {
    std::cout << "Testing ACh and NE neuromodulator implementations..." << std::endl;
    
    // Test ACh implementation
    {
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
    }
    
    // Test NE implementation
    {
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
    }
    
    // Test ACh and NE integration with brain
    {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int>(1), ConfigSource::Default);
        
        Brain brain(config);
        brain.initialize();
        
        assert(brain.getAcetylcholine() != nullptr);
        assert(brain.getNorepinephrine() != nullptr);
        
        // Test ACh effects on brain
        brain.getAcetylcholine()->setLevel(0.8f);
        brain.getAcetylcholine()->signalAttention(0.6f);
        assert(brain.getAcetylcholine()->getAttentionLevel() == 0.6f);
        
        // Test NE effects on brain
        brain.getNorepinephrine()->setLevel(0.9f);
        brain.getNorepinephrine()->signalNovelty(0.7f);
        assert(brain.getNorepinephrine()->getArousalLevel() == 0.7f);
        
        std::cout << "    ACh/NE integration with Brain: PASSED" << std::endl;
    }
    
    std::cout << "\nAll tests passed! ACh and NE neuromodulation systems implemented successfully." << std::endl;
    return 0;
}
