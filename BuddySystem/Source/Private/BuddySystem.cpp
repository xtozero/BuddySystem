#include "BuddySystem.h"

#include <cassert>
#include <cmath>

namespace
{
	template <typename T>
	T GetNearestPowerOfTwo( T x )
	{
		return static_cast<T>( 1 ) << static_cast<T>( std::ceil( std::log2( x ) ) );
	}
}

void BuddySystem::Initialize( size_t size, size_t minBlockSize ) noexcept
{
	assert( size >= minBlockSize );

	m_size = GetNearestPowerOfTwo( size );
	m_minBlockSize = GetNearestPowerOfTwo( minBlockSize );

	m_numLevel = std::log2( m_size / m_minBlockSize ) + 1;

	m_freeFlags.resize( m_numLevel );
	for ( BitArray& freeFlag : m_freeFlags )
	{
		freeFlag.resize( m_size / m_minBlockSize );
	}

	m_freeFlags.back()[0] = true;

	m_usedBlockLevels.resize( m_size / m_minBlockSize, -1 );
}

std::optional<size_t> BuddySystem::Allocate( size_t size )
{
	uint32 level = GetLevel( size );

	std::optional<size_t> blockIdx = GetFreeBlock( level );
	if ( blockIdx.has_value() == false )
	{
		for ( int curLevel = level + 1; curLevel < m_numLevel; ++curLevel )
		{
			blockIdx = GetFreeBlock( curLevel );
			if ( blockIdx.has_value() )
			{
				blockIdx = SplitBlock( curLevel, level, blockIdx.value() );
				break;
			}
		}
	}

	if ( blockIdx.has_value() == false )
	{
		return {};
	}

	SetBlockState( level, blockIdx.value(), false );

	size_t blockSize = m_minBlockSize << level;
	size_t offset = blockIdx.value() * blockSize;

	m_usedBlockLevels[offset / m_minBlockSize] = level;

	return offset;
}

void BuddySystem::Deallocate( size_t offset )
{
	int32 level = m_usedBlockLevels[offset / m_minBlockSize];

	if ( level == -1 )
	{
		assert( false && "Trying to release unallocated space" );
		return;
	}

	m_usedBlockLevels[offset / m_minBlockSize] = -1;

	size_t blockSize = m_minBlockSize << level;
	size_t blockIndex = offset / blockSize;

	SetBlockState( level, blockIndex, true );
	MergeBlcok( blockIndex, level );
}

size_t BuddySystem::GetSuitableBlockSize( size_t size ) const noexcept
{
	size = GetNearestPowerOfTwo( size );
	return std::max<size_t>( size, m_minBlockSize );
}

uint32 BuddySystem::GetLevel( size_t size ) const noexcept
{
	size = GetSuitableBlockSize( size );
	return std::log2( size / m_minBlockSize );
}

std::optional<size_t> BuddySystem::GetFreeBlock( uint32 level ) const noexcept
{
	const BitArray& freeFlag = m_freeFlags[level];
	size_t numBlock = ( m_size / m_minBlockSize ) >> level;

	for ( size_t i = 0; i < numBlock; ++i )
	{
		if ( freeFlag[i] )
		{
			return i;
		}
	}

	return {};
}

void BuddySystem::SetBlockState( uint32 level, size_t blockIndex, bool isFree ) noexcept
{
	m_freeFlags[level][blockIndex] = isFree;
}

size_t BuddySystem::SplitBlock( uint32 srcLevel, uint32 destLevel, size_t srcIndex ) noexcept
{
	for ( uint32 i = srcLevel; i > destLevel; --i )
	{
		SetBlockState( i, srcIndex, false );

		srcIndex *= 2;

		SetBlockState( i - 1, srcIndex, true );
		SetBlockState( i - 1, srcIndex + 1, true );
	}

	return srcIndex;
}

void BuddySystem::MergeBlcok( size_t blockIndex, uint32 level ) noexcept
{
	size_t parentIndex = blockIndex / 2;

	for ( uint32 l = level + 1; l < m_numLevel; ++l )
	{
		uint32 childLevel = l - 1;
		size_t leftIndex = parentIndex * 2;
		size_t rightIndex = leftIndex + 1;

		if ( ( m_freeFlags[childLevel][leftIndex] == false )
			|| ( m_freeFlags[childLevel][rightIndex] == false ) )
		{
			break;
		}

		SetBlockState( childLevel, leftIndex, false );
		SetBlockState( childLevel, rightIndex, false );
		SetBlockState( l, parentIndex, true );

		parentIndex /= 2;
	}
}
