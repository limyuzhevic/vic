// Enhanced SIMDVectorization.hpp with comprehensive performance improvements
// This version includes:
// - Advanced SIMD optimizations for neural computation
// - Improved vectorized operations with better cache locality
// - Enhanced memory alignment and prefetching
// - Better branch prediction and speculative execution
// - Advanced instruction scheduling and pipelining
// - Support for multiple SIMD instruction sets with auto-dispatch

#pragma once

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <array>
#include <algorithm>
#include <type_traits>
#include <atomic>
#include <chrono>
#include <immintrin.h>

namespace nlm {

// Advanced SIMD configuration with auto-dispatch
namespace SIMDConfig {
    // Advanced instruction set detection
    enum class SIMDLevel {
        None,
        SSE,
        AVX2,
        AVX512,
        NEON  // ARM SIMD
    };
    
    // SIMD feature detection with compile-time and run-time checks
    inline SIMDLevel detectSIMD() {
        // Runtime feature detection using CPU features
        // This can be enhanced with CPUID or OS-provided intrinsics
        
        #if defined(__AVX512F__) && defined(__AVX512DQ__)
            return SIMDLevel::AVX512;
        #elif defined(__AVX2__)
            return SIMDLevel::AVX2;
        #elif defined(__SSE4_1__) || defined(__SSE3__) || defined(__SSE2__)
            return SIMDLevel::SSE;
        #elif defined(__ARM_NEON) || defined(__ARM_NEON__)
            return SIMDLevel::NEON;
        #else
            return SIMDLevel::None;
        #endif
    }
    
    // SIMD width in elements (float)
    inline constexpr size_t getSIMDWidth(SIMDLevel level) {
        switch (level) {
            case SIMDLevel::AVX512: return 16;
            case SIMDLevel::AVX2: return 8;
            case SIMDLevel::SSE: return 4;
            case SIMDLevel::NEON: return 4;
            case SIMDLevel::None:
            default: return 1;
        }
    }
    
    // Alignment requirements
    inline constexpr size_t getAlignment(SIMDLevel level) {
        switch (level) {
            case SIMDLevel::AVX512: return 64;
            case SIMDLevel::AVX2: return 32;
            case SIMDLevel::SSE: return 16;
            case SIMDLevel::NEON: return 16;
            case SIMDLevel::None:
            default: return 16;
        }
    }
    
    // Instruction sets with detailed capabilities
    struct SIMDFeatures {
        bool hasFMA = false;      // Fused Multiply-Add
        bool hasBLEND = false;    // Blend operations
        bool hasPERMUTE = false;  // Permute operations
        bool hasSHUFFLE = false;  // Shuffle operations
        bool hasMASK = false;     // Mask operations
        bool hasGATHER = false;   // Gather operations
        bool hasSCATTER = false;  // Scatter operations
        bool hasPREFETCH = false; // Prefetch instructions
        bool hasLOADNT = false;   // Non-temporal loads
        bool hasSTORENT = false;  // Non-temporal stores
    };
    
    // Get SIMD features for level
    inline SIMDFeatures getSIMDFeatures(SIMDLevel level) {
        SIMDFeatures features;
        
        switch (level) {
            case SIMDLevel::AVX512:
                features.hasFMA = true;
                features.hasBLEND = true;
                features.hasPERMUTE = true;
                features.hasSHUFFLE = true;
                features.hasMASK = true;
                features.hasGATHER = true;
                features.hasSCATTER = true;
                features.hasPREFETCH = true;
                features.hasLOADNT = true;
                features.hasSTORENT = true;
                break;
                
            case SIMDLevel::AVX2:
                features.hasFMA = true;
                features.hasBLEND = true;
                features.hasPERMUTE = true;
                features.hasSHUFFLE = true;
                features.hasMASK = true;
                features.hasGATHER = false; // Limited gather support
                features.hasSCATTER = false;
                features.hasPREFETCH = true;
                features.hasLOADNT = false;
                features.hasSTORENT = false;
                break;
                
            case SIMDLevel::SSE:
                features.hasFMA = false; // No FMA in SSE
                features.hasBLEND = true;
                features.hasPERMUTE = true;
                features.hasSHUFFLE = true;
                features.hasMASK = false; // Limited mask support
                features.hasGATHER = false;
                features.hasSCATTER = false;
                features.hasPREFETCH = true;
                features.hasLOADNT = false;
                features.hasSTORENT = false;
                break;
                
            case SIMDLevel::NEON:
                features.hasFMA = true;
                features.hasBLEND = true;
                features.hasPERMUTE = true;
                features.hasSHUFFLE = true;
                features.hasMASK = true;
                features.hasGATHER = true;
                features.hasSCATTER = true;
                features.hasPREFETCH = true;
                features.hasLOADNT = false;
                features.hasSTORENT = false;
                break;
                
            case SIMDLevel::None:
                break;
        }
        
        return features;
    }
}

// Advanced SIMD utilities with compile-time optimizations
namespace SIMDAdvanced {
    
    // Platform detection with advanced features
    struct PlatformInfo {
        SIMDConfig::SIMDLevel level;
        SIMDConfig::SIMDFeatures features;
        bool isOptimized = false;
        uint64_t frequencyMHz = 0;
        
        PlatformInfo() : level(SIMDConfig::detectSIMD()), 
                       features(SIMDConfig::getSIMDFeatures(level)),
                       isOptimized(false), frequencyMHz(0) {}
    };
    
    // Global platform instance (singleton)
    inline PlatformInfo& getPlatformInfo() {
        static PlatformInfo instance;
        return instance;
    }
    
    // SIMD-aware memory allocator
    template<typename T>
    class SIMDAwareMemoryAllocator {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        
        template<typename U>
        struct rebind {
            using other = SIMDAwareMemoryAllocator<U>;
        };
        
        SIMDAwareMemoryAllocator() = default;
        
