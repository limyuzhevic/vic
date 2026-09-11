#include "Phase4Demo.hpp"
#include "../Phase4Experiment.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {

struct Phase4Demo::Impl {
    bool completed;
    std::string results;
    size_t trialsPerExperiment;
    size_t stepsPerEpisode;
    bool verbose;
};

Phase4Demo::Phase4Demo() : pImpl(std::make_unique<Impl>()) {
    pImpl->completed = false;
    pImpl->trialsPerExperiment = 50;
    pImpl->stepsPerEpisode = 200;
    pImpl->verbose = true;
}

Phase4Demo::~Phase4Demo() = default;

void Phase4Demo::run(size_t trialsPerExperiment, size_t stepsPerEpisode, bool verbose) {
    pImpl->trialsPerExperiment = trialsPerExperiment;
    pImpl->stepsPerEpisode = stepsPerEpisode;
    pImpl->verbose = verbose;
    
    if (pImpl->verbose) {
        std::cout << "=== NLM Phase 4: Emerging Cognition Demo ===" << std::endl;
        std::cout << "Running 10 cognitive capability experiments..." << std::endl;
    }
    
    std::ostringstream results;
    
    // Create brain once for all experiments
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        results << "ERROR: Failed to initialize brain!";
        pImpl->results = results.str();
        return;
    }
    
    // Run all 10 experiments
    for (size_t i = 0; i < 10; ++i) {
        if (pImpl->verbose) {
            std::cout << "--- Experiment " << (i + 1) << "/10 ---" << std::endl;
        }
        
        // Run each experiment
        switch (i) {
            case 0: {
                TemporalPredictionExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment);
                results << "1. Temporal Prediction: Initial error=" << res.predictionErrorInitial 
                        << ", Final error=" << res.predictionErrorFinal 
                        << ", Improvement=" << res.predictionAccuracyImprovement << "\n";
                break;
            }
            case 1: {
                WorkingMemoryExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment);
                results << "2. Working Memory: Initial retention=" << res.workingMemoryRetentionInitial 
                        << ", Final retention=" << res.workingMemoryRetentionFinal 
                        << ", Capacity=" << res.workingMemoryCapacity << "\n";
                break;
            }
            case 2: {
                EpisodicRecallExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment);
                results << "3. Episodic Memory: Recall accuracy=" << res.episodicRecallAccuracy 
                        << ", Episodes stored=" << res.episodesStored 
                        << ", Influence=" << res.episodicInfluence << "\n";
                break;
            }
            case 3: {
                ConceptFormationExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment);
                results << "4. Concept Formation: Concepts formed=" << res.conceptsFormed 
                        << ", Stability=" << res.conceptStability 
                        << ", Generalization=" << res.generalizationAbility << "\n";
                break;
            }
            case 4: {
                AttentionExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment);
                results << "5. Neural Attention: Selectivity=" << res.attentionSelectivity 
                        << ", Distraction resistance=" << res.distractionResistance << "\n";
                break;
            }
            case 5: {
                PlanningExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment / 2);
                results << "6. Multi-Step Planning: Accuracy=" << res.planningAccuracy 
                        << ", Confidence=" << res.planningConfidence << "\n";
                break;
            }
            case 6: {
                SelfModelExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment);
                results << "7. Self-Model: Prediction accuracy=" << res.selfPredictionAccuracy 
                        << ", Body awareness=" << res.bodyAwareness << "\n";
                break;
            }
            case 7: {
                SocialLearningExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment / 2);
                results << "8. Social Learning: Imitation accuracy=" << res.imitationAccuracy 
                        << ", Observations=" << res.observationsFromOthers << "\n";
                break;
            }
            case 8: {
                ContinualLearningExperiment exp;
                Phase4Results res = exp.run(brain.get(), pImpl->trialsPerExperiment);
                results << "9. Continual Learning: Improvement=" << res.behaviorImprovement 
                        << ", Transfer=" << res.transferPerformance << "\n";
                break;
            }
            case 9: {
                Phase4IntegratedExperiment exp;
                Phase4Results res = exp.run(brain.get(), 10, pImpl->stepsPerEpisode);
                results << "10. Integrated Phase 4: Total reward=" << res.totalReward 
                        << ", Episodes=" << res.episodesStored 
                        << ", Concepts=" << res.conceptsFormed << "\n";
                break;
            }
        }
        
        if (pImpl->verbose) {
            std::cout << "   Completed!" << std::endl;
        }
    }
    
    pImpl->results = results.str();
    pImpl->completed = true;
    
    if (pImpl->verbose) {
        std::cout << "=== All Phase 4 experiments completed successfully! ===" << std::endl;
    }
}

void Phase4Demo::runExperiment(size_t experimentIndex) {
    if (experimentIndex >= 10) {
        throw std::out_of_range("Experiment index must be between 0 and 9");
    }
    
    // For simplicity, just run the full demo
    run(pImpl->trialsPerExperiment, pImpl->stepsPerEpisode, pImpl->verbose);
}

std::string Phase4Demo::getResults() const {
    return pImpl->results;
}

bool Phase4Demo::isCompleted() const {
    return pImpl->completed;
}

void Phase4Demo::reset() {
    pImpl->completed = false;
    pImpl->results.clear();
}

std::string Phase4Demo::getStatistics() const {
    if (!pImpl->completed) {
        return "Demo not completed yet!";
    }
    
    std::ostringstream stats;
    stats << "=== Phase 4 Demo Statistics ===" << std::endl;
    stats << "Experiments completed: 10/10" << std::endl;
    stats << "Trials per experiment: " << pImpl->trialsPerExperiment << std::endl;
    stats << "Steps per episode: " << pImpl->stepsPerEpisode << std::endl;
    stats << "Verbose mode: " << (pImpl->verbose ? "Yes" : "No") << std::endl;
    stats << "\nResults:" << std::endl;
    stats << pImpl->results;
    return stats.str();
}

} // namespace nlm
