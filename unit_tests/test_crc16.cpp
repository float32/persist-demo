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
#include <random>
#include <gtest/gtest.h>
#include "persist/inc/crc16.h"

extern "C"
{
#include "crcany/crc.h"
}

namespace persist::test
{

static constexpr uint32_t kTestLength = 1e6;
static uint8_t data_[kTestLength];

TEST(CRC32Test, RandomData)
{
    std::minstd_rand rng;
    std::uniform_int_distribution<uint8_t> dist(0, 0xFF);
    rng.seed(0);

    for (uint32_t i = 0; i < kTestLength; i++)
    {
        data_[i] = dist(rng);
    }

    Crc16 crc;
    crc.Init();
    uint32_t expected;
    uint32_t actual;

    char desc[] = "width=16 poly=0x1021 init=0xffff refin=false refout=false "
        "xorout=0x0000 check=0x29b1 residue=0x0000 name=CRC-16/IBM-3740";
    model_t model;
    read_model(&model, desc, false);



    expected = crc_bitwise(&model, 0, data_, kTestLength);
    crc.Seed(0);
    actual = crc.Process(data_, kTestLength);
    ASSERT_EQ(expected, actual);



    expected = crc_bitwise(&model, expected, data_, kTestLength);
    actual = crc.Process(data_, kTestLength);
    ASSERT_EQ(expected, actual);



    data_[kTestLength / 2] = ~data_[kTestLength / 2];
    crc.Seed(0);
    actual = crc.Process(data_, kTestLength);
    ASSERT_NE(expected, actual);

    expected = crc_bitwise(&model, 0, data_, kTestLength);
    ASSERT_EQ(expected, actual);



    expected = crc_bitwise(&model, 12345, data_, kTestLength);
    crc.Seed(12345);
    actual = crc.Process(data_, kTestLength);
    ASSERT_EQ(expected, actual);
}

}
