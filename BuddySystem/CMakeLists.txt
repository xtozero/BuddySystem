set(HEADER
	"./Source/Public/BuddySystem.h"
	"./Source/Public/SizedTypes.h"
)

set (SOURCE
	"./Source/Private/BuddySystem.cpp"
	"./Source/Private/Main.cpp"
)

add_executable(buddy_system ${HEADER} ${SOURCE})

target_include_directories(buddy_system
PUBLIC
	Source/Public
)
