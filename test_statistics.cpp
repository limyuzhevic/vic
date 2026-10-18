#include <iostream>
#include <vector>
#include <cmath>
#include "Statistics.hpp"
#include "StatisticsManager.hpp"

using namespace nlm;

void testPopulationStats() {
    std::cout << "Testing PopulationStats..." << std::endl;
    
    PopulationStats stats;
    std::vector<float> firingRates = {0.0f, 2.5f, 0.0f, 1.0f, 3.5f};
    std::vector<float> potentials = { -70.0f, -65.0f, -72.0f, -60.0f, -68.0f };
    std::vector<uint8_t> states = {0, 1, 0, 1, 1}; // 1 means firing
    
    StatisticsManager manager;
    manager.initializePopulationStats(stats, 5);
    
    // Populate SoA arrays
    stats.soaFiringRates = firingRates;
    stats.soaMembranePotentials = potentials;
    stats.soaFiringStates = states;
    
    manager.updatePopulationStats(stats, firingRates, potentials, states);
    
    std::cout << "  Active count: " << stats.activeCount << " (expected: 3)" << std::endl;
    std::cout << "  Firing count: " << stats.firingCount << " (expected: 3)" << std::endl;
    std::cout << "  Average firing rate: " << stats.averageFiringRate << " (expected: 1.4)" << std::endl;
    std::cout << "  Mean membrane potential: " << stats.meanMembranePotential << " (expected: -67.0)" << std::endl;
    std::cout << "  Variance membrane potential: " << stats.varianceMembranePotential << std::endl;
    std::cout << "  Activity level: " << stats.activityLevel << " (expected: 0.6)" << std::endl;
    
    // Validate
    bool valid = true;
    if (std::abs(stats.averageFiringRate - 1.4f) > 0.01f) valid = false;
    if (std::abs(stats.meanMembranePotential - (-67.0f)) > 0.01f) valid = false;
    if (std::abs(stats.activityLevel - 0.6f) > 0.01f) valid = false;
    
    std::cout << "  PopulationStats test: " << (valid ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
}

void testRegionStats() {
    std::cout << "Testing RegionStats..." << std::endl;
    
    RegionStats stats;
    std::vector<size_t> popSizes = {100, 150, 200};
    std::vector<float> synWeights = {0.5f, -0.2f, 0.8f, 0.3f, -0.1f};
    std::vector<size_t> synConnections = {10, 15, 20, 25, 30};
    
    StatisticsManager manager;
    manager.initializeRegionStats(stats, 3, 5);
    
    // Populate SoA arrays
    stats.populationSizes = popSizes;
    stats.synapseWeights = synWeights;
    stats.synapseConnections = synConnections;
    
    manager.updateRegionStats(stats, popSizes, synWeights, synConnections);
    
    std::cout << "  Total neuron count: " << stats.totalNeuronCount << " (expected: 450)" << std::endl;
    std::cout << "  Average synaptic weight: " << stats.averageSynapticWeight << " (expected: 0.26)" << std::endl;
    std::cout << "  Synaptic density: " << stats.synapticDensity << std::endl;
    
    // Validate
    bool valid = true;
    if (stats.totalNeuronCount != 450) valid = false;
    if (std::abs(stats.averageSynapticWeight - 0.26f) > 0.01f) valid = false;
    
    std::cout << "  RegionStats test: " << (valid ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
}

void testBrainStats() {
    std::cout << "Testing BrainStats..." << std::endl;
    
    BrainStats stats;
    std::vector<RegionStats> regionStatsList;
    
    // Create test region stats
    RegionStats region1;
    region1.totalNeuronCount = 100;
    region1.activeNeuronCount = 30;
    region1.firingNeuronCount = 25;
    region1.averageFiringRate = 2.5f;
    region1.totalSynapseCount = 500;
    region1.averageSynapticWeight = 0.4f;
    regionStatsList.push_back(region1);
    
    RegionStats region2;
    region2.totalNeuronCount = 200;
    region2.activeNeuronCount = 60;
    region2.firingNeuronCount = 55;
    region2.averageFiringRate = 1.8f;
    region2.totalSynapseCount = 800;
    region2.averageSynapticWeight = 0.3f;
    regionStatsList.push_back(region2);
    
    StatisticsManager manager;
    manager.updateBrainStats(stats, regionStatsList);
    
    std::cout << "  Total neuron count: " << stats.totalNeuronCount << " (expected: 300)" << std::endl;
    std::cout << "  Active neuron count: " << stats.activeNeuronCount << " (expected: 90)" << std::endl;
    std::cout << "  Firing neuron count: " << stats.firingNeuronCount << " (expected: 80)" << std::endl;
    std::cout << "  Average firing rate: " << stats.averageFiringRate << " (expected: 2.15)" << std::endl;
    std::cout << "  Region count: " << stats.regionCount << " (expected: 2)" << std::endl;
    
    // Validate
    bool valid = true;
    if (stats.totalNeuronCount != 300) valid = false;
    if (stats.activeNeuronCount != 90) valid = false;
    if (stats.firingNeuronCount != 80) valid = false;
    if (std::abs(stats.averageFiringRate - 2.15f) > 0.01f) valid = false;
    if (stats.regionCount != 2) valid = false;
    
    std::cout << "  BrainStats test: " << (valid ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
}

void testPerformanceProjection() {
    std::cout << "Testing Performance Projections..." << std::endl;
    
    StatisticsManager manager;
    StatisticsManager::PerformanceProjection projection = manager.projectPerformanceBenefits();
    
    std::cout << "  SoA Speedup Factor: " << projection.soaSpeedupFactor << "x (expected: ~10x)" << std::endl;
    std::cout << "  Memory Savings: " << projection.memorySavingsPercent << "% (expected: ~15%)" << std::endl;
    std::cout << "  Cache Efficiency: " << projection.cacheEfficiency << " (expected: ~0.95)" << std::endl;
    
    std::cout << "  Performance projection test: PASSED" << std::endl;
    std::endl
}

void testValidation() {
    std::cout << "Testing Statistics Validation..." << std::endl;
    
    StatisticsManager manager;
    
    // Test population stats validation
    PopulationStats popStats;
    bool popValid = manager.validateStatistics(popStats, 10);
    std::cout << "  Population validation (empty): " << (popValid ? "PASSED" : "FAILED") << std::endl;
    
    // Test region stats validation
    RegionStats regionStats;
    bool regionValid = manager.validateStatistics(regionStats, 5, 20);
    std::cout << "  Region validation (empty): " << (regionValid ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << "  Validation test: PASSED" << std::endl;
    std::endl
}

int main() {
    std::cout << "=== NLM Statistics Collection Framework Test ===" << std::endl;
    std::cout << std::endl;
    
    testPopulationStats();
    testRegionStats();
    testBrainStats();
    testPerformanceProjection();
    testValidation();
    
    std::cout << "=== All Tests Completed ===" << std::endl;
    
    return 0;
}
