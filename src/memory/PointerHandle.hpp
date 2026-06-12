#pragma once

#include <cstdint>
#include <cstddef>

namespace zet::memory {
	// Base Allocator interface for allocator compatibility in PointerHandle
	class BaseAllocator {
	public:
		virtual ~BaseAllocator() = default;
		virtual std::byte* GetBase() const = 0;
	};
}

namespace zet {
	template <typename T>
	class PointerHandle {
	public:
		PointerHandle() : allocator(nullptr), offset(0) {}
		PointerHandle(memory::BaseAllocator* allocator, const std::uint32_t offset) 
			: allocator(allocator)
			, offset(offset) {}

		~PointerHandle() {
			Reset();
		}

		PointerHandle(const PointerHandle&) = delete;
		PointerHandle& operator=(const PointerHandle&) = delete;

		PointerHandle(PointerHandle&& other) noexcept 
			: allocator(other.allocator)
			, offset(other.offset) {
			other.allocator = nullptr;
			other.offset = 0;
		}

		PointerHandle& operator=(PointerHandle&& other) noexcept {
			if(this != &other) {
				allocator = other.allocator;
				offset = other.offset;
				other.allocator = nullptr;
				other.offset = 0;
			}

			return *this;
		}

		T* Get() const {
			if(offset == 0 || allocator == nullptr) {
				return nullptr;
			}

			return reinterpret_cast<T*>(allocator->GetBase() + offset);
		}

		void Reset() {
			allocator = nullptr;
			offset = 0;
		}

	private:
		memory::BaseAllocator* allocator;
		std::uint32_t offset;
	};
}