#pragma once

#include <utility>
#include <cassert>
#include <memory>
#include <concepts>
#include <functional>

namespace zet {
    const static std::size_t DEFAULT_MAP_CAPACITY = 1024;

    template <typename K, typename V, std::size_t C = DEFAULT_MAP_CAPACITY> 
    requires (C > 0) && std::destructible<K> && std::destructible<V>
    class Map {
    public:
        struct Pair {
            const K& Key;
            V& Value;
        };

        struct ConstPair {
            const K& Key;
            const V& Value;
        };

        class Iterator {
        public:
            constexpr Iterator(Map* map, std::size_t index) : map(map), index(index) {
                MoveToNext();
            }

            constexpr Pair operator*() const {
                return { map->keys[index].value, map->values[index].value };
            }

            constexpr Iterator& operator++() {
                ++index;
                MoveToNext();
                return *this;
            }

            constexpr bool operator==(const Iterator& other) const {
                return index == other.index;
            }

            constexpr bool operator!=(const Iterator& other) const {
                return !(*this == other);
            }

        private:
            constexpr void MoveToNext() {
                while (index < C && !map->occupied[index]) {
                    ++index;
                }
            }

            Map* map;
            std::size_t index;
        };

        class ConstIterator {
        public:
            constexpr ConstIterator(const Map* map, std::size_t index) : map(map), index(index) {
                MoveToNext();
            }

            constexpr ConstPair operator*() const {
                return { map->keys[index].value, map->values[index].value };
            }

            constexpr ConstIterator& operator++() {
                ++index;
                MoveToNext();
                return *this;
            }

            constexpr bool operator==(const ConstIterator& other) const {
                return index == other.index;
            }

            constexpr bool operator!=(const ConstIterator& other) const {
                return !(*this == other);
            }

        private:
            constexpr void MoveToNext() {
                while (index < C && !map->occupied[index]) {
                    ++index;
                }
            }

            const Map* map;
            std::size_t index;
        };

        constexpr Map() : count(0) {
            for (std::size_t i = 0; i < C; ++i) {
                occupied[i] = false;
            }
        }

        constexpr ~Map() {
            Clear();
        }

        constexpr Iterator begin() { 
            return Iterator(this, 0); 
        }

        constexpr ConstIterator begin() const { 
            return ConstIterator(this, 0); 
        }

        constexpr Iterator end() { 
            return Iterator(this, C); 
        }

        constexpr ConstIterator end() const { 
            return ConstIterator(this, C); 
        }

        template <typename... Args>
        constexpr V& Insert(const K& key, Args&&... args) {
            std::size_t hash = std::hash<K>{}(key);
            std::size_t index = hash % C;

            for (std::size_t i = 0; i < C; ++i) {
                std::size_t current = (index + i) % C;

                if (!occupied[current]) {
                    assert(count < C && "[zet::Map] HASHMAP IS FULL");
                    
                    std::construct_at(std::addressof(keys[current].value), key);
                    V* ptr = std::construct_at(std::addressof(values[current].value), std::forward<Args>(args)...);
                    occupied[current] = true;
                    ++count;
                    return *ptr;
                }
                
                if (keys[current].value == key) {
                    std::destroy_at(std::addressof(values[current].value));
                    V* ptr = std::construct_at(std::addressof(values[current].value), std::forward<Args>(args)...);
                    return *ptr;
                }
            }

            assert(false && "[zet::Map] FAILED TO INSERT (CRITICAL)");
            return values[0].value;
        }

        constexpr V* Find(const K& key) {
            std::size_t hash = std::hash<K>{}(key);
            std::size_t index = hash % C;

            for (std::size_t i = 0; i < C; ++i) {
                std::size_t current = (index + i) % C;

                if (!occupied[current]) {
                    return nullptr;
                }

                if (keys[current].value == key) {
                    return std::addressof(values[current].value);
                }
            }
            return nullptr;
        }

        constexpr const V* Find(const K& key) const {
            std::size_t hash = std::hash<K>{}(key);
            std::size_t index = hash % C;

            for (std::size_t i = 0; i < C; ++i) {
                std::size_t current = (index + i) % C;

                if (!occupied[current]) {
                    return nullptr;   
                }

                if (keys[current].value == key) {
                    return std::addressof(values[current].value);
                }
            }
            return nullptr;
        }

        constexpr void Clear() {
            if (count == 0) return;

            for (std::size_t i = 0; i < C; ++i) {
                if (occupied[i]) {
                    std::destroy_at(std::addressof(keys[i].value));
                    std::destroy_at(std::addressof(values[i].value));
                    occupied[i] = false;
                }
            }
            count = 0;
        }

        constexpr std::size_t Size() const noexcept { 
            return count; 
        }

    private:
        template <typename U>
        union Storage {
            U value;
            constexpr Storage() {}
            constexpr ~Storage() {}
        };

        Storage<K> keys[C];
        Storage<V> values[C];

        std::size_t count;
        bool occupied[C];
    };
}
