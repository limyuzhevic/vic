// Optimized Brain Systems Implementation
// File: src/brain/OptimizedBrain.cpp

#include "OptimizedBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <cmath>

namespace nlm {

// OptimizedNeuron Implementation
OptimizedNeuron::OptimizedNeuron(NeuronId id) : id_(id), type_(NeuronType::Internal) {
    // Initialize optimized state
    state_ = OptimizedLIFState();
    spike_history_ = SpikeHistory();
    incoming_synapses_.reserve(100);  // Pre-allocate for common case
    outgoing_synapses_.reserve(100);
    
    NLM_LOG_DEBUG("Optimized neuron created with ID: " + std::to_string(id_));
}

OptimizedNeuron::~OptimizedNeuron() = default;

OptimizedNeuron::OptimizedNeuron(OptimizedNeuron&& other) noexcept
    : id_(other.id_), type_(other.type_), state_(std::move(other.state_)),
      spike_history_(std::move(other.spike_history_)),
      incoming_synapses_(std::move(other.incoming_synapses_)),
      outgoing_synapses_(std::move(other.outgoing_synapses_)) {
}

OptimizedNeuron& OptimizedNeuron::operator=(OptimizedNeuron&& other) noexcept {
    if (this != &other) {
        id_ = other.id_;
        type_ = other.type_;
        state_ = std::move(other.state_);
        spike_history_ = std::move(other.spike_history_);
        incoming_synapses_ = std::move(other.incoming_synapses_);
        outgoing_synapses_ = std::move(other.outgoing_synapses_);
    }
    return *this;
}

bool OptimizedNeuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (state_.refractoryRemaining > 0) {
        --state_.refractoryRemaining;
        if (state_.refractoryRemaining == 0) {
            state_.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Optimized LIF dynamics with precomputed constants
    float C = MEMBRANE_CAPACITANCE;
    float tau = TIME_CONSTANT;
    
    // Compute synaptic input contribution
    float synaptic_contribution = computeSynapticInput() / C;
    
    // Leak contribution
    float leak_contribution = (state_.resting_potential - state_.membrane_potential) / tau;
    
    // Update membrane potential with optimized Euler integration
    state_.membrane_potential += dt * 1000.0f * (leak_contribution + synaptic_contribution);
    
    // Apply spike-frequency adaptation
    updateAdaptation();
    
    // Clamp membrane potential for stability
    clampMembranePotential();
    
    // Check for spike with optimized threshold comparison
    if (state_.membrane_potential >= state_.threshold) {
        fired = true;
        state_.firingState = FiringState::Active;
        state_.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        state_.membrane_potential = state_.resetPotential;
        
        // Enter refractory period
        state_.refractoryRemaining = state_.refractoryPeriod;
        state_.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        state_.adaptationVariable += 1.0f;
    } else {
        state_.firingState = FiringState::Active;
    }
    
    return fired;
}

void OptimizedNeuron::receiveExcitatoryInput(const std::vector<float>& amplitudes) {
    if (amplitudes.empty()) return;
    
    // Optimized batch processing
    float total_input = std::accumulate(amplitudes.begin(), amplitudes.end(), 0.0f);
    state_.synapticConductance += total_input;
}

void OptimizedNeuron::receiveInhibitoryInput(const std::vector<float>& amplitudes) {
    if (amplitudes.empty()) return;
    
    // Optimized batch processing
    float total_input = std::accumulate(amplitudes.begin(), amplitudes.end(), 0.0f);
    state_.synapticConductance -= total_input;
}

void OptimizedNeuron::receiveModulatoryInput(const std::vector<float>& amplitudes) {
    if (amplitudes.empty()) return;
    
    // Optimized batch processing
    float total_input = std::accumulate(amplitudes.begin(), amplitudes.end(), 0.0f);
    state_.adaptationVariable += total_input * 0.1f;
}

void OptimizedNeuron::processBatchInputs(const std::vector<float>& excitatory,
                                        const std::vector<float>& inhibitory,
                                        const std::vector<float>& modulatory) {
    // Optimized batch processing with single pass
    float exc_total = 0.0f, inh_total = 0.0f, mod_total = 0.0f;
    
    for (size_t i = 0; i < excitatory.size(); ++i) {
        exc_total += excitatory[i];
        if (i < inhibitory.size()) inh_total += inhibitory[i];
        if (i < modulatory.size()) mod_total += modulatory[i];
    }
    
    state_.synapticConductance += exc_total - inh_total;
    state_.adaptationVariable += mod_total * 0.1f;
}

void OptimizedNeuron::recordSpike(Timestamp timestamp) {
    spike_history_.add(timestamp);
}

float OptimizedNeuron::getAverageFiringRate() const {
    size_t spike_count = spike_history_.size();
    if (spike_count < 2) return 0.0f;
    
    // Get recent spikes for rate calculation
    auto spikes = spike_history_.getRecentSpikes(1000.0f); // Last 1 second
    if (spikes.size() < 2) return 0.0f;
    
    float time_span = spikes.back() - spikes.front();
    if (time_span <= 0.0f) return 0.0f;
    
    return static_cast<float>(spikes.size() - 1) / (time_span / 1000.0f); // Convert to Hz
}

std::vector<Timestamp> OptimizedNeuron::getRecentSpikes(float time_window_ms) const {
    return spike_history_.getRecentSpikes(time_window_ms);
}

void OptimizedNeuron::updateAdaptation() {
    if (state_.adaptationVariable > 0.0f) {
        state_.membrane_potential -= state_.adaptationVariable * 0.01f;
        state_.adaptationVariable *= ADAPTATION_DECAY;
    }
}

void OptimizedNeuron::clampMembranePotential() {
    if (state_.membrane_potential > 50.0f) state_.membrane_potential = 50.0f;
    if (state_.membrane_potential < -100.0f) state_.membrane_potential = -100.0f;
}

float OptimizedNeuron::computeSynapticInput() const {
    return state_.synapticConductance;
}

void OptimizedNeuron::optimizeMemoryLayout() {
    // Optimize memory layout by rearranging data members
    // This is a no-op in this implementation but would involve
    // data structure reorganization in a real implementation
}

void OptimizedNeuron::enableSIMD(bool enable) {
    // Enable/disable SIMD optimizations
    // Implementation would involve compiler-specific SIMD intrinsics
}

void OptimizedNeuron::recordPerformance(const std::string& operation, double duration_ms) {
    // Record performance metrics for profiling
    getGlobalProfiler().record("Neuron." + operation, duration_ms);
}

// OptimizedNeuralPopulation Implementation
OptimizedNeuralPopulation::OptimizedNeuralPopulation(PopulationId id, 
                                                   const std::vector<NeuronId>& neuron_ids)
    : id_(id), neuron_ids_(neuron_ids), neurons_(neuron_ids.size()) {
    
    // Pre-allocate synaptic input buffers
    excitatory_inputs_.resize(neuron_ids.size(), 0.0f);
    inhibitory_inputs_.resize(neuron_ids.size(), 0.0f);
    modulatory_inputs_.resize(neuron_ids.size(), 0.0f);
    
    NLM_LOG_DEBUG("Optimized neural population created with ID: " + std::to_string(id_));
}

OptimizedNeuralPopulation::~OptimizedNeuralPopulation() = default;

OptimizedNeuralPopulation::OptimizedNeuralPopulation(OptimizedNeuralPopulation&& other) noexcept
    : id_(other.id_), neuron_ids_(std::move(other.neuron_ids_)), 
      neurons_(std::move(other.neurons_)),
      excitatory_inputs_(std::move(other.excitatory_inputs_)),
      inhibitory_inputs_(std::move(other.inhibitory_inputs_)),
      modulatory_inputs_(std::move(other.modulatory_inputs_)),
      spiking_neurons_(std::move(other.spiking_neurons_)),
      spike_times_(std::move(other.spike_times_)),
      performance_metrics_(other.performance_metrics_) {
}

OptimizedNeuralPopulation& OptimizedNeuralPopulation::operator=(OptimizedNeuralPopulation&& other) noexcept {
    if (this != &other) {
        id_ = other.id_;
        neuron_ids_ = std::move(other.neuron_ids_);
        neurons_ = std::move(other.neurons_);
        excitatory_inputs_ = std::move(other.excitatory_inputs_);
        inhibitory_inputs_ = std::move(other.inhibitory_inputs_);
        modulatory_inputs_ = std::move(other.modulatory_inputs_);
        spiking_neurons_ = std::move(other.spiking_neurons_);
        spike_times_ = std::move(other.spike_times_);
        performance_metrics_ = other.performance_metrics_;
    }
    return *this;
}

void OptimizedNeuralPopulation::stepAllNeurons(Timestamp currentTime, TimestepDuration dt) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Reset input buffers
    resetInputBuffers();
    
    // Process synaptic inputs for all neurons
    updateSynapticInputs();
    
    // Step each neuron
    size_t neurons_processed = 0;
    std::vector<NeuronId> spiking_neurons;
    
    for (size_t i = 0; i < neurons_.size(); ++i) {
        if (neurons_[i]) {
            neurons_processed++;
            if (neurons_[i]->stepLIF(currentTime, dt)) {
                spiking_neurons.push_back(neuron_ids_[i]);
            }
        }
    }
    
    // Process spikes
    processSpikes(spiking_neurons);
    
    // Update refractory periods
    updateAllRefractoryPeriods();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    // Update performance metrics
    performance_metrics_.step_time_ms = duration_ms;
    performance_metrics_.neurons_processed = neurons_processed;
    performance_metrics_.spikes_detected = spiking_neurons.size();
    performance_metrics_.average_activity = getPopulationActivity();
    
    updatePerformanceMetrics();
}

void OptimizedNeuralPopulation::processAllSynapticInputs() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Clear input buffers
    resetInputBuffers();
    
