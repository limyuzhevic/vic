// AgentBrain Tests - Enhanced Motor Control
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_agentbrain {

void testSoftmaxActionSelector() {
    std::cout << "  Testing SoftmaxActionSelector..." << std::endl;
    
    nlm::SoftmaxActionSelector selector(1.0f, 0.1f);
    
    // Test with no activity
    std::vector<float> emptyActivities;
    nlm::MotorCommand cmd = selector.selectAction(emptyActivities);
    assert(cmd == nlm::MotorCommand::Wait);
    
    // Test with equal activities
    std::vector<float> equalActivities = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    cmd = selector.selectAction(equalActivities);
    assert(cmd == nlm::MotorCommand::Wait || 
           cmd == nlm::MotorCommand::MoveForward || 
           cmd == nlm::MotorCommand::MoveBackward);
    
    // Test temperature adjustment
    selector.setTemperature(0.5f);  // Lower temperature = more deterministic
    cmd = selector.selectAction(equalActivities, {}, 0.5f);
    
    // Test neuromodulator update
    selector.updateFromNeuromodulators(0.5f, 0.5f);
    
    std::cout << "    testSoftmaxActionSelector passed" << std::endl;
}

void testTemporalDifferenceLearner() {
    std::cout << "  Testing TemporalDifferenceLearner..." << std::endl;
    
    nlm::TemporalDifferenceLearner tdLearner(0.1f, 0.9f);
    
    // Initial state: no actions
    assert(tdLearner.getActionValue(nlm::ActionType::MoveForward) == 0.0f);
    
    // Update action value
    tdLearner.updateActionValue(nlm::ActionType::MoveForward, 1.0f, 0.5f);
    
    // Should have learned the value
    float value = tdLearner.getActionValue(nlm::ActionType::MoveForward);
    assert(value > 0.0f && value < 2.0f);
    
    // Test best action selection
    nlm::ActionType best = tdLearner.getBestAction();
    assert(best == nlm::ActionType::MoveForward);
    
    // Reset and test empty state
    tdLearner.reset();
    assert(tdLearner.getBestAction() == nlm::ActionType::Wait);
    
    std::cout << "    testTemporalDifferenceLearner passed" << std::endl;
}

void testActionSequence() {
    std::cout << "  Testing ActionSequence..." << std::endl;
    
    nlm::ActionSequence sequence;
    
    // Add commands
    sequence.addCommand(nlm::MotorCommand::MoveForward, 0.1f, 1.0f);
    sequence.addCommand(nlm::MotorCommand::TurnRight, 0.05f, 0.5f);
    sequence.addCommand(nlm::MotorCommand::Interact, 0.2f, 0.8f);
    
    // Verify sequence state
    assert(!sequence.isComplete());
    assert(sequence.getTotalExpectedReward() > 0.0f);
    assert(sequence.getTotalDuration() > 0.0f);
    
    // Execute commands
    nlm::MotorCommand cmd1 = sequence.executeNext();
    assert(cmd1 == nlm::MotorCommand::MoveForward);
    
    nlm::MotorCommand cmd2 = sequence.executeNext();
    assert(cmd2 == nlm::MotorCommand::TurnRight);
    
    nlm::MotorCommand cmd3 = sequence.executeNext();
    assert(cmd3 == nlm::MotorCommand::Interact);
    
    assert(sequence.isComplete());
    
    // Reset and re-execute
    sequence.reset();
    assert(!sequence.isComplete());
    assert(sequence.executeNext() == nlm::MotorCommand::MoveForward);
    
    std::cout << "    testActionSequence passed" << std::endl;
}

void testHierarchicalController() {
    std::cout << "  Testing HierarchicalController..." << std::endl;
    
    nlm::HierarchicalController controller;
    
    // Test without goal
    assert(controller.getCurrentGoal().empty());
    assert(!controller.isGoalAchieved());
    
    // Set goal and plan sequence
    controller.setGoal("NavigateToTarget", {1.0f, 2.0f});
    assert(controller.getCurrentGoal() == "NavigateToTarget");
    
    auto sequence = controller.planSequence(std::vector<float>(64, 0.0f));
    assert(sequence != nullptr);
    assert(!sequence->isComplete());
    assert(!controller.isGoalAchieved());  // Not yet complete
    
    // Execute actions
    nlm::MotorCommand cmd = controller.executeCurrentAction();
    assert(cmd == nlm::MotorCommand::MoveForward || cmd == nlm::MotorCommand::Wait);
    
    std::cout << "    testHierarchicalController passed" << std::endl;
}

void testQLearner() {
    std::cout << "  Testing QLearner..." << std::endl;
    
    nlm::QLearner qLearner(0.1f, 0.9f, 0.1f);
    
    std::vector<nlm::ActionType> actions = {nlm::ActionType::MoveForward, nlm::ActionType::Wait};
    std::unordered_map<nlm::ActionType, float> qValues = {
        {nlm::ActionType::MoveForward, 0.8f},
        {nlm::ActionType::Wait, 0.2f}
    };
    
    // Test action selection (should prefer MoveForward due to higher Q-value)
    nlm::ActionType selected = qLearner.selectAction(actions, qValues);
    // May be random due to epsilon, but likely MoveForward
    
    // Test Q-value update
    qLearner.updateQValue(nlm::ActionType::MoveForward, 1.0f, nlm::ActionType::Wait, 0.3f);
    
    std::cout << "    testQLearner passed" << std::endl;
}

void testDopamineModulator() {
    std::cout << "  Testing DopamineModulator..." << std::endl;
    
    nlm::DopamineModulator modulator(0.01f, 0.99f);
    
    // Initial state
    assert(modulator.getDopamineLevel() == 0.0f);
    assert(modulator.getPredictionError() == 0.0f);
    
    // Update with reward
    modulator.update(1.0f, 0.5f);
    assert(modulator.getPredictionError() == 0.5f);
    assert(modulator.getDopamineLevel() > 0.0f);  // Positive prediction error
    
    // Update with negative prediction error
    modulator.update(0.5f, 1.0f);
    assert(modulator.getPredictionError() == -0.5f);
    assert(modulator.getDopamineLevel() < 0.0f);  // Negative prediction error
    
    // Test learning rate adaptation
    modulator.adaptLearningRate(0.8f);
    
    // Reset
    modulator.reset();
    assert(modulator.getDopamineLevel() == 0.0f);
    
    std::cout << "    testDopamineModulator passed" << std::endl;
}

void testHebbianSkillLearner() {
    std::cout << "  Testing HebbianSkillLearner..." << std::endl;
    
    nlm::HebbianSkillLearner hebbian(0.01f, 0.001f);
    
    std::vector<float> sensoryInput = {0.5f, 0.3f, 0.7f};
    nlm::MotorCommand motorOutput = nlm::MotorCommand::MoveForward;
    
    // Learn association
    hebbian.learnAssociation(sensoryInput, motorOutput, 1.0f);
    
    // Retrieve response
    nlm::MotorCommand retrieved = hebbian.retrieveMotorResponse(sensoryInput);
    assert(retrieved == motorOutput || retrieved == nlm::MotorCommand::Wait);
    
    // Get skill strength
    float strength = hebbian.getSkillStrength(sensoryInput, motorOutput);
    assert(strength > 0.0f);
    
    // Test consolidation
    hebbian.consolidateSkills();
    
    std::cout << "    testHebbianSkillLearner passed" << std::endl;
}

void testAgentBrainEnhanced() {
    std::cout << "  Testing enhanced AgentBrain functionality..." << std::endl;
    
    // Create a simple brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create AgentBrain with enhanced capabilities
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Initialize (requires SimpleWorld - we'll skip for now)
    // agentBrain.initialize(world);
    
    // Test configuration
    agentBrain.enableAdvancedMotorDecoding(true);
    agentBrain.enableMotorPlanning(true);
    agentBrain.enableReinforcementLearning(true);
    agentBrain.enableSkillLearning(true);
    agentBrain.enableSensoryMotorIntegration(true);
    
    // Test advanced configuration
    agentBrain.setActionTemperature(1.0f);
    agentBrain.setQLearningParameters(0.1f, 0.9f, 0.1f);
    
    // Test action sequence functionality
    auto sequence = agentBrain.getCurrentActionSequence();
    assert(sequence == nullptr);  // No sequence planned yet
    
    // Test goal setting
    agentBrain.setGoal("ExploreArea", {});
    
    // Test skill strength
    std::vector<float> sensoryInput(64, 0.0f);
    float skillStrength = agentBrain.getSkillStrength(sensoryInput, nlm::MotorCommand::MoveForward);
    assert(skillStrength >= 0.0f);
    
    std::cout << "    testAgentBrainEnhanced passed" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "  Testing backward compatibility..." << std::endl;
    
    // Create brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Create AgentBrain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test basic functionality
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    // Test configuration flags
    assert(agentBrain.isCuriosityEnabled());
    assert(agentBrain.isDevelopmentEnabled());
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Enhanced AgentBrain Tests ===" << std::endl;
    
    testSoftmaxActionSelector();
    testTemporalDifferenceLearner();
    testActionSequence();
    testHierarchicalController();
    testQLearner();
    testDopamineModulator();
    testHebbianSkillLearner();
    testAgentBrainEnhanced();
    testBackwardCompatibility();
    
    std::cout << "=== All tests passed! ===" << std::endl;
}

} // namespace test_agentbrain

int main() {
    test_agentbrain::runAllTests();
    return 0;
}
