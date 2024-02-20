how to LLVM? NOTE: the below content is copy-pasta from the LLVM repo. If the
commit pointer is ever modified, this documentation may become stale. For now,
it works.

for newer versions of LLVM on macOS, these stopped providing the Intel silicon
version. 15.0.4 is the last version where the Intel macOS binaries are provided.

but, this version ended up being too new for my computer. Alas, so we go with
v14.0.6

Embedding LLVM in your project
==============================

From LLVM 3.5 onwards the CMake build system exports LLVM libraries as
importable CMake targets. This means that clients of LLVM can now reliably use
CMake to develop their own LLVM-based projects against an installed version of
LLVM regardless of how it was built.

Here is a simple example of a CMakeLists.txt file that imports the LLVM libraries
and uses them to build a simple application ``simple-tool``.

.. code-block:: cmake

  cmake_minimum_required(VERSION 3.20.0)
  project(SimpleProject)

  find_package(LLVM REQUIRED CONFIG)

  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

  # Set your project compile flags.
  # E.g. if using the C++ header files
  # you will need to enable C++11 support
  # for your compiler.

  include_directories(${LLVM_INCLUDE_DIRS})
  separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
  add_definitions(${LLVM_DEFINITIONS_LIST})

  # Now build our tools
  add_executable(simple-tool tool.cpp)

  # Find the libraries that correspond to the LLVM components
  # that we wish to use
  llvm_map_components_to_libnames(llvm_libs support core irreader)

  # Link against LLVM libraries
  target_link_libraries(simple-tool ${llvm_libs})

The ``find_package(...)`` directive when used in CONFIG mode (as in the above
example) will look for the ``LLVMConfig.cmake`` file in various locations (see
cmake manual for details).  It creates a ``LLVM_DIR`` cache entry to save the
directory where ``LLVMConfig.cmake`` is found or allows the user to specify the
directory (e.g. by passing ``-DLLVM_DIR=/usr/lib/cmake/llvm`` to
the ``cmake`` command or by setting it directly in ``ccmake`` or ``cmake-gui``).

This file is available in two different locations.

* ``<LLVM_INSTALL_PACKAGE_DIR>/LLVMConfig.cmake`` where
  ``<LLVM_INSTALL_PACKAGE_DIR>`` is the location where LLVM CMake modules are
  installed as part of an installed version of LLVM. This is typically
  ``cmake/llvm/`` within the lib directory. On Linux, this is typically
  ``/usr/lib/cmake/llvm/LLVMConfig.cmake``.

* ``<LLVM_BUILD_ROOT>/lib/cmake/llvm/LLVMConfig.cmake`` where
  ``<LLVM_BUILD_ROOT>`` is the root of the LLVM build tree. **Note: this is only
  available when building LLVM with CMake.**

If LLVM is installed in your operating system's normal installation prefix (e.g.
on Linux this is usually ``/usr/``) ``find_package(LLVM ...)`` will
automatically find LLVM if it is installed correctly. If LLVM is not installed
or you wish to build directly against the LLVM build tree you can use
``LLVM_DIR`` as previously mentioned.

The ``LLVMConfig.cmake`` file sets various useful variables. Notable variables
include

``LLVM_CMAKE_DIR``
  The path to the LLVM CMake directory (i.e. the directory containing
  LLVMConfig.cmake).

``LLVM_DEFINITIONS``
  A list of preprocessor defines that should be used when building against LLVM.

``LLVM_ENABLE_ASSERTIONS``
  This is set to ON if LLVM was built with assertions, otherwise OFF.

``LLVM_ENABLE_EH``
  This is set to ON if LLVM was built with exception handling (EH) enabled,
  otherwise OFF.

``LLVM_ENABLE_RTTI``
  This is set to ON if LLVM was built with run time type information (RTTI),
  otherwise OFF.

``LLVM_INCLUDE_DIRS``
  A list of include paths to directories containing LLVM header files.

``LLVM_PACKAGE_VERSION``
  The LLVM version. This string can be used with CMake conditionals, e.g., ``if
  (${LLVM_PACKAGE_VERSION} VERSION_LESS "3.5")``.

``LLVM_TOOLS_BINARY_DIR``
  The path to the directory containing the LLVM tools (e.g. ``llvm-as``).

Notice that in the above example we link ``simple-tool`` against several LLVM
libraries. The list of libraries is determined by using the
``llvm_map_components_to_libnames()`` CMake function. For a list of available
components look at the output of running ``llvm-config --components``.

Note that for LLVM < 3.5 ``llvm_map_components_to_libraries()`` was
used instead of ``llvm_map_components_to_libnames()``. This is now deprecated
and will be removed in a future version of LLVM.

.. _cmake-out-of-source-pass: