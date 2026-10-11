#!/usr/bin/env python3
# Conan Package Configuration for NLM
# This file defines the NLM package for Conan package manager
# Provides dependency management and cross-platform builds

from conans import ConanFile, CMake, tools
import os

class NLMConan(ConanFile):
    name = "nlm"
    version = "0.1.0"
    license = "MIT"
    author = "NLM Team <team@nlm.org>"
    url = "https://github.com/nlm/nlm"
    description = "Neural Learning Machine - Advanced brain simulation framework for C++"
    topics = ("ai", "brain", "simulation", "neuroscience", "neural-network")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tests": [True, False],
        "build_python_bindings": [True, False],
        "build_cli": [True, False],
        "build_demos": [True, False],
        "minimal_build": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_tests": False,
        "build_python_bindings": False,
        "build_cli": False,
        "build_demos": False,
        "minimal_build": False,
    }
    
    exports_sources = "conanfile.py", "CMakeLists.txt", "CMakePresets.json", "**/*.cmake"
    generators = "cmake"
    
    requires = [
        "nlohmann_json/3.11.2",
        "yaml-cpp/0.8.0",
        "pybind11/2.11.1",
    ]
    
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def configure(self):
        if self.options.shared:
            del self.options.fPIC
    
    def build_requirements(self):
        if self.settings.os == "Windows":
            self.build_requires("cmake/3.25.0")
        if self.settings.os == "Linux":
            self.build_requires("ninja/1.11.1")
    
    def source(self):
        # Source code is already available in the repository
        pass
    
    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_BUILD_TYPE"] = self.settings.build_type
        cmake.definitions["NLM_BUILD_TESTS"] = self.options.build_tests
        cmake.definitions["NLM_BUILD_PYTHON_BINDINGS"] = self.options.build_python_bindings
        cmake.definitions["NLM_BUILD_CLI"] = self.options.build_cli
        cmake.definitions["NLM_BUILD_DEMOS"] = self.options.build_demos
        cmake.definitions["NLM_BUILD_MINIMAL"] = self.options.minimal_build
        cmake.definitions["NLM_ENABLE_CONAN"] = False
        cmake.definitions["NLM_ENABLE_VCPKG"] = False
        cmake.definitions["NLM_ENABLE_CPACK"] = False
        
        build_dir = "build"
        os.makedirs(build_dir, exist_ok=True)
        
        return cmake, build_dir
    
    def build(self):
        cmake, build_dir = self._configure_cmake()
        cmake.configure()
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
        
        # Copy license files
        self.copy("LICENSE", dst="licenses", src=self.source_folder)
        
        # Remove unnecessary files from package
        tools.rmdir(os.path.join(self.package_folder, "lib", "cmake"))
        tools.rmdir(os.path.join(self.package_folder, "CMakeFiles"))
    
    def package_info(self):
        self.cpp_info.names["cmake_find_package"] = "NLM"
        self.cpp_info.names["cmake_find_package_multi"] = "NLM"
        self.cpp_info.components["core"].names["cmake_find_package"] = "NLM"
        self.cpp_info.components["core"].names["cmake_find_package_multi"] = "NLM"
        self.cpp_info.components["core"].libs = ["nlm_core"]
        self.cpp_info.components["core"].requires = ["nlohmann_json::nlohmann_json", "yaml-cpp::yaml-cpp"]
        
        self.cpp_info.components["agent"].libs = ["nlm_agent"]
        self.cpp_info.components["agent"].requires = ["core"]
        
        self.cpp_info.components["world"].libs = ["nlm_world"]
        self.cpp_info.components["world"].requires = ["core"]
        
        self.cpp_info.components["phase3"].libs = ["nlm_phase3"]
        self.cpp_info.components["phase3"].requires = ["core", "agent", "world"]
        
        self.cpp_info.components["phase4"].libs = ["nlm_phase4"]
        self.cpp_info.components["phase4"].requires = ["core", "agent", "world", "phase3"]
        
        self.cpp_info.components["cli"].libs = ["nlm_advanced_cli"]
        self.cpp_info.components["cli"].requires = ["core"]
        
        self.cpp_info.components["python_bindings"].names["python"] = "pynlm"
        self.cpp_info.components["python_bindings"].libs = ["pynlm"]
        self.cpp_info.components["python_bindings"].requires = ["core", "agent", "world", "phase3"]
        
        # Set environment paths
        bin_path = os.path.join(self.package_folder, "bin")
        self.output.info(f"Append PATH={bin_path} to your environment")
        
        include_path = os.path.join(self.package_folder, "include")
        self.output.info(f"Append {include_path} to your include path")
        
        lib_path = os.path.join(self.package_folder, "lib")
        self.output.info(f"Append {lib_path} to your library path")
    
    def package_id(self):
        # Calculate package ID based on options
        if self.options.shared:
            self.info.requires["nlohmann_json"].build_modules_mode = "global"
            self.info.requires["yaml-cpp"].build_modules_mode = "global"
        if self.options.build_tests:
            self.info.requires["nlohmann_json"].build_type = "dynamic"
        if self.options.build_python_bindings:
            self.info.requires["pybind11"].build_modules_mode = "global"
        if self.options.minimal_build:
            # Use simpler requirements for minimal build
            self.info.requires["nlohmann_json"].components = ["nlohmann_json"]
            self.info.requires["yaml-cpp"].components = ["yaml-cpp"]
            self.info.requires["pybind11"].components = ["pybind11"]
    
    def validate(self):
        # Validate configuration
        if self.settings.compiler == "gcc" and tools.Version(self.settings.compiler.version) < "9":
            raise ConanInvalidConfiguration("NLM requires GCC >= 9")
        if self.settings.compiler == "clang" and tools.Version(self.settings.compiler.version) < "10":
            raise ConanInvalidConfiguration("NLM requires Clang >= 10")
        if self.settings.compiler == "msvc" and tools.Version(self.settings.compiler.version) < "19.2":
            raise ConanInvalidConfiguration("NLM requires MSVC >= 19.2")
        
        # Validate Python bindings requirement
        if self.options.build_python_bindings and "pybind11" not in self.requires:
            raise ConanInvalidConfiguration("pybind11 is required for Python bindings")
        
        # Validate dependency compatibility
        if self.options.build_tests and self.options.minimal_build:
            raise ConanInvalidConfiguration("Tests cannot be built in minimal build mode")
    
    def system_requirements(self):
        if self.settings.os == "Linux":
            self.run("sudo apt-get update")
            self.run("sudo apt-get install -y libboost-system-dev libboost-filesystem-dev")
        elif self.settings.os == "Windows":
            # Windows dependencies are handled by vcpkg
            pass
        elif self.settings.os == "Macos":
            self.run("brew install boost")

# Additional helper functions for CI/CD

def create_conanfile_skeleton(project_name, version, dependencies):
    """Helper function to create a skeleton conanfile.py for a project"""
    conanfile = f'''#!/usr/bin/env python3
from conans import ConanFile, CMake, tools
import os

class {project_name.title().replace(" ", "")}Conan(ConanFile):
    name = "{project_name}"
    version = "{version}"
    license = "MIT"
    author = "{project_name} Team"
    url = "https://github.com/{project_name.lower()}/{project_name.lower()}"
    description = "{project_name} - Description"
    topics = ("topic1", "topic2")
    settings = "os", "compiler", "build_type", "arch"
    options = {{"shared": [True, False]}}
    default_options = {{"shared": False}}
    
    requires = {dependencies}
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def package_info(self):
        self.cpp_info.libs = ["{project_name.lower()}"]

# Usage:
# conan create . {project_name}/{version}@user/channel
'''
    return conanfile

# License and copyright information
__license__ = "MIT"
__copyright__ = "Copyright (c) 2026 NLM Team"

# Export useful functions
__all__ = ["NLMConan", "create_conanfile_skeleton"]
