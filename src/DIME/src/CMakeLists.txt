include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	)

#SUBDIRS = classes entities objects records sections tables util convert .

set(dime_srcs
	Base.cpp
	Basic.cpp
	Input.cpp
	Layer.cpp
	Model.cpp
	Output.cpp
	RecordHolder.cpp
	State.cpp
	)

add_library(dime ${dime_srcs})

