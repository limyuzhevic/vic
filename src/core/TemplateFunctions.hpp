// TemplateFunctions.hpp
#pragma once

#include <utility>
#include <type_traits>

namespace nlm {

// Template functions for repetitive operations
namespace Template {

// Factory function template for creating objects with common initialization

template<typename T, typename... Args>
std::unique_ptr<T> makeSystem(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// Configure function template
template<typename T, typename... Args>
void configureSystem(T* system, Args&&... args) {
    if (system) {
        system->configure(std::forward<Args>(args)...);
    }
}

// Batch operation template
template<typename Container, typename Operation>
void batchOperation(Container& container, Operation&& op) {
    for (auto& item : container) {
        op(item);
    }
}

// Conditional apply template
template<typename T, typename Operation>
void conditionalApply(T* system, bool condition, Operation&& op) {
    if (system && condition) {
        op(system);
    }
}

// Statistical calculation templates
template<typename Iterator>
auto calculateAverage(Iterator begin, Iterator end) {
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    value_type sum = 0;
    size_t count = 0;
    
    for (auto it = begin; it != end; ++it) {
        sum += *it;
        ++count;
    }
    
    return count > 0 ? static_cast<double>(sum) / count : 0.0;
}

template<typename Iterator, typename Predicate>
auto calculateSum(Iterator begin, Iterator end, Predicate pred) {
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    value_type sum = 0;
    
    for (auto it = begin; it != end; ++it) {
        if (pred(*it)) {
            sum += *it;
        }
    }
    
    return sum;
}

// Memory operation templates
template<typename MemorySystem>
void storeAll(MemorySystem* memory, const std::vector<NeuronId>& neurons,
             const std::vector<float>& activations) {
    if (!memory || neurons.size() != activations.size()) return;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        memory->storeToNeuron(neurons[i], activations[i]);
    }
}

template<typename MemorySystem>
std::vector<float> retrieveAll(MemorySystem* memory,
                               const std::vector<NeuronId>& neurons) {
    std::vector<float> results;
    if (!memory) return results;
    
    results.reserve(neurons.size());
    for (const auto& neuron : neurons) {
        results.push_back(memory->getNeuronActivation(neuron));
    }
    
    return results;
}

// Region operation templates
template<typename Operation>
void applyToAllRegions(const Brain* brain, Operation&& op) {
    if (!brain) return;
    
    for (const auto& region : brain->getRegions()) {
        op(region.get());
    }
}

template<typename Condition, typename Operation>
void conditionalApplyToRegions(const Brain* brain, Condition&& condition, Operation&& op) {
    if (!brain) return;
    
    for (const auto& region : brain->getRegions()) {
        if (condition(region.get())) {
            op(region.get());
        }
    }
}

// Neuron operation templates
template<typename Operation>
void applyToNeurons(const std::vector<Neuron*>& neurons, Operation&& op) {
    for (auto* neuron : neurons) {
        if (neuron) op(neuron);
    }
}

template<typename Condition, typename Operation>
void conditionalApplyToNeurons(const std::vector<Neuron*>& neurons,
                                Condition&& condition, Operation&& op) {
    for (auto* neuron : neurons) {
        if (neuron && condition(neuron)) op(neuron);
    }
}

// Time-based operation templates
struct TimeRange {
    Timestamp start;
    Timestamp end;
    bool operator<(const TimeRange& other) const { return end < other.start; }
};

template<typename Event>
void processTimeWindow(const std::vector<Event>& events, TimeRange window,
                      const std::function<void(const Event&)>& processor) {
    for (const auto& event : events) {
        if (event.time >= window.start && event.time <= window.end) {
            processor(event);
        }
    }
}

} // namespace Template

} // namespace nlm