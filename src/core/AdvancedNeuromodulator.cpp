#include "AdvancedNeuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AdvancedNeuromodulator::Impl {
    // Neuromodulator components
    struct SignalComponent {
        std::string name;
        std::function<double(double)> signalGenerator;
        double weight;
        double history[100]; // Simple history buffer
        int historyIndex;
        
        SignalComponent(const std::string& n, std::function<double(double)> gen, double w = 1.0)
            : name(n), signalGenerator(gen), weight(w), historyIndex(0) {
            std::fill(std::begin(history), std::end(history), 0.0);
        }
    };
    
    std::vector<std::unique_ptr<SignalComponent>> signalGenerators;
    std::vector<std::unique_ptr<CustomModulator>> customModulators;
    std::unique_ptr<NoveltyDetector> noveltyDetector;
    
    // Neuromodulation targets
    std::unordered_map<std::string, double> targets;
    std::map<std::string, double> targetWeights;
    
    // Learning and integration parameters
    double learningRate;
    double decayRate;
    std::string integrationMethod;
    
    // Prediction error system
    std::string predictionErrorSource;
    double predictionErrorValue;
    
    // Statistics
    double totalModulationApplied;
    double maxSignalValue;
    double minSignalValue;
    bool profilingEnabled;
    std::chrono::system_clock::time_point startTime;
    
    Logger* logger;
};

AdvancedNeuromodulator::AdvancedNeuromodulator() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
    pImpl->learningRate = 0.01;
    pImpl->decayRate = 0.99;
    pImpl->integrationMethod = "exponential_moving_average";
    pImpl->predictionErrorValue = 0.0;
    pImpl->totalModulationApplied = 0.0;
    pImpl->maxSignalValue = -std::numeric_limits<double>::max();
    pImpl->minSignalValue = std::numeric_limits<double>::max();
    pImpl->profilingEnabled = false;
    pImpl->startTime = std::chrono::system_clock::now();
}

AdvancedNeuromodulator::~AdvancedNeuromodulator() {
    shutdown();
}

AdvancedNeuromodulator::AdvancedNeuromodulator(AdvancedNeuromodulator&& other) noexcept : pImpl(std::move(other.pImpl)) {}

