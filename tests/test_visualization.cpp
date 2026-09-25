// Visualization Tests
#include "visualization/VisualizationInterface.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_visualization {

TEST_CASE("VisualizationInterface creation and initialization", "[visualization][interface][creation]") {
    nlm::VisualizationInterface visualization;
    
    REQUIRE(!visualization.isInitialized());
}

TEST_CASE("VisualizationInterface initialization with config", "[visualization][interface][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("visualization.enabled", true, nlm::ConfigSource::Runtime);
    config->set("visualization.output_format", "console", nlm::ConfigSource::Runtime);
    
    nlm::VisualizationInterface visualization;
    visualization.initialize(config);
    REQUIRE(visualization.isInitialized());
}

TEST_CASE("VisualizationInterface data rendering", "[visualization][interface][rendering]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("visualization.enabled", true, nlm::ConfigSource::Runtime);
    config->set("visualization.output_format", "text", nlm::ConfigSource::Runtime);
    
    nlm::VisualizationInterface visualization;
    visualization.initialize(config);
    
    REQUIRE(visualization.isInitialized());
    
    // Visualization should render data
    visualization.renderData("test_data", 1.0f);
}

TEST_CASE("VisualizationInterface with different output formats", "[visualization][interface][formats]") {
    // Test visualization with different output formats
    std::vector<std::string> outputFormats = {"console", "text", "json", "graph"};
    
    for (const auto& format : outputFormats) {
        auto config = std::make_shared<nlm::Config>();
        config->set("visualization.enabled", true, nlm::ConfigSource::Runtime);
        config->set("visualization.output_format", format, nlm::ConfigSource::Runtime);
        
        nlm::VisualizationInterface visualization;
        visualization.initialize(config);
        
        REQUIRE(visualization.isInitialized());
    }
}

TEST_CASE("VisualizationInterface real-time updates", "[visualization][interface][updates]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("visualization.enabled", true, nlm::ConfigSource::Runtime);
    config->set("visualization.real_time_updates", true, nlm::ConfigSource::Runtime);
    config->set("visualization.update_frequency", 60, nlm::ConfigSource::Runtime);
    
    nlm::VisualizationInterface visualization;
    visualization.initialize(config);
    
    REQUIRE(visualization.isInitialized());
    
    // Visualization should handle real-time updates
    visualization.renderData("real_time_data", 2.0f);
}

TEST_CASE("VisualizationInterface data export capabilities", "[visualization][interface][export]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("visualization.enabled", true, nlm::ConfigSource::Runtime);
    config->set("visualization.export_enabled", true, nlm::ConfigSource::Runtime);
    config->set("visualization.export_format", "csv", nlm::ConfigSource::Runtime);
    
    nlm::VisualizationInterface visualization;
    visualization.initialize(config);
    
    REQUIRE(visualization.isInitialized());
    
    // Visualization should export data
    visualization.exportData("exported_data.csv");
}

TEST_CASE("VisualizationInterface performance evaluation", "[visualization][interface][performance]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("visualization.enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::VisualizationInterface visualization;
    visualization.initialize(config);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform visualization operations
    for (int i = 0; i < 100; ++i) {
        visualization.renderData("performance_data", 0.1f * (i + 1));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
}

TEST_CASE("VisualizationInterface integration with brain simulation", "[visualization][integration][brain]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("visualization.enabled", true, nlm::ConfigSource::Runtime);
    config->set("visualization.brain_integration", true, nlm::ConfigSource::Runtime);
    
    nlm::VisualizationInterface visualization;
    visualization.initialize(config);
    
    REQUIRE(visualization.isInitialized());
    
    // Visualization should integrate with brain simulation data
    visualization.renderData("brain_simulation_data", 1.5f);
}

} // namespace test_visualization
