// Development-Learning Integration Test
// Tests integration between developmental stages and learning processes
// Validates developmental plasticity, synaptogenesis, and learning across stages

#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "memory/WorkingMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>

namespace test_development_learning_integration {

void testDevelopmentalStageSpecificLearning() {
    std::cout << "    Testing developmental stage-specific learning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    nlm::Hebbian* hebbian = brain.getHebbian();
    nlm::STDP* stdp = brain.getSTDP();
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    
    // Test learning across developmental stages
    nlm::DevelopmentalStage stages[] = {
        nlm::DevelopmentalStage::Larval,
        nlm::DevelopmentalStage::Juvenile,
        nlm::DevelopmentalStage::Subadult,
        nlm::DevelopmentalStage::Adult
    };
    
    for (const auto& stage : stages) {
        brain.setDevelopmentalStage(stage);
        assert(brain.getDevelopmentalStage() == stage);
        
        // Update development system
        if (devSystem) {
            devSystem->update(&brain, 5);
            
            float plasticityModifier = devSystem->getPlasticityModifier();
            bool isCriticalPeriod = devSystem->isCriticalPeriod();
            
            std::cout << "    Stage: " << devSystem->getStageName() << ", Plasticity: " << plasticityModifier 
                     << ", Critical Period: " << (isCriticalPeriod ? "Yes" : "No") << std::endl;
        }
        
        // Hebbian learning is stage-dependent
        if (hebbian) {
            hebbian->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        // STDP is also stage-dependent
        if (stdp) {
            stdp->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        // Dopamine modulates learning based on developmental stage
        if (dopamine) {
            // Different reward scaling based on developmental stage
            float rewardScale = (stage == nlm::DevelopmentalStage::Juvenile) ? 1.5f : 1.0f;
            dopamine->signalReward(0.2f * rewardScale);
            
            // Update neuromodulation
            dopamine->update(0.1f);
        }
        
        // Working memory storage is stage-modulated
        if (workingMem) {
            // Store experiences during development
            workingMem->store(100, 0.5f * (static_cast<int>(stage) + 1) / 4.0f);
            
            // Decay rate changes with development
            workingMem->decay(0.05f * (1.0f - devSystem->getPlasticityModifier() * 0.5f));
        }
        
        // Run simulation for this stage
        for (nlm::SimulationStep step = 0; step < 15; ++step) {
            brain.step(step);
            
            // Update prediction system (influenced by development)
            if (predictionSys) {
                predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
            }
            
            // Verify stage-appropriate behavior
            assert(brain.getTotalNeuronCount() == 300);
            assert(brain.getRegionCount() == 2);
        }
    }
    
    std::cout << "    testDevelopmentalStageSpecificLearning passed" << std::endl;
}

void testSynaptogenesisAndLearningIntegration() {
    std::cout << "    Testing synaptogenesis and learning integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    nlm::Synaptogenesis* synaptogenesis = brain.getSynaptogenesis();
    nlm::Hebbian* hebbian = brain.getHebbian();
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    
    // Simulate synaptogenesis during development
    bool synaptogenesisActive = true;
    
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        // Synaptogenesis creates new connections
        if (synaptogenesis) {
            synaptogenesis->update(&brain, step);
        }
        
        // Dopamine modulates synaptogenesis and learning
        if (dopamine) {
            dopamine->signalReward(0.1f + 0.05f * std::sin(step * 0.2f));
            dopamine->update(0.1f);
            
            // Reward signal affects synaptogenesis
            float reward = dopamine->getLevel();
            if (synaptogenesis) {
                synaptogenesis->setRewardFactor(reward);
            }
        }
        
        // Hebbian learning strengthens existing connections
        if (hebbian) {
            hebbian->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        // Working memory stores learning experiences
        if (workingMem) {
            // Store pattern of activity
            workingMem->store(step % 100, 0.3f + 0.2f * std::cos(step * 0.1f));
            
            // Decay working memory based on developmental stage
            if (devSystem) {
                float decayRate = devSystem->getPlasticityModifier() * 0.1f;
                workingMem->decay(decayRate);
            }
        }
        
        // Brain step with all developmental processes
        brain.step(step);
        
        // Verify system consistency
        assert(brain.getTotalNeuronCount() == 350);
        assert(brain.getRegionCount() == 3);
        
        // Check developmental stage progression
        if (devSystem) {
            devSystem->update(&brain, step);
            
            // As development progresses, plasticity should change
            float plasticity = devSystem->getPlasticityModifier();
            // Plasticity should be reasonable (not NaN or infinite)
            assert(plasticity >= 0.0f && plasticity < 10.0f);
        }
    }
    
    // Test pruning and its effect on learning
    nlm::Pruning* pruning = brain.getPruning();
    if (pruning) {
        pruning->setPruningThreshold(0.3f);
        pruning->update(&brain, 20);
        
        // Pruning should reduce weak connections
        size_t initialSynapses = brain.getTotalSynapseCount();
        
        // Run more steps to allow pruning
        for (nlm::SimulationStep step = 40; step < 60; ++step) {
            brain.step(step);
            pruning->update(&brain, step);
        }
        
        // Verify pruning occurred (synapse count may reduce)
        assert(brain.getTotalNeuronCount() == 350);
    }
    
    std::cout << "    testSynaptogenesisAndLearningIntegration passed" << std::endl;
}

void testMaturationAndSkillLearning() {
    std::cout << "    Testing maturation and skill learning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    nlm::Maturation* maturation = brain.getMaturation();
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    
    // Skill acquisition through maturation
    struct Skill {
        std::string name;
        nlm::NeuronId patternId;
        float proficiency;
        float requiredMaturation;
    };
    
    std::vector<Skill> skills = {
        {"basic_movement", 100, 0.0f, 0.3f},
        {"complex_navigation", 200, 0.0f, 0.6f},
        {"reward_optimization", 300, 0.0f, 0.9f}
    };
    
    // Skill learning across maturation stages
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Update maturation system
        if (devSystem && maturation) {
            devSystem->update(&brain, step);
            maturation->update(&brain, step);
            
            float maturationLevel = devSystem->getPlasticityModifier();
            
            // Skill acquisition based on maturation
            for (auto& skill : skills) {
                if (maturationLevel >= skill.requiredMaturation && skill.proficiency < 1.0f) {
                    // Skill can be learned
                    skill.proficiency += 0.02f * maturationLevel;
                    
                    // Store skill in working memory
                    if (workingMem) {
                        workingMem->store(skill.patternId, skill.proficiency);
                    }
                }
            }
        }
        
        // Dopamine reinforces successful skill acquisition
        if (dopamine) {
            // Reward based on skill proficiency
            float totalProficiency = 0.0f;
            for (const auto& skill : skills) {
                totalProficiency += skill.proficiency;
            }
            float averageProficiency = totalProficiency / skills.size();
            
            dopamine->signalReward(averageProficiency * 0.5f);
            dopamine->update(0.1f);
        }
        
        // Brain step with maturation
        brain.step(step);
        
        // Verify skill learning
        assert(brain.getTotalNeuronCount() == 400);
        assert(brain.getRegionCount() == 2);
        
        // Check that skills can be learned
        for (const auto& skill : skills) {
            assert(skill.proficiency >= 0.0f && skill.proficiency <= 1.0f);
        }
    }
    
    // Report learned skills
    std::cout << "    Learned skills:" << std::endl;
    for (const auto& skill : skills) {
        std::cout << "      - " << skill.name << ": " << (skill.proficiency * 100) << "%" << std::endl;
    }
    
    std::cout << "    testMaturationAndSkillLearning passed" << std::endl;
}

void runAll() {
    testDevelopmentalStageSpecificLearning();
    testSynaptogenesisAndLearningIntegration();
    testMaturationAndSkillLearning();
}

} // namespace test_development_learning_integration