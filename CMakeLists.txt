cmake_minimum_required(VERSION 3.21)

cmake_policy(SET CMP0126 NEW)

project(PortableProgrammingLanguage
    VERSION 0.1.0
    DESCRIPTION "A systems programming language"
    LANGUAGES C CXX)

set(ProjectName "PPL")
set(ProjectRoot "${CMAKE_CURRENT_SOURCE_DIR}")
set(UnityBuildSource "${ProjectRoot}/src/compiler.cpp")

# TODO: Support other platforms.
set(LLVM_BASE_DIR "${ProjectRoot}/vendor/clang+llvm-14.0.6-x86_64-apple-darwin" )
set(LLVM_DIR "${LLVM_BASE_DIR}/lib/cmake/llvm")

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
# Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader asmparser asmprinter linker mc codegen target
   X86 AArch64 AMDGPU ARM AVR BPF Hexagon Lanai MSP430
   Mips NVPTX PowerPC RISCV Sparc SystemZ VE WebAssembly
   XCore LTO analysis TableGen Option )

add_executable(${ProjectName} ${UnityBuildSource})

target_include_directories(
    ${ProjectName} PUBLIC
    ${LLVM_INCLUDE_DIRS}
    "${ProjectRoot}/src"
    "${ProjectRoot}/tests"
    "${ProjectRoot}/util"
    "${ProjectRoot}/src"
    "${ProjectRoot}/vendor")

target_compile_definitions(${ProjectName} PUBLIC
    ${LLVM_DEFINITIONS_LIST})

target_compile_features(${ProjectName} PRIVATE cxx_std_17)

target_link_libraries(${ProjectName} ${llvm_libs}
    "${LLVM_BASE_DIR}/lib/liblldMachO.a"
    "${LLVM_BASE_DIR}/lib/liblldCommon.a"
    "libxar.a"
    )

set_target_properties(${ProjectName}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${ProjectRoot}" )

set_target_properties(${ProjectName}
    PROPERTIES XCODE_GENERATE_SCHEME TRUE
    XCODE_SCHEME_WORKING_DIRECTORY "${ProjectRoot}" )