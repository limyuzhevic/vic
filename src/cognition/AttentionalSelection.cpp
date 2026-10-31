// AttentionalSelection Implementation
struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    
    Impl() {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                             float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        activities[i] = salience + bias;
        totalActivity += activities[i];
    }
    
    if (totalActivity < 0.001f) {
        return winners_;
    }
    
    // Apply global inhibition
    float inhibitionFactor = std::pow(globalInhibition, 1.0f / competitors.size());
    for (float& activity : activities) {
        activity *= inhibitionFactor;
    }
    
    // Normalize activities
    float maxActivity = *std::max_element(activities.begin(), activities.end());
    if (maxActivity > 0.0f) {
        for (float& activity : activities) {
            activity /= maxActivity;
        }
    }
    
    // Select winners above threshold
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] > competitionThreshold_) {
            winners_.push_back(competitors[i]);
        }
    }
    
    // If no winners, use top activity as single winner
    if (winners_.empty() && !activities.empty()) {
        size_t bestIdx = std::max_element(activities.begin(), activities.end()) - activities.begin();
        winners_.push_back(competitors[bestIdx]);
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    // Apply strong top-down bias to neurons in this region
    if (!brain_) return;
    
    const auto* regionPtr = brain_->getRegion(region);
    if (!regionPtr) return;
    
    for (const auto* neuron : regionPtr->getAllNeurons()) {
        topDownBias_[neuron->getId().value] = 2.0f;  // Strong bias
    }
}

void AttentionalSelection::releaseAttention() {
    // Clear all top-down biases
    topDownBias_.clear();
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    std::vector<RegionId> attended;
    for (const auto& bias : topDownBias_) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto* neuron : region->getAllNeurons()) {
                if (neuron->getId().value == bias.first) {
                    RegionId regionId = region->getId();
                    if (std::find(attended.begin(), attended.end(), regionId) == attended.end()) {
                        attended.push_back(regionId);
                    }
                    break;
                }
            }
        }
    }
    return attended;
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    auto it = inhibitionLevelMap_.find(neuron.value);
    return (it != inhibitionLevelMap_.end()) ? it->second : 0.0f;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = excitationLevelMap_.find(neuron.value);
    return (it != excitationLevelMap_.end()) ? it->second : 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay biases and salience over time
    for (auto& bias : topDownBias_) {
        bias.second *= (1.0f - dt * 0.01f);
    }
    
    for (auto& salience : bottomUpSalience_) {
        salience.second *= (1.0f - dt * 0.1f);
    }
    
    // Update inhibition levels
    for (auto& inhibition : inhibitionLevel_) {
        inhibition *= (1.0f - dt * 0.05f);
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return topDownBias_.find(neuron.value) != topDownBias_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    topDownBias_[neuron.value] += biasStrength;
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    bottomUpSalience_[neuron.value] += salienceStrength;
}

void AttentionalSelection::reset() {
    topDownBias_.clear();
    bottomUpSalience_.clear();
    inhibitionLevelMap_.clear();
    excitationLevelMap_.clear();
    winners_.clear();
}

} // namespace nlm