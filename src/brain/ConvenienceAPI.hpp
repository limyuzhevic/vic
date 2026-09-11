#pragma once

#include "../core/Config/Config.hpp"
#include "Brain.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Quick convenience functions for common NLM operations

// Create a default configured brain with sensible settings for quick testing
std::shared_ptr<Brain> createQuickBrain(int neuronCount = 100, const std::string& configFile = "");

// Run a basic connectivity test on a brain
bool testBrainConnectivity(std::shared_ptr<Brain> brain);

// Run a basic plasticity experiment
float testBrainPlasticity(std::shared_ptr<Brain> brain, int steps = 1000);

// Run a simple STDP verification test
bool testSTDP(std::shared_ptr<Brain> brain);

// Run a simple Hebbian verification test
bool testHebbian(std::shared_ptr<Brain> brain);

// Print brain status in a readable format
void printBrainStatus(std::shared_ptr<Brain> brain);

// Save brain state with timestamp
std::string saveBrainWithTimestamp(std::shared_ptr<Brain> brain, const std::string& basePath = "./checkpoints");

} // namespace nlm
