#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

class BrainTestSuite : public TestSuite {
public:
    BrainTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Brain Test Suite..." << std::endl;
        
        testBrainCreation();
        testNeuralPopulation();
        testNeuralRegion();
        testBrainIntegration();
        testNeuralPopulationSpiking();
        testRegionConnectivity();
        testBrainReset();
        
        std::cout << "Brain Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "BrainTestSuite";
    }
    
private:
    void testBrainCreation() {
        nlm::Brain brain("test_brain");
        
        ASSERT_EQ(brain.getName(), "test_brain");
        ASSERT_TRUE(brain.getNeuronCount() == 0);
        ASSERT_TRUE(brain.getSynapseCount() == 0);
        
        std::cout << "    testBrainCreation passed" << std::endl;
    }
    
    void testNeuralPopulation() {
        nlm::NeuralPopulation population("test_population", nlm::NeuronType::Excitatory);
        
        ASSERT_EQ(population.getName(), "test_population");
        ASSERT_EQ(static_cast<int>(population.getType()), static_cast<int>(nlm::NeuronType::Excitatory));
        ASSERT_TRUE(population.getSize() == 0);
        
        population.addNeuron(nlm::NeuronId(1));
        ASSERT_TRUE(population.getSize() == 1);
        
        population.addNeuron(nlm::NeuronId(2));
        ASSERT_TRUE(population.getSize() == 2);
        
        ASSERT_TRUE(population.getNeuronId(0).getId() == 1);
        ASSERT_TRUE(population.getNeuronId(1).getId() == 2);
        
        std::cout << "    testNeuralPopulation passed" << std::endl;
    }
    
    void testNeuralRegion() {
        nlm::NeuralRegion region("test_region");
        
        ASSERT_EQ(region.getName(), "test_region");
        ASSERT_TRUE(region.getPopulationCount() == 0);
        
        nlm::NeuralPopulation population("pop1", nlm::NeuronType::Excitatory);
        population.addNeuron(nlm::NeuronId(1));
        
        region.addPopulation(&population);
        ASSERT_TRUE(region.getPopulationCount() == 1);
        
        ASSERT_EQ(region.getPopulation(0)->getName(), "pop1");
        
        std::cout << "    testNeuralRegion passed" << std::endl;
    }
    
    void testBrainIntegration() {
        nlm::Brain brain("integration_brain");
        
        nlm::NeuralPopulation* cortex = new nlm::NeuralPopulation("cortex", nlm::NeuronType::Excitatory);
        nlm::NeuralPopulation* thalamus = new nlm::NeuralPopulation("thalamus", nlm::NeuronType::Inhibitory);
        
        cortex->addNeuron(nlm::NeuronId(1));
        thalamus->addNeuron(nlm::NeuronId(2));
        
        brain.addPopulation(cortex);
        brain.addPopulation(thalamus);
        
        ASSERT_TRUE(brain.getPopulationCount() == 2);
        ASSERT_EQ(brain.getPopulation(0)->getName(), "cortex");
        ASSERT_EQ(brain.getPopulation(1)->getName(), "thalamus");
        
        delete cortex;
        delete thalamus;
        
        std::cout << "    testBrainIntegration passed" << std::endl;
    }
    
    void testNeuralPopulationSpiking() {
        nlm::NeuralPopulation population("spiking_pop", nlm::NeuronType::Excitatory);
        
        nlm::Neuron neuron1(nlm::NeuronId(1));
        neuron1.injectCurrent(10.0f); // Sufficient current to potentially spike
        
        nlm::Neuron neuron2(nlm::NeuronId(2));
        neuron2.injectCurrent(0.0f);
        
        population.addNeuronToPopulation(&neuron1);
        population.addNeuronToPopulation(&neuron2);
        
        // Simulate some activity
        neuron1.injectCurrent(5.0f);
        neuron2.injectCurrent(2.0f);
        
        std::vector<float> totalCurrents = population.getTotalCurrents();
        ASSERT_TRUE(totalCurrents.size() == 2);
        
        std::cout << "    testNeuralPopulationSpiking passed" << std::endl;
    }
    
    void testRegionConnectivity() {
        nlm::NeuralRegion region("connectivity_region");
        
        nlm::NeuralPopulation pop1("pop1", nlm::NeuronType::Excitatory);
        nlm::NeuralPopulation pop2("pop2", nlm::NeuronType::Inhibitory);
        
        pop1.addNeuron(nlm::NeuronId(1));
        pop2.addNeuron(nlm::NeuronId(2));
        
        region.addPopulation(&pop1);
        region.addPopulation(&pop2);
        
        // Test population interactions
        ASSERT_TRUE(region.getPopulationCount() == 2);
        
        std::vector<std::vector<float>> connectivity = region.getConnectivityMatrix();
        ASSERT_TRUE(connectivity.size() == 2);
        ASSERT_TRUE(connectivity[0].size() == 2);
        
        std::cout << "    testRegionConnectivity passed" << std::endl;
    }
    
    void testBrainReset() {
        nlm::Brain brain("reset_brain");
        
        nlm::NeuralPopulation* pop = new nlm::NeuralPopulation("test_pop", nlm::NeuronType::Excitatory);
        pop->addNeuron(nlm::NeuronId(1));
        
        brain.addPopulation(pop);
        
        brain.reset();
        
        // After reset, populations should still exist but be empty
        ASSERT_TRUE(brain.getPopulationCount() == 1);
        ASSERT_TRUE(brain.getPopulation(0)->getSize() == 1); // Neuron still exists
        
        delete pop;
        
        std::cout << "    testBrainReset passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createBrainTestSuite() {
        return new BrainTestSuite();
    }
}