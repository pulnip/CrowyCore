#pragma once

#include <bit>
#include <cstdlib>
#include <format>
#include <optional>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "assert.hpp"
#include "concepts.hpp"
#include "core_types.hpp"
#include "ptr_util.hpp"
#include "semantics.hpp"

namespace Crowy
{
    template<value_type T>
    void emplace(void* dst, T&& t) noexcept{
        *static_cast<T*>(dst) = std::forward<T>(t);
    }
    template<value_type T1, all_value... TN>
    void emplace(void* dst, T1&& t1, TN&&... tn) noexcept{
        *static_cast<T1*>(dst) = std::forward<T1>(t1);
        emplace(ptrAdd<T1>(dst), std::forward<TN>(tn)...);
    }
    template<pointer_type T>
    void emplace(void* dst, const T t) noexcept{
        using U = std::remove_pointer_t<std::remove_cvref_t<T>>;

        if constexpr(!std::is_null_pointer_v<U>)
            *static_cast<U*>(dst) = *t;
    }
    template<pointer_type T1, all_pointer... TN>
    void emplace(void* dst, const T1 t1, const TN... tn) noexcept{
        using U = std::remove_pointer_t<std::remove_cvref_t<T1>>;

        if constexpr(!std::is_null_pointer_v<T1>){
            *static_cast<U*>(dst) = *t1;
            dst = ptrAdd<U>(dst);
        }
        emplace(dst, tn...);
    }
    template<optional_type T>
    void emplace(void* dst, T&& t) noexcept{
        using U = remove_optional_t<std::remove_cvref_t<T>>;

        if(t.has_value())
            *static_cast<U*>(dst) = t.value();
    }
    template<optional_type T1, all_optional... TN>
    void emplace(void* dst, T1&& t1, TN&&... tn) noexcept{
        using U = remove_optional_t<std::remove_cvref_t<T1>>;

        if(t1.has_value()){
            *static_cast<U*>(dst) = t1.value();
            dst = ptrAdd<U>(dst);
        }
        emplace(dst, std::forward<TN>(tn)...);
    }

    template<typename... T>
    consteval size_t sum_sizeof() noexcept{
        return (size_t{0} + ... + sizeof(T));
    }

    class dynamic_vector{
    private:
        void* mem = nullptr;
        const size_t CHUNK_SIZE;
        size_t size_ = 0;
        size_t cap_ = 0;

    public:
        dynamic_vector() = delete;
        ~dynamic_vector(){
            if(mem != nullptr)
                free(mem);
        }
        CROWY_DECLARE_PINNED(dynamic_vector)

        dynamic_vector(size_t CHUNK_SIZE)
            : CHUNK_SIZE(CHUNK_SIZE){}
        dynamic_vector(size_t CHUNK_SIZE, size_t initial_cap)
            : CHUNK_SIZE(CHUNK_SIZE), cap_(initial_cap)
        {
            if(CHUNK_SIZE != 0 && initial_cap != 0)
                mem = malloc(CHUNK_SIZE*initial_cap);
        }

        class const_iterator;

        class iterator{
        private:
            void* const mem;
            const size_t CHUNK_SIZE;
            Index pos;

            friend const_iterator;

        public:
            iterator(void* mem, size_t CHUNK_SIZE, Index pos)
                :mem(mem), CHUNK_SIZE(CHUNK_SIZE), pos(pos){}
            CROWY_DECLARE_CONSTRUCT_ONLY(iterator)

            void* operator*() noexcept{
                return const_cast<void*>(
                    *static_cast<const iterator&>(*this)
                );
            }
            const void* operator*() const noexcept{
                CROWY_ASSERT((CHUNK_SIZE!=0 || mem != nullptr),
                    "CHUNK_SIZE==0, intentional crash");
                return ptrAdd(mem, CHUNK_SIZE*pos);
            }
            iterator& operator++() noexcept{
                ++pos;
                return *this;
            }
            bool operator!=(const iterator& other) const noexcept{
                CROWY_ASSERT(mem == other.mem);
                return pos != other.pos;
            }
            bool operator==(const iterator& other) const noexcept{
                CROWY_ASSERT(mem == other.mem);
                return pos == other.pos;
            }
            bool operator!=(const const_iterator& other) const noexcept{
                CROWY_ASSERT(mem == other.mem);
                return pos != other.pos;
            }
            bool operator==(const const_iterator& other) const noexcept{
                CROWY_ASSERT(mem == other.mem);
                return pos == other.pos;
            }
        };
        class const_iterator{
        private:
            const void* const mem;
            const size_t CHUNK_SIZE;
            Index pos;

