#ifndef NLM_PHASE2_EXPERIMENT_HPP
#define NLM_PHASE2_EXPERIMENT_HPP

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>

namespace nlm {

class Phase2Experiment {
public:
    Phase2Experiment();
    ~Phase2Experiment();
    
    // Basic connectivity test
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    
    // Plasticity learning experiment
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    
    // STDP verification
    void runStdpVerification(std::shared_ptr<Brain> brain);
    
    // Run all experiments
    void runAll(std::shared_ptr<Brain> brain);
    
private:
    // Learning experiment structure
    struct LearningExperiment {
        std::shared_ptr<Brain> brain;
        uint64_t seed;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        std::vector<NeuronId> mostActiveNeurons;
        
        LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
            : brain(b), seed(s), initialSynapseCount(0) {}
        
        void recordInitialState();
        void recordFinalState();
        void computeStatistics();
    };
};

} // namespace nlm

#endif // NLM_PHASE2_EXPERIMENT_HPP