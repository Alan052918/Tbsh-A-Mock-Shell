# CS302 Mock Shell

SUSTech CSE 11711310 Junda AI

- [CS302 Mock Shell](#cs302-mock-shell)
  - [Overview](#overview)
  - [Development Environment](#development-environment)
  - [Usage](#usage)
  - [Configuration and Customization (TO BE IMPLEMENTED)](#configuration-and-customization-to-be-implemented)
    - [~~Prompt Information~~](#sprompt-informations)
    - [~~Alias~~](#saliass)
  - [Troubleshooting](#troubleshooting)
    - [Fixed](#fixed)
    - [Unfixed](#unfixed)

## Overview

This is a mock Unix shell application written in C. ~~Users can specify customized configurations in a file named `.tbshrc`, create one if it doesn't exist.~~ All input commands will be logged in a file name `.tbsh_history`, the program will alert on startup when the size of `.tbsh_history` exceeds 1 MB.

## Development Environment

- Operating system: macOS 10.15.3
- Processor: 2.3 GHz Dual-Core Intel Core i5
- Programming Language: C
- C compiler: clang
  - Apple clang version 11.0.0 (clang-1100.0.33.16)
  - Target: x86_64-apple-darwin19.3.0
  - Thread mode: posix
- Visual Studio Code Version: 1.42.1

## Usage

This application supports several UNIX shell commands

- `man`: format and display the on-line manual page
- `ls`: list directory contents
- `cd`: change directory
- `pwd`: print working directory
- `date`: display or set date and time
- `cal`, `ncal`: display a calender and the date of Easter
- `echo`: write arguments to standard output
- `cat`: concatenate and print files
- `touch`: change file access and modification times
- `mkdir`: make directories
- `du`: display disk usage statistics

Enter `exist` to exit program. The program can also be stopped by `Ctrl+C` or `Ctrl+D`.

## Configuration and Customization (TO BE IMPLEMENTED)

~~This application will load configuration file on start up if any. The name of the configuration file should be `.tbshrc`.~~

### ~~Prompt Information~~

~~This application supports displaying either only current folder name or complete working directory name (default) in the prompt message.~~

~~`set fulldir={cwd_display: 0/1}`~~

- ~~`0`: display only current folder name~~
- ~~`1`: display the complete working directory name~~

### ~~Alias~~

~~This application supports customized alias for automating complicated commands. Be cautious not to duplicate alias names with existing commands.~~

~~`alias {alias_name}="{the_complete_command_to_replace}"`~~

## Troubleshooting

### Fixed

- Infinite loop when `Ctrl+D` is pressed
- Abnormal exit when `Enter` is pressed

### Unfixed

- Abnormal exit when directory name contains spaces in `cd`
