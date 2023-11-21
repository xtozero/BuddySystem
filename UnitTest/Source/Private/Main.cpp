#define DoUnitTest

#include "BuddySystem.h"
#include "gtest/gtest.h"

TEST( Initialize, AlignedCase )
{
	BuddySystem buddySystem;
	{
		ASSERT_TRUE( buddySystem.Initialize( 1024, 256 ) );

		ASSERT_EQ( buddySystem.Capacity(), 1024 );
		ASSERT_EQ( buddySystem.MinimumAllocSize(), 256 );
	}

	{
		ASSERT_TRUE( buddySystem.Initialize( 2048, 512 ) );

		ASSERT_EQ( buddySystem.Capacity(), 2048 );
		ASSERT_EQ( buddySystem.MinimumAllocSize(), 512 );
	}
}

TEST( Initialize, NotAlignedCase )
{
	BuddySystem buddySystem;
	{
		ASSERT_TRUE( buddySystem.Initialize( 1023, 255 ) );

		ASSERT_EQ( buddySystem.Capacity(), 512 );
		ASSERT_EQ( buddySystem.MinimumAllocSize(), 256 );
	}

	{
		ASSERT_TRUE( buddySystem.Initialize( 9, 3 ) );

		ASSERT_EQ( buddySystem.Capacity(), 8 );
		ASSERT_EQ( buddySystem.MinimumAllocSize(), 4 );
	}
}

TEST( Initialize, ExceptionCase )
{
	BuddySystem buddySystem;
	{
		ASSERT_FALSE( buddySystem.Initialize( 0, 0 ) );
		ASSERT_FALSE( buddySystem.Initialize( 12, 0 ) );
		ASSERT_FALSE( buddySystem.Initialize( 0, 34 ) );

		ASSERT_FALSE( buddySystem.Initialize( 3, 9 ) );
	}
}