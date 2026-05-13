# Find the prometheus-cpp libraries and headers.
#
# Sets the usual variables expected for find_package scripts:
#
# Prometheus_FOUND - true if prometheus-cpp was found.
# Prometheus_INCLUDE_DIRS - include directory containing prometheus/*.h
# Prometheus_LIBRARIES - list of full-path libraries to link.
#
# Optional hints:
#   PROMETHEUS_DIR
#   ENV{PROMETHEUS_DIR}

unset(PROMETHEUS_CPP_INCLUDE_DIR CACHE)
unset(PROMETHEUS_CPP_CORE_LIBRARY CACHE)
unset(PROMETHEUS_CPP_PULL_LIBRARY CACHE)

set(_PROMETHEUS_HINTS ${PROMETHEUS_DIR} $ENV{PROMETHEUS_DIR})

find_path(PROMETHEUS_CPP_INCLUDE_DIR
        NAMES prometheus/exposer.h
        HINTS ${_PROMETHEUS_HINTS}
        PATH_SUFFIXES include
        DOC "Path to prometheus-cpp headers")

find_library(PROMETHEUS_CPP_CORE_LIBRARY
        NAMES prometheus-cpp-core
        HINTS ${_PROMETHEUS_HINTS}
        PATH_SUFFIXES lib lib64
        DOC "Path to prometheus-cpp core library")

find_library(PROMETHEUS_CPP_PULL_LIBRARY
        NAMES prometheus-cpp-pull
        HINTS ${_PROMETHEUS_HINTS}
        PATH_SUFFIXES lib lib64
        DOC "Path to prometheus-cpp pull library")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Prometheus DEFAULT_MSG
        PROMETHEUS_CPP_INCLUDE_DIR
        PROMETHEUS_CPP_CORE_LIBRARY
        PROMETHEUS_CPP_PULL_LIBRARY)

if (Prometheus_FOUND)
    set(Prometheus_INCLUDE_DIRS ${PROMETHEUS_CPP_INCLUDE_DIR})
    set(Prometheus_LIBRARIES ${PROMETHEUS_CPP_PULL_LIBRARY} ${PROMETHEUS_CPP_CORE_LIBRARY})

    if (NOT TARGET Prometheus::core)
        add_library(Prometheus::core UNKNOWN IMPORTED)
        set_target_properties(Prometheus::core PROPERTIES
                IMPORTED_LOCATION "${PROMETHEUS_CPP_CORE_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${PROMETHEUS_CPP_INCLUDE_DIR}")
    endif ()

    if (NOT TARGET Prometheus::pull)
        add_library(Prometheus::pull UNKNOWN IMPORTED)
        set_target_properties(Prometheus::pull PROPERTIES
                IMPORTED_LOCATION "${PROMETHEUS_CPP_PULL_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${PROMETHEUS_CPP_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "Prometheus::core")
    endif ()
endif ()

mark_as_advanced(
        PROMETHEUS_CPP_INCLUDE_DIR
        PROMETHEUS_CPP_CORE_LIBRARY
        PROMETHEUS_CPP_PULL_LIBRARY)
