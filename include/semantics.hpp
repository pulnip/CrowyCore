#pragma once

// copy - move semantics
#define DECLARE_DEFAULT_COPYABLE(Type) \
    Type(const Type&) = default; \
    Type& operator=(const Type&) = default;
#define DECLARE_DEFAULT_MOVABLE(Type) \
    Type(Type&&) = default; \
    Type& operator=(Type&&) = default;
#define DECLARE_NON_COPYABLE(Type) \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete;
#define DECLARE_NON_MOVABLE(Type) \
    Type(Type&&) = delete; \
    Type& operator=(Type&&) = delete;

#define DECLARE_PINNED(Type) \
    DECLARE_NON_COPYABLE(Type) \
    DECLARE_NON_MOVABLE(Type)
#define DECLARE_COPY_ONLY(Type) \
    DECLARE_DEFAULT_COPYABLE(Type) \
    DECLARE_NON_MOVABLE(Type)
#define DECLARE_MOVE_ONLY(Type) \
    DECLARE_NON_COPYABLE(Type) \
    DECLARE_DEFAULT_MOVABLE(Type)
#define DECLARE_TRANSFERABLE(Type) \
    DECLARE_DEFAULT_COPYABLE(Type) \
    DECLARE_DEFAULT_MOVABLE(Type)

// interface semantics
#define DECLARE_INTERFACE(Type) \
    Type() = default; \
    virtual ~Type() = default; \
    DECLARE_PINNED(Type)
