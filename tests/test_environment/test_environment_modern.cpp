#include "framework/test_base.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "environment/Action.hpp"
#include <algorithm>
#include <cmath>

class EnvironmentTestSuite : public TestSuite {
public:
    EnvironmentTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Environment Test Suite..." << std::endl;
        
        testEnvironmentCreation();
        testEnvironmentReset();
        testEnvironmentStep();
        testEnvironmentState();
        
        std::cout << "Environment Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "EnvironmentTestSuite";
    }
    
private:
    void testEnvironmentCreation() {
        nlm::GridWorldEnvironment env(5, 5);
        
        ASSERT_TRUE(env.getActionSpaceSize() > 0);
        ASSERT_TRUE(env.getObservationSpaceSize() > 0);
        ASSERT_TRUE(std::string(env.getName()).find("GridWorld") != std::string::npos);
        
        std::cout << "    testEnvironmentCreation passed" << std::endl;
    }
    
    void testEnvironmentReset() {
        nlm::GridWorldEnvironment env(5, 5);
        
        auto observation = env.reset();
        ASSERT_TRUE(observation != nullptr);
        
        std::cout << "    testEnvironmentReset passed" << std::endl;
    }
    
    void testEnvironmentStep() {
        nlm::GridWorldEnvironment env(5, 5);
        
        nlm::Action action;
        action.type = nlm::ActionType::Move;
        action.magnitude = 1.0f;
        
        auto observation = env.reset();
        auto nextObservation = env.step(action);
        
        ASSERT_TRUE(nextObservation != nullptr);
        
        float reward = env.getLastReward();
        ASSERT_TRUE(reward >= -1.0f && reward <= 1.0f);
        
        std::cout << "    testEnvironmentStep passed" << std::endl;
    }
    
    void testEnvironmentState() {
        nlm::GridWorldEnvironment env(5, 5);
        
        auto observation = env.reset();
        
        bool done = env.isDone();
        ASSERT_TRUE(done == false); // Initial state should not be done
        
        std::cout << "    testEnvironmentState passed" << std::endl;
    }
};

class ObservationTestSuite : public TestSuite {
public:
    ObservationTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Observation Test Suite..." << std::endl;
        
        testObservationCreation();
        testObservationData();
        testObservationProcessing();
        
        std::cout << "Observation Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "ObservationTestSuite";
    }
    
private:
    void testObservationCreation() {
        nlm::Observation obs(10);
        
        ASSERT_TRUE(obs.getSize() == 10);
        ASSERT_TRUE(obs.getValue(0) == 0.0f);
        
        std::cout << "    testObservationCreation passed" << std::endl;
    }
    
    void testObservationData() {
        nlm::Observation obs(5);
        
        std::vector<float> data = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        obs.setData(data);
        
        for (size_t i = 0; i < 5; ++i) {
            ASSERT_TRUE(std::abs(obs.getValue(i) - data[i]) < 0.001f);
        }
        
        std::cout << "    testObservationData passed" << std::endl;
    }
    
    void testObservationProcessing() {
        nlm::Observation obs(8);
        
        std::vector<float> data = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
        obs.setData(data);
        
        float mean = obs.getMean();
        ASSERT_TRUE(mean > 0.0f && mean < 1.0f);
        
        float maxVal = obs.getMax();
        ASSERT_TRUE(maxVal >= 0.8f);
        
        std::cout << "    testObservationProcessing passed" << std::endl;
    }
};

class ActionTestSuite : public TestSuite {
public:
    ActionTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Action Test Suite..." << std::endl;
        
        testActionCreation();
        testActionType();
        testActionMagnitude();
        
        std::cout << "Action Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "ActionTestSuite";
    }
    
private:
    void testActionCreation() {
        nlm::Action action;
        
        ASSERT_TRUE(action.type == nlm::ActionType::Wait);
        ASSERT_TRUE(action.magnitude == 0.0f);
        
        std::cout << "    testActionCreation passed" << std::endl;
    }
    
    void testActionType() {
        nlm::Action action;
        
        action.type = nlm::ActionType::Move;
        ASSERT_TRUE(action.type == nlm::ActionType::Move);
        
        action.type = nlm::ActionType::Rotate;
        ASSERT_TRUE(action.type == nlm::ActionType::Rotate);
        
        std::cout << "    testActionType passed" << std::endl;
    }
    
    void testActionMagnitude() {
        nlm::Action action;
        
        action.magnitude = 0.5f;
        ASSERT_TRUE(action.magnitude == 0.5f);
        
        action.magnitude = 1.0f;
        ASSERT_TRUE(action.magnitude == 1.0f);
        
        std::cout << "    testActionMagnitude passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createEnvironmentTestSuite() {
        return new EnvironmentTestSuite();
    }
    
    TestSuite* createObservationTestSuite() {
        return new ObservationTestSuite();
    }
    
    TestSuite* createActionTestSuite() {
        return new ActionTestSuite();
    }
}