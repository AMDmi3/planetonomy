# - Try to locate PUGIXML
#
# This module defines:
#
#  PUGIXML_INCLUDE_DIR
#  PUGIXML_LIBRARY
#  PUGIXML_FOUND
#

FIND_PATH(PUGIXML_INCLUDE_DIR NAMES pugixml.hpp)

FIND_LIBRARY(PUGIXML_LIBRARY NAMES pugixml)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(PUGIXML REQUIRED_VARS PUGIXML_INCLUDE_DIR PUGIXML_LIBRARY)

MARK_AS_ADVANCED(PUGIXML_INCLUDE_DIR PUGIXML_LIBRARY)