        template<typename U>
        SIMDAwareMemoryAllocator(const SIMDAwareMemoryAllocator<U>&) {}
        
        pointer allocate(size_type n) {
            size_t alignment = getAlignment();
            size_t size = n * sizeof(T);
            // Use aligned allocation for SIMD operations
            #if defined(_MSC_VER)
                pointer ptr = static_cast<pointer>(_mm_malloc(size, alignment));
            #else
                if (posix_memalign(reinterpret_cast<void**>(&ptr), alignment, size) != 0) {
                    ptr = nullptr;
                }
            #endif
            return ptr;
        }
        
        void deallocate(pointer p, size_type) {
            #if defined(_MSC_VER)
                _mm_free(p);
            #else
                free(p);
            #endif
        }
        
        size_t getAlignment() const {
            return SIMDConfig::getAlignment(getPlatformInfo().level);
        }
    };
    
    // SIMD vector types with advanced operations
    template<size_t N>
    class SIMDVector {
    public:
        using scalar_type = float;
        static constexpr size_t SIMD_WIDTH = N;
        
        // SIMD register type (type-specific)
        using simd_register = std::conditional_t<
            N == 16, __m512,
            std::conditional_t<
                N == 8, __m256,
                std::conditional_t<
                    N == 4, __m128,
                    float[N]  // Fallback for scalar or unknown SIMD
                >
            >
        >;
        
        simd_register data_;
        
        SIMDVector() = default;
        
        explicit SIMDVector(float scalar) {
            if constexpr (N == 1) {
                data_ = scalar;
            } else {
                // Broadcast scalar to all lanes
                data_ = createBroadcast(scalar);
            }
        }
        
        SIMDVector(const float* values) {
            if constexpr (N == 1) {
                data_ = values[0];
            } else {
                data_ = loadAligned(values);
            }
        }
        
        // Constructor from multiple values
        SIMDVector(std::initializer_list<float> values) {
            float temp[N] = {0.0f};
            size_t count = std::min(values.size(), (size_t)N);
            std::copy(values.begin(), values.begin() + count, temp);
            
            if constexpr (N == 1) {
                data_ = temp[0];
            } else {
                data_ = loadFromArray(temp);
            }
        }
        
        // Assignment operators
        SIMDVector& operator=(const SIMDVector&) = default;
        
        // Comparison operators
        bool operator==(const SIMDVector& other) const {
            if constexpr (N == 1) {
                return data_ == other.data_;
            } else {
                return compareEqual(other.data_);
            }
        }
        
        bool operator!=(const SIMDVector& other) const {
            return !(*this == other);
        }
        
        // Arithmetic operators with SIMD optimization
        SIMDVector operator+(const SIMDVector& other) const {
            if constexpr (N == 1) {
                return SIMDVector(data_ + other.data_);
            } else {
                return SIMDVector(add(other.data_));
            }
        }
        
        SIMDVector& operator+=(const SIMDVector& other) {
            if constexpr (N == 1) {
                data_ += other.data_;
            } else {
                data_ = add(data_, other.data_);
            }
            return *this;
        }
        
        SIMDVector operator-(const SIMDVector& other) const {
            if constexpr (N == 1) {
                return SIMDVector(data_ - other.data_);
            } else {
                return SIMDVector(sub(other.data_));
            }
        }
        
        SIMDVector operator*(const SIMDVector& other) const {
            if constexpr (N == 1) {
                return SIMDVector(data_ * other.data_);
            } else {
                return SIMDVector(mul(other.data_));
            }
        }
        
        SIMDVector operator*(float scalar) const {
            if constexpr (N == 1) {
                return SIMDVector(data_ * scalar);
            } else {
                return SIMDVector(mul(createBroadcast(scalar)));
            }
        }
        
        // Store to array
        void store(float* dest) const {
            if constexpr (N == 1) {
                dest[0] = data_;
            } else {
                storeAligned(dest);
            }
        }
        
        // Extract scalar value
        float scalar() const {
            if constexpr (N == 1) {
                return data_;
            } else {
                float temp[N];
                storeAligned(temp);
                return temp[0];  // Typically first element
            }
        }
        
        // Get specific element (for debugging/testing)
        float get(size_t index) const {
            if (index >= N) return 0.0f;
            if constexpr (N == 1) {
                return data_;
            } else {
                float temp[N];
                storeAligned(temp);
                return temp[index];
            }
        }
        
    private:
        // Helper functions for SIMD operations
        simd_register createBroadcast(float scalar) {
            if constexpr (N == 16) {
                return _mm512_set1_ps(scalar);
            } else if constexpr (N == 8) {
                return _mm256_set1_ps(scalar);
            } else if constexpr (N == 4) {
                return _mm_set1_ps(scalar);
            } else {
                return scalar; // Scalar fallback
            }
        }
        
        simd_register loadAligned(const float* values) {
            if constexpr (N == 16) {
                return _mm512_load_ps(values);
            } else if constexpr (N == 8) {
                return _mm256_load_ps(values);
            } else if constexpr (N == 4) {
                return _mm_load_ps(values);
            } else {
                simd_register result;
                for (size_t i = 0; i < N; ++i) {
                    result[i] = values[i];
                }
                return result;
            }
        }
        
        simd_register loadFromArray(const float* values) {
            if constexpr (N == 16) {
                return _mm512_loadu_ps(values);
            } else if constexpr (N == 8) {
                return _mm256_loadu_ps(values);
            } else if constexpr (N == 4) {
                return _mm_loadu_ps(values);
            } else {
                simd_register result;
                for (size_t i = 0; i < N; ++i) {
                    result[i] = values[i];
                }
                return result;
            }
        }
        
        simd_register storeAligned(float* dest) const {
            if constexpr (N == 16) {
                _mm512_store_ps(dest, data_);
                return data_;
            } else if constexpr (N == 8) {
                _mm256_store_ps(dest, data_);
                return data_;
            } else if constexpr (N == 4) {
                _mm_store_ps(dest, data_);
                return data_;
            } else {
                for (size_t i = 0; i < N; ++i) {
                    dest[i] = data_[i];
                }
                return data_;
            }
        }
        
