# NLM CMake Dependencies Module
# Handles system dependencies and dependency checking

# Check for math libraries
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    set(NLM_HAS_MATH_LIB TRUE)
endif()

# Check for real-time extensions
if(UNIX AND NOT APPLE)
    find_library(RT_LIBRARY rt)
    if(RT_LIBRARY)
        set(NLM_HAS_RT_LIB TRUE)
    endif()
endif()

# Compiler feature checks
include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)

# Check for SIMD support
check_cxx_source_compiles(" 
    #include <immintrin.h>
    int main() {
        __m128 a = _mm_setzero_ps();
        return 0;
    }
" NLM_HAS_SSE4)

check_cxx_source_compiles(" 
    #include <immintrin.h>
    int main() {
        __m256 a = _mm256_setzero_ps();
        return 0;
    }
" NLM_HAS_AVX)

check_cxx_source_compiles(" 
    #include <immintrin.h>
    int main() {
        __m512 a = _mm512_setzero_ps();
        return 0;
    }
" NLM_HAS_AVX512)
