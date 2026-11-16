// Brain Initialization Header
// Refactored initialization logic for Brain.cpp

#ifndef BRAIN_INITIALIZATION_HPP
#define BRAIN_INITIALIZATION_HPP

#include "Brain.hpp"
#include "../brain/Region.hpp"

namespace nlm {

class BrainInitialization {
public:
    // Initialize neural regions and neurons
    static void initializeRegions(Brain::Impl& pImpl, 
                                  size_t neuronCount, 
                                  size_t regionCount, 
                                  float connectionProbability);
    
    // Initialize all integrated systems
    static void initializeIntegratedSystems(Brain::Impl& pImpl, 
                                           size_t neuronCount);
    
    // Log initialization statistics
    static void logInitializationStats(Brain::Impl& pImpl);
    
    // Create brain with default configuration
    static Brain::Impl& createBrainWithDefaults(Brain::Impl& pImpl);
};

#endif // BRAIN_INITIALIZATION_HPP
