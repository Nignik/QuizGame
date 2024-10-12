set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_COMPILER x86_64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER x86_64-linux-gnu-g++)

set(VCPKG_TARGET_TRIPLET "x64-linux")
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_C_FLAGS "-O2")
set(CMAKE_CXX_FLAGS "-O2")
