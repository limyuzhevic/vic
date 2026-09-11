# Core components

file(GLOB CORE_SOURCES
    "core/Types/Types.cpp"
    "core/Config/Config.cpp"
    "core/Random/Random.cpp"
    "core/Logger/Logger.cpp"
    "core/SimulationClock/SimulationClock.cpp"
)

add_library(nlm_core STATIC ${CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)