#include "CognitiveVisualizer.hpp"
#include "BrainVisualizer.hpp"
#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace nlm {

void CognitiveVisualizer::setRegionFilter(std::vector<RegionId> regions) {
    regionFilter_ = std::move(regions);
}

void CognitiveVisualizer::setNeuronTypeFilter(std::vector<NeuronType> types) {
    neuronTypeFilter_ = std::move(types);
}

void CognitiveVisualizer::setFocusNeuron(NeuronId neuron) {
    focusNeuron_ = neuron;
}

void CognitiveVisualizer::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

void CognitiveVisualizer::setDetailLevel(int level) {
    detailLevel_ = std::max(0, std::min(level, 5));
}

void CognitiveVisualizer::setColorScheme(const std::string& scheme) {
    colorScheme_ = scheme;
}

} // namespace nlm