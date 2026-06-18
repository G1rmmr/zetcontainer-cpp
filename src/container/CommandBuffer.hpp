#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>
#include <type_traits>

namespace zet {
	template<std::size_t C = 8192>
	class CommandBuffer {
	public:
		struct Header {
			void (*Apply)(const void*);
			std::size_t PayloadSize;
		};

		template<typename T> requires std::is_trivially_copyable_v<T>
		constexpr void Push(void (*apply)(const void*), const T& data) noexcept {
			std::size_t alignSize = alignof(std::max_align_t);
			std::size_t alignedOffset = (writeOffset + alignSize - 1) & ~(alignSize - 1);
			std::size_t headerSize = sizeof(Header);
			std::size_t dataSize = sizeof(T);

			assert(alignedOffset + headerSize + dataSize <= C && "[CommandBuffer] DATA SIZE OVERFLOW");

			Header header{apply, dataSize};
			std::memcpy(stream + alignedOffset, &header, headerSize);
			std::memcpy(stream + alignedOffset + headerSize, &data, dataSize);

			writeOffset = alignedOffset + headerSize + dataSize;
		}

		constexpr void Commit() noexcept {
			std::size_t readOffset = 0;
			std::size_t alignSize = alignof(std::max_align_t);
			std::size_t headerSize = sizeof(Header);

			while (readOffset < writeOffset) {
				readOffset = (readOffset + alignSize - 1) & ~(alignSize - 1);

				if (readOffset >= writeOffset) {
					break;
				}

				Header header;
				std::memcpy(&header, &stream[readOffset], headerSize);

				header.Apply(stream + readOffset + headerSize);
				readOffset += header.PayloadSize + headerSize;
			}

			writeOffset = 0;
		}

	private:
		alignas(std::max_align_t) std::byte stream[C];
		std::size_t writeOffset = 0;
	};
}
