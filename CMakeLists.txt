cmake_minimum_required(VERSION 3.27)
project(NFSMWSecondPerson)

SET(CMAKE_SYSTEM_NAME Windows)

SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
SET(CMAKE_RANLIB i686-w64-mingw32-ranlib)

SET(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -masm=intel -s -static")

add_compile_definitions(IMGUI_USER_CONFIG="../../nya_imconfig.h" NYA_BACKEND_DX9 NYA_NO_FONT NYA_COMMON_NO_D3D)
add_library(NFSMWSecondPerson SHARED ../nya-common/nya_commontimer.cpp main.cpp)
target_include_directories(NFSMWSecondPerson PUBLIC ../nya-common)
target_include_directories(NFSMWSecondPerson PUBLIC ../nya-common/3rdparty)
target_include_directories(NFSMWSecondPerson PUBLIC ../nya-common-nfsmw)
target_include_directories(NFSMWSecondPerson PUBLIC ../CwoeeMenuLib/include)
target_link_options(NFSMWSecondPerson PRIVATE "-Wl,--exclude-all-symbols")

set_target_properties(NFSMWSecondPerson PROPERTIES PREFIX "")
set_target_properties(NFSMWSecondPerson PROPERTIES SUFFIX "_gcp.asi")