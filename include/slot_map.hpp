#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <format>
#include <iterator>
#include <new>
#include <ranges>
#include <vector>
#include "generic_handle.hpp"
#include "math.hpp"
#include "core_types.hpp"

namespace Crowy
{
    template<typename T>
    class slot_map{
    private:
        using Handle = generic_handle<T>;

        struct Slot{
            alignas(T) std::byte storage[sizeof(T)];
            uint32_t generation = 0;
            bool isUsing = false;

            T* get(){
                return const_cast<T*>(
                    static_cast<const Slot&>(*this).get()
                );
            }
            const T* get() const{
                return std::launder(
                    reinterpret_cast<const T*>(&storage)
                );
            }

            Slot() = default;
        };
        std::vector<Slot> slots;
        using Indexes = std::vector<Index>;
        Indexes freeIndexes;

    public:
        slot_map() = default;
        ~slot_map(){
            std::sort(freeIndexes.begin(), freeIndexes.end());

            Index freeIdxPtr = 0;
            for(Index i=0; i<slots.size(); ++i){
                if(freeIdxPtr<freeIndexes.size() && freeIndexes[freeIdxPtr]==i){
                    ++freeIdxPtr;
                    continue;
                }
                std::destroy_at(slots[i].get());
            }
        }

        Handle push(T&& t){
            Index freeIndex = std::numeric_limits<uint32_t>::max();

            if(freeIndexes.size() > 0){
                freeIndex = freeIndexes[freeIndexes.size() - 1];
                freeIndexes.resize(freeIndexes.size() - 1);
            }
            else{
                slots.resize(slots.size() + 1);
                freeIndex = slots.size() - 1;
            }
            std::construct_at(slots[freeIndex].get(), std::move(t));
            ++slots[freeIndex].generation;
            slots[freeIndex].isUsing = true;

            return {
                .index = freeIndex,
                .generation = slots[freeIndex].generation
            };
        }

        template<typename... Args>
        Handle emplace(Args&&... args){
            return push(T(std::forward<Args>(args)...));
        }

        void remove(Handle handle){
            auto outOfIndex = handle.index >= slots.size();
            if(outOfIndex)
                throw std::out_of_range(std::format(
                    "Handle index {} is out of range (size={})",
                    handle.index, slots.size()
                ));

            auto expiredGen = slots[handle.index].generation != handle.generation;
            if(expiredGen)
                throw std::out_of_range(std::format(
                    "Handle(index={}) has expired: generation {} != {}",
                    handle.index, handle.generation, slots[handle.index].generation
                ));

            slots[handle.index].isUsing = false;
            ++slots[handle.index].generation;
            std::destroy_at(slots[handle.index].get());
            freeIndexes.push_back(handle.index);
        }

        void clear(){
            std::sort(freeIndexes.begin(), freeIndexes.end());

            Index freeIdxPtr = 0;
            for(Index i=0; i<slots.size(); ++i){
                if(freeIdxPtr<freeIndexes.size() && freeIndexes[freeIdxPtr]==i){
                    ++freeIdxPtr;
                    continue;
                }
                slots[i].isUsing = false;
                std::destroy_at(slots[i].get());
            }

            freeIndexes.clear();
            for(Index i=0; i<slots.size(); ++i){
                ++slots[i].generation;
                freeIndexes.push_back(i);
            }
        }

        T* find(Handle handle){
            return const_cast<T&>(
                static_cast<const slot_map&>(*this).find(handle)
            );
        }
        const T* find(Handle handle) const{
            auto outOfIndex = handle.index >= slots.size();
            if(outOfIndex)
                return nullptr;

            auto expiredGen = slots[handle.index].generation != handle.generation;
            if(expiredGen)
                return nullptr;

            return slots[handle.index].get();
        }
        T& operator[](Handle handle){
            return const_cast<T&>(
                static_cast<const slot_map&>(*this)[handle]
            );
        }
        const T& operator[](Handle handle) const{
            auto outOfIndex = handle.index >= slots.size();
            if(outOfIndex)
                throw std::out_of_range(std::format(
                    "Handle index {} is out of range (size={})",
                    handle.index, slots.size()
                ));

            auto expiredGen = slots[handle.index].generation != handle.generation;
            if(expiredGen)
                throw std::out_of_range(std::format(
                    "Handle(index={}) has expired: generation {} != {}",
                    handle.index, handle.generation, slots[handle.index].generation
                ));

            return *slots[handle.index].get();
        }

        void reserve(size_t size){
            if(size <= slots.size())
                return;
            for(auto i=slots.size(); i<size; ++i)
                freeIndexes.push_back(i);
            slots.resize(size);
        }

        size_t size() const{
            return slots.size() - freeIndexes.size();
        }
        size_t capacity() const{
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

            void skip_free(){
                while(index < slots->size() && !(*slots)[index].isUsing)
                    ++index;
            }

        public:
            iterator(std::vector<Slot>* s, Index i)
                : slots(s), index(i) { skip_free(); }

            reference operator*(){
                return *(*slots)[index].get();
            }
            pointer operator->(){
                return (*slots)[index].get();
            }

            iterator& operator++(){
                ++index;
                skip_free();
                return *this;
            }
            iterator operator++(int){
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const{
                assert(slots == other.slots);
                return index == other.index;
            }
            bool operator!=(const iterator& other) const{
                assert(slots == other.slots);
                return index != other.index;
            }
            bool operator==(const const_iterator& other) const{
                assert(slots == other.slots);
                return index == other.index;
            }
            bool operator!=(const const_iterator& other) const{
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

            void skip_free(){
                while(index < slots->size() && !(*slots)[index].occupied)
                    ++index;
            }

        public:
            const_iterator(std::vector<Slot>* s, Index i)
                : slots(s), index(i) { skip_free(); }

            reference operator*(){
                return *(*slots)[index].get();
            }
            pointer operator->(){
                return (*slots)[index].get();
            }

            const_iterator& operator++(){
                ++index;
                skip_free();
                return *this;
            }
            const_iterator operator++(int){
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const{
                assert(slots == other.slots);
                return index == other.index;
            }
            bool operator!=(const iterator& other) const{
                assert(slots == other.slots);
                return index != other.index;
            }
            bool operator==(const const_iterator& other) const{
                assert(slots == other.slots);
                return index == other.index;
            }
            bool operator!=(const const_iterator& other) const{
                assert(slots == other.slots);
                return index != other.index;
            }
        };

        iterator begin(){
            return iterator(&slots, 0);
        }
        iterator end(){
            return iterator(&slots, static_cast<Index>(slots.size()));
        }
        const_iterator begin() const{
            return const_iterator(&slots, 0);
        }
        const_iterator end() const{
            return const_iterator(&slots, static_cast<Index>(slots.size()));
        }
        const_iterator cbegin() const{
            return begin();
        }
        const_iterator cend() const{
            return end();
        }
    };
}
