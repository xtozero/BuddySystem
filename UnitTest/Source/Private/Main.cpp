#define DoUnitTest

#include "BuddySystem.h"
#include "gtest/gtest.h"

#include <random>
#include <vector>
#include <utility>

template <typename T>
T GetNearestPowerOfTwo( T x )
{
	return static_cast<T>( 1 ) << static_cast<T>( std::ceil( std::log2( x ) ) );
}

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

TEST( Allocation, AlignedCase )
{
	BuddySystem buddySystem;
	{
		ASSERT_TRUE( buddySystem.Initialize( 1024, 256 ) );

		auto offset = buddySystem.Allocate( 128 );
		ASSERT_TRUE( offset );
		ASSERT_EQ( *offset, 0 );

		offset = buddySystem.Allocate( 256 );
		ASSERT_TRUE( offset );
		ASSERT_EQ( *offset, 256 );

		offset = buddySystem.Allocate( 512 );
		ASSERT_TRUE( offset );
		ASSERT_EQ( *offset, 512 );

		offset = buddySystem.Allocate( 256 );
		ASSERT_FALSE( offset );
	}
}

TEST( Allocation, NotAlignedCase )
{
	BuddySystem buddySystem;
	{
		ASSERT_TRUE( buddySystem.Initialize( 1024, 256 ) );

		auto offset = buddySystem.Allocate( 20 );
		ASSERT_TRUE( offset );
		ASSERT_EQ( *offset, 0 );

		offset = buddySystem.Allocate( 50 );
		ASSERT_TRUE( offset );
		ASSERT_EQ( *offset, 256 );

		offset = buddySystem.Allocate( 500 );
		ASSERT_TRUE( offset );
		ASSERT_EQ( *offset, 512 );

		offset = buddySystem.Allocate( 255 );
		ASSERT_FALSE( offset );
	}
}

TEST( Deallocation, AlignedCase )
{
	BuddySystem buddySystem;
	{
		ASSERT_TRUE( buddySystem.Initialize( 1024, 256 ) );

		auto offset1 = buddySystem.Allocate( 256 );
		ASSERT_TRUE( offset1 );
		ASSERT_EQ( *offset1, 0 );

		auto offset2 = buddySystem.Allocate( 512 );
		ASSERT_TRUE( offset2 );
		ASSERT_EQ( *offset2, 512 );

		auto offset3 = buddySystem.Allocate( 256 );
		ASSERT_TRUE( offset3 );
		ASSERT_EQ( *offset3, 256 );

		auto offset4 = buddySystem.Allocate( 256 );
		ASSERT_FALSE( offset4 );

		buddySystem.Deallocate( *offset2 );
		buddySystem.Deallocate( *offset1 );
		buddySystem.Deallocate( *offset3 );

		ASSERT_EQ( buddySystem.AvailableSize(), 1024 );
	}
}

TEST( Deallocation, NotAlignedCase )
{
	BuddySystem buddySystem;
	{
		ASSERT_TRUE( buddySystem.Initialize( 1024, 256 ) );

		auto offset1 = buddySystem.Allocate( 200 );
		ASSERT_TRUE( offset1 );
		ASSERT_EQ( *offset1, 0 );

		auto offset2 = buddySystem.Allocate( 400 );
		ASSERT_TRUE( offset2 );
		ASSERT_EQ( *offset2, 512 );

		auto offset3 = buddySystem.Allocate( 150 );
		ASSERT_TRUE( offset3 );
		ASSERT_EQ( *offset3, 256 );

		auto offset4 = buddySystem.Allocate( 210 );
		ASSERT_FALSE( offset4 );

		buddySystem.Deallocate( *offset2 );
		buddySystem.Deallocate( *offset1 );
		buddySystem.Deallocate( *offset3 );

		ASSERT_EQ( buddySystem.AvailableSize(), 1024 );
	}
}

TEST( Total, ShuffledCase )
{
	BuddySystem buddySystem;
	buddySystem.Initialize( 64 * 1024 * 1024, 256 );

	std::random_device rd{};
	std::mt19937 rnd( rd() );

	std::vector<std::pair<size_t, size_t>> offsetInfos;
	offsetInfos.reserve( 1000 );

	size_t trace = 0;

	for ( int i = 0; i < 1000; ++i )
	{
		std::uniform_int_distribution cmd( 0, 1 );
		int index = cmd( rnd );
		if ( offsetInfos.empty() )
		{
			index = 0;
		}

		switch ( index )
		{
		case 0:
		{
			std::uniform_int_distribution dist( 1, 1024 );
			int allocSize = dist( rnd );
			auto offset = buddySystem.Allocate( allocSize );
			ASSERT_TRUE( offset );
			ASSERT_TRUE( *offset % 256 == 0 );

			allocSize = std::max( GetNearestPowerOfTwo( allocSize ), 256 );
			offsetInfos.emplace_back( *offset, allocSize );

			trace += allocSize;
			break;
		}
		case 1:
		{
			std::shuffle( std::begin( offsetInfos ), std::end( offsetInfos ), rnd );
			
			buddySystem.Deallocate( offsetInfos.back().first );
			trace -= offsetInfos.back().second;

			offsetInfos.pop_back();
			break;
		}
		}
	}

	ASSERT_EQ( buddySystem.UsedSize(), trace );
}
