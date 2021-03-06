cmake_minimum_required ( VERSION 2.8.8 )
project ( moaiandroid )

get_filename_component(MY_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake-modules" ABSOLUTE)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${MY_SOURCE}")
set(BUILD_ANDROID TRUE)
set(MOAI_ROOT ${MOAI_SDK_HOME} )
set(HOST_ROOT ${MOAI_SDK_HOME}/src )
# Setup the "MOAI_ROOT" variable
get_filename_component(MOAI_ROOT "${MOAI_ROOT}" ABSOLUTE)
get_filename_component(HOST_ROOT "${HOST_ROOT}" ABSOLUTE)

set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -DANDROID_NDK -DDISABLE_IMPORTGL" )
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -DANDROID_NDK -DDISABLE_IMPORTGL" )

include(platform-android)
include(platform-common)
include(module-setup)
include(libmoai-setup)

add_subdirectory(../host-modules host-modules)



set ( INCLUDES_MOAIANDROID
  "${MOAI_ROOT}/src/"
  "${CMAKE_BINARY_DIR}/libmoai/config"
  "${MOAI_ROOT}/3rdparty"
 )

include_directories (
    ${INCLUDES_MOAIANDROID}
)

LINK_DIRECTORIES("${LUA_LIB_DIR}")

add_library ( moai SHARED
    "${MOAI_ROOT}/android-ndk/jni/src/jni.cpp"
    "${MOAI_ROOT}/src/moai-android/moai.cpp"
)


target_link_libraries ( moai
   host-modules
   moai-sim
   moai-util
   moai-android
   moai-core
   zlcore
   log
   GLESv1_CM
   GLESv2
)
