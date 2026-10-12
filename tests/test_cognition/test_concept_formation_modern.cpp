#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include <algorithm>
#include <cmath>

class ConceptFormationTestSuite : public TestSuite {
public:
    ConceptFormationTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Concept Formation Test Suite..." << std::endl;
        
        testConceptFormationCreation();
        testConceptFormationAcquisition();
        testConceptFormationCategorization();
        testConceptFormationAbstraction();
        
        std::cout << "Concept Formation Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "ConceptFormationTestSuite";
    }
    
private:
    void testConceptFormationCreation() {
        nlm::Brain brain("test_brain");
        nlm::ConceptFormation cf;
        
        cf.initialize(&brain);
        ASSERT_TRUE(cf.getConceptCount() == 0);
        
        std::cout << "    testConceptFormationCreation passed" << std::endl;
    }
    
    void testConceptFormationAcquisition() {
        nlm::Brain brain("test_brain");
        nlm::ConceptFormation cf;
        
        cf.initialize(&brain);
        
        std::vector<float> patternA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> patternB = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        
        cf.learnPattern(patternA, 1.0f);
        ASSERT_TRUE(cf.getConceptCount() >= 1);
        
        cf.learnPattern(patternB, 1.0f);
        ASSERT_TRUE(cf.getConceptCount() >= 2);
        
        std::cout << "    testConceptFormationAcquisition passed" << std::endl;
    }
    
    void testConceptFormationCategorization() {
        nlm::Brain brain("test_brain");
        nlm::ConceptFormation cf;
        
        cf.initialize(&brain);
        
        std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
        std::vector<float> pattern2 = {0.4f, 0.5f, 0.6f};
        std::vector<float> pattern3 = {0.1f, 0.2f, 0.3f}; // Similar to pattern1
        
        cf.learnPattern(pattern1, 1.0f);
        cf.learnPattern(pattern2, 1.0f);
        cf.learnPattern(pattern3, 1.0f);
        
        std::vector<size_t> category1 = cf.categorize(pattern1);
        std::vector<size_t> category2 = cf.categorize(pattern2);
        std::vector<size_t> category3 = cf.categorize(pattern3);
        
        ASSERT_TRUE(category1.size() >= 1);
        ASSERT_TRUE(category2.size() >= 1);
        ASSERT_TRUE(category3.size() >= 1);
        
        std::cout << "    testConceptFormationCategorization passed" << std::endl;
    }
    
    void testConceptFormationAbstraction() {
        nlm::Brain brain("test_brain");
        nlm::ConceptFormation cf;
        
        cf.initialize(&brain);
        
        std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> pattern2 = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        
        cf.learnPattern(pattern1, 1.0f);
        cf.learnPattern(pattern2, 1.0f);
        
        std::vector<float> abstract = cf.getAbstract(pattern1);
        ASSERT_TRUE(abstract.size() == pattern1.size());
        
        std::cout << "    testConceptFormationAbstraction passed" << std::endl;
    }
};

class NeuralPlannerTestSuite : public TestSuite {
public:
    NeuralPlannerTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Neural Planner Test Suite..." << std::endl;
        
        testNeuralPlannerCreation();
        testNeuralPlannerInitialization();
        testNeuralPlannerActionPlanning();
        testNeuralPlannerEvaluation();
        
        std::cout << "Neural Planner Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "NeuralPlannerTestSuite";
    }
    
private:
    void testNeuralPlannerCreation() {
        nlm::Brain brain("test_brain");
        nlm::NeuralPlanner planner;
        
        planner.initialize(&brain);
        ASSERT_TRUE(planner.getPlanningDepth() > 0);
        
        std::cout << "    testNeuralPlannerCreation passed" << std::endl;
    }
    
    void testNeuralPlannerInitialization() {
        nlm::Brain brain("test_brain");
        nlm::NeuralPlanner planner;
        
        planner.initialize(&brain);
        ASSERT_TRUE(planner.getPlanningDepth() == 3); // Default depth
        
        std::cout << "    testNeuralPlannerInitialization passed" << std::endl;
    }
    
    void testNeuralPlannerActionPlanning() {
        nlm::Brain brain("test_brain");
        nlm::NeuralPlanner planner;
        
        planner.initialize(&brain);
        
        std::vector<float> currentState = {0.1f, 0.2f, 0.3f};
        nlm::ActionType action = planner.planAction(currentState, 0.5f);
        
        ASSERT_TRUE(static_cast<int>(action) >= 0 && static_cast<int>(action) < 10); // Reasonable action range
        
        std::cout << "    testNeuralPlannerActionPlanning passed" << std::endl;
    }
    
    void testNeuralPlannerEvaluation() {
        nlm::Brain brain("test_brain");
        nlm::NeuralPlanner planner;
        
        planner.initialize(&brain);
        
        std::vector<nlm::ActionType> actions = {nlm::ActionType::Move, nlm::ActionType::Wait};
        std::vector<float> startState = {0.1f, 0.2f, 0.3f};
        
        nlm::PlanningCandidate candidate = planner.evaluateSequence(actions, startState);
        
        ASSERT_TRUE(candidate.actions.size() == actions.size());
        ASSERT_TRUE(candidate.expectedReward >= 0.0f);
        ASSERT_TRUE(candidate.confidence >= 0.0f && candidate.confidence <= 1.0f);
        
        std::cout << "    testNeuralPlannerEvaluation passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createConceptFormationTestSuite() {
        return new ConceptFormationTestSuite();
    }
    
    TestSuite* createNeuralPlannerTestSuite() {
        return new NeuralPlannerTestSuite();
    }
}