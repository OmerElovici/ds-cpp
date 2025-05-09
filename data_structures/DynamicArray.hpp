#pragma once

#include <cstddef>

// TODO continue after finishing Allocator

namespace DataStructures {

template <typename T>
class DynamicArray {
private:
    T* data_;
    std::size_t count_;
    std::size_t capacity_;
};


} // namespace DataStructures