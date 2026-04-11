# @authors S.V. Paulauskas, K. Smith, and C. Thronsberry

cmake_minimum_required(VERSION 3.5)

project(PAASS C CXX Fortran)

#Set the C++ standard to c++11.
set(CMAKE_CXX_STANDARD 11)

#Use rpath on Mac OS
set(CMAKE_MACOSX_RPATH TRUE)

if (CMAKE_COMPILER_IS_GNUCXX)
    #Make compiler messages nice with colored tags.
    if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.9")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto")
    endif ()

    execute_process(
    COMMAND uname -r
    OUTPUT_VARIABLE KERNEL_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Extract the major and minor versions
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" KERNEL_VERSION_SHORT "${KERNEL_VERSION}")
    string(REGEX MATCH "^([0-9]+)" KERNEL_VERSION_MAJOR "${KERNEL_VERSION_SHORT}")

    # Print out the detected kernel version
    message(STATUS "Detected Linux kernel version: ${KERNEL_VERSION}")

    # Now you can use these variables for conditional checks
    if(KERNEL_VERSION_MAJOR GREATER 3 )
        set(PAASS_NEW_KERNEL TRUE)
    else()
        set(PAASS_NEW_KERNEL FALSE)
    endif()


    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -fPIC")
endif ()

#if user does not specify prefix we assign it to the install directory
#@TODO I do not like the fact that it makes the install directory before I'm
#ready for it. There are cases where I do not want to actually install just
#build. This configuration adds additional junk to my directory that I may
#not actually want or need.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(STATUS "Install Prefix not specified.")
    file(MAKE_DIRECTORY install)
    get_filename_component(INSTALL_DIR ${CMAKE_BINARY_DIR}/../install REALPATH)
    set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR} CACHE PATH "Install Prefix" FORCE)
endif ()
message(STATUS "Installing to ${CMAKE_INSTALL_PREFIX}")

#Define the default build type to be Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
            "Build type, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message(STATUS "Build type not defined, using default: ${CMAKE_BUILD_TYPE}")
