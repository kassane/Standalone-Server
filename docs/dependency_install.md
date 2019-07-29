# Dependency installation instructions

Installation instructions for the dependencies of the example files on a selection of platforms.

Default build with Boost.Asio is assumed. Turn on CMake option `USE_STANDALONE_ASIO` to instead use standalone Asio.

## Debian based distributions

```sh
sudo apt-get install libssl-dev libboost-filesystem-dev libboost-thread-dev
```

## Arch Linux based distributions

```sh
sudo pacman -S boost
```

## MacOS

```sh
brew install openssl boost
```