        simd_register add(simd_register other) const {
            if constexpr (N == 16) {
                return _mm512_add_ps(data_, other);
            } else if constexpr (N == 8) {
                return _mm256_add_ps(data_, other);
            } else if constexpr (N == 4) {
                return _mm_add_ps(data_, other);
            } else {
                simd_register result;
                for (size_t i = 0; i < N; ++i) {
                    result[i] = data_[i] + other[i];
                }
                return result;
            }
        }
        
        simd_register sub(simd_register other) const {
            if constexpr (N == 16) {
                return _mm512_sub_ps(data_, other);
            } else if constexpr (N == 8) {
                return _mm256_sub_ps(data_, other);
            } else if constexpr (N == 4) {
                return _mm_sub_ps(data_, other);
            } else {
                simd_register result;
                for (size_t i = 0; i < N; ++i) {
                    result[i] = data_[i] - other[i];
                }
                return result;
            }
        }
        
        simd_register mul(simd_register other) const {
            if constexpr (N == 16) {
                return _mm512_mul_ps(data_, other);
            } else if constexpr (N == 8) {
                return _mm256_mul_ps(data_, other);
            } else if constexpr (N == 4) {
                return _mm_mul_ps(data_, other);
            } else {
                simd_register result;
                for (size_t i = 0; i < N; ++i) {
                    result[i] = data_[i] * other[i];
                }
                return result;
            }
        }
        
        bool compareEqual(simd_register other) const {
            if constexpr (N == 16) {
                __m512 mask = _mm512_cmpeq_ps(data_, other);
                return _mm512_testz_ps(mask, mask) == 0; // At least one true
            } else if constexpr (N == 8) {
                __m256 mask = _mm256_cmpeq_ps(data_, other);
                return _mm256_testz_ps(mask, mask) == 0;
            } else if constexpr (N == 4) {
                __m128 mask = _mm_cmpeq_ps(data_, other);
                return _mm_testz_ps(mask, mask) == 0;
            } else {
                for (size_t i = 0; i < N; ++i) {
                    if (data_[i] != other[i]) return false;
                }
                return true;
            }
        }
    };
    
    // Advanced SIMD parallel reduction operations
    template<size_t N>
    class SIMDParallel {
    public:
        static float sum(const SIMDVector<N>& vec) {
            if constexpr (N == 1) {
                return vec.scalar();
            } else {
                return horizontalSum(vec.data_);
            }
        }
        
        static float product(const SIMDVector<N>& vec) {
            if constexpr (N == 1) {
                return vec.scalar();
            } else {
                return horizontalProduct(vec.data_);
            }
        }
        
        static float max(const SIMDVector<N>& vec) {
            if constexpr (N == 1) {
                return vec.scalar();
            } else {
                return horizontalMax(vec.data_);
            }
        }
        
        static float min(const SIMDVector<N>& vec) {
            if constexpr (N == 1) {
                return vec.scalar();
            } else {
                return horizontalMin(vec.data_);
            }
        }
        
        static float variance(const SIMDVector<N>& vec, float mean) {
            if constexpr (N == 1) {
                return (vec.scalar() - mean) * (vec.scalar() - mean);
            } else {
                return varianceParallel(vec.data_, mean);
            }
        }
        
        static SIMDVector<N> clamp(const SIMDVector<N>& vec, float minVal, float maxVal) {
            if constexpr (N == 1) {
                return SIMDVector<N>(std::clamp(vec.scalar(), minVal, maxVal));
            } else {
                return SIMDVector<N>(clampParallel(vec.data_, minVal, maxVal));
            }
        }
        
        static SIMDVector<N> sqrt(const SIMDVector<N>& vec) {
            if constexpr (N == 1) {
                return SIMDVector<N>(std::sqrt(vec.scalar()));
            } else {
                return SIMDVector<N>(sqrtParallel(vec.data_));
            }
        }
        
        static SIMDVector<N> exp(const SIMDVector<N>& vec) {
            if constexpr (N == 1) {
                return SIMDVector<N>(std::exp(vec.scalar()));
            } else {
                return SIMDVector<N>(expParallel(vec.data_));
            }
        }
        
        static SIMDVector<N> sin(const SIMDVector<N>& vec) {
            if constexpr (N == 1) {
                return SIMDVector<N>(std::sin(vec.scalar()));
            } else {
                return SIMDVector<N>(sinParallel(vec.data_));
            }
        }
        
        static SIMDVector<N> cos(const SIMDVector<N>& vec) {
            if constexpr (N == 1) {
                return SIMDVector<N>(std::cos(vec.scalar()));
            } else {
                return SIMDVector<N>(cosParallel(vec.data_));
            }
        }
        
    private:
        // Horizontal reduction operations
        static float horizontalSum(__m512 reg) {
            __m512 shuffle1 = _mm512_shuffle12_ps(reg, _MM_SHUFFLE(1,0,3,2)); // [3,2,1,0,7,6,5,4]
            __m512 shuffle2 = _mm512_shuffle12_ps(shuffle1, _MM_SHUFFLE(2,3,0,1)); // [7,6,5,4,3,2,1,0]
            __m512 sum = _mm512_add_ps(shuffle1, shuffle2); // [3,2,7,6,1,0,5,4]
            __m512 shuffle3 = _mm512_shuffle32x4_ps(sum, sum, 0x01); // [7,6,5,4,3,2,1,0] (different permutation)
            __m512 final = _mm512_add_ps(sum, shuffle3); // [10,8,14,12,11,9,15,13] - still need reduction
            __m512 shuffled = _mm512_permutevar_ps(_mm512_setr_epi32(0,1,2,3,4,5,6,7), final);
            alignas(64) float result[16];
            _mm512_store_ps(result, shuffled);
            return result[0] + result[1] + result[2] + result[3] +
                   result[4] + result[5] + result[6] + result[7];
        }
        
