# The practice part of a webinar with an example of how to use Redis API.

The practice part of a webinar with an example of how to use redis-plus-plus (https://github.com/sewenew/redis-plus-plus).

## Build

It uses [Conan package manager](https://conan.io/) to install dependencies.

### Install dependencies

```bash
$ mkdir build && cd build
$ conan install ..
$ cmake ..
```

On error "BinaryMissing" try:
```bash
$ conan install .. --build=missing
```

### Build executable

Run inside "build" directory:
```bash
$ cmake --build .
```

Look for your executable in "build/bin"

## Usage

```bash
$ redis_demo
```
