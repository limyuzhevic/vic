from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.build import cross_building
import os

class NlmConan(ConanFile):
    name = "nlm"
    version = "0.1.0"
    license = "MIT"
    author = "NLM Authors <nlm-authors@example.com>"
    url = "https://github.com/nlm-project/nlm"
    description = "Neural Learning Machine - A comprehensive neural simulation framework"
    topics = ("neural", "simulation", "spiking", "brain", "learning", "ai", "neuroscience")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tests": [True, False],
        "build_python_bindings": [True, False],
        "build_demos": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_tests": False,
        "build_python_bindings": True,
        "build_demos": False,
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("pybind11/2.11.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("gtest/1.13.0")
        self.requires("catch2/2.13.10")

        if self.options.build_tests:
            self.requires("gtest/1.13.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.22.0")

    def source(self):
        self.run(f"git clone https://github.com/nlm-project/nlm.git .")
        self.run(f"git checkout {self.version}")

    def generate(self):
        passes = False
        if cross_building(self):
            passes = True

        if passes:
            self.output.info("Cross-building detected, skipping CMake integration")

    def build(self):
        cmake = self.cmake_layout
        cmake.configure(
            cache_variables={
                "NLM_BUILD_PYTHON_BINDINGS": self.options.build_python_bindings,
                "NLM_BUILD_TESTS": self.options.build_tests,
                "NLM_BUILD_DEMOS": self.options.build_demos,
                "CMAKE_POSITION_INDEPENDENT_CODE": self.options.get_safe("fPIC", True),
            }
        )
        cmake.build()

    def package(self):
        cmake = self.cmake_layout
        cmake.install()

        self.copy("LICENSE", dst="licenses", src=self.source_folder)
        self.copy("README.md", dst="licenses", src=self.source_folder)

    def package_info(self):
        self.cpp_info.libs = ["nlm_core", "nlm_agent", "nlm_world"]

    def package_id(self):
        self.info.requires["pybind11"].build_modules = False