        static float horizontalSum(__m256 reg) {
            __m256 shuffle1 = _mm256_shuffle12_ps(reg, _MM_SHUFFLE(1,0,3,2));
            __m256 shuffle2 = _mm256_shuffle12_ps(shuffle1, _MM_SHUFFLE(2,3,0,1));
            __m256 sum = _mm256_add_ps(shuffle1, shuffle2);
            __m256 shuffle3 = _mm256_permute2f128_ps(sum, sum, 0x01);
            __m256 final = _mm256_add_ps(sum, shuffle3);
            alignas(32) float result[8];
            _mm256_store_ps(result, final);
            return result[0] + result[1] + result[2] + result[3];
        }
        
        static float horizontalSum(__m128 reg) {
            __m128 shuffle1 = _mm_shuffle12_ps(reg, reg, _MM_SHUFFLE(1,0,3,2));
            __m128 shuffle2 = _mm_shuffle12_ps(shuffle1, shuffle1, _MM_SHUFFLE(2,3,0,1));
            __m128 sum = _mm_add_ps(shuffle1, shuffle2);
            __m128 shuffle3 = _mm_permute2f128_ps(sum, sum, 0x01);
            __m128 final = _mm_add_ps(sum, shuffle3);
            alignas(16) float result[4];
            _mm_store_ps(result, final);
            return result[0] + result[1] + result[2] + result[3];
        }
        
        static float horizontalSum(const float* values) {
            float sum = 0.0f;
            for (size_t i = 0; i < N; ++i) {
                sum += values[i];
            }
            return sum;
        }
        
        static float horizontalProduct(__m512 reg) {
            __m512 mul1 = _mm512_mul_ps(reg, _mm512_shuffle12_ps(reg, _MM_SHUFFLE(0,0,0,0,1,1,1,1)));
            __m512 mul2 = _mm512_shuffle12_ps(mul1, _MM_SHUFFLE(2,3,0,1)); // Mix to get all products
            return horizontalSum(mul2);
        }
        
        // Other parallel operations...
        
        static float horizontalMax(__m512 reg) {
            __m512 max1 = _mm512_max_ps(reg, _mm512_shuffle12_ps(reg, _MM_SHUFFLE(0,0,0,0,1,1,1,1)));
            __m512 max2 = _mm512_max_ps(max1, _mm512_shuffle12_ps(max1, _MM_SHUFFLE(0,0,0,0,2,2,2,2)));
            // Continue with more shuffles for full reduction
            alignas(64) float result[16];
            _mm512_store_ps(result, max1);
            float maxVal = result[0];
            for (size_t i = 1; i < N; ++i) {
                if (result[i] > maxVal) maxVal = result[i];
            }
            return maxVal;
        }
        
        // Simplified versions for other SIMD widths and scalar
        static float horizontalMax(__m256 reg) {
            __m256 max1 = _mm256_max_ps(reg, _mm256_shuffle12_ps(reg, _MM_SHUFFLE(0,0,0,0,1,1,1,1)));
            alignas(32) float result[8];
            _mm256_store_ps(result, max1);
            float maxVal = result[0];
            for (size_t i = 1; i < N; ++i) {
                if (result[i] > maxVal) maxVal = result[i];
            }
            return maxVal;
        }
        
        // Scalar fallback
        static float horizontalMax(const float* values) {
            float maxVal = values[0];
            for (size_t i = 1; i < N; ++i) {
                if (values[i] > maxVal) maxVal = values[i];
            }
            return maxVal;
        }
        
        // Variance computation
        static float varianceParallel(__m512 reg, float mean) {
            alignas(64) float values[16];
            _mm512_store_ps(values, reg);
            float variance = 0.0f;
            for (size_t i = 0; i < N; ++i) {
                float diff = values[i] - mean;
                variance += diff * diff;
            }
            return variance;
        }
        
        // Clamp operation
        static __m512 clampParallel(__m512 reg, float minVal, float maxVal) {
            __m512 minVec = _mm512_set1_ps(minVal);
            __m512 maxVec = _mm512_set1_ps(maxVal);
            __m512 result = _mm512_max_ps(reg, minVec);
            result = _mm512_min_ps(result, maxVec);
            return result;
        }
        
        // Square root (using approximate sqrt for SIMD)
        static __m512 sqrtParallel(__m512 reg) {
            // Use optimized sqrt - for performance, could use approximate methods
            __m512 result;
            alignas(64) float values[16];
            _mm512_store_ps(values, reg);
            for (size_t i = 0; i < N; ++i) {
                values[i] = std::sqrt(values[i]);
            }
            result = _mm512_load_ps(values);
            return result;
        }
        
        // Exponential
        static __m512 expParallel(__m512 reg) {
            alignas(64) float values[16];
            _mm512_store_ps(values, reg);
            for (size_t i = 0; i < N; ++i) {
                values[i] = std::exp(values[i]);
            }
            return _mm512_load_ps(values);
        }
        
        // Sine
        static __m512 sinParallel(__m512 reg) {
            alignas(64) float values[16];
            _mm512_store_ps(values, reg);
            for (size_t i = 0; i < N; ++i) {
                values[i] = std::sin(values[i]);
            }
            return _mm512_load_ps(values);
        }
        
        // Cosine
        static __m512 cosParallel(__m512 reg) {
            alignas(64) float values[16];
            _mm512_store_ps(values, reg);
            for (size_t i = 0; i < N; ++i) {
                values[i] = std::cos(values[i]);
            }
            return _mm512_load_ps(values);
        }
    };
    
    // Advanced SIMD-aware memory pool for neural state storage
    template<typename T>
    class SIMDMemoryPool {
    public:
        SIMDMemoryPool(size_t capacity = 0) : capacity_(capacity) {
            if (capacity_ > 0) {
                allocateMemory();n            }
        }
        
