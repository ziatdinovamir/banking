# Banking Library

## Status

[![CI](https://github.com/ziatdinovamir/banking/actions/workflows/ci.yml/badge.svg)](https://github.com/ziatdinovamir/banking/actions/workflows/ci.yml)

## Coverage

[![Coverage Status](https://coveralls.io/repos/github/ziatdinovamir/banking/badge.svg)](https://coveralls.io/github/ziatdinovamir/banking)

## Description

A simple banking library with Account and Transaction classes, demonstrating unit testing with GoogleTest and GMock.

## Build and Test

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest --output-on-failure


