add_library(nlm_threading STATIC
    ThreadSafeSimulationManager.cpp
)

target_include_directories(nlm_threading PUBLIC src)
