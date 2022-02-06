# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

file(GLOB SRC_FILES *.c)
set(COMPONENT_SRCS ${SRC_FILES})
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
