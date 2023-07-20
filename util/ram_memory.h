// MIT License
//
// Copyright 2023 Tyler Coy
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdint>
#include <cstring>

namespace demo
{

template <uint32_t size>
struct RamMemory
{
    static constexpr uint32_t kSize = size;
    static constexpr uint32_t kEraseGranularity = 1;
    static constexpr uint32_t kWriteGranularity = 1;
    static constexpr uint8_t kFillByte = 0xFF;

    uint8_t mem_[kSize];

    void Init(void)
    {
        std::memset(mem_, kFillByte, kSize);
    }

    bool Read(void* dst, uint32_t location, uint32_t length)
    {
        auto src = reinterpret_cast<const void*>(&mem_[location]);
        std::memcpy(dst, src, length);
        return true;
    }

    bool Writable(uint32_t, uint32_t)
    {
        return true;
    }

    bool Write(uint32_t location, const void* src, uint32_t length)
    {
        auto dst = reinterpret_cast<void*>(&mem_[location]);
        std::memcpy(dst, src, length);
        return true;
    }

    bool Erase(uint32_t, uint32_t)
    {
        return true;
    }
};


}
