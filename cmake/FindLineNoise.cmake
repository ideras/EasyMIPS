find_library(LINENOISE_LIBRARY NAMES linenoise liblinenoise
        PATHS $ENV{LINENOISE_DIR} /usr /usr/local /opt/local
        PATH_SUFFIXES lib lib64 x86_64-linux-gnu
)

find_path(LINENOISE_INCLUDE_DIR linenoise.h
          PATHS $ENV{LINENOISE_DIR} /usr /usr/local /opt/local
          PATH_SUFFIXES include include/x86_64-linux-gnu x86_64-linux-gnu
         )

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LINENOISE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LineNoise  DEFAULT_MSG
                                  LINENOISE_LIBRARY LINENOISE_INCLUDE_DIR)

mark_as_advanced(LINOISE_INCLUDE_DIR LINOISE_LIBRARY)

if(LINENOISE_FOUND)
    set(LINOISE_LIBRARIES ${LINOISE_LIBRARY})
    set(LINOISE_INCLUDE_DIRS ${LINOISE_INCLUDE_DIR})
endif(LINENOISE_FOUND) 
