PLUGIN_FIND_INCLUDE (UUID uuid.h /usr/include/uuid)
CONFIG_DEFINE (HAVE_UUID_H)

PLUGIN_FIND_INCLUDE (SYS_UUID sys/uuid.h)
CONFIG_DEFINE (HAVE_SYS_UUID_H)

PLUGIN_FIND_LIBRARY (UUID uuid)

IF (HAVE_LIBUUID)
  SET (CMAKE_REQUIRED_LIBRARIES uuid)
ENDIF (HAVE_LIBUUID)

CHECK_FUNCTION_EXISTS (uuidgen HAVE_UUIDGEN)
CONFIG_DEFINE (HAVE_UUIDGEN)

CHECK_FUNCTION_EXISTS (uuid_generate HAVE_UUID_GENERATE)
CONFIG_DEFINE (HAVE_UUID_GENERATE)

# IF (UUID_UUID_GENERATE)
#   SET (HAVE_UUID_GENERATE 1)
#   PLUGIN_LINK_LIBRARIES (uuid)
# ENDIF (UUID_UUID_GENERATE)

