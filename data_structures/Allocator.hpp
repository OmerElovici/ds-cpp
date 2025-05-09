#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic, cppcoreguidelines-owning-memory,
// cppcoreguidelines-no-malloc, hicpp-no-malloc)


template <typename T>
struct AllocationResult {
    T* ptr_;
    std::size_t count_;
};


template <typename T>
class Allocator {
private:
    [[nodiscard]] T* do_allocate(
        const std::size_t n,
        const std::size_t alignment = alignof(T)) noexcept {
        constexpr std::size_t kSizePerElement = sizeof(T);
        std::size_t total_size = n * kSizePerElement;

        // Allocate a bit more memory for aligned address
        auto total_allocated_memory = total_size + alignment - 1 + sizeof(void*);
        auto* possibly_unaligned = malloc(total_allocated_memory);
        if (possibly_unaligned == nullptr) {
            return nullptr;
        }

        // Skip the memory space reserved for original pointer. We skip beacuse free() expects the
        // exact pointer returned by malloc.
        auto aligned = reinterpret_cast<std::uintptr_t>(possibly_unaligned) + sizeof(void*);

        // Add alignment -1 to handle potential misalignment
        aligned += alignment - 1;

        // Align the address. removing the lower bits gives value that is multiple of the alignment
        aligned = aligned & ~(alignment - 1); // finds the start of alignment boundary

        // calculate how much to add to original pointer address to store pointer to it
        auto bias = aligned - sizeof(void*) - reinterpret_cast<std::uintptr_t>(possibly_unaligned);
        void** store_original =
            reinterpret_cast<void**>(static_cast<char*>(possibly_unaligned) + bias);
        *store_original = possibly_unaligned;

        return reinterpret_cast<T*>(aligned);
    }

public:
    [[nodiscard]] T* allocate(const std::size_t n) {
        if (n == 0) {
            return nullptr;
        }

        return do_allocate(n);
    }

    [[nodiscard]] T* allocate(const std::size_t n, const std::size_t alignment) {
        if (n == 0) {
            return nullptr;
        }

        // Aligment must be power of 2, and be equel or bigger then alignment of void
        if ((alignment & (alignment - 1)) != 0 || alignment < alignof(void*)) {
            return nullptr;
        }

        return do_allocate(n, alignment);
    }


    void deallcoate(T* ptr, const std::size_t /*n*/) noexcept {
        if (ptr) {
            // convert to pointer aritmetic and move back sizeof(void*) to get to the pointer we
            // stored before in allocate.
            char* original_location = reinterpret_cast<char*>(ptr) - sizeof(void*);
            void* original_ptr = *reinterpret_cast<void**>(original_location);

            // check if pointer has not been nullfied. which means it already been freed
            if (original_ptr != nullptr) {
                free(original_ptr);
                *reinterpret_cast<void**>(reinterpret_cast<char*>(ptr) - sizeof(void*)) = nullptr;
            }
        }
    }


    [[nodiscard]] AllocationResult<T> allocate_at_least(const std::size_t n) {
        T* alloc_ptr = allocate(n);
        return {.ptr_ = alloc_ptr, .count_ = n};
    }
};

// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic, cppcoreguidelines-owning-memory,
// cppcoreguidelines-no-malloc, hicpp-no-malloc)
