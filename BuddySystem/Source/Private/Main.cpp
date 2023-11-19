#include "BuddySystem.h"

#include <iostream>

int main( )
{
	BuddySystem buddySystem;
	buddySystem.Initialize( 1024, 256 );

	auto offset1 = buddySystem.Allocate( 256 );
	auto offset2 = buddySystem.Allocate( 256 );
	auto offset3 = buddySystem.Allocate( 512 );

	buddySystem.Deallocate( offset1.value() );
	buddySystem.Deallocate( offset2.value() );
	buddySystem.Deallocate( offset3.value() );

	return 0;
}