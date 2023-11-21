set (HEADER)
 
set (SOURCE
	"./Source/Private/Main.cpp"
)
 
enable_testing()

 
add_executable(unit_test ${HEADER} ${SOURCE})
 
target_link_libraries(unit_test 
PUBLIC
		buddy_system
PRIVATE
		gtest_main)

target_include_directories(unit_test
PRIVATE
		../ThirdParty/googletest/googletest/include)
 
add_test(NAME unit_test COMMAND unit_test)
