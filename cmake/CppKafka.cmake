function(AddCppKafka target)
    find_package(RdKafka CONFIG REQUIRED)

    find_path(HYPODERMIC_INCLUDE_DIRS "Hypodermic/ActivatedRegistrationInfo.h")

    include(FindPackageHandleStandardArgs)
    include(SelectLibraryConfigurations)

    find_path(CPPKAFKA_INCLUDES NAMES cppkafka.h HINTS ${CURRENT_INSTALLED_DIR} PATH_SUFFIXES cppkafka)

    find_library(CPPKAFKA_LIBRARY_DEBUG NAMES cppkafka PATH_SUFFIXES lib PATHS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}" NO_DEFAULT_PATH REQUIRED)
    find_library(CPPKAFKA_LIBRARY_RELEASE NAMES cppkafka PATH_SUFFIXES lib PATHS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug" NO_DEFAULT_PATH REQUIRED)

    select_library_configurations(CPPKAFKA)

    target_link_libraries(${target} PRIVATE ${CPPKAFKA_LIBRARY} RdKafka::rdkafka++)
    target_include_directories(${target} PRIVATE ${CPPKAFKA_INCLUDES}/..)
endfunction()