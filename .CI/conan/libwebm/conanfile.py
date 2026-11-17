from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy, get, replace_in_file
import os

# Copy of the conan-center-index recipe for libwebm, with one change in
# source(): newer MSVC builds (19.44.35229+, VS 2026) refuse to let the nested
# ChildParser class use the private HasTag helper of MasterValueParser, so
# HasTag is made public. Exported in CI before `conan install`.

required_conan_version = ">=1.53.0"


class LibwebmConan(ConanFile):
    name = "libwebm"
    description = "Library for muxing and demuxing WebM media container files"
    topics = ("webm", "container", "demuxing", "muxing", "media", "audio", "video")
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://chromium.googlesource.com/webm/libwebm/"
    license = "BSD-3-Clause"

    settings = "os", "arch", "compiler", "build_type"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_pes_ts": [True, False],
        "with_new_parser_api": [True, False],
    }

    default_options = {
        "shared": False,
        "fPIC": True,
        "with_pes_ts": True,
        "with_new_parser_api": False,
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self, src_folder="src")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)
        header = os.path.join(self.source_folder, "webm_parser", "src", "master_value_parser.h")
        replace_in_file(self, header,
                        "  // Helper struct that will be std::true_type if Tag is in Tags, or",
                        " public:\n  // Helper struct that will be std::true_type if Tag is in Tags, or")
        replace_in_file(self, header,
                        "  template <typename Base, typename F, typename... Tags>\n  class ChildParser : public Base {",
                        " private:\n  template <typename Base, typename F, typename... Tags>\n  class ChildParser : public Base {")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["ENABLE_WEBMTS"] = self.options.with_pes_ts
        tc.variables["ENABLE_WEBM_PARSER"] = self.options.with_new_parser_api
        tc.variables["ENABLE_WEBMINFO"] = False
        tc.variables["ENABLE_SAMPLE_PROGRAMS"] = False
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "LICENSE.TXT", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "webm")
        self.cpp_info.set_property("cmake_target_name", "webm::webm")
        self.cpp_info.set_property("pkg_config_name", "webm")
        self.cpp_info.libs = ["webm"]
        self.cpp_info.includedirs.append("include/webm")

        if self.settings.os in ["Linux", "FreeBSD", "Android"]:
            self.cpp_info.system_libs.append("m")
