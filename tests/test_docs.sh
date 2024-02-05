#!/bin/bash
#
grep '^#' $0
#
# What are the requirements of the testing system for PPL??
# ===============================================================
#
# - the tests are useful because they provide information that the software
# does not meet some requirement. this is to the end of eventually fixing the
# software to meet that requirement.
#
# how do we enable that the tests are fulfilling their value add?
#
# REQUIREMENT 1; the testing system is an easy, fun, and scalable tool:
# - running a test is an easy to understand operation.
# - running a test is very fast. depends on the test but the simplest possible
#   test it should take under 100ms after I hit enter on the keyboard.
# - the rules of the testing system are desgined just enough, but not too much,
#   to enable emergent patterns that were not forseeable from the rules.
# - the system should never frustrate the user (i.e. do something that the user
#   didn't ask for).
# - should be easy to write new tests.
#
# REQUIREMENT 2; the testing system is unambiguous:
# - it should be extremely clear what was tested and the result of the test;
#   most interpretations of the report will be correct ones.
# - the rate at which someone could read the test results is very quick. no
#   matter what rate they read, they will never interpret incorrect
#   information.
#
# REQUIREMENT 3; testing should be reproducible:
# - tests should be reproducible to the end that a more detailed testing
#   process afterwards could be done; e.g. running the test but keeping a
#   detailed watch over the granular steps (using a debugger).
#
# ===============================================================
#
#
# Design
# ===============================================================
#
# Overview
#
# simplest possible thing: a single folder for each platform.
# the compiler does not support cross-compilation, so "each platform"
# means both the host machine and the target executable format.
#
# within that folder there exists a set of .sh files; each file is a test.
#
# easy to understand the operation & understand what is tested.
# 1. very clear file name.
# 2. first line it prints is a short description of what is being tested.
# 3. run the test with -h option to just print the help info, which includes a
#    longer description of what is being tested.
# 4. any specifically compiled testing artefact must be very clear about the
#    functions being tested or what the custom test case is.
# idea: test report should include the timestamp for each build artefact so
#       that it's clear what build is getting tested. but not only this, what
#       commit of Git that we're testing. and if the code is modified w.r.t.
#       that commit.
#
#   - for black box tests & for the purpose of understanding what I just did,
#   the report should include the steps that I took (what command line ops did
#   we do?). Make it so I can copy+paste.
#
# running a test is fast.
# - not gonna have any upfront design here. just going to implement system
#   and check if fast enough.
#
# emergent ruleset.
# - a single test has the scope to:
#   - run another test or set of tests.
#   - execute PPL artefacts with any input whatsoever.
#   - have input to alter its behaviour.
# - the test author has the right to maintain a test version of the build
#   artefacts (via a macro) to enable triggerable "test paths", or to compile
#   an artefact specifically for testing (consider e.g. testing individual
#   functions via something like catch2).
#
# not frustrate the user.
# 
# - in order to test, there is a prerequisite step of building.
#
# Q: should running a test check for build prereq or assume?
# A: yes, running a test should check for prerequisites. this is
# to avoid false positives (where system falsely reports a failing test).
#
# Q: should running a test perform the build prereq automatically
# or simply inform the user that they need to build first?
# A: do not do automatically because some users did not ask for that. 
#    the simplest possible thing is to report they need to build first.
#    we could be nice to the user and prompt for a simple Y/N if they
#    would like to build.
#
# easy to write new tests.
# - writing a black box test is already easy; just execute the build artefact,
#   varying the parameters.
# - writing a more integrated test means maintaing clean codebase-internal
#   abstractions.
#
# understand the result of the test & fast to read.
# - use clear red or green for pass or failure.
# - if lots of stuff just happened, provide a summary of exactly how many
#   failed and passed, plus a list of which failed, and provide a listing of
#   detailed failures.
#
# testing is reproducible.
# - require clear determinism. to this end, tests report the details of the
#   machine that was just tested on. this include the operating system version
#   and/or versions of any relevant software dependencies, plus a listing of
#   the detailed hardware.
# - report on the pseudorandom seeds that were used.
#
# stepping into debugger.
# - for and test.sh, make it so that I can copy+paste a single command to
#   literally kickstart the debugging session for that test.
#
#
