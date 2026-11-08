all:
	@echo "To build tests, use CMake: mkdir build && cd build && cmake .. && make"
	@echo "To run tests: cd build && ./nlm_test"

run_tests:
	@if [ -d build ]; then \
		echo "Running existing tests..."; \
		cd build && ctest --output-on-failure; \
	else \
		echo "Build directory not found. Run 'make all' first.";; \
	fi

