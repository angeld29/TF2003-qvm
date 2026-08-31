# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR i686)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER x86_64-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER x86_64-linux-gnu-g++)
SET(CMAKE_C_FLAGS "-m32")
SET(CMAKE_CXX_FLAGS "-m32")
SET(CMAKE_EXE_LINKER_FLAGS "-m32")
SET(CMAKE_SHARED_LINKER_FLAGS "-m32")

# here is the target environment located
#SET(CMAKE_FIND_ROOT_PATH  /usr/i586-mingw32msvc /home/alex/mingw-install )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
