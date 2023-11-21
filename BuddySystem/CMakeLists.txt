set(HEADER
	"./Source/Public/BuddySystem.h"
	"./Source/Public/SizedTypes.h"
)

set (SOURCE
	"./Source/Private/BuddySystem.cpp"
)

add_library(buddy_system STATIC ${HEADER}
								${SOURCE})

target_include_directories(buddy_system
PUBLIC
	Source/Public
)
