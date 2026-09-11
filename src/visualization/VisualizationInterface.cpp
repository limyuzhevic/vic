bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // PLACEHOLDER: No visualization in Phase 1
    pImpl->active = false;
    return true;
}

void VisualizationInterface::update() {
    // PLACEHOLDER: No visualization in Phase 1
}

void VisualizationInterface::render() {
    // PLACEHOLDER: No visualization in Phase 1
}

void VisualizationInterface::close() {
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // TODO PHASE 2: Implement network visualization
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // TODO PHASE 2: Implement activity visualization
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // TODO PHASE 2: Implement spike raster plot
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // TODO PHASE 2: Implement weight matrix visualization
}

void VisualizationInterface::setBrain(std::shared_ptr<Brain> brain) {
    brain_ = brain;
}

std::shared_ptr<Brain> VisualizationInterface::getBrain() const {
    return brain_;
}

bool VisualizationInterface::toggleActive() {
    pImpl->active = !pImpl->active;
    if (pImpl->active) {
        NLM_LOG_INFO("Visualization interface activated");
    } else {
        NLM_LOG_INFO("Visualization interface deactivated");
    }
    return pImpl->active;
}

void VisualizationInterface::renderNeurons(const Brain& brain) {
    if (!isActive() || !brain_) return;
    
    // Extract neuron information for visualization
    auto regions = brain.getRegions();
    for (size_t r = 0; r < regions.size(); ++r) {
        auto region = regions[r].get();
        auto populations = region->getPopulations();
        
        for (size_t p = 0; p < populations.size(); ++p) {
            auto pop = populations[p].get();
            auto neurons = pop->getNeurons();
            
            for (size_t n = 0; n < neurons.size(); ++n) {
                Neuron* neuron = neurons[n];
                if (!neuron) continue;
                
                // Get neuron state for visualization
                auto state = neuron->getState();
                float potential = state.membranePotential;
                bool firing = neuron->checkThreshold();
                
                // Visual properties based on neuron state
                float r, g, b;
                if (firing) {
                    // Active neuron: bright red/yellow
                    r = 1.0f;
                    g = 0.5f;
                    b = 0.0f;
                } else if (potential > -60.0f) {
                    // Depolarized: green gradient
                    float t = (potential + 60.0f) / 40.0f;  // Normalize to [0, 1]
                    r = 0.0f;
                    g = t;
                    b = 1.0f - t;
                } else {
                    // Hyperpolarized: blue gradient
                    float t = (potential + 100.0f) / 40.0f;  // Normalize to [0, 1]
                    r = 0.0f;
                    g = 0.0f;
                    b = 1.0f - t;
                }
                
                // In real implementation, render 3D sphere or 2D circle
                // RenderNeuronSphere(position, radius, color)
                // or
                // RenderNeuronCircle(position, radius, color)
                
                NLM_LOG_INFO("Visualizing neuron: ID=" + std::to_string(neuron->getId()) + 
                           " potential=" + std::to_string(potential) + 
                           " firing=" + std::to_string(firing));
            }
        }
    }
}

void VisualizationInterface::renderConnections(const Brain& brain) {
    if (!isActive() || !brain_) return;
    
    // Visualize synaptic connections
    auto regions = brain.getRegions();
    for (size_t r1 = 0; r1 < regions.size(); ++r1) {
        auto region1 = regions[r1].get();
        auto synapses = region1->getSynapses();
        
        for (size_t s = 0; s < synapses.size(); ++s) {
            auto synapse = synapses[s].get();
            
            // Get source and target neurons
            auto sourceNeurons = region1->getAllNeurons();
            auto targetNeurons = region1->getAllNeurons();
            
            // In real implementation, find actual source and target
            // For now, just log the connection
            
            float weight = synapse->getWeight();
            
            // Visualize connection based on weight
            float opacity = std::min(1.0f, std::max(0.0f, weight / 10.0f));  // Scale weight to opacity
            
            // Render connection line
            // RenderConnectionLine(sourcePos, targetPos, opacity, color)
            
            NLM_LOG_INFO("Visualizing connection: weight=" + std::to_string(weight) + 
                       " opacity=" + std::to_string(opacity));
        }
    }
}

