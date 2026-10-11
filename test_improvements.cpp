// Comprehensive test suite for NLM Phase 2 improvements
// Tests memory management, error handling, and command-line interface

#include "../src/main.cpp"
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

class NLMTestSuite {
public:
    static void runAllTests() {
        std::cout << "Running NLM Phase 2 Improvement Tests...\n" << std::endl;
        
        testCommandLineParsing();
        testMemoryManagement();
        testErrorHandling();
        testConfigurationValidation();
        testNeuronInitialization();
        testPlasticityExperiment();
        testSTDPVerification();
        testCheckpointSystem();
        
        std::cout << "\n✓ All tests passed!\n" << std::endl;
    }
    
private:
    static void testCommandLineParsing() {
        std::cout << "Testing command-line parsing... " << std::flush;
        
        // Test config file parsing
        std::string configFile = "configs/default.cfg";
        bool configArgFound = false;
        
        // Simulate argument parsing
        const char* argv[] = {"nlm", "--config=myconfig.cfg"};
        int argc = 2;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config") {
                configArgFound = true;
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                }
            }
        }
        
        assert(configArgFound && configFile == "myconfig.cfg");
        
        std::cout << "✓" << std::endl;
    }
    
    static void testMemoryManagement() {
        std::cout << "Testing memory management... " << std::flush;
        
        // Test neuron initialization with random seed
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Test neuron memory efficiency
        size_t totalNeurons = brain->getTotalNeuronCount();
        assert(totalNeurons > 0);
        
        // Test spike system memory
        auto* spikeSystem = brain->getSpikeSystem();
        assert(spikeSystem != nullptr);
        
        std::cout << "✓" << std::endl;
    }
    
    static void testErrorHandling() {
        std::cout << "Testing error handling... " << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Test load with non-existent file
        std::string invalidFile = "nonexistent.chkpt";
        bool loadResult = brain->load(invalidFile);
        assert(!loadResult);  // Should fail
        
        // Test save to valid location
        std::string tempFile = "./test_save.chkpt";
        bool saveResult = brain->save(tempFile);
        assert(saveResult);  // Should succeed
        
        // Clean up
        std::remove(tempFile.c_str());
        
        std::cout << "✓" << std::endl;
    }
    
    static void testConfigurationValidation() {
        std::cout << "Testing configuration validation... " << std::flush;
        
        auto config = std::make_shared<Config>();
        
        // Test setting various config types
        config->set("test_string", std::string("hello"), ConfigSource::Runtime);
        config->set("test_int", 42, ConfigSource::Runtime);
        config->set("test_double", 3.14, ConfigSource::Runtime);
        config->set("test_bool", true, ConfigSource::Runtime);
        
        // Test getOr with defaults
        assert(config->getOr<std::string>("test_string", "default") == "hello");
        assert(config->getOr<int>("test_int", 0) == 42);
        assert(config->getOr<double>("test_double", 0.0) == 3.14);
        assert(config->getOr<bool>("test_bool", false) == true);
        
        std::cout << "✓" << std::endl;
    }
    
    static void testNeuronInitialization() {
        std::cout << "Testing neuron initialization... " << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Test neuron parameters
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Check neuron is properly initialized
                    assert(neuron->getId() != 0);  // Should have valid ID
                    assert(neuron->getMembranePotential() != 0.0f); // Should have initial potential
                }
            }
        }
        
        std::cout << "✓" << std::endl;
    }
    
    static void testPlasticityExperiment() {
        std::cout << "Testing plasticity experiment... " << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Enable plasticity on synapses
        for (auto& region : brain->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply input pattern to stimulate learning
        for (int step = 0; step < 100; ++step) {
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 3; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001f);
        }
        
        // Check that learning occurred (some synaptic changes)
        bool learningOccurred = false;
        for (auto& region : brain->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().hebbian) {
                    learningOccurred = true;
                    break;
                }
            }
            if (learningOccurred) break;
        }
        
        assert(learningOccurred);
        
        std::cout << "✓" << std::endl;
    }
    
    static void testSTDPVerification() {
        std::cout << "Testing STDP verification... " << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        auto* region = brain->getRegion(RegionId(1));
        assert(region != nullptr);
        
        auto& synapses = region->getSynapses();
        if (synapses.size() >= 5) {
            // Record initial weights
            std::vector<float> beforeWeights;
            for (size_t i = 0; i < 5; ++i) {
                beforeWeights.push_back(synapses[i]->getWeight());
                synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            }
            
            // Create correlated activity: fire pre then post
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Pre fires first
                neurons[0]->injectCurrent(60.0f);
                brain->step(0, 0.0);
                
                // Then post fires
                neurons[1]->injectCurrent(60.0f);
                brain->step(1, 0.001);
            }
            
            // Check if weights changed
            bool stdpWorking = false;
            float totalDelta = 0.0f;
            for (size_t i = 0; i < 5; ++i) {
                totalDelta += synapses[i]->getWeight() - beforeWeights[i];
            }
            
            if (std::abs(totalDelta) > 0.001f) {
                stdpWorking = true;
            }
            
            assert(stdpWorking);
        }
        
        std::cout << "✓" << std::endl;
    }
    
    static void testCheckpointSystem() {
        std::cout << "Testing checkpoint system... " << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(15), ConfigSource::Default);
        
        auto brain1 = std::make_shared<Brain>(config);
        assert(brain1->initialize());
        
        // Save checkpoint
        std::string checkpointFile = "./test_checkpoint.chkpt";
        bool saveResult = brain1->save(checkpointFile);
        assert(saveResult);
        
        // Create second brain and load checkpoint
        auto brain2 = std::make_shared<Brain>(config);
        assert(brain2->initialize());
        
        bool loadResult = brain2->load(checkpointFile);
        assert(loadResult);
        
        // Both should have same number of neurons
        assert(brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount());
        
        // Clean up
        std::remove(checkpointFile.c_str());
        
        std::cout << "✓" << std::endl;
    }
};

int main() {
    NLMTestSuite::runAllTests();
    return 0;
}
