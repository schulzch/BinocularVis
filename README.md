# BinocularVis

BinocularVis is a [data cleansing](https://en.wikipedia.org/wiki/Data_cleansing) tool for low-level time-varying data.

Note: This is a research prototype. The graph editing has been replaced with hard coded graphs (see `mainwindow.cpp`) due to licensing issues. If you're interested in using it, please contact me.

## Building

First you need to install the following tools and external libraries:

- A C++11 compiler, i.e. [clang 2.9+](http://clang.llvm.org/), [GCC 4.7+](https://gcc.gnu.org/), or [Visual Studio 2013+](https://www.visualstudio.com)
- [CMake 3.x](https://cmake.org/)
- [Qt 5.x](http://www.qt.io/)
- [OpenCV 3.x](http://opencv.org/)

Once all tools are installed and external libraries have been added to the `CMAKE_PREFIX_PATH` environment variable:

1. Start `cmake-gui`
2. Click `Browse Source...` and navigate to the project's root
3. Click `Configure` and choose your preferred generator, e.g. `Unix Makefiles`
4. Click `Generate` and wait for the process to finish
5. Start building, e.g. `make`

## Versioning

This project is maintained under the [Semantic Versioning](http://semver.org/) guidelines as much as possible:

- Version number format is `<major>.<minor>.<patch>`
- Breaking backward compatibility bumps the major (resetting minor and patch)
- New additions without breaking backward compatibility bumps the minor (resetting patch)
- Bug fixes and other changes bumps the patch

## License

Dual licensed under the [MIT](https://opensource.org/licenses/MIT) and [GPL-2.0](https://opensource.org/licenses/GPL-2.0) licenses.

Please note, external dependencies may limit your options to `GPL-2.0`, depending on the used feature set.