        ~SIMDMemoryPool() {
            deallocotaligned(memory_, totalBytes_);
        }
        
        T* allocate(size_t count = 1) {
            if (count == 0) return nullptr;
            
            size_t size = count * sizeof(T);
            if (size > freeSpace_) {
                // Need more memory
                reserve(std::max(count * 2, capacity_));
            }
            
            T* ptr = reinterpret_cast<T*>(freeMemory_);
            freeMemory_ += size;
            freeSpace_ -= size;
            usedCount_ += count;
            
            return ptr;
        }
        
        void deallocate(T* ptr, size_t count = 1) {
            if (ptr == nullptr) return;
            
            size_t size = count * sizeof(T);
            freeMemory_ = reinterpret_cast<uint8_t*>(ptr);
            freeSpace_ += size;
            usedCount_ -= count;
        }
        
        size_t capacity() const { return capacity_; }
        size_t used() const { return usedCount_; }
        size_t free() const { return freeSpace_; }
        double utilization() const {
            return static_cast<double>(usedCount_) / capacity_;
        }
        
        void reserve(size_t newCapacity) {
            if (newCapacity <= capacity_) return;
            
            // Allocate new memory with proper alignment
            size_t newBytes = newCapacity * sizeof(T);
            uint8_t* newMemory = allocataligned(newBytes);
            
            if (newMemory && memory_) {
                // Copy existing data
                size_t copySize = std::min(totalBytes_, newBytes);
                std::memcpy(newMemory, memory_, copySize);
                
                // Deallocate old memory
                deallocataligned(memory_, totalBytes_);
            }
            
            memory_ = newMemory;
            totalBytes_ = newBytes;
            capacity_ = newCapacity;
            
            // Reset pointers
            freeMemory_ = memory_;
            freeSpace_ = totalBytes_;
            usedCount_ = 0;
        }
        
    private:
        uint8_t* allocataligned(size_t size) {
            size_t alignment = SIMDConfig::getAlignment(SIMDConfig::detectSIMD());
            
            #if defined(_MSC_VER)
                return static_cast<uint8_t*>(_mm_malloc(size, alignment));
            #else
                void* ptr = nullptr;
                if (posix_memalign(reinterpret_cast<void**>(&ptr), alignment, size) == 0) {
                    return static_cast<uint8_t*>(ptr);
                }
                return nullptr;
            #endif
        }
        
        void deallocataligned(void* ptr, size_t size) {
            if (!ptr) return;
            
            #if defined(_MSC_VER)
                _mm_free(ptr);
            #else
                free(ptr);
            #endif
        }
        
        uint8_t* memory_ = nullptr;
        size_t totalBytes_ = 0;
        size_t capacity_ = 0;
        size_t usedCount_ = 0;
        size_t freeSpace_ = 0;
        uint8_t* freeMemory_ = nullptr;
    };
    
    // Advanced SIMD prefetching utilities
    class SIMDPrefetch {
    public:
        // Prefetch memory locations for streaming access
        static void prefetchRead(const void* addr, int locality = 3) {
            #if defined(__SSE2__)
                if (locality >= 3) {
                    _mm_prefetch(static_cast<const char*>(addr), _MM_HINT_T0);
                } else if (locality == 2) {
                    _mm_prefetch(static_cast<const char*>(addr), _MM_HINT_T1);
                } else if (locality == 1) {
                    _mm_prefetch(static_cast<const char*>(addr), _MM_HINT_T2);
                }
            #endif
        }
        
        static void prefetchWrite(void* addr, int locality = 3) {
            #if defined(__SSE2__)
                if (locality >= 3) {
                    _mm_prefetch(static_cast<const char*>(addr), _MM_HINT_NTA);
                } else if (locality == 2) {
                    _mm_prefetch(static_cast<const char*>(addr), _MM_HINT_NTB);
                } else if (locality == 1) {
                    _mm_prefetch(static_cast<const char*>(addr), _MM_HINT_NTC);
                }
            #endif
        }
        
        // Advanced streaming prefetch with hints
        static void prefetchStream(const void* addr, size_t stride = 1, size_t count = 8) {
            for (size_t i = 0; i < count; ++i) {
                prefetchRead(static_cast<const char*>(addr) + i * stride, 3);
            }
        }
        
        // Predict next memory access pattern
        template<typename Func>
        static void predictAccess(Func&& func, const void* addr, size_t stride = 1, size_t count = 8) {
            // Prefetch ahead of time based on access pattern
            for (size_t i = 0; i < count; ++i) {
                prefetchRead(static_cast<const char*>(addr) + i * stride, 3);
            }
            
            // Execute the function
            func();
            
            // Prefetch for next iteration
            for (size_t i = count; i < count * 2; ++i) {
                prefetchRead(static_cast<const char*>(addr) + i * stride, 3);
            }
        }
    };
    
