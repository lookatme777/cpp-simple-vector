#pragma once
#include <cassert>
#include <iostream>
#include <numeric>
#include <initializer_list>
#include <memory>
#include <stdexcept>

using namespace std;

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t i) {
        capacity_to_reserve = i;
    }
    size_t capacity_to_reserve;
};

template <typename Type>
class SimpleVector
{
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    std::unique_ptr<Type[]> array_;

public:
    using Iterator = Type*;
    using ConstIterator = const Type*;


    SimpleVector() noexcept = default;

    SimpleVector(ReserveProxyObj other) {
        // ???????? ???? ???????????? ??????????????
        Reserve(other.capacity_to_reserve);
    }


    explicit SimpleVector(size_t size)
        : size_(size), capacity_(size), array_(std::make_unique<Type[]>(capacity_))
    {
    }

    SimpleVector(size_t size, const Type& value)
        : size_(size), capacity_(size), array_(std::make_unique<Type[]>(capacity_)) {
        for (size_t i = 0; i < size_; ++i) {
            array_[i] = value;
        }
    }

    SimpleVector(std::initializer_list<Type> init)
        : size_(init.size()), capacity_(init.size()), array_(std::make_unique<Type[]>(capacity_))
    {
        size_t i = 0;
        for (const auto& item : init)
        {
            array_[i] = item;
            ++i;
        }
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        return array_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return array_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index is out of range");
        }
        return array_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index is out of range");
        }
        return array_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            auto new_array = std::make_unique<Type[]>(new_size);
            for (size_t i = 0; i < size_; ++i) {
                new_array[i] = std::move(array_[i]);
            }
            array_ = std::move(new_array);
            capacity_ = new_size;
        }
        for (size_t i = size_; i < new_size; ++i) {
            array_[i] = Type();
        }
        size_ = new_size;
    }

    Iterator begin() noexcept {
        return array_.get();
    }

    Iterator end() noexcept {
        return array_.get() + size_;
    }

    ConstIterator begin() const noexcept {
        return array_.get();
    }

    ConstIterator end() const noexcept {
        return array_.get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return array_.get();
    }

    ConstIterator cend() const noexcept {
        return array_.get() + size_;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(array_, other.array_);
    }

    Iterator Erase(ConstIterator pos) {
        const size_t idx = pos - cbegin();
        for (size_t i = idx + 1; i != size_; ++i) {
            array_[i - 1] = std::move(array_[i]);
        }
        --size_;
        return begin() + idx;
    }

    void PopBack() noexcept {
        --size_;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        const size_t idx = pos - cbegin();
        if (size_ == capacity_) {
            capacity_ = capacity_ == 0 ? 1 : 2 * capacity_;
            auto new_array = std::make_unique<Type[]>(capacity_);
            for (size_t i = 0; i != idx; ++i) {
                new_array[i] = std::move(array_[i]);
            }
            new_array[idx] = std::move(value);
            for (size_t i = idx; i != size_; ++i) {
                new_array[i + 1] = std::move(array_[i]);
            }
            array_ = std::move(new_array);
        }
        else {
            for (size_t i = size_; i != idx; --i) {
                array_[i] = std::move(array_[i - 1]);
            }
            array_[idx] = std::move(value);
        }
        ++size_;
        return begin() + idx;
    }


    void PushBack(Type&& item) {
        Insert(end(), std::move(item));
    }


    SimpleVector& operator=(const SimpleVector& rhs) {
        if (&rhs == this)
            return *this;
        SimpleVector tmp(rhs);
        swap(tmp);
        return *this;
    }

    SimpleVector(const SimpleVector& other)
        : size_(other.GetSize())
        , capacity_(other.GetCapacity())
        , array_(std::make_unique<Type[]>(capacity_))
    {
        std::copy(other.begin(), other.end(), begin());
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            auto new_array = std::make_unique<Type[]>(new_capacity);
            std::copy(begin(), end(), new_array.get());
            array_ = std::move(new_array);
            capacity_ = new_capacity;
        }
    }

    SimpleVector(SimpleVector&& other) noexcept {
        *this = std::move(other);
    }

    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (this != &rhs) {
            size_ = rhs.size_;
            capacity_ = rhs.capacity_;
            array_ = std::move(rhs.array_);
            rhs.size_ = 0;
            rhs.capacity_ = 0;
        }
        return *this;
    }

};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template<typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template<typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
