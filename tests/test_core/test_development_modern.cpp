#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "core/Random/Random.hpp"
#include <cmath>

class DevelopmentSystemTestSuite : public TestSuite {
public:
    DevelopmentSystemTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Development System Test Suite..." << std::endl;
        
        testDevelopmentSystemCreation();
        testDevelopmentStages();
        testPlasticityModifiers();
        testCriticalPeriods();
        testDevelopmentUpdate();
        
        std::cout << "Development System Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "DevelopmentSystemTestSuite";
    }
    
private:
    void testDevelopmentSystemCreation() {
        nlm::DevelopmentSystem devSystem;
        
        nlm::DevelopmentalStage stage = devSystem.getStage();
        ASSERT_TRUE(static_cast<int>(stage) >= 0 && static_cast<int>(stage) <= 10);
        
        std::cout << "    testDevelopmentSystemCreation passed" << std::endl;
    }
    
    void testDevelopmentStages() {
        nlm::DevelopmentSystem devSystem;
        
        nlm::DevelopmentalStage initialStage = devSystem.getStage();
        
        devSystem.advanceStage();
        nlm::DevelopmentalStage newStage = devSystem.getStage();
        
        ASSERT_TRUE(static_cast<int>(newStage) > static_cast<int>(initialStage));
        
        std::cout << "    testDevelopmentStages passed" << std::endl;
    }
    
    void testPlasticityModifiers() {
        nlm::DevelopmentSystem devSystem;
        
        devSystem.setStage(nlm::DevelopmentalStage::Early); // Use available enum
        
        float plasticity = devSystem.getPlasticityModifier();
        ASSERT_TRUE(plasticity > 0.0f && plasticity <= 1.0f);
        
        std::cout << "    testPlasticityModifiers passed" << std::endl;
    }
    
    void testCriticalPeriods() {
        nlm::DevelopmentSystem devSystem;
        
        bool isCritical = devSystem.isCriticalPeriod();
        ASSERT_TRUE(isCritical == false); // Initially should not be in critical period
        
        std::cout << "    testCriticalPeriods passed" << std::endl;
    }
    
    void testDevelopmentUpdate() {
        nlm::Brain brain("test_brain");
        nlm::DevelopmentSystem devSystem;
        
        nlm::RandomGenerator rng(42);
        
        devSystem.update(&brain, rng, 1.0f);
        
        ASSERT_TRUE(devSystem.getDevelopmentalAge() >= 0.0);
        
        std::cout << "    testDevelopmentUpdate passed" << std::endl;
    }
};

class SynaptogenesisTestSuite : public TestSuite {
public:
    SynaptogenesisTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Synaptogenesis Test Suite..." << std::endl;
        
        testSynaptogenesisCreation();
        testSynaptogenesisFormation();
        testSynaptogenesisPruning();
        
        std::cout << "Synaptogenesis Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "SynaptogenesisTestSuite";
    }
    
private:
    void testSynaptogenesisCreation() {
        nlm::Brain brain("test_brain");
        nlm::Synaptogenesis synaptogenesis;
        
        synaptogenesis.initialize(&brain);
        ASSERT_TRUE(synaptogenesis.getFormationRate() > 0.0f);
        
        std::cout << "    testSynaptogenesisCreation passed" << std::endl;
    }
    
    void testSynaptogenesisFormation() {
        nlm::Brain brain("test_brain");
        nlm::Synaptogenesis synaptogenesis;
        
        synaptogenesis.initialize(&brain);
        
        nlm::SynapticFormation formation(0.5f, 0.8f, 0.3f, 0.7f, 0.1f);
        synaptogenesis.createNewSynapse(formation);
        
        ASSERT_TRUE(synaptogenesis.getSynapseCount() > 0);
        
        std::cout << "    testSynaptogenesisFormation passed" << std::endl;
    }
    
    void testSynaptogenesisPruning() {
        nlm::Brain brain("test_brain");
        nlm::Synaptogenesis synaptogenesis;
        
        synaptogenesis.initialize(&brain);
        
        nlm::SynapticPruning pruning(0.3f, 0.5f, 0.2f);
        synaptogenesis.pruneSynapses(pruning);
        
        ASSERT_TRUE(synaptogenesis.getPrunedCount() > 0);
        
        std::cout << "    testSynaptogenesisPruning passed" << std::endl;
    }
};

class MaturationTestSuite : public TestSuite {
public:
    MaturationTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Maturation Test Suite..." << std::endl;
        
        testMaturationCreation();
        testMaturationProcess();
        testMaturationMetrics();
        
        std::cout << "Maturation Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "MaturationTestSuite";
    }
    
private:
    void testMaturationCreation() {
        nlm::Brain brain("test_brain");
        nlm::Maturation maturation;
        
        maturation.initialize(&brain);
        ASSERT_TRUE(maturation.getMaturationLevel() > 0.0f);
        
        std::cout << "    testMaturationCreation passed" << std::endl;
    }
    
    void testMaturationProcess() {
        nlm::Brain brain("test_brain");
        nlm::Maturation maturation;
        
        maturation.initialize(&brain);
        
        maturation.updateMaturation(0.1f, 0.5f);
        
        ASSERT_TRUE(maturation.getMaturationLevel() > 0.0f);
        
        std::cout << "    testMaturationProcess passed" << std::endl;
    }
    
    void testMaturationMetrics() {
        nlm::Brain brain("test_brain");
        nlm::Maturation maturation;
        
        maturation.initialize(&brain);
        
        nlm::MaturationMetrics metrics;
        metrics.synapseCount = 100;
        metrics.neuronCount = 1000;
        metrics.compartmentCount = 50;
        
        maturation.updateMetrics(metrics);
        
        ASSERT_TRUE(maturation.getMaturationMetrics().synapseCount == 100);
        
        std::cout << "    testMaturationMetrics passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createDevelopmentSystemTestSuite() {
        return new DevelopmentSystemTestSuite();
    }
    
    TestSuite* createSynaptogenesisTestSuite() {
        return new SynaptogenesisTestSuite();
    }
    
    TestSuite* createMaturationTestSuite() {
        return new MaturationTestSuite();
    }
}