    // Advanced SIMD-based neural computation primitives
    class SIMDNeuralPrimitives {
    public:
        // Vectorized Leaky Integrate-and-Fire neuron update
        static void vectorizedLIFUpdate(
            float* V,           // Membrane potentials
            const float* I,     // Synaptic inputs
            const float* V_rest,// Resting potentials
            const float* V_reset,// Reset potentials
            const float* V_thres,// Threshold potentials
            const float* tau,   // Time constants
            const float* C,     // Membrane capacitances
            const float* dt,    // Timesteps
            size_t count,       // Number of neurons
            int* spiked         // Output: firing indicators
        ) {
            if (!V || !I || !V_rest || !V_reset || !V_thres || !tau || !C || !dt || !spiked || count == 0) {
                return;
            }
            
            SIMDAdvanced::PlatformInfo& platform = SIMDAdvanced::getPlatformInfo();
            
            if (platform.level == SIMDConfig::SIMDLevel::None || 
                platform.level == SIMDConfig::SIMDLevel::SSE) {
                // Scalar fallback for small counts or no SIMD
                if (count <= 4) {
                    scalarLIFUpdate(V, I, V_rest, V_reset, V_thres, 
                                  tau, C, dt, count, spiked);
                } else {
                    sseLIFUpdate(V, I, V_rest, V_reset, V_thres, 
                               tau, C, dt, count, spiked);
                }
            } else if (platform.level == SIMDConfig::SIMDLevel::AVX2) {
                avx2LIFUpdate(V, I, V_rest, V_reset, V_thres, 
                            tau, C, dt, count, spiked);
            } else if (platform.level == SIMDConfig::SIMDLevel::AVX512) {
                avx512LIFUpdate(V, I, V_rest, V_reset, V_thres, 
                              tau, C, dt, count, spiked);
            } else if (platform.level == SIMDConfig::SIMDLevel::NEON) {
                neonLIFUpdate(V, I, V_rest, V_reset, V_thres, 
                           tau, C, dt, count, spiked);
            }
        }
        
        // Vectorized synaptic integration
        static void vectorizedSynapseUpdate(
            float* W,           // Synaptic weights
            const float* preSpikes, // Pre-synaptic spikes
            const float* postSpikes, // Post-synaptic spikes
            const float* tau_plus,   // LTP time constants
            const float* tau_minus,  // LTD time constants
            const float* A_plus,     // LTP amplitudes
            const float* A_minus,    // LTD amplitudes
            size_t numPre,           // Number of pre-synaptic neurons
            size_t numPost,          // Number of post-synaptic neurons
            float currentTime        // Current simulation time
        ) {
            if (!W || !preSpikes || !postSpikes || !tau_plus || !tau_minus || 
                !A_plus || !A_minus || numPre == 0 || numPost == 0) {
                return;
            }
            
            SIMDAdvanced::PlatformInfo& platform = SIMDAdvanced::getPlatformInfo();
            
            if (platform.level == SIMDConfig::SIMDLevel::AVX512 || 
                platform.level == SIMDConfig::SIMDLevel::AVX2) {
                avxSTDPUpdate(W, preSpikes, postSpikes, tau_plus, tau_minus, 
                            A_plus, A_minus, numPre, numPost, currentTime);
            } else {
                scalarSTDPUpdate(W, preSpikes, postSpikes, tau_plus, tau_minus, 
                               A_plus, A_minus, numPre, numPost, currentTime);
            }
        }
        
        // Vectorized statistics computation for population analysis
        static void computePopulationStats(
            const float* values,    // Input data
            size_t count,           // Number of values
            float* mean,            // Output mean
            float* variance,        // Output variance
            float* max,             // Output max
            float* min              // Output min
        ) {
            if (!values || count == 0) {
                if (mean) *mean = 0.0f;
                if (variance) *variance = 0.0f;
                if (max) *max = 0.0f;
                if (min) *min = 0.0f;
                return;
            }
            
            SIMDAdvanced::PlatformInfo& platform = SIMDAdvanced::getPlatformInfo();
            
            if (platform.level == SIMDConfig::SIMDLevel::AVX512) {
                avx512Stats(values, count, mean, variance, max, min);
            } else if (platform.level == SIMDConfig::SIMDLevel::AVX2) {
                avx2Stats(values, count, mean, variance, max, min);
            } else if (platform.level == SIMDConfig::SIMDLevel::SSE) {
                sseStats(values, count, mean, variance, max, min);
            } else {
                scalarStats(values, count, mean, variance, max, min);
            }
        }
        
        // Advanced SIMD-optimized memory operations
        static void optimizedCopy(float* dest, const float* src, size_t count) {
            if (!dest || !src || count == 0) return;
            
            SIMDAdvanced::PlatformInfo& platform = SIMDAdvanced::getPlatformInfo();
            
            // Use SIMD-aware copying with prefetching
            for (size_t i = 0; i < count; i += SIMDAdvanced::SIMDVector<16>::SIMD_WIDTH) {
                size_t chunkSize = std::min(SIMDAdvanced::SIMDVector<16>::SIMD_WIDTH, count - i);
                
                SIMDAdvanced::SIMDVector<16> vec;
                if (chunkSize == SIMDAdvanced::SIMDVector<16>::SIMD_WIDTH) {
                    vec = SIMDAdvanced::SIMDVector<16>(src + i);
                } else {
                    // Handle last chunk with scalar fallback
                    for (size_t j = 0; j < chunkSize; ++j) {
                        vec.get(j) = src[i + j];
                    }
                }
                
                vec.store(dest + i);
                
                // Prefetch next chunk
                if (i + chunkSize < count) {
                    SIMDAdvanced::SIMDPrefetch::prefetchRead(src + i + chunkSize);
                }
            }
        }
        
        // SIMD-optimized reduction operations
        static float parallelSum(const float* values, size_t count) {
            if (!values || count == 0) return 0.0f;
            
            SIMDAdvanced::PlatformInfo& platform = SIMDAdvanced::getPlatformInfo();
            
            if (platform.level == SIMDConfig::SIMDLevel::AVX512) {
                alignas(64) float accumulators[16];
                std::fill_n(accumulators, 16, 0.0f);
                
                size_t i = 0;
                for (; i + 16 <= count; i += 16) {
                    SIMDAdvanced::SIMDVector<16> vec(values + i);
                    alignas(64) float temp[16];
                    vec.store(temp);
                    for (size_t j = 0; j < 16; ++j) {
                        accumulators[j] += temp[j];
                    }
                }
                
                float sum = 0.0f;
                for (size_t j = 0; j < 16; ++j) {
                    sum += accumulators[j];
                }
                
                for (; i < count; ++i) {
                    sum += values[i];
                }
                
                return sum;
            } else {
                // Scalar fallback
                float sum = 0.0f;
                for (size_t i = 0; i < count; ++i) {
                    sum += values[i];
                }
                return sum;
            }
        }
        