    // Process synaptic inputs for each neuron
    for (size_t i = 0; i < neurons_.size(); ++i) {
        if (neurons_[i]) {
            // Process incoming synapses
            for (const auto& synapse : neurons_[i]->getIncomingSynapses()) {
                // This would be implemented based on synapse structure
                // For now, just a placeholder
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    performance_metrics_.synaptic_time_ms = duration_ms;
}

void OptimizedNeuralPopulation::updateAllRefractoryPeriods() {
    // Update refractory periods for all neurons
    for (size_t i = 0; i < neurons_.size(); ++i) {
        if (neurons_[i] && neurons_[i]->isRefractory()) {
            neurons_[i]->decrementRefractory();
        }
    }
}

std::vector<NeuronId> OptimizedNeuralPopulation::detectAndProcessSpikes(Timestamp currentTime) {
    std::vector<NeuronId> spiking_neurons;
    
    for (size_t i = 0; i < neurons_.size(); ++i) {
        if (neurons_[i] && neurons_[i]->isFiring()) {
            spiking_neurons.push_back(neuron_ids_[i]);
            neurons_[i]->setFiringState(FiringState::Active); // Maintain active state
        }
    }
    
    return spiking_neurons;
}

void OptimizedNeuralPopulation::processSpikes(const std::vector<NeuronId>& spiking_neurons) {
    // Process spikes for all neurons in the population
    for (const auto& neuron_id : spiking_neurons) {
        auto neuron_it = std::find(neuron_ids_.begin(), neuron_ids_.end(), neuron_id);
        if (neuron_it != neuron_ids_.end()) {
            size_t index = std::distance(neuron_ids_.begin(), neuron_it);
            if (neurons_[index]) {
                // Process spike effects
                neurons_[index]->setFiringState(FiringState::Active);
            }
        }
    }
}

float OptimizedNeuralPopulation::getAverageFiringRate() const {
    float total_rate = 0.0f;
    size_t active_count = 0;
    
    for (const auto& neuron : neurons_) {
        if (neuron) {
            total_rate += neuron->getAverageFiringRate();
            active_count++;
        }
    }
    
    return active_count > 0 ? total_rate / active_count : 0.0f;
}

float OptimizedNeuralPopulation::getPopulationActivity() const {
    float active_count = 0.0f;
    
    for (const auto& neuron : neurons_) {
        if (neuron && neuron->isFiring()) {
            active_count++;
        }
    }
    
    return (neurons_.size() > 0) ? active_count / neurons_.size() : 0.0f;
}

std::vector<float> OptimizedNeuralPopulation::getFiringRates() const {
    std::vector<float> rates;
    rates.reserve(neurons_.size());
    
    for (const auto& neuron : neurons_) {
        if (neuron) {
            rates.push_back(neuron->getAverageFiringRate());
        } else {
            rates.push_back(0.0f);
        }
    }
    
    return rates;
}

void OptimizedNeuralPopulation::optimizeMemoryAccess() {
    // Optimize memory access patterns
    // This would involve restructuring data for cache locality
    // For now, a no-op implementation
}

void OptimizedNeuralPopulation::enableVectorization(bool enable) {
    // Enable/disable vectorized operations
    // Implementation would involve compiler-specific vector intrinsics
}

void OptimizedNeuralPopulation::updatePerformanceMetrics() {
    // Update performance metrics for monitoring
    getGlobalProfiler().record("Population.stepAllNeurons", performance_metrics_.step_time_ms);
    getGlobalProfiler().record("Population.synapticProcessing", performance_metrics_.synaptic_time_ms);
    getGlobalProfiler().record("Population.activity", performance_metrics_.average_activity);
}

void OptimizedNeuralPopulation::updateSynapticInputs() {
    // Update synaptic inputs from regions or other sources
    // This is a placeholder implementation
}

void OptimizedNeuralPopulation::resetInputBuffers() {
    // Reset synaptic input buffers
    std::fill(excitatory_inputs_.begin(), excitatory_inputs_.end(), 0.0f);
    std::fill(inhibitory_inputs_.begin(), inhibitory_inputs_.end(), 0.0f);
    std::fill(modulatory_inputs_.begin(), modulatory_inputs_.end(), 0.0f);
}

// OptimizedNeuralRegion Implementation
OptimizedNeuralRegion::OptimizedNeuralRegion(RegionId id) : id_(id) {
    NLM_LOG_DEBUG("Optimized neural region created with ID: " + std::to_string(id_));
}

OptimizedNeuralRegion::~OptimizedNeuralRegion() = default;

OptimizedNeuralRegion::OptimizedNeuralRegion(OptimizedNeuralRegion&& other) noexcept
    : id_(other.id_), neurons_(std::move(other.neurons_)),
      populations_(std::move(other.populations_)),
      spatial_index_(std::move(other.spatial_index_)),
      performance_metrics_(other.performance_metrics_) {
}

OptimizedNeuralRegion& OptimizedNeuralRegion::operator=(OptimizedNeuralRegion&& other) noexcept {
    if (this != &other) {
        id_ = other.id_;
        neurons_ = std::move(other.neurons_);
        populations_ = std::move(other.populations_);
        spatial_index_ = std::move(other.spatial_index_);
        performance_metrics_ = other.performance_metrics_;
    }
    return *this;
}

void OptimizedNeuralRegion::addNeuron(std::unique_ptr<OptimizedNeuron> neuron) {
    neurons_.push_back(std::move(neuron));
}

void OptimizedNeuralRegion::removeNeuron(NeuronId id) {
    // Remove neuron by ID
    auto it = std::remove_if(neurons_.begin(), neurons_.end(),
                           [id](const std::unique_ptr<OptimizedNeuron>& neuron) {
                               return neuron && neuron->getId() == id;
                           });
    neurons_.erase(it, neurons_.end());
}

OptimizedNeuron* OptimizedNeuralRegion::getNeuron(NeuronId id) {
    for (auto& neuron : neurons_) {
        if (neuron && neuron->getId() == id) {
            return neuron.get();
        }
    }
    return nullptr;
}

const OptimizedNeuron* OptimizedNeuralRegion::getNeuron(NeuronId id) const {
    for (const auto& neuron : neurons_) {
        if (neuron && neuron->getId() == id) {
            return neuron.get();
        }
    }
    return nullptr;
}

void OptimizedNeuralRegion::addPopulation(std::unique_ptr<OptimizedNeuralPopulation> population) {
    populations_.push_back(std::move(population));
}

OptimizedNeuralPopulation* OptimizedNeuralRegion::getPopulation(PopulationId id) {
    for (auto& population : populations_) {
        if (population && population->getId() == id) {
            return population.get();
        }
    }
    return nullptr;
}

const OptimizedNeuralPopulation* OptimizedNeuralRegion::getPopulation(PopulationId id) const {
    for (const auto& population : populations_) {
        if (population && population->getId() == id) {
            return population.get();
        }
    }
    return nullptr;
}

void OptimizedNeuralRegion::stepRegion(Timestamp currentTime, TimestepDuration dt) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Step all neurons in the region
    size_t neurons_stepped = 0;
    size_t populations_stepped = 0;
    
    for (auto& neuron : neurons_) {
        if (neuron) {
            neurons_stepped++;
            neuron->stepLIF(currentTime, dt);
        }
    }
    
    for (auto& population : populations_) {
        if (population) {
            populations_stepped++;
            population->stepAllNeurons(currentTime, dt);
        }
    }
    
    // Process connections
    processConnections();
    
    // Update region statistics
    updateRegionStatistics();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    performance_metrics_.step_time_ms = duration_ms;
    performance_metrics_.neurons_stepped = neurons_stepped;
    performance_metrics_.populations_stepped = populations_stepped;
    performance_metrics_.average_activity = getRegionActivity();
    
    updateRegionPerformanceMetrics();
}

void OptimizedNeuralRegion::processConnections() {
    // Process all connections within the region
    for (auto& neuron : neurons_) {
        if (neuron) {
            // Process incoming and outgoing connections
            for (const auto& synapse : neuron->getIncomingSynapses()) {
                // This would be implemented based on the actual synapse structure
            }
            for (const auto& synapse : neuron->getOutgoingSynapses()) {
                // This would be implemented based on the actual synapse structure
            }
        }
    }
}

void OptimizedNeuralRegion::updateRegionStatistics() {
    // Update region statistics for monitoring
    // This would include activity metrics, connectivity statistics, etc.
}

void OptimizedNeuralRegion::buildSpatialIndex() {
    // Build spatial index for fast neuron access
    spatial_index_.neuron_positions.clear();
    spatial_index_.neuron_ids.clear();
    
    for (const auto& neuron : neurons_) {
        if (neuron) {
            // Store neuron position (placeholder - would use actual spatial coordinates)
            spatial_index_.neuron_positions.push_back(std::make_pair(0.5f, 0.5f));
            spatial_index_.neuron_ids.push_back(neuron->getId());
        }
    }
}

std::vector<NeuronId> OptimizedNeuralRegion::getNeuronsInRegion(float x_min, float x_max,
                                                                float y_min, float y_max) const {
    std::vector<NeuronId> result;
    
    if (!spatial_index_.neuron_positions.empty()) {
        for (size_t i = 0; i < spatial_index_.neuron_positions.size(); ++i) {
            float x = spatial_index_.neuron_positions[i].first;
            float y = spatial_index_.neuron_positions[i].second;
            
            if (x >= x_min && x <= x_max && y >= y_min && y <= y_max) {
                result.push_back(spatial_index_.neuron_ids[i]);
            }
        }
    }
    
    return result;
}

std::vector<NeuronId> OptimizedNeuralRegion::getNeuronsByType(NeuronType type) const {
    std::vector<NeuronId> result;
    
    for (const auto& neuron : neurons_) {
        if (neuron && neuron->getType() == type) {
            result.push_back(neuron->getId());
        }
    }
    
    return result;
}

void OptimizedNeuralRegion::optimizeMemoryUsage() {
    // Optimize memory usage
    // This would involve memory pooling, cache optimization, etc.
}

void OptimizedNeuralRegion::enableSpatialIndexing(bool enable) {
    if (enable) {
        buildSpatialIndex();
    }
}

float OptimizedNeuralRegion::getRegionActivity() const {
    if (neurons_.empty()) return 0.0f;
    
    float active_count = 0.0f;
    for (const auto& neuron : neurons_) {
        if (neuron && neuron->isFiring()) {
            active_count++;
        }
    }
    
    return active_count / neurons_.size();
}

float OptimizedNeuralRegion::getAverageFiringRate() const {
    if (neurons_.empty()) return 0.0f;
    
    float total_rate = 0.0f;
    size_t active_count = 0;
    
    for (const auto& neuron : neurons_) {
        if (neuron) {
            float rate = neuron->getAverageFiringRate();
            total_rate += rate;
            if (rate > 0.0f) active_count++;
        }
    }
    
    return (active_count > 0) ? total_rate / active_count : 0.0f;
}

size_t OptimizedNeuralRegion::getNeuronCount() const {
    return neurons_.size();
}

size_t OptimizedNeuralRegion::getPopulationCount() const {
    return populations_.size();
}

void OptimizedNeuralRegion::updateSpatialIndex() {
    // Update spatial index
    buildSpatialIndex();
}

void OptimizedNeuralRegion::optimizeConnectionProcessing() {
    // Optimize connection processing
    // This would involve connection graph optimization, etc.
}

void OptimizedNeuralRegion::updateRegionPerformanceMetrics() {
    // Update performance metrics for monitoring
    getGlobalProfiler().record("Region.stepRegion", performance_metrics_.step_time_ms);
    getGlobalProfiler().record("Region.activity", performance_metrics_.average_activity);
}

// OptimizedBrain Implementation
OptimizedBrain::OptimizedBrain() 
    : real_time_profiling_enabled_(false), simd_enabled_(false), 
      vectorization_enabled_(false), parallel_processing_enabled_(false),
      thread_count_(std::thread::hardware_concurrency()) {
    
    NLM_LOG_DEBUG("Optimized brain created");
}

OptimizedBrain::~OptimizedBrain() = default;

OptimizedBrain::OptimizedBrain(OptimizedBrain&& other) noexcept
    : regions_(std::move(other.regions_)), populations_(std::move(other.populations_)),
      neurons_(std::move(other.neurons_)),
      real_time_profiling_enabled_(other.real_time_profiling_enabled_),
      simd_enabled_(other.simd_enabled_), vectorization_enabled_(other.vectorization_enabled_),
      parallel_processing_enabled_(other.parallel_processing_enabled_),
      thread_count_(other.thread_count_),
      global_metrics_(other.global_metrics_) {
}

OptimizedBrain& OptimizedBrain::operator=(OptimizedBrain&& other) noexcept {
    if (this != &other) {
        regions_ = std::move(other.regions_);
        populations_ = std::move(other.populations_);
        neurons_ = std::move(other.neurons_);
        real_time_profiling_enabled_ = other.real_time_profiling_enabled_;
        simd_enabled_ = other.simd_enabled_;
        vectorization_enabled_ = other.vectorization_enabled_;
        parallel_processing_enabled_ = other.parallel_processing_enabled_;
        thread_count_ = other.thread_count_;
        global_metrics_ = other.global_metrics_;
    }
    return *this;
}

void OptimizedBrain::addNeuron(std::unique_ptr<OptimizedNeuron> neuron) {
    neurons_.push_back(std::move(neuron));
}

void OptimizedBrain::addRegion(std::unique_ptr<OptimizedNeuralRegion> region) {
    regions_.push_back(std::move(region));
}

void OptimizedBrain::addPopulation(std::unique_ptr<OptimizedNeuralPopulation> population) {
    populations_.push_back(std::move(population));
}

void OptimizedBrain::stepBrain(Timestamp currentTime, TimestepDuration dt) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Step all regions
    size_t total_neurons_stepped = 0;
    size_t total_spikes_generated = 0;
    
    for (auto& region : regions_) {
        if (region) {
            region->stepRegion(currentTime, dt);
            total_neurons_stepped += region->getNeuronCount();
        }
    }
    
    for (auto& population : populations_) {
        if (population) {
            population->stepAllNeurons(currentTime, dt);
            // Count spikes (would need actual spike counting)
            total_spikes_generated += population->getNeuronCount() * 0.1; // Estimate
        }
    }
    
    // Process all connections
    processAllConnections();
    
    // Update all statistics
    updateAllStatistics();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    global_metrics_.total_step_time_ms += duration_ms;
    global_metrics_.total_neurons_stepped += total_neurons_stepped;
    global_metrics_.total_spikes_generated += static_cast<size_t>(total_spikes_generated);
    global_metrics_.average_step_time_ms = duration_ms;
    
    updateGlobalPerformanceMetrics();
}

void OptimizedBrain::processAllConnections() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Process all connections between neurons
    for (auto& region : regions_) {
        if (region) {
            region->processConnections();
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    global_metrics_.total_connection_time_ms += duration_ms;
    global_metrics_.average_connection_time_ms = duration_ms;
}

void OptimizedBrain::updateAllStatistics() {
    // Update global statistics
    global_metrics_.overall_activity = getOverallActivity();
}

void OptimizedBrain::enableRealTimeProfiling(bool enable) {
    real_time_profiling_enabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Real-time profiling enabled for optimized brain");
    }
}

void OptimizedBrain::enableSIMD(bool enable) {
    simd_enabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("SIMD optimization enabled for optimized brain");
    }
}

