# Project in draft state

# Application for counting unique words in a file

* This application will receive a file name as input and will process using as many threads as given by `std::thread::hardware_concurrency()` and return the amount of unique words contained in it;
* It is assumed that the given input file only contain spaces and ASCII letters.

# How to build

## Requirements
* This repository uses [bazel](https://bazel.build/) as build system manager;
* A C++ compiler supporting C++17 standard is required;
* pthreads library

## Build main application:

`bazel build //app:unique_word_counter`

## Build & Run main application with bazel

`bazel run //app:unique_word_counter -- <file_name>`

Example: `bazel run //app:unique_word_counter -- /home/huge_file.txt`

## Build & Run Tests

`bazel run //uwc/test:uwc_test`

The tests in file_processing_test.cpp are disabled due to the fact a file and an expectation needs to be set there.

# Improvements to be made

* Implement a consistent and reliable error handling mechanism;
* A better folder and namespace organization;
* Implement more tests;
* Refactor UniqueWordsChunkProcessor class in order to make improve its 'unit-testability';
* Define a clean interface for the ChunkProcessor;
* There are a lot of @todo comments in the code, depicting some other improvement to be made.

