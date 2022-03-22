# wxFormBuilder SDK

The wxFormBuilder SDK allows building custom plugins to extend the supported widgets of wxFormBuilder.
Since wxFormBuilder and plugins use C++, it is important that both are build using the same compiler and settings.

For easier development of plugins, the SDK uses wxFormBuilder as subproject to be able to execute the plugin directly
from the plugin project. The recommended setup is to integrate the wxFormBuilder project as git submodule of the
plugin repository. This directory contains an example `CMakeLists.txt` file that can be used as top level CMake file
of a plugin project.
