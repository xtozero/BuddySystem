#pragma once
#include "SizedTypes.h"

#include <optional>
#include <vector>

class BuddySystem final
{
public:
	void Initialize( size_t size, size_t minBlockSize ) noexcept;

	[[nodiscard]] std::optional<size_t> Allocate( size_t size );
	void Deallocate( size_t offset );

private:
	size_t GetSuitableBlockSize( size_t size ) const noexcept;
	uint32 GetLevel( size_t size ) const noexcept;

	std::optional<size_t> GetFreeBlock( uint32 level ) const noexcept;
	void SetBlockState( uint32 level, size_t blockIndex, bool isFree ) noexcept;
	size_t SplitBlock( uint32 srcLevel, uint32 destLevel, size_t srcIndex ) noexcept;
	void MergeBlcok( size_t blockIndex, uint32 level ) noexcept;

	size_t m_size = 0;
	size_t m_minBlockSize = 0;

	uint32 m_numLevel = 0;

	using BitArray = std::vector<bool>;
	std::vector<BitArray> m_freeFlags;

	std::vector<int32> m_usedBlockLevels;
};