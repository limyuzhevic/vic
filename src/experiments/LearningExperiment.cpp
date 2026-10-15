#include "LearningExperiment.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

struct LearningExperiment::Impl {
    std::vector<std::pair<std::string, std::shared_ptr<Brain>>> brains;
    std::vector<SynapticChangeRecord> changes;
    
    Impl() {}
};

LearningExperiment::LearningExperiment() : pImpl(new Impl) {}

LearningExperiment::~LearningExperiment() = default;

void LearningExperiment::addBrain(const std::string& brainName, std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot add null brain to experiment");
        return;
    }
    
    pImpl->brains.emplace_back(brainName, brain);
    NLM_LOG_INFO("Added brain " + brainName + " to experiment");
}

void LearningExperiment::recordChange(const SynapticChangeRecord& change) {
    pImpl->changes.push_back(change);
    
    float weightChange = change.getWeightChange();
    if (std::abs(weightChange) > 0.01f) {
        NLM_LOG_INFO("Recorded synaptic change: " + change.preNeuron.value + 
                    " -> " + change.postNeuron.value + 
                    " (weight: " + std::to_string(weightChange) + ")");
    }
}

size_t LearningExperiment::getTotalChangesRecorded() const {
    return pImpl->changes.size();
}

void LearningExperiment::reset() {
    pImpl->brains.clear();
    pImpl->changes.clear();
    NLM_LOG_INFO("Learning experiment reset");
}

void LearningExperiment::exportToJSON(const std::string& filename) const {
    json experimentData;
    
    json brainsArray = json::array();
    for (const auto& pair : pImpl->brains) {
        json brainData;
        brainData["name"] = pair.first;
        brainData["neuron_count"] = pair.second->getTotalNeuronCount();
        brainArray.push_back(brainData);
    }
    experimentData["brains"] = brainsArray;
    
    json changesArray = json::array();
    for (const auto& change : pImpl->changes) {
        json changeData;
        changeData["pre_neuron"] = change.preNeuron.value;
        changeData["post_neuron"] = change.postNeuron.value;
        changeData["old_weight"] = change.oldWeight;
        changeData["new_weight"] = change.newWeight;
        changeData["weight_change"] = change.getWeightChange();
        changesArray.push_back(changeData);
    }
    experimentData["changes"] = changesArray;
    
    std::ofstream file(filename);
    if (file.is_open()) {
        file << experimentData.dump(2);
        NLM_LOG_INFO("Exported experiment data to JSON: " + filename);
    }
}

void LearningExperiment::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to export experiment data to CSV: " + filename);
        return;
    }
    
    file << "pre_neuron,post_neuron,old_weight,new_weight,weight_change\n";
    
    for (const auto& change : pImpl->changes) {
        file << change.preNeuron.value << "," 
             << change.postNeuron.value << "," 
             << change.oldWeight << "," 
             << change.newWeight << "," 
             << change.getWeightChange() << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Exported experiment data to CSV: " + filename);
}

std::string LearningExperiment::getTrainingSummary() const {
    std::stringstream ss;
    ss << "Learning Experiment Summary" << std::endl;
    ss << "===========================" << std::endl;
    ss << "Brains monitored: " << pImpl->brains.size() << std::endl;
    ss << "Total synaptic changes: " << pImpl->changes.size() << std::endl;
    
    if (!pImpl->changes.empty()) {
        float totalChange = 0.0f;
        for (const auto& change : pImpl->changes) {
            totalChange += change.getWeightChange();
        }
        ss << "Average weight change: " << (totalChange / pImpl->changes.size()) << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm