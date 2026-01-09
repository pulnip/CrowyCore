#pragma once

// copy - move semantics
#define CROWY_DECLARE_DEFAULT_COPYABLE(Type) \
    Type(const Type&) = default; \
    Type& operator=(const Type&) = default;
#define CROWY_DECLARE_DEFAULT_MOVABLE(Type) \
    Type(Type&&) = default; \
    Type& operator=(Type&&) = default;
#define CROWY_DECLARE_NON_COPYABLE(Type) \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete;
#define CROWY_DECLARE_NON_MOVABLE(Type) \
    Type(Type&&) = delete; \
    Type& operator=(Type&&) = delete;

#define CROWY_DECLARE_PINNED(Type) \
    CROWY_DECLARE_NON_COPYABLE(Type) \
    CROWY_DECLARE_NON_MOVABLE(Type)
#define CROWY_DECLARE_COPY_ONLY(Type) \
    CROWY_DECLARE_DEFAULT_COPYABLE(Type) \
    CROWY_DECLARE_NON_MOVABLE(Type)
#define CROWY_DECLARE_MOVE_ONLY(Type) \
    CROWY_DECLARE_NON_COPYABLE(Type) \
    CROWY_DECLARE_DEFAULT_MOVABLE(Type)
#define CROWY_DECLARE_TRANSFERABLE(Type) \
    CROWY_DECLARE_DEFAULT_COPYABLE(Type) \
    CROWY_DECLARE_DEFAULT_MOVABLE(Type)

#define CROWY_DECLARE_NON_ASSIGNABLE(Type) \
    Type& operator=(const Type&) = delete; \
    Type& operator=(Type&&) = delete;
#define CROWY_DECLARE_CONSTRUCT_ONLY(Type) \
    Type(const Type&) = default; \
    Type(Type&&) = default; \
    CROWY_DECLARE_NON_ASSIGNABLE(Type)

// interface semantics
#define CROWY_DECLARE_INTERFACE(Type) \
    Type() = default; \
    virtual ~Type() = default; \
    CROWY_DECLARE_PINNED(Type)
