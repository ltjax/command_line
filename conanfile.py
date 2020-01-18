from conans import ConanFile, CMake
from conans.errors import ConanInvalidConfiguration


class CommandLineConan(ConanFile):
    name = "command_line"
    version = "0.1"
    license = "MIT"
    author = "Marius Elvert marius.elvert@googlemail.com"
    url = "https://github.com/ltjax/command_line"
    description = "Simple CLI parser."
    topics = ("cli", "parser")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"
    exports_sources = "source/*", "include/*", "CMakeLists.txt"
    build_requires = "Catch2/2.7.2@catchorg/stable",
    
    def _configured_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".", defs={})
        return cmake

    def configure(self):
        pass

    def requirements(self):
        pass

    def build(self):
        self._configured_cmake().build()
    
    def package(self):
        self._configured_cmake().install()
    
    def package_info(self):
        self.cpp_info.libs = ["command_line"]
