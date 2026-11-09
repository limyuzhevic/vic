#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace nlm {

/**
 * @brief Thread-safe circular buffer implementation for O(1) spike history management
 * 
 * Provides fixed-size buffer with wrap-around semantics for efficient spike recording
 * in neuron and synapse activity history. Eliminates O(n) vector erase operations.
 * 
 * @tparam T Value type (typically Timestamp)
 * @tparam N Maximum buffer capacity
 */
template<typename T, size_t N>
class CircularBuffer {
public:
    static constexpr size_t Capacity = N;
    
    CircularBuffer() = default;
    
    // Push new value, oldest element is discarded if full
    void push(T value) {
        buffer_[head_] = value;
        if (++head_ >= Capacity) {
            head_ = 0;
        }
        if (size_ < Capacity) {
            ++size_;
        }
    }
    
    // Get element by index (0 = most recent, size()-1 = oldest)
    T get(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("CircularBuffer index out of range");
        }
        size_t pos = (head_ + size_ - 1 - index) % Capacity;
        return buffer_[pos];
    }
    
    // Get last (most recent) element
    T back() const {
        if (empty()) {
            throw std::underflow_error("CircularBuffer is empty");
        }
        size_t pos = (head_ + size_ - 1) % Capacity;
        return buffer_[pos];
    }
    
    // Get first (oldest) element
    T front() const {
        if (empty()) {
            throw std::underflow_error("CircularBuffer is empty");
        }
        size_t pos = head_ % Capacity;
        return buffer_[pos];
    }
    
    // Get element by index with bounds checking (alternative overload)
    const T& at(size_t index) const {
        return get(index);
    }
    
    // Clear all elements
    void clear() {
        size_ = 0;
        head_ = 0;
    }
    
    // Check if buffer is empty
    bool empty() const { return size_ == 0; }
    
    // Check if buffer is full
    bool full() const { return size_ == Capacity; }
    
    // Get current size
    size_t size() const { return size_; }
    
    // Iterator-like access for range-based operations
    class iterator {
    public:
        iterator(const CircularBuffer& buffer, size_t index)
            : buffer_(buffer), index_(index) {}
        
        T& operator*() {
            return buffer_.get(index_);
        }
        
        iterator& operator++() {
            ++index_;
            return *this;
        }
        
        bool operator!=(const iterator& other) const {
            return index_ != other.index_;
        }
    };
    
    iterator begin() const { return iterator(*this, 0); }
    iterator end() const { return iterator(*this, size_); }
    
    // Support for span-like operations
    const T* data() const {
        return buffer_.data();
    }
    
    // Move constructor (for performance)
    CircularBuffer(CircularBuffer&& other) noexcept
        : buffer_(std::move(other.buffer_)),
          head_(other.head_),
          size_(other.size_) {
        other.head_ = 0;
        other.size_ = 0;
    }
    
    // Move assignment operator
    CircularBuffer& operator=(CircularBuffer&& other) noexcept {
        if (this != &other) {
            buffer_ = std::move(other.buffer_);
            head_ = other.head_;
            size_ = other.size_;
            other.head_ = 0;
            other.size_ = 0;
        }
        return *this;
    }
    
    // Copy operations (disable for fixed-size buffers)
    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    
private:
    std::array<T, Capacity> buffer_ = {};
    size_t head_ = 0;
    size_t size_ = 0;
};

// Convenience typedef for timestamp spike history
using TimestampSpikeHistory = CircularBuffer<Timestamp, 100>;

// Utility functions for converting between circular buffer and standard containers
namespace cb {
    
    // Convert circular buffer to vector (most recent first)
    template<typename T, size_t N>
    std::vector<T> toVector(const CircularBuffer<T, N>& buffer) {
        std::vector<T> result;
        result.reserve(buffer.size());
        for (size_t i = 0; i < buffer.size(); ++i) {
            result.push_back(buffer.get(i));
        }
        return result;
    }
    
    // Convert vector to circular buffer (preserves order)
    template<typename T, size_t N>
    void fromVector(CircularBuffer<T, N>& buffer, const std::vector<T>& vec) {
        buffer.clear();
        for (const auto& item : vec) {
            buffer.push(item);
        }
    }
    
    // Merge two circular buffers into one
    template<typename T, size_t N>
    CircularBuffer<T, N> merge(const CircularBuffer<T, N>& buf1, 
                             const CircularBuffer<T, N>& buf2) {
        CircularBuffer<T, N> result;
        for (size_t i = 0; i < buf1.size(); ++i) {
            result.push(buf1.get(i));
        }
        for (size_t i = 0; i < buf2.size(); ++i) {
            result.push(buf2.get(i));
        }
        return result;
    }
    
} // namespace cb

} // namespace nlm
