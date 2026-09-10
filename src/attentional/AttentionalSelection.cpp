#include "AttentionalSelection.cpp"
#include "../brain/Brain.hpp"

namespace nlm
{
void AttentionalSelection::applyToBrain(Brain* brain)
{
    if (!brain) return;
    
    // Apply attention to current neural activity
    // Use current brain state for attention allocation
    auto regions = brain->getRegions();
    
    // Track activity in each region
    std::vector<float> regionActivities;
    regionActivities.reserve(regions.size());
    
    for (const auto& region : regions) {
        if (region) {
            float activity = region->getAverageFiringRate();
            regionActivities.push_back(activity);
            
            // Apply top-down bias based on attention
            if (activity > 0.1f) {
                // Increase saliency of active regions
                for (auto& neuron : region->getAllNeurons()) {
                    applyBottomUpSalience(neuron->getId(), activity * 0.5f);
                }
            }
        }
    }
    
    // Select winner region based on highest activity
    if (!regionActivities.empty()) {
        size_t maxIdx = 0;
        float maxActivity = regionActivities[0];
        for (size_t i = 1; i < regionActivities.size(); ++i) {
            if (regionActivities[i] > maxActivity) {
                maxActivity = regionActivities[i];
                maxIdx = i;
            }
        }
        
        // Focus attention on most active region
        if (maxIdx < regions.size()) {
            RegionId regionId = regions[maxIdx]->getId();
            focusOnRegion(regionId);
            
            // Apply strong excitation to neurons in attended region
            for (auto* neuron : regions[maxIdx]->getAllNeurons()) {
                brain->injectCurrent(neuron->getId(), excitationStrength_ * 3.0f);
            }
        }
    }
}

void AttentionalSelection::reset()
{
    winners_.clear();
    attendedRegions_.clear();
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    if (pImpl) {
        pImpl->inhibitionLevel.clear();
    }
}

} // namespace nlm
