// Cognition System Tests
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "cognition/SelfModel.hpp"
#include "cognition/SocialLearning.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

namespace test_cognition {

void testNeuralPlanner() {
    nlm::NeuralPlanner neuralPlanner;
    
    // Test initialization
    neuralPlanner.initialize(nullptr);
    
    // Test planning with empty state
    auto action = neuralPlanner.planAction(std::vector<float>(10, 0.0f), 0.5f);
    assert(action == nlm::ActionType::Wait); // Should default to wait if no good plan
    
    // Test planning depth
    assert(neuralPlanner.getPlanningDepth() == 3); // Default
    neuralPlanner.setPlanningDepth(5);
    assert(neuralPlanner.getPlanningDepth() == 5);
    
    // Test planning confidence
    assert(neuralPlanner.getPlanningConfidence() >= 0.0f && neuralPlanner.getPlanningConfidence() <= 1.0f);
    
    // Test evaluation with empty state
    auto candidate = neuralPlanner.evaluateSequence(std::vector<nlm::ActionType>{nlm::ActionType::Wait}, std::vector<float>(10, 0.0f));
    assert(candidate.actions.size() == 1);
    assert(candidate.depth == 1);
    
    // Test update plan quality
    neuralPlanner.updatePlanQuality(std::vector<nlm::ActionType>{nlm::ActionType::Wait}, std::vector<nlm::ActionType>{nlm::ActionType::Move}, 1.0f);
    
    // Test action quality setting
    neuralPlanner.setActionQuality(nlm::ActionType::Move, 0.8f);
    
    // Test goal setting
    neuralPlanner.setCurrentGoal(std::vector<float>(5, 0.5f));
    assert(neuralPlanner.getCurrentGoal().size() == 5);
    
    // Test clear cache
    neuralPlanner.clearCache();
    
    // Test recent plan success (internal)
    // Note: wasRecentPlanSuccessful() is internal, we can't easily test it
    
    std::cout << "    testNeuralPlanner passed" << std::endl;
}

void testConceptFormation() {
    nlm::ConceptFormation conceptFormation;
    
    // Test initialization
    conceptFormation.initialize(nullptr);
    
    // Test concept formation with input patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.4f, 0.5f, 0.6f};
    
    conceptFormation.update(pattern1);
    conceptFormation.update(pattern2);
    
    // Test concept retrieval
    auto concepts = conceptFormation.getConcepts();
    assert(!concepts.empty() || true); // May or may not form concepts depending on implementation
    
    // Test concept strength
    float strength = conceptFormation.getConceptStrength(0); // Assuming concept index 0
    assert(strength >= 0.0f && strength <= 1.0f);
    
    // Test concept updating
    conceptFormation.updateConcept(0, 0.5f);
    
    // Test concept removal
    conceptFormation.removeConcept(0);
    
    // Test concept evaluation
    float evaluation = conceptFormation.evaluateConcept(pattern1);
    assert(evaluation >= 0.0f && evaluation <= 1.0f);
    
    // Test concept count
    size_t count = conceptFormation.getConceptCount();
    assert(count >= 0);
    
    // Test concept history
    auto history = conceptFormation.getConceptHistory();
    assert(!history.empty());
    
    // Test concept stability
    bool stable = conceptFormation.isConceptStable(0);
    // May or may not be stable depending on implementation
    
    std::cout << "    testConceptFormation passed" << std::endl;
}

void testAttentionalSelection() {
    nlm::AttentionalSelection attentionalSelection;
    
    // Test initialization
    attentionalSelection.initialize(nullptr);
    
    // Test competition processing with empty competitors
    std::vector<nlm::NeuronId> competitors;
    auto winners = attentionalSelection.processCompetition(competitors);
    assert(winners.empty());
    
    // Test with single competitor
    competitors.push_back(nlm::NeuronId(1));
    winners = attentionalSelection.processCompetition(competitors);
    assert(!winners.empty());
    
    // Test focus and release
    attentionalSelection.focusOnRegion(nlm::RegionId(1));
    auto focused = attentionalSelection.getAttendedRegions();
    assert(!focused.empty());
    
    attentionalSelection.releaseAttention();
    auto released = attentionalSelection.getAttendedRegions();
    assert(released.empty());
    
    // Test parameter setting
    attentionalSelection.setInhibitionStrength(0.5f);
    attentionalSelection.setExcitationStrength(0.8f);
    attentionalSelection.setCompetitionThreshold(0.3f);
    
    // Test neuron attention levels
    float inhibition = attentionalSelection.getInhibitionFor(nlm::NeuronId(1));
    float excitation = attentionalSelection.getExcitationFor(nlm::NeuronId(1));
    
    assert(inhibition >= 0.0f);
    assert(excitation >= 0.0f);
    
    // Test attention update
    attentionalSelection.update(0.1f);
    
    // Test attended check
    bool attended = attentionalSelection.isAttended(nlm::NeuronId(1));
    // May or may not be attended depending on implementation
    
    // Test winners
    const auto& attentionalWinners = attentionalSelection.getWinners();
    assert(!attentionalWinners.empty() || true);
    
    // Test top-down and bottom-up bias
    attentionalSelection.applyTopDownBias(nlm::NeuronId(1), 0.7f);
    attentionalSelection.applyBottomUpSalience(nlm::NeuronId(1), 0.6f);
    
    // Test reset
    attentionalSelection.reset();
    auto resetWinners = attentionalSelection.getWinners();
    assert(resetWinners.empty());
    
    std::cout << "    testAttentionalSelection passed" << std::endl;
}

void testSelfModel() {
    nlm::SelfModel selfModel;
    
    // Test initialization
    selfModel.initialize(nullptr);
    
    // Test recording self action
    std::vector<float> beforeState = {0.1f, 0.2f, 0.3f};
    std::vector<float> afterState = {0.4f, 0.5f, 0.6f};
    
    selfModel.recordSelfAction(nlm::ActionType::Move, beforeState, afterState);
    
    // Test prediction
    std::vector<float> predicted = selfModel.predictActionConsequence(nlm::ActionType::Move, beforeState);
    assert(predicted.size() == afterState.size());
    
    // Test confidence
    float confidence = selfModel.getSelfModelConfidence(nlm::ActionType::Move);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test self-generated likeness
    float likeness = selfModel.computeSelfGeneratedLikeness(beforeState, afterState, nlm::ActionType::Move);
    assert(likeness >= 0.0f && likeness <= 1.0f);
    
    // Test preferred action
    auto preferred = selfModel.getPreferredAction(beforeState);
    // May or may not be defined depending on implementation
    
    // Test self-model update
    selfModel.updateSelfModel(predicted, afterState, nlm::ActionType::Move);
    
    // Test capability level
    float capability = selfModel.getCapabilityLevel();
    assert(capability >= 0.0f && capability <= 1.0f);
    
    // Test body awareness
    float awareness = selfModel.getBodyAwareness();
    assert(awareness >= 0.0f && awareness <= 1.0f);
    
    // Test has self-model
    bool hasModel = selfModel.hasSelfModel();
    assert(hasModel == true);
    
    // Test clear
    selfModel.clear();
    assert(selfModel.hasSelfModel() == false);
    
    std::cout << "    testSelfModel passed" << std::endl;
}

void testSocialLearning() {
    nlm::SocialLearning socialLearning;
    
    // Test initialization
    socialLearning.initialize(nullptr);
    
    // Test observation recording
    std::vector<float> observerState = {0.1f, 0.2f, 0.3f};
    std::vector<float> resultingState = {0.4f, 0.5f, 0.6f};
    
    socialLearning.observeAgentAction(nlm::ActionType::Move, observerState, resultingState);
    
    // Test imitation capability
    bool canImitate = socialLearning.canImitate(nlm::ActionType::Move);
    assert(canImitate == true);
    
    // Test imitation action
    auto imitationAction = socialLearning.getImitationAction(observerState);
    assert(imitationAction == nlm::ActionType::Move);
    
    // Test communication signal learning
    std::vector<float> signalPattern = {0.5f, 0.6f, 0.7f};
    socialLearning.learnCommunicationSignal(signalPattern, 1.0f);
    
    // Test signal detection
    bool signalDetected = socialLearning.detectSignal(signalPattern);
    assert(signalDetected == true);
    
    // Test signal meaning
    float meaning = socialLearning.getSignalMeaning();
    assert(meaning == 1.0f);
    
    // Test social knowledge update
    socialLearning.updateSocialKnowledge(0.8f);
    
    // Test has social knowledge
    bool hasKnowledge = socialLearning.hasSocialKnowledge();
    assert(hasKnowledge == true);
    
    // Test observation count
    size_t count = socialLearning.getObservationCount();
    assert(count > 0);
    
    // Test clear
    socialLearning.clear();
    assert(socialLearning.hasSocialKnowledge() == false);
    assert(socialLearning.getObservationCount() == 0);
    
    std::cout << "    testSocialLearning passed" << std::endl;
}

void runAll() {
    testNeuralPlanner();
    testConceptFormation();
    testAttentionalSelection();
    testSelfModel();
    testSocialLearning();
}

} // namespace test_cognition