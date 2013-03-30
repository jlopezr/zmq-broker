# Copyright (c) 2010-2011 Phorm, Inc.
# License: GNU LGPL v 3.0, see http://www.gnu.org/licenses/lgpl-3.0-standalone.html 
# Author: Andrey Skryabin <andrew@zmqmessage.org>, et al.

# - Find czmq
# Find the zeromq includes and library
#
#  CZEROMQ_INCLUDE_DIR - Where to find czmq include sub-directory.
#  CZEROMQ_LIBRARIES   - List of libraries when using czmq.
#  CZEROMQ_FOUND       - True if czmq found.

IF (CZEROMQ_INCLUDE_DIR)
  # Already in cache, be silent.
  SET(CZEROMQ_FIND_QUIETLY TRUE)
ENDIF (CZEROMQ_INCLUDE_DIR)

FIND_PATH(CZEROMQ_INCLUDE_DIR czmq.h)

SET(CZEROMQ_NAMES czmq)
FIND_LIBRARY(CZEROMQ_LIBRARY NAMES ${CZEROMQ_NAMES} )

# Handle the QUIETLY and REQUIRED arguments and set ZEROMQ_FOUND to
# TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  CZEROMQ DEFAULT_MSG
  CZEROMQ_LIBRARY CZEROMQ_INCLUDE_DIR 
)

IF(CZEROMQ_FOUND)
  SET( CZEROMQ_LIBRARIES ${CZEROMQ_LIBRARY} )
  #MESSAGE ( STATUS "Found CZMQ:" )
  #MESSAGE ( STATUS "  (Headers)       ${CZEROMQ_INCLUDE_DIR}" )
  #MESSAGE ( STATUS "  (Library)       ${CZEROMQ_LIBRARIES}" )
ELSE(CZEROMQ_FOUND)
  SET( CZEROMQ_LIBRARIES )
  #MESSAGE ( "CZMQ NOT FOUND!" )
ENDIF(CZEROMQ_FOUND)
