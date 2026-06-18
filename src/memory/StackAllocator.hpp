#pragma once

#include "MemoryAllocator.hpp"

#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace zet::memory {
	class StackAllocator : public MemoryAllocator {
	private:
		struct destructor {
			void (*Func)(void*) = nullptr;
			destructor* Next = nullptr;
			void* Target = nullptr;
		};

	public:
		explicit StackAllocator(const std::size_t chunkSize) {
			start = static_cast<std::byte*>(::operator new(chunkSize, std::align_val_t{64}));
			end = start + chunkSize;
			offset = start;
		}

		~StackAllocator() override {
			Reset();
			::operator delete(start, std::align_val_t{64});
		}

		StackAllocator(const StackAllocator&) = delete;
		StackAllocator& operator=(const StackAllocator&) = delete;

		void* Allocate(const std::size_t size, const std::size_t alignment = alignof(std::max_align_t)) {
			void* currentPtr = static_cast<void*>(offset);
			std::size_t space = end - offset;

			if (std::align(alignment, size, currentPtr, space)) {
				offset = static_cast<std::byte*>(currentPtr) + size;
				return currentPtr;
			}

			return nullptr;
		}

		template <typename T, bool IsInternal = false, typename... Args>
		T* Create(Args&&... args) {
			if constexpr (!IsInternal) {
				static_assert(std::is_trivially_destructible_v<T>, "[STACK ALLOCATOR] POLICY VIOLATION");
			}

			void* memory = Allocate(sizeof(T), alignof(T));
			if (!memory) {
				return nullptr;
			}

			T* objPtr = ::new (memory) T(std::forward<Args>(args)...);

			if constexpr (!std::is_trivially_destructible_v<T>) {
				void* node = Allocate(sizeof(destructor), alignof(destructor));
				if (!node) {
					objPtr->~T();
					return nullptr;
				}
				destructor* newNode = ::new (node) destructor();
				newNode->Func = [](void* ptr) {
					static_cast<T*>(ptr)->~T();
				};

				newNode->Next = lastDestructor;
				newNode->Target = objPtr;

				lastDestructor = newNode;
			}

			return objPtr;
		}

		[[nodiscard]] std::byte* GetBase() const override {
			return start;
		}

		template <typename T, bool IsInternal = false, typename... Args>
		PointerHandle<T> CreateHandle(Args&&... args) {
			if constexpr (!IsInternal) {
				static_assert(std::is_trivially_destructible_v<T>, "[STACK ALLOCATOR] POLICY VIOLATION");
			}

			std::size_t space = end - offset;
			void* ptr = offset;

			if(!std::align(alignof(T), sizeof(T), ptr, space)) {
				return PointerHandle<T>();
			}

			std::uint32_t offsetVal = static_cast<std::uint32_t>(static_cast<std::byte*>(ptr) - start);

			if constexpr (!std::is_trivially_destructible_v<T>) {
				std::byte* nextOffset = static_cast<std::byte*>(ptr) + sizeof(T);
				std::size_t destSpace = end - nextOffset;
				void* destPtr = nextOffset;
				if (!std::align(alignof(destructor), sizeof(destructor), destPtr, destSpace)) {
					return PointerHandle<T>();
				}

				offset = static_cast<std::byte*>(destPtr) + sizeof(destructor);

				T* objPtr = ::new(ptr) T(std::forward<Args>(args)...);

				destructor* newNode = ::new(destPtr) destructor();
				newNode->Func = [](void* p) {
					static_cast<T*>(p)->~T();
				};
				newNode->Next = lastDestructor;
				newNode->Target = objPtr;
				lastDestructor = newNode;

				return PointerHandle<T>(this, offsetVal);
			} else {
				offset = static_cast<std::byte*>(ptr) + sizeof(T);
				::new(ptr) T(std::forward<Args>(args)...);
				return PointerHandle<T>(this, offsetVal);
			}
		}

		void FreeToMarker(std::byte* marker) {
			assert(marker >= start && marker <= offset);

			while (lastDestructor && (void*)lastDestructor > (void*)marker) {
				lastDestructor->Func(lastDestructor->Target);
				lastDestructor = lastDestructor->Next;
			}
			offset = marker;
		}

		void Reset() noexcept {
			destructor* currentNode = lastDestructor;
			while (currentNode != nullptr) {
				currentNode->Func(currentNode->Target);
				currentNode = currentNode->Next;
			}
			offset = start;
			lastDestructor = nullptr;
		}

	private:
		destructor* lastDestructor = nullptr;

		std::byte* start = nullptr;
		std::byte* end = nullptr;
		std::byte* offset = nullptr;
	};
}
