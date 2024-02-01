#!/usr/bin/env python3

import sys
import os
import shutil
import zipfile

# TODO: need to ensure that we clean the pstdlib directory before copy because it might contain
# .o files.

required_version = (3, 3)
if sys.version_info < required_version:
    print(f"This script requires Python {required_version[0]}.{required_version[1]} or higher.")
    sys.exit(1)

package_dir = "package"

if not os.path.exists(package_dir):
    os.mkdir(package_dir)
else:
    # clean the old directory.
    shutil.rmtree(package_dir)
    os.mkdir(package_dir)

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

shutil.copy("LICENSE.TXT", os.path.join(package_dir, "LICENSE.TXT"))
shutil.copy("README.TXT", os.path.join(package_dir, "README.TXT"))
shutil.copy(os.path.join("util", "RUN.bat"), os.path.join(package_dir, "RUN.bat"))

codegen_whitelist = ["program1.c"]
grammar_whitelist = []
preparse_whitelist = []

def copy_files(source_dir, destination_dir, file_list):
    for file_path in file_list:
        source_path = os.path.join(source_dir, file_path)
        destination_path = os.path.join(destination_dir, file_path) # keep directory structure.
        shutil.copy(source_path, destination_path)

# Copy the whitelisted files

# TODO: the website needs changing.
# shutil.copytree("_site", os.path.join(package_dir, "_site"))

shutil.copytree( "pstdlib", os.path.join(package_dir, "pstdlib") )
shutil.copytree( "examples", os.path.join(package_dir, "examples") )

# Create a zip file of the package.
with zipfile.ZipFile("package.zip", "w", zipfile.ZIP_DEFLATED) as zipf:
    for root, _, files in os.walk(package_dir):
        for file in files:
            file_path = os.path.join(root, file)
            arcname = os.path.relpath(file_path, package_dir)
            zipf.write(file_path, arcname)

# Clean up by removing the temporary package directory.
shutil.rmtree(package_dir)
