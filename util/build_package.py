#!/usr/bin/env python3

import sys
import os
import shutil
import zipfile

required_version = (3, 3)
if sys.version_info < required_version:
    print(f"This script requires Python {required_version[0]}.{required_version[1]} or higher.")
    sys.exit(1)

package_dir = "package"
tests_dir = os.path.join(package_dir, "tests")
codegen_dir = os.path.join(tests_dir, "codegen")
grammar_dir = os.path.join(tests_dir, "grammar")
preparse_dir = os.path.join(tests_dir, "preparse")

tests_dir_src = "tests"
codegen_dir_src = os.path.join(tests_dir_src, "codegen")
grammar_dir_src = os.path.join(tests_dir_src, "grammar")
preparse_dir_src = os.path.join(tests_dir_src, "preparse")

backend_dir = os.path.join(tests_dir, "backend")
docs_dir = os.path.join(package_dir, "docs")

if not os.path.exists(package_dir):
    os.mkdir(package_dir)

if not os.path.exists(tests_dir):
    os.mkdir(tests_dir)

if not os.path.exists(codegen_dir):
    os.mkdir(codegen_dir)

if not os.path.exists(grammar_dir):
    os.mkdir(grammar_dir)

if not os.path.exists(preparse_dir):
    os.mkdir(preparse_dir)

if not os.path.exists(docs_dir):
    os.mkdir(docs_dir)

# copy the primary toolchain binary for the user.

macos_binary = os.path.join("bin", "ppl")
windows_binary = os.path.join("bin", "ppl.exe")

on_macos = os.path.exists( macos_binary )
on_windows = os.path.exists( windows_binary )

if on_macos:
    shutil.copy(macos_binary, os.path.join(package_dir, "ppl"))
    shutil.copy("nasm", os.path.join(package_dir, "nasm"))
elif on_windows:
    shutil.copy(windows_binary, os.path.join(package_dir, "ppl.exe"))

shutil.copy("LICENSE.txt", os.path.join(package_dir, "LICENSE.txt"))
shutil.copy("README.txt", os.path.join(package_dir, "README.txt"))

codegen_whitelist = ["program1.c"]
grammar_whitelist = []
preparse_whitelist = []
docs_whitelist = ["phase1.cpp", "phase2.cpp"]

def copy_files(source_dir, destination_dir, file_list):
    for file_path in file_list:
        source_path = os.path.join(source_dir, file_path)
        destination_path = os.path.join(destination_dir, file_path) # keep directory structure.
        shutil.copy(source_path, destination_path)

# Copy the whitelisted files
copy_files(codegen_dir_src, codegen_dir, codegen_whitelist)
copy_files(grammar_dir_src, grammar_dir, grammar_whitelist)
copy_files(preparse_dir_src, preparse_dir, preparse_whitelist)
copy_files( os.path.join("docs", "PPL"), docs_dir, docs_whitelist)

# TODO: the website needs changing.
# shutil.copytree("_site", os.path.join(package_dir, "_site"))

shutil.copytree( os.path.join("backend", "tests"), backend_dir)
shutil.copytree( os.path.join("backend", "pstdlib"), os.path.join(package_dir, "pstdlib"))

# Create a zip file of the package.
with zipfile.ZipFile("package.zip", "w", zipfile.ZIP_DEFLATED) as zipf:
    for root, _, files in os.walk(package_dir):
        for file in files:
            file_path = os.path.join(root, file)
            arcname = os.path.relpath(file_path, package_dir)
            zipf.write(file_path, arcname)

# Clean up by removing the temporary package directory.
shutil.rmtree(package_dir)
