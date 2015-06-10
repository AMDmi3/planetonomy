# Planetonomy

[![Build Status](https://travis-ci.org/AMDmi3/planetonomy.svg?branch=master)](https://travis-ci.org/AMDmi3/planetonomy)

Old-school platformer/exploration game with CGA graphics

## Building

Dependencies:

* cmake
* SDL2
* SDL2_image

The project also uses libSDL2pp, C++11 bindings library for SDL2.
It's included into git repository as a submodule, so if you've
obtained source through git, don't forget to run ```git submodule
init && git submodule update```.

To build the project, run:

```
cmake . && make
```

## Author

* [Dmitry Marakasov](https://github.com/AMDmi3) <amdmi3@amdmi3.ru>

## License

GPLv3 or later, see COPYING

The project also bundles third party software under its own licenses:

* extlibs/libSDL2pp (C++11 SDL2 wrapper library) - zlib license
