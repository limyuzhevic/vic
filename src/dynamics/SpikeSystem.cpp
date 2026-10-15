#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>

namespace nlm {

// Optimized memory pool for SpikeEvent objects to reduce allocation overhead
class SpikeEventPool {
public:
    static constexpr size_t POOL_SIZE = 1024;
    
    struct Node {
        SpikeEvent event;
        std::unique_ptr<Node> next;
        
        Node(const SpikeEvent& e) : event(e) {}
    };
    
    SpikeEventPool() {
        // Pre-allocate initial nodes
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            nodes.push_back(std::make_unique<Node>(SpikeEvent()));
        }
        head = nodes[0].get();
    }
    
    SpikeEvent* allocate(const SpikeEvent& event) {
        if (!head || !freeList) {
            // Need to expand pool
            expandPool();
        }
        
        Node* node = freeList;
        freeList = freeList->next;
        node->event = event;
        node->next = nullptr;
        
        // Insert at head for LIFO (better for cache locality)
        node->next = head;
        head = node;
        
        return &node->event;
    }
    
    void deallocate(Node* node) {
        if (!node) return;
        
        // Remove from linked list
        if (node == head) {
            head = node->next;
        } else {
            Node* prev = head;
            while (prev && prev->next != node) {
                prev = prev->next.get();
            }
            if (prev) {
                prev->next = node->next;
            }
        }
        
        // Add to free list
        node->next = freeList;
        freeList = node;
    }
    
    ~SpikeEventPool() {
        // Clean up linked list
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    
private:
    void expandPool() {
        // Allocate new batch of nodes
        size_t oldSize = nodes.size();
        nodes.reserve(oldSize + POOL_SIZE);
        
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            nodes.emplace_back(std::make_unique<Node>(SpikeEvent()));
        }
        
        // Link new nodes together in free list
        Node* lastFree = freeList;
        for (size_t i = oldSize; i < nodes.size(); ++i) {
            if (i == oldSize) {
                // First new node links to current free list
                nodes[i]->next = freeList;
                freeList = nodes[i].get();
            } else {
                nodes[i]->next = freeList;
                freeList = nodes[i].get();
            }
        }
    }
    
    std::vector<std::unique_ptr<Node>> nodes;
    Node* head = nullptr;
    Node* freeList = nullptr;
};

// Optimized DelayedSpikeEvent pool
class DelayedSpikeEventPool {
public:
    static constexpr size_t POOL_SIZE = 512;
    
    struct Node {
        DelayedSpikeEvent event;
        std::unique_ptr<Node> next;
        
        Node(const DelayedSpikeEvent& e) : event(e) {}
    };
    
    DelayedSpikeEventPool() {
        // Pre-allocate initial nodes
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            nodes.push_back(std::make_unique<Node>(DelayedSpikeEvent()));
        }
        freeHead = nodes[0].get();
    }
    
    DelayedSpikeEvent* allocate(const DelayedSpikeEvent& event) {
        if (!freeHead) {
            expandPool();
        }
        
        Node* node = freeHead;
        freeHead = freeHead->next;
        node->event = event;
        node->next = nullptr;
        
        return &node->event;
    }
    
    void deallocate(Node* node) {
        if (!node) return;
        node->next = freeHead;
        freeHead = node;
    }
    
    ~DelayedSpikeEventPool() {
        Node* current = freeHead;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    
private:
    void expandPool() {
        size_t oldSize = nodes.size();
        nodes.reserve(oldSize + POOL_SIZE);
        
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            nodes.emplace_back(std::make_unique<Node>(DelayedSpikeEvent()));
        }
        
        // Link new nodes together
        Node* lastFree = freeHead;
        for (size_t i = oldSize; i < nodes.size(); ++i) {
            if (i == oldSize) {
                nodes[i]->next = freeHead;
                freeHead = nodes[i].get();
            } else {
                nodes[i]->next = freeHead;
                freeHead = nodes[i].get();
            }
        }
    }
    
    std::vector<std::unique_ptr<Node>> nodes;
    Node* freeHead = nullptr;
};

struct SpikeSystem::Impl {
    // Optimized spike queue using memory pool
    struct SpikeQueue {
        struct Node {
            SpikeEvent event;
            std::unique_ptr<Node> next;
            Node(const SpikeEvent& e) : event(e) {}
        };
        