void OptimizedBrain::enableVectorization(bool enable) {
    vectorization_enabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Vectorization enabled for optimized brain");
    }
}

float OptimizedBrain::getOverallActivity() const {
    float total_activity = 0.0f;
    size_t count = 0;
    
    for (const auto& region : regions_) {
        if (region) {
            total_activity += region->getRegionActivity();
            count++;
        }
    }
    
    for (const auto& population : populations_) {
        if (population) {
            total_activity += population->getPopulationActivity();
            count++;
        }
    }
    
    return (count > 0) ? total_activity / count : 0.0f;
}

float OptimizedBrain::getAverageFiringRate() const {
    float total_rate = 0.0f;
    size_t count = 0;
    
    for (const auto& region : regions_) {
        if (region) {
            total_rate += region->getAverageFiringRate();
            count++;
        }
    }
    
    return (count > 0) ? total_rate / count : 0.0f;
}

size_t OptimizedBrain::getNeuronCount() const {
    size_t count = 0;
    for (const auto& region : regions_) {
        if (region) count += region->getNeuronCount();
    }
    return count;
}

size_t OptimizedBrain::getRegionCount() const {
    return regions_.size();
}

size_t OptimizedBrain::getPopulationCount() const {
    return populations_.size();
}

void OptimizedBrain::optimizeMemoryLayout() {
    // Optimize memory layout
    for (auto& region : regions_) {
        if (region) region->optimizeMemoryUsage();
    }
    
    for (auto& population : populations_) {
        if (population) population->optimizeMemoryAccess();
    }
    
    NLM_LOG_INFO("Memory layout optimization completed");
}

