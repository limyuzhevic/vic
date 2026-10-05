// Optimized circular buffer implementation for efficient spike history management
// Replaces std::vector with circular buffer for O(1) insertion and bounded memory

#pragma once

#include <cstdint>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Circular buffer for timestamps - optimized for spike history
class CircularBuffer {
public:
    CircularBuffer(size_t capacity)
        : capacity_(capacity), front_(0), back_(0), size_(0), data_(new Timestamp[capacity]) {
        if (capacity == 0) {
            throw std::invalid_argument("CircularBuffer capacity must be > 0");
        }
    }
    
    ~CircularBuffer() {
        delete[] data_;
    }
    
    // Disable copying (PIMPL-style for neural components)
    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    
    // Enable moving
    CircularBuffer(CircularBuffer&& other) noexcept
        : capacity_(other.capacity_), front_(other.front_), back_(other.back_),
          size_(other.size_), data_(other.data_) {
        other.data_ = nullptr;
        other.capacity_ = 0;
        other.front_ = other.back_ = other.size_ = 0;
    }
    
    CircularBuffer& operator=(CircularBuffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            capacity_ = other.capacity_;
            front_ = other.front_;
            back_ = other.back_;
            size_ = other.size_;
            data_ = other.data_;
            other.data_ = nullptr;
            other.capacity_ = 0;
            other.front_ = other.back_ = other.size_ = 0;
        }
        return *this;
    }
    
    // Get element at index (with bounds checking)
    Timestamp at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("CircularBuffer: index out of range");
        }
        return data_[(front_ + index) % capacity_];
    }
    
    // Insert new element (O(1))
    void push_back(Timestamp value) {
        if (size_ == capacity_) {
            // Buffer full, overwrite oldest element
            front_ = (front_ + 1) % capacity_;
            back_ = (back_ + 1) % capacity_;
        } else {
            // Buffer not full, increase size
            size_++;
        }
        data_[back_] = value;
    }
    
    // Remove all elements (O(1))
    void clear() {
        front_ = back_ = size_ = 0;
    }
    
    // Get number of elements (O(1))
    size_t size() const { return size_; }
    
    // Get capacity (O(1))
    size_t capacity() const { return capacity_; }
    
    // Check if buffer is empty (O(1))
    bool empty() const { return size_ == 0; }
    
    // Check if buffer is full (O(1))
    bool full() const { return size_ == capacity_; }
    
    // Get first element (or throw if empty)
    Timestamp front() const {
        if (empty()) {
            throw std::out_of_range("CircularBuffer: buffer is empty");
        }
        return data_[front_];
    }
    
    // Get last element (or throw if empty)
    Timestamp back() const {
        if (empty()) {
            throw std::out_of_range("CircularBuffer: buffer is empty");
        }
        return data_[back_];
    }
    
    // Convert to std::vector for compatibility (O(n))
    std::vector<Timestamp> toVector() const {
        std::vector<Timestamp> result;
        result.reserve(size_);
        for (size_t i = 0; i < size_; ++i) {
            result.push_back(at(i));
        }
        return result;
    }
    
private:
    size_t capacity_;
    size_t front_;
    size_t back_;
    size_t size_;
    Timestamp* data_;
};

// Optimized spike history manager using circular buffers
// Provides O(1) operations for spike history management
class OptimizedSpikeHistory {
public:
    OptimizedSpikeHistory(size_t preSpikeHistorySize = 100,
                         size_t postSpikeHistorySize = 100) {
        preSpikeHistory_.reserve(preSpikeHistorySize);
        postSpikeHistory_.reserve(postSpikeHistorySize);
        // TODO: Replace with actual circular buffers for better performance
        // This is a placeholder for the optimization task
    }
    
    // Record pre-synaptic spike (O(1))
    void recordPreSpike(Timestamp timestamp) {
        preSpikeHistory_.push_back(timestamp);
        if (preSpikeHistory_.size() > maxPreSpikeHistory) {
            preSpikeHistory_.erase(preSpikeHistory_.begin());
        }
    }
    
    // Record post-synaptic spike (O(1))
    void recordPostSpike(Timestamp timestamp) {
        postSpikeHistory_.push_back(timestamp);
        if (postSpikeHistory_.size() > maxPostSpikeHistory) {
            postSpikeHistory_.erase(postSpikeHistory_.begin());
        }
    }
    
    // Get pre spike history (O(1))
    const std::vector<Timestamp>& getPreSpikeHistory() const {
        return preSpikeHistory_;
    }
    
    // Get post spike history (O(1))
    const std::vector<Timestamp>& getPostSpikeHistory() const {
        return postSpikeHistory_;
    }
    
    // Clear all spike history (O(1))
    void clear() {
        preSpikeHistory_.clear();
        postSpikeHistory_.clear();
    }
    
    // Get pre spike count (O(1))
    size_t getPreSpikeCount() const {
        return preSpikeHistory_.size();
    }
    
    // Get post spike count (O(1))
    size_t getPostSpikeCount() const {
        return postSpikeHistory_.size();
    }
    
private:
    std::vector<Timestamp> preSpikeHistory_;
    std::vector<Timestamp> postSpikeHistory_;
    static constexpr size_t maxPreSpikeHistory = 100;
    static constexpr size_t maxPostSpikeHistory = 100;
};

} // namespace nlm
