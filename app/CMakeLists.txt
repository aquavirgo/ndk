# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#add_library( # Sets the name of the library.
#             native-lib
#
#             # Sets the library as a shared library.
#           SHARED
#
#            # Provides a relative path to your source file(s).
#            src/main/cpp/native-lib.cpp
#
#           )

add_library( # Sets the name of the library.
                         Store

                         # Sets the library as a shared library.
                         SHARED

                         # Provides a relative path to your source file(s).
                        src/main/jni/Store.cpp
                        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
find_library( # Defines the name of the path variable that stores the
              # location of the NDK library.
              log-lib

              # Specifies the name of the NDK library that
              # CMake needs to locate.
              log )


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

# Links your native library against one or more other native libraries.
target_link_libraries( # Specifies the target library.
                      Store

                       # Links the log library to the target library.
                       ${log-lib} )