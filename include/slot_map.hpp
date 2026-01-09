#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <format>
#include <iterator>
#include <new>
#include <optional>
#include <ranges>
#include <vector>
#include "assert.hpp"
#include "generic_handle.hpp"
#include "math.hpp"
#include "semantics.hpp"
#include "core_types.hpp"

namespace Crowy
{
    struct iterator_end_tag{};

    template<typename T>
    class slot_map{
    private:
        using Handle = generic_handle<T>;

        struct Slot{
            std::optional<T> data;
            uint32_t generation = 0;

            Slot() = default;
            ~Slot() = default;
            CROWY_DECLARE_MOVE_ONLY(Slot)

            void reset() noexcept{
                ++generation;
                data.reset();
            }

            bool isUsing() const noexcept{
                return data.has_value();
            }
            T* get() noexcept{
                return const_cast<T*>(
                    static_cast<const Slot&>(*this).get()
                );
            }
            const T* get() const noexcept{
                return data ? &*data : nullptr;
            }
        };
        std::vector<Slot> slots;
        using Indexes = std::vector<Index>;
        Indexes freeIndexes;

    public:
        slot_map() = default;
        ~slot_map() = default;

        template<typename... Args>
        Handle emplace(Args&&... args){
            Index freeIndex = acquireSlot();

            slots[freeIndex].data.emplace(std::forward<Args>(args)...);

            return{
                .index = freeIndex,
                .generation = slots[freeIndex].generation
            };
        }

        Handle push(T&& t){
            return emplace(std::move(t));
        }

        bool remove(Handle handle){
            auto outOfIndex = handle.index >= slots.size();
            if(outOfIndex) return false;
            auto expiredGen = slots[handle.index].generation != handle.generation;
            if(expiredGen) return false;

            slots[handle.index].reset();
            freeIndexes.push_back(handle.index);

            return true;
        }

        void clear(){
            for(auto& slot: slots){
                if(slot.isUsing()){
                    slot.reset();
                }
            }

            freeIndexes.clear();
            freeIndexes.reserve(slots.size());
            for(Index i=0; i<slots.size(); ++i){
                freeIndexes.push_back(i);
            }
        }

        T* find(Handle handle) noexcept{
            return const_cast<T*>(
                static_cast<const slot_map&>(*this).find(handle)
            );
        }
        const T* find(Handle handle) const noexcept{
            auto outOfIndex = handle.index >= slots.size();
            if(outOfIndex)
                return nullptr;

            auto expiredGen = slots[handle.index].generation != handle.generation;
            if(expiredGen)
                return nullptr;

            return slots[handle.index].get();
        }
        T& get(Handle handle) noexcept{
            return const_cast<T&>(
                static_cast<const slot_map&>(*this).get(handle)
            );
        }
        const T& get(Handle handle) const noexcept{
            CROWY_ASSERT(
                handle.index < slots.size() &&
                slots[handle.index].generation == handle.generation
            );
            return *slots[handle.index].get();
        }

        void reserve(size_t size){
            if(size <= slots.size())
                return;
            for(auto i=slots.size(); i<size; ++i)
                freeIndexes.push_back(i);
            slots.resize(size);
        }

        size_t size() const noexcept{
            return slots.size() - freeIndexes.size();
        }
        size_t capacity() const noexcept{
            return slots.size();
        }

        class const_iterator;

        class iterator{
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            friend const_iterator;

        private:
            std::vector<Slot>* slots;
            Index index;

            void skip_free() noexcept{
                while(index < slots->size() && !(*slots)[index].isUsing())
                    ++index;
            }

        public:
            iterator(std::vector<Slot>* s, Index i)
                :slots(s), index(i){ skip_free(); }
            iterator(std::vector<Slot>* s, Index i, iterator_end_tag)
                :slots(s), index(i){}

            reference operator*(){
                return *(*slots)[index].get();
            }
            pointer operator->(){
                return (*slots)[index].get();
            }

            iterator& operator++() noexcept{
                ++index;
                skip_free();
                return *this;
            }
            iterator operator++(int) noexcept{
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const noexcept{
                assert(slots == other.slots);
                return index == other.index;
            }
            bool operator!=(const iterator& other) const noexcept{
                assert(slots == other.slots);
                return index != other.index;
            }
            bool operator==(const const_iterator& other) const noexcept{
                assert(slots == other.slots);
                return index == other.index;
            }
            bool operator!=(const const_iterator& other) const noexcept{
                assert(slots == other.slots);
                return index != other.index;
            }
        };

        class const_iterator{
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T*;
            using reference = const T&;

            friend iterator;

        private:
            const std::vector<Slot>* slots;
            Index index;

            void skip_free() noexcept{
                while(index < slots->size() && !(*slots)[index].isUsing())
                    ++index;
            }

        public:
            const_iterator(const std::vector<Slot>* s, Index i)
                :slots(s), index(i){ skip_free(); }
            const_iterator(const std::vector<Slot>* s, Index i, iterator_end_tag)
                :slots(s), index(i){}

            reference operator*() const{
                return *(*slots)[index].get();
            }
            pointer operator->() const{
                return (*slots)[index].get();
            }

            const_iterator& operator++() noexcept{
                ++index;
                skip_free();
                return *this;
            }
            const_iterator operator++(int) noexcept{
                const_iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const noexcept{
                assert(slots == other.slots);
                return index == other.index;
            }
            bool operator!=(const iterator& other) const noexcept{
                assert(slots == other.slots);
                return index != other.index;
            }
            bool operator==(const const_iterator& other) const noexcept{
                assert(slots == other.slots);
                return index == other.index;
            }
            bool operator!=(const const_iterator& other) const noexcept{
                assert(slots == other.slots);
                return index != other.index;
            }
        };

        iterator begin() noexcept{
            return iterator(&slots, 0);
        }
        iterator end() noexcept{
            return iterator(
                &slots,
                static_cast<Index>(slots.size()),
                iterator_end_tag{}
            );
        }
        const_iterator begin() const noexcept{
            return const_iterator(&slots, 0);
        }
        const_iterator end() const noexcept{
            return const_iterator(
                &slots,
                static_cast<Index>(slots.size()),
                iterator_end_tag{}
            );
        }
        const_iterator cbegin() const noexcept{
            return begin();
        }
        const_iterator cend() const noexcept{
            return end();
        }

    private:
        Index acquireSlot(){
            if(!freeIndexes.empty()){
                Index idx = freeIndexes.back();
                freeIndexes.pop_back();
                return idx;
            }
            slots.emplace_back();
            return slots.size() - 1;
        }
    };
}
