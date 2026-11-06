// Helper Functions for Main Application
// Contains utility functions for brain monitoring and status reporting

#include "main/helpers.hpp"
#include <iomanip>

using namespace nlm;

void printBrainSummary(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Brain Summary ===");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Excitation/Inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
}