            friend iterator;

        public:
            const_iterator(const void* mem, size_t CHUNK_SIZE, Index pos)
                :mem(mem), CHUNK_SIZE(CHUNK_SIZE), pos(pos){}
            CROWY_DECLARE_CONSTRUCT_ONLY(const_iterator)

            const void* operator*() noexcept{
                return *static_cast<const const_iterator&>(*this);
            }
            const void* operator*() const noexcept{
                CROWY_ASSERT((CHUNK_SIZE!=0 || mem != nullptr),
                    "CHUNK_SIZE==0, intentional crash");
                return ptrAdd(mem, CHUNK_SIZE*pos);
            }
            const_iterator& operator++(){
                ++pos;
                return *this;
            }
            bool operator!=(const const_iterator& other) const noexcept{
                CROWY_ASSERT(mem == other.mem);
                return pos != other.pos;
            }
            bool operator==(const const_iterator& other) const noexcept{
                CROWY_ASSERT(mem == other.mem);
                return pos == other.pos;
            }
            bool operator!=(const iterator& other) const noexcept{
                CROWY_ASSERT(mem == other.mem);
                return pos != other.pos;
            }
            bool operator==(const iterator& other) const noexcept{
                CROWY_ASSERT(mem == other.mem);
                return pos == other.pos;
            }
        };

        void* operator[](Index index) noexcept{
            return const_cast<void*>(
                static_cast<const dynamic_vector&>(*this)[index]
            );
        }
        const void* operator[](Index index) const noexcept{
            CROWY_ASSERT(index < size_);
            CROWY_ASSERT(CHUNK_SIZE!=0, "CHUNK_SIZE==0, intentional crash");
            return ptrAdd(mem, CHUNK_SIZE*index);
        }
        auto  begin()       noexcept{ return       iterator(mem, CHUNK_SIZE,     0); }
        auto    end()       noexcept{ return       iterator(mem, CHUNK_SIZE, size_); }
        auto  begin() const noexcept{ return const_iterator(mem, CHUNK_SIZE,     0); }
        auto    end() const noexcept{ return const_iterator(mem, CHUNK_SIZE, size_); }
        auto cbegin() const noexcept{ return begin();                                }
        auto   cend() const noexcept{ return end();                                  }

        size_t     size() const noexcept{ return size_; }
        size_t capacity() const noexcept{ return  cap_; }
        void resize(size_t new_size){
            if(new_size > cap_){
                reserve(std::bit_ceil(new_size));
            }
            size_ = new_size;
        }
        void reserve(size_t new_cap){
            if(new_cap <= cap_)
                return;

            if(CHUNK_SIZE != 0){
                auto new_mem = realloc(mem, CHUNK_SIZE*new_cap);
                if(new_mem == nullptr)
                    throw std::runtime_error("realloc failed!");
                mem = new_mem;
            }
            cap_ = new_cap;
            CROWY_ASSERT(cap_ >= size_);
        }
        void clear() noexcept{ size_ = 0; }

        template<all_value... T>
        void emplace(T&&... t){
            static_assert(
                (std::is_trivially_copyable_v<std::remove_reference_t<T>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... + sizeof(T));
            CROWY_ASSERT(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = ptrAdd(mem, (size_-1)*CHUNK_SIZE);
            Crowy::emplace(dst, std::forward<T>(t)...);
        }
        template<all_pointer... T>
        void emplace(const T... t){
            static_assert(
                (std::is_trivially_copyable_v<std::remove_pointer_t<T>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... +
                (std::is_null_pointer_v<T> ? 0 : sizeof(std::remove_pointer_t<T>)));
            CROWY_ASSERT(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = ptrAdd(mem, (size_-1)*CHUNK_SIZE);
            Crowy::emplace(dst, t...);
        }
        template<all_optional... T>
        void emplace(T&&... t){
            static_assert(
                (std::is_trivially_copyable_v<remove_optional_t<std::remove_cvref_t<T>>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... +
                (t.has_value() ? sizeof(remove_optional_t<std::remove_cvref_t<T>>) : 0));
            CROWY_ASSERT(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = ptrAdd(mem, (size_-1)*CHUNK_SIZE);
            Crowy::emplace(dst, std::forward<T>(t)...);
        }

        void swap_remove(Index index) noexcept{
            CROWY_ASSERT(index < size_, "swap_remove out of range");
            if(index < size_ - 1 && CHUNK_SIZE > 0)
                memcpy((*this)[index], (*this)[size_-1], CHUNK_SIZE);
            --size_;
        }
    };
}
