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
#include <random>
#include <set>
#include <vector>
#include <algorithm>

#include <gtest/gtest.h>

#include "persist/persist.h"
#include "util/ram_memory.h"

namespace persist::test
{

template <uint32_t size, uint32_t erase_granularity, uint32_t write_granularity>
struct Memory
{
    static constexpr uint32_t kSize = size;
    static constexpr uint32_t kEraseGranularity = erase_granularity;
    static constexpr uint32_t kWriteGranularity = write_granularity;
    static constexpr uint8_t kFillByte = 0xFF;

    uint8_t mem_[kSize];
    uint32_t write_histogram_[kSize];
    uint32_t erase_histogram_[kSize];
    uint32_t write_count_;
    uint32_t erase_count_;

    void Init(void)
    {
        memset(mem_, kFillByte, kSize);

        for (uint32_t i = 0; i < kSize; i++)
        {
            write_histogram_[i] = 0;
            erase_histogram_[i] = 0;
            write_count_ = 0;
            erase_count_ = 0;
        }
    }

    bool Read(void* dst, uint32_t location, uint32_t length)
    {
        if (!Accessible(location, length))
        {
            return false;
        }

        auto src = reinterpret_cast<const void*>(&mem_[location]);
        memcpy(dst, src, length);
        return true;
    }

    bool Writable(uint32_t location, uint32_t length)
    {
        if (!Accessible(location, length))
        {
            return false;
        }

        if ((location % kWriteGranularity) || (length % kWriteGranularity))
        {
            return false;
        }

        for (uint32_t i = 0; i < length; i++)
        {
            if (mem_[location + i] != kFillByte)
            {
                return false;
            }
        }

        return true;
    }

    bool Write(uint32_t location, const void* src, uint32_t length)
    {
        auto dst = reinterpret_cast<void*>(&mem_[location]);
        memcpy(dst, src, length);
        write_count_ += length;

        for (uint32_t i = 0; i < length; i++)
        {
            write_histogram_[location + i]++;
        }

        return true;
    }

    bool Erase(uint32_t location, uint32_t length)
    {
        if (!Erasable(location, length))
        {
            return false;
        }

        auto dst = reinterpret_cast<void*>(&mem_[location]);
        memset(dst, kFillByte, length);
        erase_count_ += length;

        for (uint32_t i = 0; i < length; i++)
        {
            erase_histogram_[location + i]++;
        }

        return true;
    }

    bool Accessible(uint32_t location, uint32_t length)
    {
        return (location + length) <= kSize;
    }

    bool Erasable(uint32_t location, uint32_t length)
    {
        return Accessible(location, length) &&
            (location % kEraseGranularity == 0) &&
            (length % kEraseGranularity == 0);
    }

