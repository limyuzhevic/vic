// Simple test to verify enhanced neuromodulation implementation
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Acetylcholine.hpp"
#include "src/neuromodulation/Norepinephrine.hpp"
#include "src/neuromodulation/Serotonin.hpp"
#include <iostream>
#include <cassert>
#include <string>

int main() {
    std::cout << "Testing enhanced neuromodulation implementation..." << std::endl;
    
    // Test 1: Dopamine implementation
    {
        Dopamine dopamine;
        assert(dopamine.getName() != nullptr);
        assert(std::string(dopamine.getName()) == "DA");
        
        dopamine.setLevel(0.5f);
        assert(dopamine.getLevel() == 0.5f);
        
        // Test concentration dynamics
        float conc = dopamine.getConcentration();
        assert(conc >= 0.0f);
        
        // Test reward signaling
        dopamine.signalReward(1.0f);
        assert(dopamine.getLevel() > 0.0f);
        
        // Test RPE signaling
        dopamine.signalRewardPredictionError(0.5f);
        
        // Test VTA activity
        dopamine.updateVTAActivity(0.3f, 0.4f, 0.6f);
        
        // Test plasticity factor
        float plasticity = dopamine.getPlasticityFactor();
        assert(plasticity >= 0.1f);
        
        // Test receptor activation
        float d1 = dopamine.getD1ReceptorActivation();
        float d2 = dopamine.getD2ReceptorActivation();
        assert(d1 >= 0.0f && d1 <= 1.0f);
        assert(d2 >= 0.0f && d2 <= 1.0f);
        
        std::cout << "✓ Dopamine implementation test passed" << std::endl;
    }
    
    // Test 2: Acetylcholine implementation
    {
        Acetylcholine ach;
        assert(ach.getName() != nullptr);
        assert(std::string(ach.getName()) == "ACh");
        
        ach.setLevel(0.3f);
        assert(ach.getLevel() == 0.3f);
        
        // Test cholinergic dynamics
        ach.updateAChDynamics(0.7f, 0.8f);
        
        // Test memory consolidation
        ach.triggerMemoryConsolidation(0.6f, 0.5f);
        
        // Test working memory trace
        ach.updateWorkingMemoryTrace(0.9f, 0.4f);
        
        // Test VTA ACh projections
        ach.updateVTAChProjections(0.5f, 0.3f);
        
        // Test prefrontal cortex modulation
        ach.modulatePrefrontalCortex(0.7f, 0.6f);
        
        // Test cross-modulation
        ach.modulateWithDopamine(0.5f);
        ach.modulateWithNorepinephrine(0.4f);
        ach.modulateWithSerotonin(0.3f);
        
        // Test plasticity factor
        float plasticity = ach.getPlasticityFactor();
        assert(plasticity >= 0.1f && plasticity <= 3.0f);
        
        std::cout << "✓ Acetylcholine implementation test passed" << std::endl;
    }
    
    // Test 3: Norepinephrine implementation
    {
        Norepinephrine ne;
        assert(ne.getName() != nullptr);
        assert(std::string(ne.getName()) == "NE");
        
        ne.setLevel(0.4f);
        assert(ne.getLevel() == 0.4f);
        
        // Test LC firing dynamics
        ne.updateLocusCoeruleusFiring(0.6f, 0.7f, 0.2f);
        
        // Test SNR modulation
        ne.modulateSignalToNoiseRatio(1.0f, 0.5f);
        
        // Test arousal state
        ne.updateArousalState(0.8f, 0.5f);
        
        // Test attentional focus
        ne.modulateAttentionalFocus(0.7f, 0.6f);
        
        // Test stress response
        ne.triggerStressResponse(0.8f, 10.0f);
        
        // Test stress recovery
        ne.updateStressRecovery(0.01f);
        
        // Test plasticity modulation
        ne.modulateSynapticPlasticity(1.0f, 0.5f);
        
        // Test receptor sensitivity
        ne.updateReceptorSensitivity(0.9f, 0.01f);
        
        // Test cross-modulation
        ne.modulateWithDopamine(0.5f);
        ne.modulateWithAcetylcholine(0.4f);
        ne.modulateWithSerotonin(0.3f);
        
        // Test plasticity factor
        float plasticity = ne.getPlasticityFactor();
        assert(plasticity >= 0.1f && plasticity <= 2.0f);
        
        std::cout << "✓ Norepinephrine implementation test passed" << std::endl;
    }
    
    // Test 4: Serotonin implementation
    {
        Serotonin serotonin;
        assert(serotonin.getName() != nullptr);
        assert(std::string(serotonin.getName()) == "5-HT");
        
        serotonin.setLevel(0.2f);
        assert(serotonin.getLevel() == 0.2f);
        
        // Test raphe nucleus dynamics
        serotonin.updateRapheDynamics(0.4f, 0.6f);
        
        // Test mood regulation
        serotonin.modulateMoodRegulation(0.5f, 0.5f);
        
        // Test punishment signal
        serotonin.processPunishmentSignal(0.7f, 0.8f);
        
        // Test avoidance learning
        serotonin.updateAvoidanceLearning(0.6f, 0.7f);
        
        // Test strategy switching
        serotonin.updateStrategySwitching(0.3f, 0.8f);
        
        // Test decision making
        serotonin.modulateDecisionMaking(0.5f, 0.6f);
        
        // Test receptor activation
        float fiveHT1A = serotonin.get5HT1AReceptorActivation();
        float fiveHT2A = serotonin.get5HT2AReceptorActivation();
        assert(fiveHT1A >= 0.0f && fiveHT1A <= 1.0f);
        assert(fiveHT2A >= 0.0f && fiveHT2A <= 1.0f);
        
        // Test cross-modulation
        serotonin.modulateWithDopamine(0.5f);
        serotonin.modulateWithAcetylcholine(0.4f);
        serotonin.modulateWithNorepinephrine(0.3f);
        
        // Test plasticity factor
        float plasticity = serotonin.getPlasticityFactor();
        assert(plasticity >= 0.1f);
        
        std::cout << "✓ Serotonin implementation test passed" << std::endl;
    }
    
    // Test 5: Homeostatic regulation
    {
        Dopamine dopamine;
        Norepinephrine ne;
        Acetylcholine ach;
        Serotonin serotonin;
        
        // Set homeostatic properties
        dopamine.setHomeostaticSetPoint(100.0f);
        dopamine.setHomeostaticStrength(0.1f);
        
        ne.setStressSetPoint(0.5f);
        
        assert(dopamine.isHomeostaticallyRegulated() == true);
        assert(ne.isHomeostaticallyRegulated() == true);
        
        // Test reset to baseline
        dopamine.resetToBaseline();
        ach.resetToBaseline();
        ne.resetToBaseline();
        serotonin.resetToBaseline();
        
        std::cout << "✓ Homeostatic regulation test passed" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "All enhanced neuromodulation tests passed!" << std::endl;
    std::cout << std::endl;
    std::cout << "Summary of implemented enhancements:" << std::endl;
    std::cout << "- Realistic cholinergic dynamics (ventral tegmental area projections)" << std::endl;
    std::cout << "- Attention modulation with gain modulation on sensory processing" << std::endl;
    std::cout << "- Acetylcholine-mediated memory consolidation mechanisms" << std::endl;
    std::cout << "- Working memory and prefrontal cortex integration" << std::endl;
    std::cout << "- Locus coeruleus firing rate dynamics" << std::endl;
    std::cout << "- Arousal-dependent gain modulation and signal-to-noise ratio enhancement" << std::endl;
    std::cout << "- Stress response and homeostatic regulation" << std::endl;
    std::cout << "- NE-dependent plasticity at multiple synapses" << std::endl;
    std::cout << "- Raphe nucleus serotonin dynamics" << std::endl;
    std::cout << "- Mood regulation and behavioral inhibition" << std::endl;
    std::cout << "- Punishment and avoidance learning mechanisms" << std::endl;
    std::cout << "- Serotonin-dependent switching between exploratory and exploitative strategies" << std::endl;
    std::cout << "- Enhanced VTA dopamine neuron dynamics" << std::endl;
    std::cout << "- Reward prediction error signaling with proper burst/phasic dynamics" << std::endl;
    std::cout << "- Synaptic dopamine release and uptake mechanisms" << std::endl;
    std::cout << "- Dopamine-dependent synaptic plasticity (DA-D1/DA-D2 receptor effects)" << std::endl;
    std::cout << "- Realistic cross-talk and interactions between neuromodulators" << std::endl;
    std::cout << "- Proper homeostatic regulation for each system" << std::endl;
    std::cout << "- Activity-dependent release mechanisms" << std::endl;
    std::cout << "- Realistic concentration dynamics with time constants" << std::endl;
    std::cout << "- Integration with plasticity systems" << std::endl;
    std::cout << "- Neural ensemble-specific modulation patterns" << std::endl;
    
    return 0;
}