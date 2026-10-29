// SpikeRingBuffer - Lock-free SPSC ring buffer for immediate spikes
// 
// Provides a single-producer, single-consumer ring buffer for fast spike event delivery.
// Optimized for high-frequency spike events with minimal overhead.
// 
// Features:
// - Lock-free operation for maximum performance
// - Pre-allocated circular buffer
// - Fast enqueue/dequeue operations
// - No memory allocation during simulation
// - Cache-friendly data layout
// 
// Biological motivation:
// - Spikes occur at high frequency (hundreds of Hz per neuron)
// - Requires extremely fast processing to match biological timing
// - Memory-efficient storage like biological neural transmission

#pragma once

#include "../core/Types/Types.hpp"
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <vector>

namespace nlm {

/**
 * SpikeRingBuffer - Lock-free SPSC ring buffer for spike events
 * 
 * Optimized for high-frequency spike delivery with minimal overhead.
 * Uses power-of-two sizing for fast modulo operations.
 */
class SpikeRingBuffer {
public:
    /**
     * Create a ring buffer
     * @param capacity Power of two capacity (e.g., 1024, 2048, 4096)
     */
    explicit SpikeRingBuffer(size_t capacity = 2048);
    
    ~SpikeRingBuffer();
    
    // Disable copying
    SpikeRingBuffer(const SpikeRingBuffer&) = delete;
    SpikeRingBuffer& operator=(const SpikeRingBuffer&) = delete;
    
    /**
     * Enqueue a spike event
     * @param spikeEvent Spike event to add
     * @return true if successful, false if buffer full
     */
    bool enqueue(const SpikeEvent& spikeEvent);
    
    /**
     * Dequeue a spike event
     * @param spikeEvent Output parameter for spike event
     * @return true if successful, false if buffer empty
     */
    bool dequeue(SpikeEvent& spikeEvent);
    
    /**
     * Check if buffer is full
     */
    bool full() const { return size_.load(std::memory_order_relaxed) == capacity_; }
    
    /**
     * Check if buffer is empty
     */
    bool empty() const { return size_.load(std::memory_order_relaxed) == 0; }
    
    /**
     * Get current buffer size
     */
    size_t size() const { return size_.load(std::memory_order_relaxed); }
    
    /**
     * Clear all events (buffer not underflow safe after clear)
     */
    void clear();
    
    /**
     * Get maximum capacity
     */
    size_t capacity() const { return capacity_; }
    
private:
    // Ring buffer implementation
    struct RingBuffer {
        SpikeEvent* buffer;
        size_t head;  // Consumer index
        size_t tail;  // Producer index
        
        RingBuffer(size_t cap);
        ~RingBuffer();
    };
    
    std::unique_ptr<RingBuffer> ringBuffer_;
    std::atomic<size_t> size_;
    
    // Helper functions
    size_t increment(size_t index) const { return (index + 1) & (capacity_ - 1); }
};

} // namespace nlm
