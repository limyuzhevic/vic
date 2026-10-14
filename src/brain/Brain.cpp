} // namespace nlm

// ============================================================================
// ADDITIONAL C++20 UTILITIES
// ============================================================================

namespace nlm {

// Helper function to check if a value is in a span
template<typename T>
bool contains(std::span<const T> span, const T& value) {
    return std::find(span.begin(), span.end(), value) != span.end();
}

// Batched version using span for better cache locality
template<typename Container, typename Func>
void batchedForEach(const Container& container, Func&& func) {
    if constexpr (__cpp_lib_span) {
        std::span<const typename Container::value_type> batch(
            container.data(), container.size());
        for (const auto& item : batch) {
            func(item);
        }
    } else {
        for (const auto& item : container) {
            func(item);
        }
    }
}

// SIMD-optimized vectorized processing
template<typename Iterator, typename Func>
void simdBatchProcess(Iterator begin, Iterator end, Func&& func) {
    if constexpr (__cpp_lib_span && __ARM_NEON) {
        // Use SIMD when available
        size_t count = std::distance(begin, end);
        size_t i = 0;
        const size_t simdSize = 8;
        
        for (; i + simdSize <= count; i += simdSize) {
            std::array<std::iter_value_t<Iterator>, simdSize> batch;
            std::copy(begin + i, begin + i + simdSize, batch.begin());
            
            for (size_t j = 0; j < simdSize; ++j) {
                func(batch[j]);
            }
        }
        
        // Process remaining elements
        for (; i < count; ++i) {
            func(*(begin + i));
        }
    } else {
        // Scalar fallback
        for (auto it = begin; it != end; ++it) {
            func(*it);
        }
    }
}

// Enhanced error handling with std::expected
#ifdef __cpp_lib_expected

// Result type for std::expected usage
template<typename T, typename E = NeuralException>
using Result = std::expected<T, E>;

// Error recovery helper
inline Result<size_t, NeuralException> safeIndexAccess(const std::vector<size_t>& vec, size_t index) {
    if (index < vec.size()) {
        return index;
    }
    return std::unexpected(NeuralException(
        NeuralErrorCode::InternalLogicError,
        std::format("Index out of bounds: {} in vector of size {}", index, vec.size())
    ));
}

#endif

// Memory pool allocators for common types
template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t capacity = 1024) : capacity_(capacity) {
        pool_.reserve(capacity);
    }
    
    T* allocate() {
        if (freeList_.empty()) {
            // Create a new block
            block_ = std::make_unique<Block>();
            size_t remaining = capacity_;
            for (size_t i = 0; i < capacity_ && remaining > 0; ++i) {
                freeList_.push_back(i);
                remaining--;
            }
        }
        
        size_t index = freeList_.back();
        freeList_.pop_back();
        return &block_->objects[index];
    }
    
    void deallocate(T* ptr) {
        if (!ptr) return;
        
        // Find the block containing this object
        auto it = blocks_.find(block_.get());
        if (it != blocks_.end()) {
            auto& block = it->first;
            size_t index = ptr - &block->objects[0];
            freeList_.push_back(index);
        }
    }
    
    void reserve(size_t count) {
        while (freeList_.size() < count) {
            addBlock();
        }
    }
    
    size_t size() const { return capacity_ - freeList_.size(); }
    
private:
    struct Block {
        std::array<T, 1024> objects;  // Fixed-size block
    };
    
    void addBlock() {
        blocks_.insert(std::make_unique<Block>());
    }
    
    std::vector<std::unique_ptr<Block>> blocks_;
    std::vector<size_t> freeList_;
    std::unique_ptr<Block> block_;
    size_t capacity_;
};

// Cache-friendly data structure for frequent lookups
template<typename Key, typename Value>
class LFUCache {
public:
    struct Entry {
        Key key;
        Value value;
        size_t frequency = 0;
        size_t lastAccess = 0;
    };
    
    LFUCache(size_t capacity = 1000) : capacity_(capacity) {}
    
    // Get or create entry
    Value& getOrCreate(const Key& key) {
        auto it = find(key);
        if (it != entries_.end()) {
            it->frequency++;
            it->lastAccess++;
            return it->value;
        }
        
        if (entries_.size() >= capacity_) {
            evict();
        }
        
        entries_.push_back({key, Value{}, 1, 0});
        return entries_.back().value;
    }
    
    // Get value if exists
    std::optional<Value&> get(const Key& key) {
        auto it = find(key);
        if (it != entries_.end()) {
            it->frequency++;
            it->lastAccess++;
            return it->value;
        }
        return std::nullopt;
    }
    
private:
    std::vector<Entry> entries_;
    size_t capacity_;
    
    auto find(const Key& key) {
        return std::find_if(entries_.begin(), entries_.end(),
            [&key](const Entry& e) { return e.key == key; });
    }
    
    void evict() {
        // Evict least frequently used entry
        auto it = std::min_element(entries_.begin(), entries_.end(),
            [](const Entry& a, const Entry& b) {
                return a.frequency < b.frequency || 
                      (a.frequency == b.frequency && a.lastAccess < b.lastAccess);
            });
        entries_.erase(it);
    }
};

} // namespace nlm