        Node* head = nullptr;
        Node* tail = nullptr;
        size_t size = 0;
        
        void push(const SpikeEvent& event) {
            Node* node = new Node(event);
            if (!head) {
                head = tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
            size++;
        }
        
        SpikeEvent pop() {
            SpikeEvent event = head->event;
            Node* oldHead = head;
            head = head->next;
            delete oldHead;
            size--;
            if (!head) tail = nullptr;
            return event;
        }
        
        bool empty() const { return size == 0; }
        size_t count() const { return size; }
        
        ~SpikeQueue() {
            Node* current = head;
            while (current) {
                Node* next = current->next;
                delete current;
                current = next;
            }
        }
    };
    
    SpikeQueue pendingSpikes;
    
    // Optimized history with pre-allocated capacity
    std::vector<DetailedSpikeEvent> spikeHistory;
    size_t maxHistorySize;
    
    // Optimized handlers using small vectors
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Grouped delayed spikes - optimized for cache locality
    struct DelayedSpikeGroup {
        std::vector<DelayedSpikeEvent> events;
        SimulationStep deliveryStep;
        
        DelayedSpikeGroup(SimulationStep step) : deliveryStep(step) {}
    };
    
    std::vector<DelayedSpikeGroup> delayedSpikesByStep;
    std::unordered_map<SimulationStep, size_t> stepToIndex;  // O(1) lookup
    
    // Optimized spike counting
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;
    
    // Memory pools for efficient allocation/deallocation
    SpikeEventPool eventPool;
    DelayedSpikeEventPool delayedEventPool;
    
    // Cache-friendly statistics
    size_t totalSpikesProcessed;
    Timestamp lastSpikeTime;
    
