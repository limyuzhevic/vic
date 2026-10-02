#include "neuromodulation/AdvancedNeuromodulation.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void testAdvancedNeuromodulation() {
    std::cout << "=== Testing Advanced Neuromodulation ===" << std::endl;
    
    // Test 1: Advanced neuromodulator creation
    std::cout << "1. Testing advanced neuromodulator creation..." << std::endl;
    
    auto dopamineEffect = nlm::NeuromodulationUtils::createSophisticatedEffect(
        nlm::NeuromodulationType::Dopamine,
        1.0f,  // baseStrength
        0.8f,  // inputGain
        0.05f, // adaptationTime
        0.8f,  // saturationLevel
        0.3f   // inhibitionThreshold
    );
    
    auto serotoninEffect = nlm::NeuromodulationUtils::createSophisticatedEffect(
        nlm::NeuromodulationType::Serotonin,
        0.7f,
        0.6f,
        0.1f,
        0.7f,
        0.4f
    );
    
    auto acetylcholineEffect = nlm::NeuromodulationUtils::createSophisticatedEffect(
        nlm::NeuromodulationType::Acetylcholine,
        0.8f,
        0.7f,
        0.08f,
        0.75f,
        0.35f
    );
    
    auto norepinephrineEffect = nlm::NeuromodulationUtils::createSophisticatedEffect(
        nlm::NeuromodulationType::Norepinephrine,
        0.85f,
        0.75f,
        0.06f,
        0.85f,
        0.25f
    );
    
    auto adrenalineEffect = nlm::NeuromodulationUtils::createSophisticatedEffect(
        nlm::NeuromodulationType::Adrenaline,
        1.2f,
        0.9f,
        0.03f,
        0.9f,
        0.2f
    );
    
    std::cout << "   Created 5 neuromodulator effects" << std::endl;
    
    // Test 2: Advanced neuromodulator instantiation
    std::cout << "2. Testing advanced neuromodulator instantiation..." << std::endl;
    
    nlm::AdvancedNeuromodulator dopamine(nlm::NeuromodulationType::Dopamine, dopamineEffect);
    nlm::AdvancedNeuromodulator serotonin(nlm::NeuromodulationType::Serotonin, serotoninEffect);
    nlm::AdvancedNeuromodulator acetylcholine(nlm::NeuromodulationType::Acetylcholine, acetylcholineEffect);
    nlm::AdvancedNeuromodulator norepinephrine(nlm::NeuromodulationType::Norepinephrine, norepinephrineEffect);
    nlm::AdvancedNeuromodulator adrenaline(nlm::NeuromodulationType::Adrenaline, adrenalineEffect);
    
    std::cout << "   Created 5 advanced neuromodulators" << std::endl;
    std::cout << "   Dopamine type: " << static_cast<int>(dopamine.getType()) << std::endl;
    std::cout << "   Serotonin type: " << static_cast<int>(serotonin.getType()) << std::endl;
    std::cout << "   Acetylcholine type: " << static_cast<int>(acetylcholine.getType()) << std::endl;
    std::cout << "   Norepinephrine type: " << static_cast<int>(norepinephrine.getType()) << std::endl;
    std::cout << "   Adrenaline type: " << static_cast<int>(adrenaline.getType()) << std::endl;
    
    // Test 3: Neuromodulation update simulation
    std::cout << "3. Testing neuromodulation update simulation..." << std::endl;
    
    // Simulate update over time
    for (int i = 0; i < 10; ++i) {
        dopamine.update(0.1f);
        serotonin.update(0.1f);
        acetylcholine.update(0.1f);
        norepinephrine.update(0.1f);
        adrenaline.update(0.1f);
        
        // Apply prediction errors
        dopamine.updateFromPredictionError((i % 3 == 0) ? 0.5f : -0.2f, 0.1f);
        serotonin.updateFromRewardPrediction((i % 4 == 0) ? 1.0f : 0.0f, 0.1f);
        acetylcholine.updateFromNovelty((i % 5 == 0) ? 0.8f : 0.1f, 0.1f);
        
        std::cout << "   Step " << i << ": Dopamine=" << dopamine.getLevel() << 
                  ", Serotonin=" << serotonin.getLevel() << 
                  ", Acetylcholine=" << acetylcholine.getLevel() << "; "
                  "Norepinephrine=" << norepinephrine.getLevel() << "; "
                  "Adrenaline=" << adrenaline.getLevel() << std::endl;
    }
    
    // Test 4: Neuromodulation effects on plasticity
    std::cout << "4. Testing neuromodulation effects on plasticity..." << std::endl;
    
    std::cout << "   Dopamine plasticity factor: " << dopamine.getPlasticityFactor() << std::endl;
    std::cout << "   Serotonin plasticity factor: " << serotonin.getPlasticityFactor() << std::endl;
    std::cout << "   Acetylcholine plasticity factor: " << acetylcholine.getPlasticityFactor() << std::endl;
    std::cout << "   Norepinephrine plasticity factor: " << norepinephrine.getPlasticityFactor() << std::endl;
    std::cout << "   Adrenaline plasticity factor: " << adrenaline.getPlasticityFactor() << std::endl;
    
    std::cout << "   Dopamine excitability factor: " << dopamine.getExcitabilityFactor() << std::endl;
    std::cout << "   Serotonin excitability factor: " << serotonin.getExcitabilityFactor() << std::endl;
    std::cout << "   Acetylcholine excitability factor: " << acetylcholine.getExcitabilityFactor() << std::endl;
    std::cout << "   Norepinephrine excitability factor: " << norepinephrine.getExcitabilityFactor() << std::endl;
    std::cout << "   Adrenaline excitability factor: " << adrenaline.getExcitabilityFactor() << std::endl;
    
    // Test 5: Neuromodulation manager
    std::cout << "5. Testing neuromodulation manager..." << std::endl;
    
    nlm::AdvancedNeuromodulationManager manager;
    
    manager.addNeuromodulator("dopamine", nlm::NeuromodulationType::Dopamine, dopamineEffect);
    manager.addNeuromodulator("serotonin", nlm::NeuromodulationType::Serotonin, serotoninEffect);
    manager.addNeuromodulator("acetylcholine", nlm::NeuromodulationType::Acetylcholine, acetylcholineEffect);
    manager.addNeuromodulator("norepinephrine", nlm::NeuromodulationType::Norepinephrine, norepinephrineEffect);
    manager.addNeuromodulator("adrenaline", nlm::NeuromodulationType::Adrenaline, adrenalineEffect);
    
    std::cout << "   Neuromodulator manager initialized with 5 neuromodulators" << std::endl;
    
    // Test manager statistics
    auto stats = manager.getStats();
    std::cout << "   Manager statistics:" << std::endl;
    std::cout << "     Total neuromodulators: " << stats.totalNeuromodulators << std::endl;
    std::cout << "     Average level: " << stats.averageLevel << std::endl;
    std::cout << "     Min level: " << stats.minLevel << std::endl;
    std::cout << "     Max level: " << stats.maxLevel << std::endl;
    std::cout << "     Plasticity impact: " << stats.plasticityImpact << std::endl;
    std::cout << "     Excitability impact: " << stats.excitabilityImpact << std::endl;
    
    // Test 6: Preset effects
    std::cout << "6. Testing preset effects..." << std::endl;
    
    auto presets = nlm::AdvancedNeuromodulationManager::getPresetEffects();
    std::cout << "   Available presets: " << presets.size() << std::endl;
    
    for (const auto& pair : presets) {
        std::cout << "     " << pair.first << ": strength=" << pair.second.baseStrength << ", gain=" << pair.second.inputGain << std::endl;
    }
    
    // Test 7: Integrated configuration
    std::cout << "7. Testing integrated configuration..." << std::endl;
    
    auto integrated = nlm::NeuromodulationUtils::createIntegratedConfiguration();
    std::cout << "   Integrated configuration with " << integrated.size() << " neuromodulators" << std::endl;
    
    float synergy = nlm::NeuromodulationUtils::calculateNeuromodulationSynergy(integrated);
    std::cout << "   Neuromodulation synergy: " << synergy << std::endl;
    
    // Test 8: Neuromodulation schedules
    std::cout << "8. Testing neuromodulation schedules..." << std::endl;
    
    auto learningSchedule = nlm::NeuromodulationUtils::createLearningSchedule();
    auto emergencySchedule = nlm::NeuromodulationUtils::createEmergencySchedule();
    auto restorationSchedule = nlm::NeuromodulationUtils::createRestorationSchedule();
    
    std::cout << "   Learning schedule name: " << learningSchedule.name << std::endl;
    std::cout << "   Emergency schedule name: " << emergencySchedule.name << std::endl;
    std::cout << "   Restoration schedule name: " << restorationSchedule.name << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "=== NLM Advanced Neuromodulation Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testAdvancedNeuromodulation();
        
        std::cout << "=== All neuromodulation tests completed ===" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Neuromodulation test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