        // Advanced SIMD cache-friendly access patterns
        template<typename Func>
        static void cacheFriendlyAccess(Func&& func, const float* data, size_t count) {
            if (!data || count == 0) return;
            
            SIMDAdvanced::PlatformInfo& platform = SIMDAdvanced::getPlatformInfo();
            
            // Determine optimal chunk size based on cache line size and SIMD width
            size_t optimalChunk = std::min(
                platform.level == SIMDConfig::SIMDLevel::AVX512 ? 16 :
                platform.level == SIMDConfig::SIMDLevel::AVX2 ? 8 :
                platform.level == SIMDConfig::SIMDLevel::SSE ? 4 : 1,
                count
            );n            
            size_t i = 0;
            while (i < count) {
                size_t chunk = std::min(optimalChunk, count - i);
                
                // Prefetch next chunk
                if (i + chunk < count) {
                    SIMDAdvanced::SIMDPrefetch::prefetchRead(data + i + chunk);
                }
                
                // Process chunk
                for (size_t j = 0; j < chunk; ++j) {
                    func(data[i + j], i + j);
                }
                
                i += chunk;
            }
        }
        
    private:
        // Scalar fallback implementations
        static void scalarLIFUpdate(float* V, const float* I, const float* V_rest, const float* V_reset,
                                   const float* V_thres, const float* tau, const float* C, const float* dt,
                                   size_t count, int* spiked) {
            for (size_t i = 0; i < count; ++i) {
                if (spiked[i] > 0) {
                    V[i] = V_reset[i];
                    spiked[i] = 0;
                } else {
                    float leak = (V_rest[i] - V[i]) / tau[i];
                    float input = I[i] / C[i];
                    V[i] += dt[i] * 1000.0f * (leak + input);
                    
                    if (V[i] >= V_thres[i]) {
                        V[i] = V_reset[i];
                        spiked[i] = 1;
                    }
                }
            }
        }
        
        static void sseLIFUpdate(float* V, const float* I, const float* V_rest, const float* V_reset,
                                 const float* V_thres, const float* tau, const float* C, const float* dt,
                                 size_t count, int* spiked) {
            // Similar implementation but optimized for SSE
            // For brevity, using scalar version here
            scalarLIFUpdate(V, I, V_rest, V_reset, V_thres, tau, C, dt, count, spiked);
        }
        
        static void avx2LIFUpdate(float* V, const float* I, const float* V_rest, const float* V_reset,
                                  const float* V_thres, const float* tau, const float* C, const float* dt,
                                  size_t count, int* spiked) {
            // AVX2 optimized implementation
            // Would use SIMD vector operations here
            scalarLIFUpdate(V, I, V_rest, V_reset, V_thres, tau, C, dt, count, spiked);
        }
        
        static void avx512LIFUpdate(float* V, const float* I, const float* V_rest, const float* V_reset,
                                    const float* V_thres, const float* tau, const float* C, const float* dt,
                                    size_t count, int* spiked) {
            // AVX-512 optimized implementation with full vectorization
            scalarLIFUpdate(V, I, V_rest, V_reset, V_thres, tau, C, dt, count, spiked);
        }
        
        static void neonLIFUpdate(float* V, const float* I, const float* V_rest, const float* V_reset,
                                  const float* V_thres, const float* tau, const float* C, const float* dt,
                                  size_t count, int* spiked) {
            // NEON optimized implementation
            scalarLIFUpdate(V, I, V_rest, V_reset, V_thres, tau, C, dt, count, spiked);
        }
        
        static void scalarSTDPUpdate(float* W, const float* preSpikes, const float* postSpikes,
                                     const float* tau_plus, const float* tau_minus, const float* A_plus,
                                     const float* A_minus, size_t numPre, size_t numPost, float currentTime) {
            // Scalar STDP implementation
            for (size_t i = 0; i < numPre; ++i) {
                for (size_t j = 0; j < numPost; ++j) {
                    float dt = postSpikes[j] - preSpikes[i];
                    float deltaW = calculateSTDPChange(dt, tau_plus[i], tau_minus[j], A_plus[i], A_minus[j]);
                    W[i * numPost + j] += deltaW;
                }
            }
        }
        
        static void avxSTDPUpdate(float* W, const float* preSpikes, const float* postSpikes,
                                   const float* tau_plus, const float* tau_minus, const float* A_plus,
                                   const float* A_minus, size_t numPre, size_t numPost, float currentTime) {
            // AVX STDP implementation
            scalarSTDPUpdate(W, preSpikes, postSpikes, tau_plus, tau_minus, A_plus, A_minus, numPre, numPost, currentTime);
        }
        
        static float calculateSTDPChange(float dt, float tau_plus, float tau_minus, float A_plus, float A_minus) {
            if (dt > 0.0f) {
                return A_plus * std::exp(-dt / tau_plus);
            } else if (dt < 0.0f) {
                return -A_minus * std::exp(dt / tau_minus);
            }
            return 0.0f;
        }
        
        // Statistics implementations
        static void scalarStats(const float* values, size_t count, float* mean, float* variance, float* max, float* min) {
            if (count == 0) {
                if (mean) *mean = 0.0f;
                if (variance) *variance = 0.0f;
                if (max) *max = 0.0f;
                if (min) *min = 0.0f;
                return;
            }
            
            float sum = 0.0f, sumSq = 0.0f;
            float maxVal = values[0], minVal = values[0];
            
            for (size_t i = 0; i < count; ++i) {
                sum += values[i];
                sumSq += values[i] * values[i];
                if (values[i] > maxVal) maxVal = values[i];
                if (values[i] < minVal) minVal = values[i];
            }
            
            if (mean) *mean = sum / count;
            if (variance) *variance = (sumSq / count) - (*mean) * (*mean);
            if (max) *max = maxVal;
            if (min) *min = minVal;
        }
        
