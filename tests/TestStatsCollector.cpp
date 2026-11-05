// TestStatsCollector.cpp - Test the new StatsCollector functionality

#include "brain/Brain.hpp"
#include "brain/StatsCollector.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

void testStatsCollectorBasics() {
    std::cout << "=== Testing StatsCollector Basics ===" << std::endl;
    
    // Create a simple config
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    assert(brain->initialize());
    
    // Test basic brain stats collection
    auto stats = StatsCollector::collect(brain.get());
    
    std::cout << "Collected statistics:" << std::endl;
    std::cout << "  Total neurons: " << stats.totalNeurons << std::endl;
    std::cout << "  Total synapses: " << stats.totalSynapses << std::endl;
    std::cout << "  Active neurons: " << stats.activeNeurons << std::endl;
    std::cout << "  Firing neurons: " << stats.firingNeurons << std::endl;
    std::cout << "  Total spikes: " << stats.totalSpikes << std::endl;
    std::cout << "  Average firing rate: " << stats.averageFiringRate << " Hz" << std::endl;
    std::cout << "  Stability score: " << stats.stabilityScore << std::endl;
    std::cout << "  Coherence score: " << stats.coherenceScore << std::endl;
    
    // Test formatted stats
    std::string formatted = StatsCollector::formatStats(stats, "Test Statistics");
    std::cout << "\nFormatted statistics (first 500 chars):" << std::endl;
    std::cout << formatted.substr(0, std::min<size_t>(500, formatted.length())) << "..." << std::endl;
    
    // Test export formats
    std::string json = StatsCollector::exportToJSON(stats);
    std::cout << "\nJSON export (first 300 chars):" << std::endl;
    std::cout << json.substr(0, std::min<size_t>(300, json.length())) << "..." << std::endl;
    
    std::string csv = StatsCollector::exportToCSV(stats);
    std::cout << "\nCSV export (first 200 chars):" << std::endl;
    std::cout << csv.substr(0, std::min<size_t>(200, csv.length())) << "..." << std::endl;
    
    std::string markdown = StatsCollector::exportToMarkdown(stats);
    std::cout << "\nMarkdown export (first 200 chars):" << std::endl;
    std::cout << markdown.substr(0, std::min<size_t>(200, markdown.length())) << "..." << std::endl;
    
    // Validate statistics
    assert(StatsCollector::validateStats(stats));
    
    std::cout << "\n✓ StatsCollector basic tests passed!" << std::endl;
}

void testBrainStatsInterface() {
    std::cout << "\n=== Testing Brain Stats Interface ===" << std::endl;
    
    // Create a simple config
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(50));
    config->set("region_count", static_cast<int64_t>(1));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test new Brain stats interface methods
    auto stats = brain->getStats();
    std::cout << "Using Brain::getStats():" << std::endl;
    std::cout << "  Total neurons: " << stats.totalNeurons << std::endl;
    std::cout << "  Total synapses: " << stats.totalSynapses << std::endl;
    
    std::string formatted = brain->getFormattedStats();
    std::cout << "  Formatted stats length: " << formatted.length() << " chars" << std::endl;
    
    std::string json = brain->getStatsJSON();
    std::cout << "  JSON stats length: " << json.length() << " chars" << std::endl;
    
    std::string csv = brain->getStatsCSV();
    std::cout << "  CSV stats length: " << csv.length() << " chars" << std::endl;
    
    std::string markdown = brain->getStatsMarkdown();
    std::cout << "  Markdown stats length: " << markdown.length() << " chars" << std::endl;
    
    // Test logStatus (should use StatsCollector internally)
    std::cout << "\nTesting logStatus() (should use StatsCollector):" << std::endl;
    brain->logStatus();
    
    std::cout << "\n✓ Brain stats interface tests passed!" << std::endl;
}

void testStatsCollectorCalculations() {
    std::cout << "\n=== Testing StatsCollector Calculations ===" << std::endl;
    
    // Create config and brain
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Collect stats before simulation
    auto stats1 = StatsCollector::collect(brain.get());
    float initialLearning = stats1.learningScore;
    float initialCoherence = stats1.coherenceScore;
    
    std::cout << "Initial learning score: " << initialLearning << std::endl;
    std::cout << "Initial coherence score: " << initialCoherence << std::endl;
    
    // Run some simulation steps
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Collect stats after simulation
    auto stats2 = StatsCollector::collect(brain.get());
    float finalLearning = stats2.learningScore;
    float finalCoherence = stats2.coherenceScore;
    
    std::cout << "After simulation learning score: " << finalLearning << std::endl;
    std::cout << "After simulation coherence score: " << finalCoherence << std::endl;
    
    // Calculate derived metrics
    float stability = StatsCollector::calculateStability(stats2);
    float coherence = StatsCollector::calculateCoherence(stats2);
    float learning = StatsCollector::calculateLearning(stats2, stats1);
    
    std::cout << "Calculated stability: " << stability << std::endl;
    std::cout << "Calculated coherence: " << coherence << std::endl;
    std::cout << "Calculated learning: " << learning << std::endl;
    
    // Validate that stats are consistent
    assert(stats2.totalNeurons == brain->getTotalNeuronCount());
    assert(stats2.totalSynapses == brain->getTotalSynapseCount());
    assert(stats2.totalSpikes == brain->getTotalSpikeCount());
    assert(stats2.firingNeurons == brain->getFiringNeuronCount());
    
    std::cout << "\n✓ StatsCollector calculation tests passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM StatsCollector Test Suite ===" << std::endl;
    std::cout << "Testing new StatsCollector functionality and Brain integration" << std::endl;
    
    try {
        testStatsCollectorBasics();
        testBrainStatsInterface();
        testStatsCollectorCalculations();
        
        std::cout << "\n=== All Tests Passed! ===" << std::endl;
        std::cout << "StatsCollector is working correctly and integrated with Brain class." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n*** Test Failed: " << e.what() << " ***" << std::endl;
        return 1;
    }
}