    void Fill(uint8_t byte)
    {
        std::fill_n(mem_, kSize, byte);
    }
};

template <int A, int B, int C, int D>
using ParamType = std::tuple<
    std::integral_constant<int, A>,
    std::integral_constant<int, B>,
    std::integral_constant<int, C>,
    std::integral_constant<int, D>>;
using ParamTypeList = ::testing::Types<
    /*[[[cog
    import itertools

    mem_size          = (100, 256, 4096)
    erase_granularity = (1, 4, 256, 1024)
    write_granularity = (1, 4, 32)
    data_size         = (1, 4, 15, 150)

    lines = []
    for params in itertools.product(
            mem_size, erase_granularity, write_granularity, data_size):
        mem_size, erase_granularity, write_granularity, data_size = params
        if erase_granularity <= mem_size and data_size < mem_size:
            lines.append('ParamType<{:4}, {:4}, {:2}, {:3}>'.format(*params))
    cog.outl(',\n'.join(lines))
    ]]]*/
    ParamType< 100,    1,  1,   1>,
    ParamType< 100,    1,  1,   4>,
    ParamType< 100,    1,  1,  15>,
    ParamType< 100,    1,  4,   1>,
    ParamType< 100,    1,  4,   4>,
    ParamType< 100,    1,  4,  15>,
    ParamType< 100,    1, 32,   1>,
    ParamType< 100,    1, 32,   4>,
    ParamType< 100,    1, 32,  15>,
    ParamType< 100,    4,  1,   1>,
    ParamType< 100,    4,  1,   4>,
    ParamType< 100,    4,  1,  15>,
    ParamType< 100,    4,  4,   1>,
    ParamType< 100,    4,  4,   4>,
    ParamType< 100,    4,  4,  15>,
    ParamType< 100,    4, 32,   1>,
    ParamType< 100,    4, 32,   4>,
    ParamType< 100,    4, 32,  15>,
    ParamType< 256,    1,  1,   1>,
    ParamType< 256,    1,  1,   4>,
    ParamType< 256,    1,  1,  15>,
    ParamType< 256,    1,  1, 150>,
    ParamType< 256,    1,  4,   1>,
    ParamType< 256,    1,  4,   4>,
    ParamType< 256,    1,  4,  15>,
    ParamType< 256,    1,  4, 150>,
    ParamType< 256,    1, 32,   1>,
    ParamType< 256,    1, 32,   4>,
    ParamType< 256,    1, 32,  15>,
    ParamType< 256,    1, 32, 150>,
    ParamType< 256,    4,  1,   1>,
    ParamType< 256,    4,  1,   4>,
    ParamType< 256,    4,  1,  15>,
    ParamType< 256,    4,  1, 150>,
    ParamType< 256,    4,  4,   1>,
    ParamType< 256,    4,  4,   4>,
    ParamType< 256,    4,  4,  15>,
    ParamType< 256,    4,  4, 150>,
    ParamType< 256,    4, 32,   1>,
    ParamType< 256,    4, 32,   4>,
    ParamType< 256,    4, 32,  15>,
    ParamType< 256,    4, 32, 150>,
    ParamType< 256,  256,  1,   1>,
    ParamType< 256,  256,  1,   4>,
    ParamType< 256,  256,  1,  15>,
    ParamType< 256,  256,  1, 150>,
    ParamType< 256,  256,  4,   1>,
    ParamType< 256,  256,  4,   4>,
    ParamType< 256,  256,  4,  15>,
    ParamType< 256,  256,  4, 150>,
    ParamType< 256,  256, 32,   1>,
    ParamType< 256,  256, 32,   4>,
    ParamType< 256,  256, 32,  15>,
    ParamType< 256,  256, 32, 150>,
    ParamType<4096,    1,  1,   1>,
    ParamType<4096,    1,  1,   4>,
    ParamType<4096,    1,  1,  15>,
    ParamType<4096,    1,  1, 150>,
    ParamType<4096,    1,  4,   1>,
    ParamType<4096,    1,  4,   4>,
    ParamType<4096,    1,  4,  15>,
    ParamType<4096,    1,  4, 150>,
    ParamType<4096,    1, 32,   1>,
    ParamType<4096,    1, 32,   4>,
    ParamType<4096,    1, 32,  15>,
    ParamType<4096,    1, 32, 150>,
    ParamType<4096,    4,  1,   1>,
    ParamType<4096,    4,  1,   4>,
    ParamType<4096,    4,  1,  15>,
    ParamType<4096,    4,  1, 150>,
    ParamType<4096,    4,  4,   1>,
    ParamType<4096,    4,  4,   4>,
    ParamType<4096,    4,  4,  15>,
    ParamType<4096,    4,  4, 150>,
    ParamType<4096,    4, 32,   1>,
    ParamType<4096,    4, 32,   4>,
    ParamType<4096,    4, 32,  15>,
    ParamType<4096,    4, 32, 150>,
    ParamType<4096,  256,  1,   1>,
    ParamType<4096,  256,  1,   4>,
    ParamType<4096,  256,  1,  15>,
    ParamType<4096,  256,  1, 150>,
    ParamType<4096,  256,  4,   1>,
    ParamType<4096,  256,  4,   4>,
    ParamType<4096,  256,  4,  15>,
    ParamType<4096,  256,  4, 150>,
    ParamType<4096,  256, 32,   1>,
    ParamType<4096,  256, 32,   4>,
    ParamType<4096,  256, 32,  15>,
    ParamType<4096,  256, 32, 150>,
    ParamType<4096, 1024,  1,   1>,
    ParamType<4096, 1024,  1,   4>,
    ParamType<4096, 1024,  1,  15>,
    ParamType<4096, 1024,  1, 150>,
    ParamType<4096, 1024,  4,   1>,
    ParamType<4096, 1024,  4,   4>,
    ParamType<4096, 1024,  4,  15>,
    ParamType<4096, 1024,  4, 150>,
    ParamType<4096, 1024, 32,   1>,
    ParamType<4096, 1024, 32,   4>,
    ParamType<4096, 1024, 32,  15>,
    ParamType<4096, 1024, 32, 150>
    //[[[end]]]
    >;

template <typename T>
class PersistTest : public ::testing::Test
{
public:
    static constexpr int kMemSize          = std::tuple_element_t<0, T>::value;
    static constexpr int kEraseGranularity = std::tuple_element_t<1, T>::value;
    static constexpr int kWriteGranularity = std::tuple_element_t<2, T>::value;
    static constexpr int kDataSize         = std::tuple_element_t<3, T>::value;

