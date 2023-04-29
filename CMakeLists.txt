# This file exists to support using XCode for debugging compiler on macOS.
# I suppose it could also be for using Visual Studio on Windows.

cmake_minimum_required(VERSION 3.21)

cmake_policy(SET CMP0126 NEW)

project(PortableProgrammingLanguage
    VERSION 0.1.0
    DESCRIPTION "A systems programming language"
    LANGUAGES C CXX)

set(ProjectName "PPL")
set(ProjectRoot "${CMAKE_CURRENT_SOURCE_DIR}")

set(UnityBuildSource "${ProjectRoot}/util/build.cpp")

add_executable(${ProjectName} ${UnityBuildSource})

target_include_directories(
    ${ProjectName} PUBLIC
    "${ProjectRoot}/src" "${ProjectRoot}/util" "${ProjectRoot}/src" "${ProjectRoot}/backend/src" "${ProjectRoot}/vendor")

target_compile_features(${ProjectName} PRIVATE cxx_std_11)