endif (NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#Add additional CXX flags if we use the Debug option
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif (CMAKE_BUILD_TYPE MATCHES "Debug")

#------------------------------------------------------------------------------
#We are going to include this additional module here since it could be useful
#when setting all of the following options.
include(CMakeDependentOption)

#Install options
option(PAASS_BUILD_ACQ "Build and install Acquisition software" OFF)
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
  option(PAASS_BUILD_SCAN_UTILITIES "Build and install the Analysis Utilities" OFF)
  option(PAASS_BUILD_SHARED_LIBS "Install only scan libraries" OFF)
else()
  option(PAASS_BUILD_SCAN_UTILITIES "Build and install the Analysis Utilities" ON)
  option(PAASS_BUILD_SHARED_LIBS "Install only scan libraries" ON)
endif()
option(PAASS_BUILD_TESTS "Builds programs designed to test the package. Including UnitTest++ test." OFF)
option(PAASS_BUILD_UTKSCAN "Build utkscan" OFF)
option(PAASS_USE_DAMM "Use DAMM for MCA" ON)
option(PAASS_USE_NCURSES "Use ncurses for terminal" ON)
option(PAASS_USE_ROOT "Use ROOT (Currently REQUIRED!!)" ON)

mark_as_advanced(PAASS_USE_NCURSES)
mark_as_advanced(PAASS_USE_DAMM)
mark_as_advanced(PAASS_USE_ROOT)
mark_as_advanced(PAASS_BUILD_SCAN_UTILITIES)
mark_as_advanced(PAASS_BUILD_SHARED_LIBS)
mark_as_advanced(PAASS_BUILD_TESTS)

option(PAASS_EXPORT_COMPILE_COMMANDS "CMAKE_EXPORT_COMPILE_COMMANDS WRAPPER" ON)
mark_as_advanced(PAASS_EXPORT_COMPILE_COMMANDS)

#Generate the compile_commands.json file which is used by VSCode (among others) to generate the clang autocomplete
#Needs the PAASS_EXPORT_COMPILE_COMMANDS wrapper because of the "FORCE" which is required to even set the var from CMakeList.txt,
#while maintaining the ability to toggle it on/off without wiping the build dir
if(PAASS_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Generates compile_commands.json, needed for some IDEs among other things" FORCE)
else ()
    set(CMAKE_EXPORT_COMPILE_COMMANDS OFF CACHE BOOL "Generates compile_commands.json, needed for some IDEs among other things" FORCE)
endif (PAASS_EXPORT_COMPILE_COMMANDS)

#------------------------------------------------------------------------------

#Definitions with compiler options
#The MCA will write DAMM histograms as output
if (PAASS_USE_DAMM)
    add_definitions("-D USE_DAMM")
endif (PAASS_USE_DAMM)

#------------------------------------------------------------------------------
#Find packages needed for the software.
#Load additional find_package scripts.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Cmake/modules/")

#Find thread library for poll2 and scanLib
find_package(Threads REQUIRED)

#Find curses library used for pretty much everything at this point.
if (PAASS_USE_NCURSES)
    find_package(Curses REQUIRED)
endif (PAASS_USE_NCURSES)


if (CURSES_FOUND)
    add_definitions("-D USE_NCURSES")
    mark_as_advanced(FORCE CURSES_HAVE_CURSES_H CURSES_CURSES_H_PATH CURSES_FORM_LIBRARY)
else ()
    message(STATUS "Curses unavailable, basic terminal will be used.")
    set(PAASS_USE_NCURSES OFF)
endif (CURSES_FOUND)

#Find the UnitTest++ Package. This package can be obtained from
#https://github.com/unittest-cpp/unittest-cpp.git
if (PAASS_BUILD_TESTS)
    find_package(UnitTest++ REQUIRED)
    link_directories(${UNITTEST++_LIBRARY_DIR})
    include_directories(${UNITTEST++_INCLUDE_DIR})
endif (PAASS_BUILD_TESTS)

if (PAASS_BUILD_ACQ AND DEFINED ENV{PLX_SDK_DIR})
    set(PAASS_ACQ_API "Auto" CACHE STRING "Acquisition API: Auto, PixieSDK-Legacy, or XIA_API")
    set_property(CACHE PAASS_ACQ_API PROPERTY STRINGS Auto PixieSDK-Legacy XIA_API)
    message(STATUS "[PAASS::ACQ] PAASS Requested API: ${PAASS_ACQ_API}")
    option(PAASS_BUILD_SETUP "Include the older setup programs in installation" OFF)



    # Find all the things, and decide which API to use based on the override or the AUTO mode
    if (PAASS_ACQ_API STREQUAL "PixieSDK-Legacy")
        if (DEFINED ENV{XIA_PIXIE_SDK})
            set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{XIA_PIXIE_SDK}/cmake/modules")
            find_package(BroadcomAPI)
            find_package(PixieSDK)

            if (BroadcomAPI_FOUND AND PixieSDK_FOUND)
                set(PAASS_ACQ_API_RESOLVED "PixieSDK-Legacy")
            else()
                message(FATAL_ERROR "[PAASS::ACQ] PAASS_ACQ_API=PixieSDK-Legacy requested, but BroadcomAPI/PixieSDK were not found.")
            endif()
        else()
            message(FATAL_ERROR "[PAASS::ACQ] PAASS_ACQ_API=PixieSDK-Legacy requested, but XIA_PIXIE_SDK is not set.")
        endif()

    elseif (PAASS_ACQ_API STREQUAL "XIA_API")
        if (DEFINED ENV{XIA_FIRMWARE_DIR})
            find_package(PLX)
            find_package(XIA)

            if (PLX_FOUND AND XIA_FOUND)
                set(PAASS_ACQ_API_RESOLVED "XIA_API")
            else()
                message(FATAL_ERROR "[PAASS::ACQ] PAASS_ACQ_API=XIA_API requested, but PLX/XIA were not found.")
            endif()
        else()
            message(FATAL_ERROR "[PAASS::ACQ] PAASS_ACQ_API=XIA_API requested, but XIA_FIRMWARE_DIR is not set.")
        endif()

    elseif (PAASS_ACQ_API STREQUAL "Auto")
        if (PAASS_NEW_KERNEL)
            message(STATUS "[PAASS::ACQ] New kernel detected; trying PixieSDK-Legacy first")

            if (DEFINED ENV{XIA_PIXIE_SDK})
                set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{XIA_PIXIE_SDK}/cmake/modules")
                find_package(BroadcomAPI REQUIRED)
                find_package(PixieSDK REQUIRED)
            endif()

            if (BroadcomAPI_FOUND AND PixieSDK_FOUND)
                set(PAASS_ACQ_API_RESOLVED "PixieSDK-Legacy")
                message(STATUS "[PAASS::ACQ] Resolved acquisition API: PixieSDK-Legacy")
            else()
                message(STATUS "[PAASS::ACQ] PixieSDK-Legacy not found; falling back to XIA_API")

                if (DEFINED ENV{XIA_FIRMWARE_DIR})
                    find_package(PLX REQUIRED)
                    find_package(XIA REQUIRED)

                    if (PLX_FOUND AND XIA_FOUND)
                        set(PAASS_ACQ_API_RESOLVED "XIA_API")
                        message(STATUS "[PAASS::ACQ] Resolved acquisition API: XIA_API")
                    else()
                        set(PAASS_BUILD_ACQ OFF CACHE BOOL "Build and install Acquisition" FORCE)
                        message(STATUS "[PAASS::ACQ] Neither PixieSDK-Legacy nor XIA_API dependencies were found; disabling acquisition build.")
                    endif()
                else()
                    set(PAASS_BUILD_ACQ OFF CACHE BOOL "Build and install Acquisition" FORCE)
                    message(STATUS "[PAASS::ACQ] \"XIA_FIRMWARE_DIR\" or \"XIA_PIXIE_SDK\" not set; disabling acquisition build.")
                endif()
            endif()
        else()
            if (DEFINED ENV{XIA_FIRMWARE_DIR})
                find_package(PLX REQUIRED)
                find_package(XIA REQUIRED)

                if (PLX_FOUND AND XIA_FOUND)
                    set(PAASS_ACQ_API_RESOLVED "XIA_API")
                    message(STATUS "[PAASS::ACQ] Resolved acquisition API: XIA_API")
                else()
                    set(PAASS_BUILD_ACQ OFF CACHE BOOL "Build and install Acquisition" FORCE)
                    message(STATUS "[PAASS::ACQ] Old-kernel XIA_API dependencies not found; disabling acquisition build.")
                endif()
            else()
                set(PAASS_BUILD_ACQ OFF CACHE BOOL "Build and install Acquisition" FORCE)
                message(STATUS "[PAASS::ACQ] XIA_FIRMWARE_DIR not set; disabling acquisition build.")
            endif()
        endif()

    else()
        message(FATAL_ERROR "[PAASS::ACQ] Unknown PAASS_ACQ_API='${PAASS_ACQ_API}'. Valid values: Auto, PixieSDK-Legacy, XIA_API")
    endif()


    if (PAASS_ACQ_API_RESOLVED STREQUAL "XIA_API")
        message(STATUS "[PAASS::ACQ] Configuring acquisition for XIA_API (v2 API mode)")

        if (PLX_FOUND AND XIA_FOUND)
            link_directories(${PLX_LIBRARY_DIR})
            # need to add PLX_LIBRARY_DIR to the RPATH so the linker can find the PLX lib at runtime, otherwise we have to add it to the LD_LIBRARY_PATH
            set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
            # list(APPEND CMAKE_INSTALL_RPATH "${PLX_LIBRARY_DIR}")

            include_directories(${XIA_INCLUDE_DIR})
            link_directories(${XIA_LIBRARY_DIR})

            add_definitions("-D V2_API_MODE")
            set(USING_V2_API TRUE)

            message(STATUS "[PAASS::ACQ] Found: PLX_LIBRARY_DIR = ${PLX_LIBRARY_DIR}")
            message(STATUS "[PAASS::ACQ] Found: XIA_LIBRARY_DIR = ${XIA_LIBRARY_DIR}")
            message(STATUS "[PAASS::ACQ] Found: XIA_INCLUDE_DIR = ${XIA_INCLUDE_DIR}")
        endif(PLX_FOUND AND XIA_FOUND)

    elseif (PAASS_ACQ_API_RESOLVED STREQUAL "PixieSDK-Legacy")
        message(STATUS "[PAASS::ACQ] Configuring acquisition for PixieSDK-Legacy mode")

        if (BroadcomAPI_FOUND AND PixieSDK_FOUND)
            include_directories(${PLX_LIBRARY_DIR})
            link_directories(${PLX_LIBRARY_DIR})
            include_directories(${PIXIESDK_LEGACY_INCLUDE_DIR})
            link_directories(${PIXIESDK_LEGACY_LIBRARY})

            set(USING_V2_API FALSE)

            message(STATUS "[PAASS::ACQ] Found: PLX_LIBRARY_DIR = ${PLX_LIBRARY_DIR}")
            message(STATUS "[PAASS::ACQ] Found: PIXIESDK_LIBRARY_DIR = ${PIXIESDK_LEGACY_LIBRARY}")
            message(STATUS "[PAASS::ACQ] Found: PIXIESDK_INCLUDE_DIR = ${PIXIESDK_LEGACY_INCLUDE_DIR}")

        endif(BroadcomAPI_FOUND AND PixieSDK_FOUND)
            
    else()
        set(PAASS_BUILD_ACQ OFF CACHE BOOL "Build and install Acquisition" FORCE)
        message(WARNING "[PAASS::ACQ] No resolved acquisition API; disabling acquisition build.")
    endif()

    if(PAASS_BUILD_SETUP)
        message(STATUS "[PAASS::ACQ] Setup programs will be included in installation.")
    endif(PAASS_BUILD_SETUP)

else()
    set(PAASS_BUILD_ACQ OFF CACHE BOOL "Various ENV vars not set or ACQ not requested" FORCE)
    message(STATUS "[PAASS::ACQ] PAASS_BUILD_ACQ: ${PAASS_BUILD_ACQ}.")
endif()


#Find ROOT if USE_ROOT was set.
if (PAASS_USE_ROOT)
    find_package(ROOT REQUIRED COMPONENTS TreePlayer)
    set(ROOTSYS $ENV{ROOTSYS} CACHE STRING "ROOT directory.")
    find_program(ROOT_CONFIG_EXECUTABLE root-config PATHS ${ROOTSYS}/bin)
    find_program(ROOTCINT_EXECUTABLE rootcint PATHS $ENV{ROOTSYS}/bin)
    find_program(GENREFLEX_EXECUTABLE genreflex PATHS $ENV{ROOTSYS}/bin)
    if (ROOT_CONFIG_EXECUTABLE)
        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --prefix
                OUTPUT_VARIABLE ROOTSYS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --version
                OUTPUT_VARIABLE ROOT_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir
                OUTPUT_VARIABLE ROOT_INCLUDE_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --libs
                OUTPUT_VARIABLE ROOT_LIBRARIES
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --glibs
                OUTPUT_VARIABLE ROOT_GUI_LIBRARIES
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        set(ROOT_LIBRARY_DIR ${ROOTSYS}/lib)

        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-minuit2
                OUTPUT_VARIABLE ROOT_HAS_MINUIT2
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-cxx17
                OUTPUT_VARIABLE ROOT_HAS_CXX17)

        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-cxx14
                OUTPUT_VARIABLE ROOT_HAS_CXX14)

        execute_process(
                COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-cxx11
                OUTPUT_VARIABLE ROOT_HAS_CXX11)

  endif ()

  if (${ROOT_VERSION} VERSION_LESS "6.10")
    message(FATAL_ERROR "Minimum ROOT version is 6.10.00.")
  endif (${ROOT_VERSION} VERSION_LESS "6.10")
  mark_as_advanced(FORCE GENREFLEX_EXECUTABLE ROOTCINT_EXECUTABLE ROOT_CONFIG_EXECUTABLE)
  #if ( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
  #  include_directories(${ROOT_INCLUDE_DIR})
  #  link_directories(${ROOT_LIBRARY_DIR})
  #else() 
    include_directories(SYSTEM ${ROOT_INCLUDE_DIR})
    link_directories(SYSTEM ${ROOT_LIBRARY_DIR})
  #endif()
  add_definitions("-D USE_ROOT")
  add_definitions("-D useroot")
  if (ROOT_HAS_CXX11 MATCHES "yes")
    message(STATUS "[PAASS] ROOT was built with cxx11. Setting CMAKE_CXX_STANDARD to match")
    set(CMAKE_CXX_STANDARD 11)
  elseif (ROOT_HAS_CXX14 MATCHES "yes")
    message(STATUS "[PAASS] ROOT was built with cxx14. Setting CMAKE_CXX_STANDARD to match")
    set(CMAKE_CXX_STANDARD 14)
  elseif (ROOT_HAS_CXX17 MATCHES "yes")
    message(STATUS "[PAASS] ROOT was built with cxx17. Setting CMAKE_CXX_STANDARD to match")
    set(CMAKE_CXX_STANDARD 17)
  endif()
endif (PAASS_USE_ROOT)

#------------------------------------------------------------------------------
#Put all the include directories that we might need at this point. Ensure
#that the Third party includes come before the Resources.
include_directories(ThirdParty/include)
include_directories(Resources/include)
include_directories(Core/include)

#Build the Third party software since it's useful for other programs
add_subdirectory(ThirdParty)

#Add the resources to compilation
add_subdirectory(Resources)

#Build the PaassCoreLibrary.
add_subdirectory(Core)

#Build Acquisition software, we don't have this broken into as fine of a
#granularity as the Analysis software, so we just wrap the whole thing in an if.
if (PAASS_BUILD_ACQ)
    add_subdirectory(Acquisition)
else ()
    #Ensure that we can still build set2* even when BUILD_ACQ is off
    add_subdirectory(Acquisition/set2root)
endif (PAASS_BUILD_ACQ)

#Build any of the analysis related things that we need to build.
add_subdirectory(Analysis)

#Build/install the miscellaneous stuff
add_subdirectory(Share)