    Impl() : maxHistorySize(10000), totalSpikesProcessed(0), lastSpikeTime(0.0) {
        spikeHistory.reserve(maxHistorySize);
        delayedSpikesByStep.reserve(100);  // Expect up to 100 unique delivery steps
        stepToIndex.reserve(100);
    }
};

SpikeSystem::SpikeSystem() : pImpl(new Impl) {}

SpikeSystem::~SpikeSystem() = default;

SpikeSystem::SpikeSystem(SpikeSystem&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

SpikeSystem& SpikeSystem::operator=(SpikeSystem&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    pImpl->pendingSpikes.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    SpikeEvent event;
    event.source_neuron = neuron;
    event.timestamp = timestamp;
    event.step = step;
    pImpl->pendingSpikes.push(event);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // O(1) lookup for existing step group
    auto it = pImpl->stepToIndex.find(event.delivery_step);
    
    if (it != pImpl->stepToIndex.end()) {
        // Use existing group
        size_t index = it->second;
        pImpl->delayedSpikesByStep[index].events.push_back(event);
    } else {
        // Create new group
        pImpl->delayedSpikesByStep.emplace_back(event.delivery_step);
        size_t index = pImpl->delayedSpikesByStep.size() - 1;
        pImpl->delayedSpikesByStep[index].events.push_back(event);
        pImpl->stepToIndex[event.delivery_step] = index;
    }
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Process all pending immediate spikes efficiently
    while (!pImpl->pendingSpikes.empty()) {
        SpikeEvent event = pImpl->pendingSpikes.pop();
        
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        // Update statistics
        pImpl->totalSpikesProcessed++;
        pImpl->lastSpikeTime = std::max(pImpl->lastSpikeTime, event.timestamp);
        
        // Trim history if needed (using vector::shrink_to_fit for memory efficiency)
        if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
            size_t eraseCount = pImpl->spikeHistory.size() - pImpl->maxHistorySize;
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                                   pImpl->spikeHistory.begin() + eraseCount);
        }
        
        // Call handlers
        for (auto& handler : pImpl->handlers) {
            handler(detailed);
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Find and process all delayed spikes scheduled for this step
    auto it = pImpl->stepToIndex.find(currentStep);
    if (it != pImpl->stepToIndex.end()) {
        size_t index = it->second;
        
        // Process all spikes in the group
        DelayedSpikeGroup& group = pImpl->delayedSpikesByStep[index];
        
        for (const auto& delayedEvent : group.events) {
            // Call delayed spike handlers
            for (auto& handler : pImpl->delayedHandlers) {
                handler(delayedEvent);
            }
        }
        
        // Clear the group (instead of erasing to preserve O(1) lookup)
        group.events.clear();
        
        // Remove from index map
        pImpl->stepToIndex.erase(it);
        
        // Mark group for reuse (we could recycle it instead of creating new ones)
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    pImpl->handlers.push_back(handler);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    pImpl->delayedHandlers.push_back(handler);
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->totalSpikesProcessed = 0;
    pImpl->lastSpikeTime = 0.0;
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    return pImpl->pendingSpikes.count();
}

size_t SpikeSystem::getPendingDelayedCount() const {
    size_t count = 0;
    for (const auto& group : pImpl->delayedSpikesByStep) {
        count += group.events.size();
    }
    return count;
}

float SpikeSystem::getAverageSpikeRate() const {
    if (pImpl->spikeHistory.size() < 2 || pImpl->lastSpikeTime <= 0.0f) {
        return 0.0f;
    }
    
    return static_cast<float>(pImpl->spikeHistory.size()) / pImpl->lastSpikeTime;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    for (const auto& pair : pImpl->spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    // Sort by count descending
    std::sort(neuronCounts.begin(), neuronCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.emplace_back(neuronCounts[i].first);
    }
    
    return result;
}

void SpikeSystem::reset() {
    pImpl->pendingSpikes.head = pImpl->pendingSpikes.tail = nullptr;
    pImpl->pendingSpikes.size = 0;
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->totalSpikesProcessed = 0;
    pImpl->lastSpikeTime = 0.0;
    
    // Clear delayed spikes
    for (auto& group : pImpl->delayedSpikesByStep) {
        group.events.clear();
    }
    pImpl->delayedSpikesByStep.clear();
    pImpl->stepToIndex.clear();
}

// Enhanced optimized functions for better performance
void SpikeSystem::processSpikesOptimized(SimulationStep currentStep) {
    // Optimized version for scenarios with many spikes
    size_t batchSize = 1000;
    std::vector<DetailedSpikeEvent> batch;
    
    batch.reserve(batchSize);
    
    // Process spikes in batches for better cache locality
    while (!pImpl->pendingSpikes.empty() && batch.size() < batchSize) {
        SpikeEvent event = pImpl->pendingSpikes.pop();
        
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        batch.push_back(detailed);
        
        // Update statistics
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        pImpl->totalSpikesProcessed++;
        pImpl->lastSpikeTime = std::max(pImpl->lastSpikeTime, event.timestamp);
    }
    
    // Add batch to history
    pImpl->spikeHistory.insert(pImpl->spikeHistory.end(), batch.begin(), batch.end());
    
    // Trim history if needed
    if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
        size_t eraseCount = pImpl->spikeHistory.size() - pImpl->maxHistorySize;
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                               pImpl->spikeHistory.begin() + eraseCount);
    }
    
    // Process handlers
    for (const auto& detailed : batch) {
        for (auto& handler : pImpl->handlers) {
            handler(detailed);
        }
    }
}

// Memory efficiency function - clears unused pools
void SpikeSystem::optimizeMemory() {
    // Clear spike count map if it has grown too large
    if (pImpl->spikeCountPerNeuron.size() > pImpl->spikeHistory.size() * 2) {
        pImpl->spikeCountPerNeuron.clear();
    }
    
    // Compact history vector if it has many gaps
    if (pImpl->spikeHistory.size() > pImpl->maxHistorySize * 0.8) {
        size_t compactSize = std::min(pImpl->spikeHistory.size(), pImpl->maxHistorySize / 2);
        pImpl->spikeHistory.resize(compactSize);
    }
    
    // Clean up old delayed spike groups
    pImpl->delayedSpikesByStep.erase(
        std::remove_if(pImpl->delayedSpikesByStep.begin(), pImpl->delayedSpikesByStep.end(),
                      [](const DelayedSpikeGroup& group) { return group.events.empty(); }),
        pImpl->delayedSpikesByStep.end()
    );
    
    // Rebuild index map
    pImpl->stepToIndex.clear();
    for (size_t i = 0; i < pImpl->delayedSpikesByStep.size(); ++i) {
        pImpl->stepToIndex[pImpl->delayedSpikesByStep[i].deliveryStep] = i;
    }
}

} // namespace nlm
