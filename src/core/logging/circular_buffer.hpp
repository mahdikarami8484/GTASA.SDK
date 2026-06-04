/**
 * @file circular_buffer.hpp
 * @brief Thread-safe circular buffer implementation
 *
 * This file provides a template-based circular buffer with thread-safe
 * operations for producer-consumer scenarios.
 */

#pragma once

#include <array>
#include <mutex>

namespace Logging
{

    /**
     * @brief Thread-safe circular buffer for fixed-size queues
     *
     * A circular buffer (ring buffer) that provides thread-safe push and pop
     * operations. When full, push operations will fail rather than overwrite
     * existing data.
     *
     * @tparam T The type of elements stored in the buffer
     * @tparam Size The maximum number of elements the buffer can hold
     */
    template <typename T, size_t Size> class CircularBuffer
    {
    public:
        CircularBuffer() : head(0), tail(0), full(false) {}

        /**
         * @brief Push an item to the buffer
         * @param item The item to push
         * @return true if successful, false if buffer is full
         */
        bool push(const T& item)
        {
            std::lock_guard<std::mutex> lock(mutex);

            if (full) return false;

            buffer[tail] = item;
            tail = (tail + 1) % Size;

            if (tail == head) full = true;

            return true;
        }

        /**
         * @brief Pop an item from the buffer
         * @param out Reference to store the popped item
         * @return true if successful, false if buffer is empty
         */
        bool pop(T& out)
        {
            std::lock_guard<std::mutex> lock(mutex);

            if (empty()) return false;

            out = buffer[head];
            head = (head + 1) % Size;
            full = false;
            return true;
        }

        /**
         * @brief Check if the buffer is empty
         * @return true if empty, false otherwise
         */
        bool empty() const
        {
            return (!full && head == tail);
        }

        /**
         * @brief Check if the buffer is full
         * @return true if full, false otherwise
         */
        bool isFull() const
        {
            return full;
        }

    private:
        std::array<T, Size> buffer;
        size_t head;
        size_t tail;
        bool full;
        mutable std::mutex mutex;
    };

} // namespace Logging