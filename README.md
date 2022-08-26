# vkoverhead
`vkoverhead` is a tool for evaluating CPU-based overhead of Vulkan drivers. It was inspired heavily by `drawoverhead` from [piglit](https://gitlab.freedesktop.org/mesa/piglit).

## Features
* extreme granularity for profiling specific scenarios
* simple to use
* CSV output for ease of script parsing
* zero dependencies

## Building
Standard Meson [build procedures](https://mesonbuild.com/Quick-guide.html#compiling-a-meson-project) should apply.

## Cross-platform
This reuses code from Mesa and aims to be just as portable.

## Usage
* `./vkoverhead` to run all test cases
* `./vkoverhead -list` to list all test cases
* `./vkoverhead -test 5` to run test case 5
* `./vkoverhead -submit-only` to run only the submit-related test cases
* `./vkoverhead -nocolor` to suppress color output (for scripting)
* `./vkoverhead -duration 5` to use a minimum duration of 5 seconds (default is 1 second)
* `./vkoverhead -help` for a less helpful summary of available options

## Interpreting Results
[![vkoverhead.png](https://github.com/zmike/vkoverhead/blob/main/vkoverhead.png)](https://github.com/zmike/vkoverhead/blob/main/vkoverhead.png)

Results have 4 columns:
* Test ID
* Test name
* Test result
* Test result comparative performance

Each category of tests has a "base" case. Other cases in the category are compared against it, resulting in a percentage value which represents the overhead that each operation incurs. The goal is to have the percentage value for each case be as close to 100% as possible.
