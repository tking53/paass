# @authors C. R. Thornsberry, K. Smith
set(Interface_SOURCES PixieInterface.cpp Lock.cpp)

add_library(PixieInterface STATIC ${Interface_SOURCES})
target_compile_definitions(PixieInterface PUBLIC PLX_LINUX)

#Order is important, XIA before PLX
if (USING_V2_API)
        target_link_libraries(PixieInterface PUBLIC PaassCoreStatic ${XIA_LIBRARIES} ${PLX_LIBRARIES})
else()
	target_link_libraries(PixieInterface PUBLIC PaassCoreStatic ${PIXIESDK_LEGACY_LIBRARY} ${PLX_STATIC_LIB})
endif (USING_V2_API)

set(Support_SOURCES PixieSupport.cpp)
add_library(PixieSupport STATIC ${Support_SOURCES})
target_compile_definitions(PixieSupport PUBLIC PLX_LINUX)

add_library(Utility STATIC Utility.cpp)
target_compile_definitions(Utility PUBLIC PLX_LINUX)