AdvancedNeuromodulator& AdvancedNeuromodulator::operator=(AdvancedNeuromodulator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool AdvancedNeuromodulator::initialize(const Config& config) {
    pImpl->logger->info("Advanced neuromodulator initialized");
    return true;
}

void AdvancedNeuromodulator::update(double timestep) {
    // Update signal generators and custom modulators
    for (auto& generator : pImpl->signalGenerators) {
        double signal = generator->signalGenerator(timestep);
        
        // Apply decay if using exponential moving average
        if (pImpl->integrationMethod == "exponential_moving_average") {
            generator->history[generator->historyIndex++] = signal;
            if (generator->historyIndex >= 100) generator->historyIndex = 0;
        }
        
        // Update statistics
        if (signal > pImpl->maxSignalValue) pImpl->maxSignalValue = signal;
        if (signal < pImpl->minSignalValue) pImpl->minSignalValue = signal;
    }
    
    // Update custom modulators
    for (auto& modulator : pImpl->customModulators) {
        NeuromodulatorContext context;
        context.timestep = timestep;
        context.predictionError = pImpl->predictionErrorValue;
        context.noveltyScore = getNoveltyScore();
        
        double signal = modulator->computeSignal(timestep, context);
        pImpl->totalModulationApplied += std::abs(signal);
    }
    
    // Update novelty detector
    if (pImpl->noveltyDetector) {
        pImpl->noveltyDetector->update(timestep);
    }
}

void AdvancedNeuromodulator::shutdown() {
    pImpl->logger->info("Advanced neuromodulator shutdown");
    pImpl->signalGenerators.clear();
    pImpl->customModulators.clear();
    pImpl->noveltyDetector.reset();
}

void AdvancedNeuromodulator::applyToNeuron(Neuron& neuron, double timestep) {
    NeuromodulatorContext context;
    context.timestep = timestep;
    context.neuronId = neuron.getId();
    context.neuronState = neuron.getState();
    context.predictionError = pImpl->predictionErrorValue;
    context.noveltyScore = getNoveltyScore();
    
    // Apply custom modulators
    double totalModulation = 0.0;
    for (auto& modulator : pImpl->customModulators) {
        double signal = modulator->computeSignal(timestep, context);
        totalModulation += signal * (pImpl->targetWeights[modulator->getName()] + 1.0);
    }
    
    // Apply basic neuromodulation if neuron is firing
    if (neuron.isFiring()) {
        double firingRate = neuron.getFiringRate();
        totalModulation += firingRate * pImpl->learningRate;
    }
    
    // Integrate the modulation into neuron state
    if (pImpl->integrationMethod == "exponential_moving_average") {
        double currentPotential = neuron.getMembranePotential();
        double newPotential = currentPotential + totalModulation * pImpl->learningRate;
        neuron.setMembranePotential(newPotential);
    } else {
        // Simple additive integration
        neuron.addToMembranePotential(totalModulation);
    }
}

double AdvancedNeuromodulator::getSignal(const std::string& target) const {
    auto it = pImpl->targets.find(target);
    if (it != pImpl->targets.end()) {
        return it->second;
    }
    return 0.0;
}

void AdvancedNeuromodulator::addModulationTarget(const std::string& target, double weight) {
    pImpl->targets[target] = 0.0;
    pImpl->targetWeights[target] = weight;
}

void AdvancedNeuromodulator::addCustomModulator(std::unique_ptr<CustomModulator> modulator) {
    if (modulator) {
        pImpl->customModulators.push_back(std::move(modulator));
    }
}

class CustomModulator* AdvancedNeuromodulator::getModulator(const std::string& name) const {
    for (auto& modulator : pImpl->customModulators) {
        if (modulator->getName() == name) {
            return modulator.get();
        }
    }
    return nullptr;
}

void AdvancedNeuromodulator::setLearningRate(double rate) {
    pImpl->learningRate = rate;
}

void AdvancedNeuromodulator::setDecayRate(double rate) {
    pImpl->decayRate = rate;
}

void AdvancedNeuromodulator::setIntegrationMethod(const std::string& method) {
    pImpl->integrationMethod = method;
}

void AdvancedNeuromodulator::setPredictionErrorSource(const std::string& source) {
    pImpl->predictionErrorSource = source;
}

double AdvancedNeuromodulator::getPredictionError() const {
    return pImpl->predictionErrorValue;
}

void AdvancedNeuromodulator::setPredictionError(double error) {
    pImpl->predictionErrorValue = error;
}

void AdvancedNeuromodulator::setNoveltyDetector(std::unique_ptr<NoveltyDetector> detector) {
    pImpl->noveltyDetector = std::move(detector);
}

class NoveltyDetector* AdvancedNeuromodulator::getNoveltyDetector() const {
    return pImpl->noveltyDetector.get();
}

double AdvancedNeuromodulator::getNoveltyScore() const {
    if (pImpl->noveltyDetector) {
        return pImpl->noveltyDetector->computeNovelty(std::vector<float>()); // Would need actual features
    }
    return 0.0;
}

void AdvancedNeuromodulator::addCustomSignalGenerator(const std::string& name, 
                                                      std::function<double(double)> generator) {
    auto signalGen = std::make_unique<Impl::SignalComponent>(name, generator);
    pImpl->signalGenerators.push_back(std::move(signalGen));
}

double AdvancedNeuromodulator::generateCustomSignal(const std::string& name, double timestep) const {
    for (const auto& generator : pImpl->signalGenerators) {
        if (generator->name == name) {
            return generator->signalGenerator(timestep);
        }
    }
    return 0.0;
}

std::string AdvancedNeuromodulator::getNeuromodulationStats() const {
    std::stringstream ss;
    ss << "Neuromodulation Statistics:\n";
    ss << "=========================\n";
    ss << "Custom modulators: " << pImpl->customModulators.size() << "\n";
    ss << "Signal generators: " << pImpl->signalGenerators.size() << "\n";
    ss << "Modulation targets: " << pImpl->targets.size() << "\n";
    ss << "Total modulation applied: " << pImpl->totalModulationApplied << "\n";
    ss << "Max signal value: " << pImpl->maxSignalValue << "\n";
    ss << "Min signal value: " << pImpl->minSignalValue << "\n";
    ss << "Learning rate: " << pImpl->learningRate << "\n";
    ss << "Decay rate: " << pImpl->decayRate << "\n";
    
    if (pImpl->profilingEnabled) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
        ss << "Runtime: " << duration.count() << " seconds\n";
    }
    
    return ss.str();
}

void AdvancedNeuromodulator::enableProfiling(bool enable) {
    pImpl->profilingEnabled = enable;
}

bool AdvancedNeuromodulator::isProfilingEnabled() const {
    return pImpl->profilingEnabled;
}

} // namespace nlm
