#pragma once

#include <cstddef>

namespace zet {
	class MemoryAllocator {
	public:
		virtual ~MemoryAllocator() = default;
		virtual std::byte* GetBase() const = 0;
	};
}