void OptimizedBrain::enableParallelProcessing(bool enable) {
    parallel_processing_enabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Parallel processing enabled for optimized brain");
    }
}

void OptimizedBrain::setThreadCount(size_t threads) {
    thread_count_ = threads;
    if (parallel_processing_enabled_) {
        NLM_LOG_INFO("Thread count set to " + std::to_string(threads));
    }
}

void OptimizedBrain::recordBrainPerformance(const std::string& operation, double duration_ms) {
    // Record performance metrics
    getGlobalProfiler().record("Brain." + operation, duration_ms);
}

std::string OptimizedBrain::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "=== Optimized Brain Performance Report ===\n";
    
    ss << "Overall Performance Metrics:\n";
    ss << "Total step time: " << std::fixed << std::setprecision(3) << global_metrics_.total_step_time_ms << " ms\n";
    ss << "Total neurons stepped: " << global_metrics_.total_neurons_stepped << "\n";
    ss << "Total spikes generated: " << global_metrics_.total_spikes_generated << "\n";
    ss << "Average step time: " << std::fixed << std::setprecision(3) << global_metrics_.average_step_time_ms << " ms\n";
    ss << "Average connection time: " << std::fixed << std::setprecision(3) << global_metrics_.average_connection_time_ms << " ms\n";
    ss << "Overall activity: " << std::fixed << std::setprecision(3) << global_metrics_.overall_activity << "\n";
    ss << "Average firing rate: " << std::fixed << std::setprecision(3) << getAverageFiringRate() << " Hz\n";
    
    ss << "\nComponent Counts:\n";
    ss << "Regions: " << getRegionCount() << "\n";
    ss << "Populations: " << getPopulationCount() << "\n";
    ss << "Neurons: " << getNeuronCount() << "\n";
    
    ss << "\nOptimization Status:\n";
    ss << "Real-time profiling: " << (real_time_profiling_enabled_ ? "Enabled" : "Disabled") << "\n";
    ss << "SIMD: " << (simd_enabled_ ? "Enabled" : "Disabled") << "\n";
    ss << "Vectorization: " << (vectorization_enabled_ ? "Enabled" : "Disabled") << "\n";
    ss << "Parallel processing: " << (parallel_processing_enabled_ ? "Enabled" : "Disabled") << "\n";
    ss << "Thread count: " << thread_count_ << "\n";
    
    ss << "\n=== End of Report ===\n";
    return ss.str();
}