void VisualizationInterface::renderSpikeRasters(const std::vector<SpikeEvent>& spikes) {
    if (!isActive() || spikes.empty()) return;
    
    // Render spike raster plot
    // Sort spikes by time
    std::vector<SpikeEvent> sortedSpikes = spikes;
    std::sort(sortedSpikes.begin(), sortedSpikes.end(), 
              [](const SpikeEvent& a, const SpikeEvent& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Create raster plot: neurons on y-axis, time on x-axis
    float maxTime = sortedSpikes.back().timestamp;
    size_t maxNeurons = 0;
    
    // Count unique neurons
    std::set<NeuronId> neuronIds;
    for (const auto& spike : sortedSpikes) {
        neuronIds.insert(spike.neuronId);
    }
    maxNeurons = neuronIds.size();
    
    NLM_LOG_INFO("Rendering spike raster: " + std::to_string(spikes.size()) + 
               " spikes, " + std::to_string(maxNeurons) + " neurons, " +
               std::to_string(maxTime) + " time units");
    
    // In real implementation:
    // 1. Create 2D coordinate system
    // 2. Plot each spike as a dot at (time, neuronIndex)
    // 3. Add neuron labels on y-axis
    // 4. Add time axis on x-axis
    // 5. Color-code by neuron type or plasticity state
    
    for (size_t i = 0; i < sortedSpikes.size() && i < 50; ++i) {  // Limit to first 50 for logging
        const auto& spike = sortedSpikes[i];
        NLM_LOG_INFO("Spike " + std::to_string(i) + ": neuron " + std::to_string(spike.neuronId) + 
                   " at time " + std::to_string(spike.timestamp));
    }
}

void VisualizationInterface::updateNeuronColor(NeuronId neuronId, const glm::vec3& color) {
    // Update color of specific neuron
    // In real implementation, maintain color mapping and update render
    neuronColors_[neuronId] = color;
    NLM_LOG_INFO("Updated neuron " + std::to_string(neuronId) + " color to (" +
                 std::to_string(color.r) + ", " + std::to_string(color.g) + ", " +
                 std::to_string(color.b) + ")");
}

void VisualizationInterface::updateConnectionOpacity(SynapseId synapseId, float opacity) {
    // Update opacity of specific connection
    // In real implementation, maintain connection opacity mapping and update render
    connectionOpacities_[synapseId] = opacity;
    NLM_LOG_INFO("Updated connection " + std::to_string(synapseId) + 
               " opacity to " + std::to_string(opacity));
}

const std::unordered_map<NeuronId, glm::vec3>& VisualizationInterface::getNeuronColors() const {
    return neuronColors_;
}

const std::unordered_map<SynapseId, float>& VisualizationInterface::getConnectionOpacities() const {
    return connectionOpacities_;
}

void VisualizationInterface::clearVisualizationData() {
    neuronColors_.clear();
    connectionOpacities_.clear();
    NLM_LOG_INFO("Cleared visualization data");
}

void VisualizationInterface::setRenderMode(RenderMode mode) {
    currentRenderMode_ = mode;
    NLM_LOG_INFO("Set render mode to " + std::to_string(static_cast<int>(mode)));
}

RenderMode VisualizationInterface::getRenderMode() const {
    return currentRenderMode_;
}

void VisualizationInterface::setColorMap(const std::string& colormapName) {
    currentColorMap_ = colormapName;
    NLM_LOG_INFO("Set color map to " + colormapName);
}

std::string VisualizationInterface::getColorMap() const {
    return currentColorMap_;
}

void VisualizationInterface::setTimeScale(float scale) {
    timeScale_ = scale;
    NLM_LOG_INFO("Set time scale to " + std::to_string(scale));
}

float VisualizationInterface::getTimeScale() const {
    return timeScale_;
}

void VisualizationInterface::setNeuronSize(float size) {
    neuronSize_ = size;
    NLM_LOG_INFO("Set neuron size to " + std::to_string(size));
}

float VisualizationInterface::getNeuronSize() const {
    return neuronSize_;
}

void VisualizationInterface::setConnectionWidth(float width) {
    connectionWidth_ = width;
    NLM_LOG_INFO("Set connection width to " + std::to_string(width));
}

float VisualizationInterface::getConnectionWidth() const {
    return connectionWidth_;
}

void VisualizationInterface::setEnableAntiAliasing(bool enable) {
    enableAntiAliasing_ = enable;
    NLM_LOG_INFO("Set anti-aliasing to " + std::to_string(enable));
}

bool VisualizationInterface::getEnableAntiAliasing() const {
    return enableAntiAliasing_;
}
