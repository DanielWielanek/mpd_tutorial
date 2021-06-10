# Create a library called "libMpdField" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/mpdbase
 ${CMAKE_SOURCE_DIR}/mcstack
 ${CMAKE_SOURCE_DIR}/mpddst
 ${CMAKE_SOURCE_DIR}/mpddst/mcDst
 ${CMAKE_SOURCE_DIR}/mpddst/MpdMiniEvent
 ${CMAKE_SOURCE_DIR}/mytask
  
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
MyTaskCompiled.cxx
)

Set(HEADERS)
Set(LINKDEF MyTaskLinkDef.h)
Set(LIBRARY_NAME MyTask)
Set(DEPENDENCIES Core Base MpdDst MpdBase MpdMCStack)

GENERATE_LIBRARY()
