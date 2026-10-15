// Prediction-Cognition Integration Test
// Tests integration between prediction systems and cognitive functions
// Validates predictive coding, action planning, and concept formation

#include "brain/Brain.hpp"
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "motor/Action.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>

namespace test_prediction_cognition_integration {

void testPredictiveCodingAndActionPlanning() {
    std::cout << "    Testing predictive coding and action planning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    nlm::Dopamine* dopamine = brain.getDopamine();
    
    // Test 1: Predictive coding drives action planning
    for (nlm::SimulationStep step = 0; step < 25; ++step) {
        double currentTime = step * 0.1;
        
        // Get current neural state for prediction
        size_t totalNeurons = brain.getTotalNeuronCount();
        float avgFiringRate = brain.getAverageFiringRate();
        
        // Update prediction system with current state
        if (predictionSys) {
            predictionSys->update(totalNeurons, avgFiringRate);
        }
        
        // Generate prediction error
        if (predError && predictionSys) {
            float predictionError = predictionSys->getPredictionError();
            predError->setPredictionError(predictionError);
        }
        
        // Use prediction error to update action planning
        if (planner && predError) {
            float error = predError->getPredictionError();
            if (error > 0.0f) {
                planner->update(totalNeurons, avgFiringRate, error);
            } else {
                planner->update(totalNeurons, avgFiringRate);
            }
        }
        
        // Generate planned actions
        if (planner) {
            auto plan = planner->getPlan();
            
            // Concept formation influences action parameters
            if (conceptForm) {
                conceptForm->update(totalNeurons, brain.getFiringNeuronCount());
                auto concepts = conceptForm->getActiveConcepts();
                
                // Create actions based on plan
                if (!plan.empty() && !concepts.empty()) {
                    for (const auto& concept : concepts) {
                        if (!concept.empty()) {
                            auto action = brain.produceAction();
                            if (action) {
                                // Modify action parameters based on concepts
                                std::vector<float> params = action->getParameters();
                                for (auto& param : params) {
                                    param *= (1.0f + concepts.size() * 0.05f);
                                }
                                action->setParameters(params);
                            }
                        }
                    }
                }
            }
        }
        
        // Apply neuromodulation based on prediction error
        if (dopamine && predError) {
            float error = predError->getPredictionError();
            dopamine->signalRewardPredictionError(error);
        }
        
        // Brain processes everything
        brain.step(step, currentTime);
        
        // Verify integration
        assert(brain.getTotalNeuronCount() == 350);
        assert(brain.getRegionCount() == 2);
        
        // Check prediction system state
        if (predictionSys) {
            assert(predictionSys->getPredictionError() >= 0.0f);
        }
    }
    
    std::cout << "    testPredictiveCodingAndActionPlanning passed" << std::endl;
}

void testConceptFormationAndPredictionLoop() {
    std::cout << "    Testing concept formation and prediction loop..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    nlm::NeuralPlanner* planner = brain.getPlanner();
    
    // Create a learning loop where concepts influence predictions
    std::vector<std::string> learnedConcepts;
    
    for (nlm::SimulationStep step = 0; step < 35; ++step) {
        // Update concept formation with neural activity
        if (conceptForm) {
            conceptForm->update(brain.getTotalNeuronCount(), brain.getFiringNeuronCount());
            
            auto concepts = conceptForm->getActiveConcepts();
            learnedConcepts.insert(learnedConcepts.end(), concepts.begin(), concepts.end());
            
            // Concepts provide structure for prediction
            if (!concepts.empty()) {
                // Use concepts to modulate prediction
                if (predictionSys) {
                    float conceptInfluence = std::min(1.0f, concepts.size() * 0.1f);
                    predictionSys->update(brain.getTotalNeuronCount(), 
                                        brain.getAverageFiringRate(), 
                                        conceptInfluence);
                }
            }
        }
        
        // Prediction system makes predictions based on concepts
        if (predictionSys) {
            predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
            
            // Get prediction error
            float error = predictionSys->getPredictionError();
            
            // Use prediction error to refine concepts
            if (conceptForm && error > 0.0f) {
                conceptForm->update(brain.getTotalNeuronCount(), brain.getFiringNeuronCount(), error);
            }
        }
        
        // Planner uses concepts and predictions for action selection
        if (planner) {
            planner->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
            
            auto plan = planner->getPlan();
            
            // Create actions based on plan
            if (!plan.empty()) {
                auto action = brain.produceAction();
                if (action) {
                    // Action parameters can be influenced by concepts
                    if (conceptForm) {
                        auto concepts = conceptForm->getActiveConcepts();
                        if (!concepts.empty()) {
                            // Modify action based on learned concepts
                            std::vector<float> params = action->getParameters();
                            for (size_t i = 0; i < params.size() && i < concepts.size(); ++i) {
                                params[i] *= (1.0f + concepts[i].size() * 0.05f);
                            }
                            action->setParameters(params);
                        }
                    }
                }
            }
        }
        
        brain.step(step);
        
        // Verify learning loop
        assert(brain.getTotalNeuronCount() == 400);
        assert(brain.getRegionCount() == 2);
        
        // Concepts should evolve over time
        if (conceptForm) {
            auto currentConcepts = conceptForm->getActiveConcepts();
            // Concepts should be strings (not empty)
            for (const auto& concept : currentConcepts) {
                assert(!concept.empty());
            }
        }
    }
    
    // Analyze concept evolution
    std::cout << "    Learned " << learnedConcepts.size() << " concepts during simulation" << std::endl;
    
    // Remove duplicates
    std::sort(learnedConcepts.begin(), learnedConcepts.end());
    learnedConcepts.erase(std::unique(learnedConcepts.begin(), learnedConcepts.end()), learnedConcepts.end());
    
    std::cout << "    Unique concepts: " << learnedConcepts.size() << std::endl;
    
    std::cout << "    testConceptFormationAndPredictionLoop passed" << std::endl;
}

void testAttentionSelectionAndPrediction() {
    std::cout << "    Testing attentional selection and prediction..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    nlm::AttentionalSelection* attention = brain.getAttention();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    
    // Test attentional modulation of prediction and cognition
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        // Update prediction system
        if (predictionSys) {
            predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        // Update attentional selection
        if (attention) {
            // Attentional selection influences prediction and cognition
            attention->setPriority(nlm::NeuronType::Sensory, 0.8f);
            attention->setPriority(nlm::NeuronType::Motor, 0.7f);
            attention->setPriority(nlm::NeuronType::Plastic, 0.5f);
            
            // Get attentional focus
            auto focus = attention->getFocus();
            
            // Attentional focus influences prediction
            if (!focus.empty() && predictionSys) {
                // Attentional focus can reduce prediction error for attended inputs
                float attentionalGain = 1.0f;
                predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate(), attentionalGain);
            }
        }
        
        // Update concept formation with attentional modulation
        if (conceptForm) {
            conceptForm->update(brain.getTotalNeuronCount(), brain.getFiringNeuronCount());
            
            // Concepts influence attentional selection
            if (attention) {
                auto concepts = conceptForm->getActiveConcepts();
                if (!concepts.empty()) {
                    // Attentional priority based on concepts
                    float conceptPriority = std::min(0.9f, concepts.size() * 0.1f);
                    attention->setPriority(nlm::NeuronType::Cognitive, conceptPriority);
                }
            }
        }
        
        // Update planning with attentional modulation
        if (planner) {
            planner->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
            
            auto plan = planner->getPlan();
            
            // Actions can be attention-modulated
            if (!plan.empty()) {
                auto action = brain.produceAction();
                if (action) {
                    // Action parameters modulated by attention
                    std::vector<float> params = action->getParameters();
                    if (!params.empty()) {
                        float attentionalScale = attention ? 0.9f : 1.0f;
                        for (auto& param : params) {
                            param *= attentionalScale;
                        }
                        action->setParameters(params);
                    }
                }
            }
        }
        
        // Prediction error drives attentional allocation
        if (predError) {
            predError->setPredictionError(std::abs(std::sin(step * 0.1f)));
            
            // High prediction error should increase attentional focus
            if (attention) {
                float error = predError->getPredictionError();
                if (error > 0.5f) {
                    attention->setPriority(nlm::NeuronType::Sensory, 0.9f);
                }
            }
        }
        
        brain.step(step);
        
        // Verify attentional modulation
        assert(brain.getTotalNeuronCount() == 450);
        assert(brain.getRegionCount() == 3);
    }
    
    std::cout << "    testAttentionSelectionAndPrediction passed" << std::endl;
}

void runAll() {
    testPredictiveCodingAndActionPlanning();
    testConceptFormationAndPredictionLoop();
    testAttentionSelectionAndPrediction();
}

} // namespace test_prediction_cognition_integration