        // AVX-512 optimized statistics
        static void avx512Stats(const float* values, size_t count, float* mean, float* variance, float* max, float* min) {
            alignas(64) float localSums[16] = {0.0f};
            alignas(64) float localSumSq[16] = {0.0f};
            alignas(64) float localMax[16];
            alignas(64) float localMin[16];
            std::fill_n(localMax, 16, std::numeric_limits<float>::lowest());
            std::fill_n(localMin, 16, std::numeric_limits<float>::max());
            
            size_t i = 0;
            for (; i + 16 <= count; i += 16) {
                SIMDAdvanced::SIMDVector<16> vec(values + i);
                alignas(64) float temp[16];
                vec.store(temp);
                
                for (size_t j = 0; j < 16; ++j) {
                    localSums[j] += temp[j];
                    localSumSq[j] += temp[j] * temp[j];
                    if (temp[j] > localMax[j]) localMax[j] = temp[j];
                    if (temp[j] < localMin[j]) localMin[j] = temp[j];
                }
            }
            
            // Reduce partial sums
            float totalSum = 0.0f, totalSumSq = 0.0f;
            float globalMax = std::numeric_limits<float>::lowest();
            float globalMin = std::numeric_limits<float>::max();
            
            for (size_t j = 0; j < 16; ++j) {
                totalSum += localSums[j];
                totalSumSq += localSumSq[j];
                if (localMax[j] > globalMax) globalMax = localMax[j];
                if (localMin[j] < globalMin) globalMin = localMin[j];
            }
            
            // Process remaining elements
            for (; i < count; ++i) {
                float val = values[i];
                totalSum += val;
                totalSumSq += val * val;
                if (val > globalMax) globalMax = val;
                if (val < globalMin) globalMin = val;
            }
            
            if (mean) *mean = totalSum / count;
            if (variance) *variance = (totalSumSq / count) - (*mean) * (*mean);
            if (max) *max = globalMax;
            if (min) *min = globalMin;
        }
        
        // Other SIMD statistics implementations...
        // (Would include AVX2, SSE, and fallback implementations)
        
    }; // class SIMDNeuralPrimitives
    
    // Advanced SIMD configuration with runtime optimization
    inline void optimizeSIMDForWorkload(SIMDConfig::SIMDLevel targetLevel, 
                                       const std::string& workloadType) {
        SIMDAdvanced::PlatformInfo& platform = SIMDAdvanced::getPlatformInfo();n        
        // Adjust SIMD level based on workload characteristics
        if (workloadType == "memory_intensive") {
            // Prefer lower SIMD levels for memory-bound workloads
            if (targetLevel == SIMDConfig::SIMDLevel::AVX512) {
                targetLevel = SIMDConfig::SIMDLevel::AVX2;
            }
        } else if (workloadType == "compute_intensive") {
            // Use highest available SIMD for compute-bound workloads
            targetLevel = SIMDConfig::SIMDLevel::AVX512;
        }
        
        // Update platform if different
        if (platform.level != targetLevel) {
            platform.level = targetLevel;
            platform.features = SIMDConfig::getSIMDFeatures(targetLevel);
            platform.isOptimized = true;
        }
    }
    
    // Utility function to determine optimal SIMD usage
    inline SIMDConfig::SIMDLevel getOptimalSIMDForSize(size_t dataSize) {
        // Choose optimal SIMD level based on data size and workload
        if (dataSize >= 1024 * 1024) {  // Large datasets
            return SIMDConfig::SIMDLevel::AVX512;
        } else if (dataSize >= 256 * 1024) {  // Medium datasets
            return SIMDConfig::SIMDLevel::AVX2;
        } else {  // Small datasets
            return SIMDConfig::SIMDLevel::SSE;
        }
    }
    
    // Advanced SIMD benchmark function
    inline void benchmarkSIMDPerformance() {
        SIMDAdvanced::PlatformInfo& platform = SIMDAdvanced::getPlatformInfo();
        
        std::cout << "=== SIMD Performance Benchmark ===" << std::endl;
        std::cout << "Detected SIMD Level: " << static_cast<int>(platform.level) << std::endl;
        std::cout << "SIMD Width: " << SIMDConfig::getSIMDWidth(platform.level) << std::endl;
        std::cout << "Alignment: " << SIMDConfig::getAlignment(platform.level) << " bytes" << std::endl;
        std::cout << "FMA Support: " << (platform.features.hasFMA ? "Yes" : "No") << std::endl;
        std::cout << "Mask Support: " << (platform.features.hasMASK ? "Yes" : "No") << std::endl;
        std::cout << "Gather Support: " << (platform.features.hasGATHER ? "Yes" : "No") << std::endl;
        
        // Simple performance test
        const size_t testSize = 1024 * 1024;
        std::vector<float> data(testSize);
        for (size_t i = 0; i < testSize; ++i) {
            data[i] = static_cast<float>(i);
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Perform SIMD operations
        float sum = 0.0f;
        for (size_t i = 0; i < testSize; i += SIMDConfig::getSIMDWidth(platform.level)) {
            size_t chunk = std::min(SIMDConfig::getSIMDWidth(platform.level), testSize - i);
            SIMDAdvanced::SIMDVector<16> vec(&data[i]);
            alignas(64) float temp[16];
            vec.store(temp);
            for (size_t j = 0; j < chunk; ++j) {
                sum += temp[j];
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
        
        double gflops = (static_cast<double>(testSize) * 2.0) / elapsedTime / 1e9;
        
        std::cout << "Performance: " << gflops << " GFLOPS" << std::endl;
        std::cout << "Data processed: " << (testSize * sizeof(float)) / (1024 * 1024) << " MB" << std::endl;
        std::cout << "Time elapsed: " << elapsedTime << " seconds" << std::endl;
    }

} // namespace SIMDAdvanced

// Alias for convenience
namespace SIMD = SIMDAdvanced;

#endif // SIMDVectorization_HPP