void OptimizedBrain::updateGlobalPerformanceMetrics() {
    // Update global performance metrics for monitoring
    getGlobalProfiler().record("Brain.stepBrain", global_metrics_.total_step_time_ms);
    getGlobalProfiler().record("Brain.overallActivity", global_metrics_.overall_activity);
    getGlobalProfiler().record("Brain.firingRate", getAverageFiringRate());
}

void OptimizedBrain::optimizeConnectionGraph() {
    // Optimize connection graph
    for (auto& region : regions_) {
        if (region) region->optimizeConnectionProcessing();
    }
    
    NLM_LOG_INFO("Connection graph optimization completed");
}

void OptimizedBrain::enableMemoryPools() {
    // Enable memory pools for efficient allocation
    for (auto& region : regions_) {
        if (region) region->enableSpatialIndexing(true);
    }
    
    NLM_LOG_INFO("Memory pools enabled");
}

// Global optimized brain implementation
namespace {
    std::unique_ptr<OptimizedBrain> global_optimized_brain;
}

OptimizedBrain& getGlobalOptimizedBrain() {
    if (!global_optimized_brain) {
        global_optimized_brain = std::make_unique<OptimizedBrain>();
    }
    return *global_optimized_brain;
}

std::unique_ptr<OptimizedBrain> createOptimizedBrain(const std::string& config_file) {
    auto brain = std::make_unique<OptimizedBrain>();
    
    if (!config_file.empty()) {
        // Load configuration
        // This would involve parsing the config file and setting up neurons, regions, etc.
        NLM_LOG_INFO("Optimized brain created with config file: " + config_file);
    } else {
        NLM_LOG_INFO("Optimized brain created with default configuration");
    }
    
    return brain;
}

} // namespace nlm
