# NLM Makefile
# Build and test script for NLM neural simulation framework

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -O2
LDFLAGS =

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

# Targets
TARGETS = nlm nlm_enhanced nlm_phase3_demo nlm_phase4_demo
TEST_TARGETS = test_types test_synapse test_stdp test_random test_neuron test_main test_config test_clock test_brain

# Default target
all: $(TARGETS) $(TEST_TARGETS)

# Build NLM executables
nlm: $(BUILD_DIR)/nlm
nlm_enhanced: $(BUILD_DIR)/nlm_enhanced
nlm_phase3_demo: $(BUILD_DIR)/nlm_phase3_demo
nlm_phase4_demo: $(BUILD_DIR)/nlm_phase4_demo

# Build test executables
$(BUILD_DIR)/test_types: $(TEST_DIR)/test_types.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/test_synapse: $(TEST_DIR)/test_synapse.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/test_stdp: $(TEST_DIR)/test_stdp.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/test_random: $(TEST_DIR)/test_random.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/test_neuron: $(TEST_DIR)/test_neuron.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/test_main: $(TEST_DIR)/test_main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/test_config: $(TEST_DIR)/test_config.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/test_clock: $(TEST_DIR)/test_clock.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/test_brain: $(TEST_DIR)/test_brain.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

# Build main executables using CMake (simplified)
$(BUILD_DIR)/nlm: $(SRC_DIR)/main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/nlm_enhanced: $(SRC_DIR)/enhanced_cli.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/nlm_phase3_demo: $(SRC_DIR)/experiments/Phase3Demo.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

$(BUILD_DIR)/nlm_phase4_demo: $(SRC_DIR)/experiments/Phase4Demo.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) $< -o $@

# Run tests
test: $(TEST_TARGETS)
	@echo "Running tests..."
	@for test in $(TEST_TARGETS); do \
		if [ -f $(BUILD_DIR)/$$test ]; then \
			echo "Running $$test..."; \
			./$(BUILD_DIR)/$$test; \
		else \
			echo "$$test not found, skipping"; \
		fi; \
	done

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Run examples
run-demo:
	@echo "Running NLM demo..."
	@$(BUILD_DIR)/nlm --mode demo --steps 100 --verbose

run-enhanced:
	@echo "Running NLM enhanced CLI..."
	@$(BUILD_DIR)/nlm_enhanced --mode demo --steps 100 --verbose

run-benchmark:
	@echo "Running NLM benchmark..."
	@$(BUILD_DIR)/nlm --mode benchmark --steps 500

# Help
help:
	@echo "Available targets:"
	@echo "  all        - Build all executables and tests (default)"
	@echo "  test       - Run all tests"
	@echo "  clean      - Remove build artifacts"
	@echo "  run-demo   - Run demo mode"
	@echo "  run-enhanced - Run enhanced CLI"
	@echo "  run-benchmark - Run benchmark mode"
	@echo "  help       - Show this help message"

.PHONY: all test clean run-demo run-enhanced run-benchmark help