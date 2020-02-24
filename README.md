# Tbsh: A Mock Shell

SUSTech CSE 11711310 Junda AI

- [Tbsh: A Mock Shell](#tbsh-a-mock-shell)
  - [Overview](#overview)
  - [Development Environment](#development-environment)
  - [Usage](#usage)
  - [Configuration and Customization](#configuration-and-customization)
    - [Prompt Information](#prompt-information)
    - [Alias](#saliass-to-be-implemented)
  - [Troubleshooting](#troubleshooting)
    - [Fixed](#fixed)
    - [Unfixed](#unfixed)

## Overview

This is a mock Unix shell application written in C. Users can specify customized configurations in a file named `.tbshrc`, create one if it doesn't exist. All input commands will be logged in a file name `.tbsh_history`, the program will alert on startup when the size of `.tbsh_history` exceeds 2 KB.

## Development Environment

- Operating system: macOS 10.15.3
- Processor: 2.3 GHz Dual-Core Intel Core i5
- Programming Language: C
- C compiler: clang
  - Apple clang version 11.0.0 (clang-1100.0.33.16)
  - Target: x86_64-apple-darwin19.3.0
  - Thread mode: posix

## Usage

This application is not compatible with the Windows operating system.

It calls `execvp()` declared in `unistd.h` to carry out specific Unix commands. `unistd.h` is a header file that provides access to the POSIX operating system API.

Enter `exit` or `Ctrl+D` to exit program. The program can also be stopped by `Ctrl+C`.

**Warning**: caution [unfixed issues](#unfixed).

## Configuration and Customization

This application will load configuration file on start up if any. The name of the configuration file should be `.tbshrc`.

### Prompt Information

This application supports displaying either only current folder name or complete working directory name (default) in the prompt message.

`fulldir={cwd_display: 0/1}`

- `0`: display only current folder name
- `1`: display the complete working directory name (default)

### Alias (TO BE IMPLEMENTED)

This application supports customized alias for automating complicated commands. Be cautious not to duplicate alias names with existing commands.

`alias {alias_name}="{the_complete_command_to_replace}"`

## Troubleshooting

### Fixed

- Infinite loop when `Ctrl+D` is pressed
- Abnormal exit when `Enter` is pressed
- Abnormal exit due to memory allocation failure

### Unfixed

- Abnormal exit when directory name contains spaces in `cd`
- When this application is running a child process, pressing `Ctrl+C` will cause the application to exit rather than stopping the running child process and returning to the application cli