    using MemType = Memory<kMemSize, kEraseGranularity, kWriteGranularity>;

    struct Payload
    {
        uint8_t data[kDataSize];
    };

    static constexpr uint8_t kVersion = 100;
    using PersistType = Persist<MemType, Payload, kVersion, false>;

    MemType mem_;
    Payload payload_;
    PersistType persist_{mem_};

    void SetUp() override
    {
        mem_.Init();
        persist_.Init();
    }

    void RandomizePayload(int seed)
    {
        std::minstd_rand rng;
        rng.seed(seed);
        rng.discard(1);
        std::uniform_int_distribution<uint8_t> dist;

        for (auto& byte : payload_.data)
        {
            byte = dist(rng);
        };
    }

    void FillPayload(uint8_t byte)
    {
        std::fill_n(payload_.data, kDataSize, byte);
    }
};

TYPED_TEST_CASE(PersistTest, ParamTypeList);

TYPED_TEST(PersistTest, FirstReadFails)
{
    int result = this->persist_.Load(this->payload_);
    ASSERT_EQ(result, persist::RESULT_FAIL_NO_DATA);
}

TYPED_TEST(PersistTest, WriteRead)
{
    std::minstd_rand rng;
    std::uniform_int_distribution<uint8_t> dist;
    int result;

    for (uint32_t i = 0; i < 10000; i++)
    {
        this->RandomizePayload(i + 1);

        // Write payload to memory
        result = this->persist_.Save(this->payload_);
        ASSERT_EQ(result, persist::RESULT_SUCCESS);

        // Clear payload data
        this->FillPayload(0);

        // Read payload from memory
        typename TestFixture::PersistType read_persist{this->mem_};
        read_persist.Init();
        result = read_persist.Load(this->payload_);
        ASSERT_EQ(result, persist::RESULT_SUCCESS);

        // Verify payload
        rng.seed(i + 1);
        rng.discard(1);

        for (auto byte : this->payload_.data)
        {
            uint8_t expected = dist(rng);
            ASSERT_EQ(expected, byte);
        }
    }
}

TYPED_TEST(PersistTest, WearLeveling)
{
    for (uint32_t i = 0; i < 10000; i++)
    {
        this->RandomizePayload(i + 1);

        // Write payload to memory
        this->persist_.Save(this->payload_);
    }

    std::set<uint32_t> values;
    std::vector<uint32_t> view;

    // Number of erases for each byte should be zero or within one of other
    // nonzero values
    values.clear();
    for (uint32_t i = 0; i < this->mem_.kSize; i++)
    {
        uint32_t value = this->mem_.erase_histogram_[i];
        if (value != 0)
        {
            values.insert(value);
        }
    }
    ASSERT_GE(values.size(), 1);
    ASSERT_LE(values.size(), 2);
    if (values.size() == 2)
    {
        view.clear();
        std::copy(values.begin(), values.end(), std::back_inserter(view));
        uint32_t delta = view[1] - view[0];
        ASSERT_EQ(delta, 1);
    }

    // Number of writes for each byte should be zero or within one of other
    // nonzero values
    values.clear();
    for (uint32_t i = 0; i < this->mem_.kSize; i++)
    {
        uint32_t value = this->mem_.write_histogram_[i];
        if (value != 0)
        {
            values.insert(value);
        }
    }
    ASSERT_GE(values.size(), 1);
    ASSERT_LE(values.size(), 2);
    if (values.size() == 2)
    {
        view.clear();
        std::copy(values.begin(), values.end(), std::back_inserter(view));
        uint32_t delta = view[1] - view[0];
        ASSERT_EQ(delta, 1);
    }
}

TYPED_TEST(PersistTest, Tamper)
{
    int result;

    this->RandomizePayload(0);

    // Write payload to memory
    result = this->persist_.Save(this->payload_);
    ASSERT_EQ(result, persist::RESULT_SUCCESS);

    // Tamper with memory
    this->mem_.mem_[0] ^= 1;

    // Read payload from memory
    typename TestFixture::PersistType read_persist{this->mem_};
    read_persist.Init();
    result = read_persist.Load(this->payload_);
    ASSERT_EQ(result, persist::RESULT_FAIL_NO_DATA);
}

TYPED_TEST(PersistTest, WrongVersion)
{
    int result;

    this->RandomizePayload(0);

    // Write payload to memory
    result = this->persist_.Save(this->payload_);
    ASSERT_EQ(result, persist::RESULT_SUCCESS);

    // Read payload from memory
    using MemType = typename TestFixture::MemType;
    using Payload = typename TestFixture::Payload;
    uint8_t constexpr kVersion = TestFixture::kVersion;

    Persist<MemType, Payload, kVersion - 1, false> older{this->mem_};
    older.Init();
    result = older.Load(this->payload_);
    ASSERT_EQ(result, persist::RESULT_FAIL_NO_DATA);

    Persist<MemType, Payload, kVersion + 1, false> newer{this->mem_};
    newer.Init();
    result = newer.Load(this->payload_);
    ASSERT_EQ(result, persist::RESULT_FAIL_NO_DATA);
}

TYPED_TEST(PersistTest, AllZeros)
{
    int result;

    this->RandomizePayload(0);

    using PersistType = typename TestFixture::PersistType;

    // Write payload to memory
    result = this->persist_.Save(this->payload_);
    ASSERT_EQ(result, persist::RESULT_SUCCESS);

    // Fill memory with zeros
    this->mem_.Fill(0);

    // Read payload from memory
    PersistType read_persist{this->mem_};
    read_persist.Init();
    result = read_persist.Load(this->payload_);
    ASSERT_EQ(result, persist::RESULT_FAIL_NO_DATA);
}

TYPED_TEST(PersistTest, AllOnes)
{
    int result;

    this->RandomizePayload(0);

    using PersistType = typename TestFixture::PersistType;

    // Write payload to memory
    result = this->persist_.Save(this->payload_);
    ASSERT_EQ(result, persist::RESULT_SUCCESS);

    // Fill memory with ones
    this->mem_.Fill(0xFF);

    // Read payload from memory
    PersistType read_persist{this->mem_};
    read_persist.Init();
    result = read_persist.Load(this->payload_);
    ASSERT_EQ(result, persist::RESULT_FAIL_NO_DATA);
}

TYPED_TEST(PersistTest, SameData)
{
    int result;
    ASSERT_EQ(this->mem_.write_count_, 0);

    for (uint32_t i = 0; i < 1000; i++)
    {
        this->RandomizePayload(i + 1);

        // Write payload to memory
        uint32_t write_count = this->mem_.write_count_;
        result = this->persist_.Save(this->payload_);
        ASSERT_EQ(result, persist::RESULT_SUCCESS);

        ASSERT_GT(this->mem_.write_count_, write_count);

        // Write same payload to memory
        write_count = this->mem_.write_count_;
        result = this->persist_.Save(this->payload_);
        ASSERT_EQ(result, persist::RESULT_SUCCESS);

        // Write count should not increase
        ASSERT_EQ(this->mem_.write_count_, write_count);
    }
}



TEST(PersistTest, SequenceNumberWrap)
{
    // Make the memory large enough to hold the maximum number of Blocks
    using MemType = demo::RamMemory<1024 * 1024>;
    using PersistType = Persist<MemType, int32_t, 0>;

    // Allocate on the heap to prevent segfault
    MemType& memory = *(new MemType);
    memory.Init();
    PersistType persist{memory};

    persist::Result result = persist.Init();
    ASSERT_EQ(result, persist::RESULT_SUCCESS);

    for (int32_t i = 0; i < 10000000; i++)
    {
        // Write data to memory
        result = persist.Save(i);
        ASSERT_EQ(result, persist::RESULT_SUCCESS);

        // Read and verify data every time the maximum number of Blocks were
        // written
        if ((i & 0x7FFF) == 0)
        {
            PersistType read_persist{memory};
            result = read_persist.Init();
            ASSERT_EQ(result, persist::RESULT_SUCCESS);
            int32_t data = -1;
            result = read_persist.Load(data);
            ASSERT_EQ(result, persist::RESULT_SUCCESS);
            ASSERT_EQ(data, i);
        }
    }

    delete &memory;
}

}
