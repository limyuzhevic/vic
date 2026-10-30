// Implementation of constants for NLM
// This file provides implementations for helper functions and inline definitions

#include "core/Constants.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {
namespace Constants {

// Implementation of helper functions
bool isValidNeuronCount(int64_t count) {
    return count >= ExperimentLimits::MIN_NEURONS && 
           count <= ExperimentLimits::MAX_NEURONS;
}

bool isValidRegionCount(size_t regions) {
    return regions >= ExperimentLimits::MIN_BRAIN_REGIONS && 
           regions <= ExperimentLimits::MAX_BRAIN_REGIONS;
}

bool isValidConnectionProbability(float prob) {
    return prob >= ExperimentLimits::MIN_CONNECTION_PROBABILITY && 
           prob <= ExperimentLimits::MAX_CONNECTION_PROBABILITY;
}

bool isValidTimestep(double timestep) {
    return timestep >= ExperimentLimits::MIN_TIMESTEP && 
           timestep <= ExperimentLimits::MAX_TIMESTEP;
}

bool isValidSTDPWeight(float weight) {
    return weight >= ExperimentLimits::MIN_STDP_WEIGHT && 
           weight <= ExperimentLimits::MAX_STDP_WEIGHT;
}

bool isValidPlasticityRate(float rate) {
    return rate >= ExperimentLimits::MIN_SYNAPTOGENESIS_RATE && 
           rate <= ExperimentLimits::MAX_SYNAPTOGENESIS_RATE;
}

} // namespace Constants

} // namespace nlm