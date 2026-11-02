#pragma once

#include <memory>
#include "brain/Brain.hpp"
#include "main/TestUtilities.hpp"

namespace nlm {

namespace stdp_verification {

class StdpVerification {
public:
    static void run(std::shared_ptr<Brain> brain);
    
private:
    static Synapse* getTestSynapse(std::shared_ptr<Brain> brain);
    static void createCorrelatedPrePostActivity(std::shared_ptr<Brain> brain, Synapse* syn, int trials);
    static float computeSynapticChange(Synapse* syn, float beforeWeight);
    static void analyzeStdpResults(float delta);
};

} // namespace stdp_verification

} // namespace nlm