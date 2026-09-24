# Find dependencies for NLM

# Find required dependencies
find_package(CXX REQUIRED)

# Find optional dependencies
if(NLM_ENABLE_TESTS)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        set(NLM_HAS_GTEST TRUE)
        message(STATUS "Found GTest: ${GTEST_INCLUDE_DIRS}")
    else()
        message(WARNING "GTest not found. Tests will not be available.")
    endif()
endif()

if(NLM_ENABLE_PYBIND11)
    find_package(pybind11 QUIET)
    if(pybind11_FOUND)
        set(NLM_HAS_PYBIND11 TRUE)
        message(STATUS "Found pybind11: ${pybind11_VERSION}")
    else()
        message(WARNING "pybind11 not found. Python bindings will not be available.")
    endif()
endif()

# Find OpenMP for parallel processing (optional)
find_package(OpenMP QUIET)
if(OpenMP_Found)
    set(NLM_HAS_OPENMP TRUE)
    message(STATUS "Found OpenMP: ${OpenMP_CXX_FLAGS}")
endif()

# Find Eigen for linear algebra (optional)
find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    set(NLM_HAS_EIGEN TRUE)
    message(STATUS "Found Eigen: ${Eigen3_VERSION}")
endif()
