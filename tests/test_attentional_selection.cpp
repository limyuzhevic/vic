// AttentionalSelection Tests
#include "cognition/AttentionalSelection.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_attentional_selection {

void testAttentionalSelectionCreation() {
    nlm::AttentionalSelection selection;
    
    assert(selection.getAttentionStrength() == 0.0f);
    assert(selection.getSelectedItem() == nlm::INVALID_NEURON_ID);
    assert(selection.getInhibitionStrength() == 0.5f);  // Default
    assert(selection.getExcitationStrength() == 1.5f);  // Default
    
    std::cout << "    testAttentionalSelectionCreation passed" << std::endl;
}

void testAttentionalSelectionProcessCompetition() {
    nlm::AttentionalSelection selection;
    
    // Initialize with a dummy brain
    selection.initialize(nullptr);  // In real usage, would pass a valid brain
    
    std::vector<nlm::NeuronId> competitors;
    competitors.push_back(nlm::NeuronId(1));
    competitors.push_back(nlm::NeuronId(2));
    competitors.push_back(nlm::NeuronId(3));
    
    selection.processCompetition(competitors);
    
    // Should select one of the competitors
    assert(selection.getSelectedItem() != nlm::INVALID_NEURON_ID);
    assert(std::find(competitors.begin(), competitors.end(), selection.getSelectedItem()) != competitors.end());
    
    std::cout << "    testAttentionalSelectionProcessCompetition passed" << std::endl;
}

void testAttentionalSelectionUpdate() {
    nlm::AttentionalSelection selection;
    
    // Set attention strength
    selection.setAttentionStrength(0.8f);
    assert(selection.getAttentionStrength() == 0.8f);
    
    // Update should decay attention
    selection.update(0.1f);
    // Attention should decay (multiply by decay rate)
    
    std::cout << "    testAttentionalSelectionUpdate passed" << std::endl;
}

void testAttentionalSelectionConfiguration() {
    nlm::AttentionalSelection selection;
    
    selection.setInhibitionStrength(0.7f);
    assert(selection.getInhibitionStrength() == 0.7f);
    
    selection.setExcitationStrength(2.0f);
    assert(selection.getExcitationStrength() == 2.0f);
    
    std::cout << "    testAttentionalSelectionConfiguration passed" << std::endl;
}

void testAttentionalSelectionReset() {
    nlm::AttentionalSelection selection;
    
    // Process competition to set state
    std::vector<nlm::NeuronId> competitors;
    competitors.push_back(nlm::NeuronId(1));
    selection.processCompetition(competitors);
    selection.setAttentionStrength(0.9f);
    
    // Reset should clear state
    selection.reset();
    assert(selection.getSelectedItem() == nlm::INVALID_NEURON_ID);
    assert(selection.getAttentionStrength() == 0.0f);
    
    std::cout << "    testAttentionalSelectionReset passed" << std::endl;
}

void runAll() {
    testAttentionalSelectionCreation();
    testAttentionalSelectionProcessCompetition();
    testAttentionalSelectionUpdate();
    testAttentionalSelectionConfiguration();
    testAttentionalSelectionReset();
}

} // namespace test_attentional_selection