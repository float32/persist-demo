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

#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace demo
{

class FileMemory
{
public:
    static constexpr uint32_t kSize = 256;
    static constexpr uint32_t kEraseGranularity = 64;
    static constexpr uint32_t kWriteGranularity = 16;
    static constexpr uint8_t kFillByte = 0xFF;

    FileMemory(const std::string file_path)
    {
        file_ = fopen(file_path.c_str(), "ab");
        assert(file_ != nullptr);
        file_ = freopen(file_path.c_str(), "r+b", file_);
        assert(file_ != nullptr);

        fseek(file_, 0, SEEK_END);
        uint32_t size = ftell(file_);

        while (size++ < kSize)
        {
            // Pad file to kSize
            fputc(kFillByte, file_);
        }
    }

    ~FileMemory()
    {
        if (file_ != nullptr)
        {
            fclose(file_);
        }
    }

    bool Read(void* dst, uint32_t location, uint32_t size)
    {
        fseek(file_, location, SEEK_SET);
        uint32_t num = fread(dst, 1, size, file_);
        return num == size;
    }

    bool Writable(uint32_t location, uint32_t size)
    {
        if ((location % kWriteGranularity) || (size % kWriteGranularity))
        {
            return false;
        }

        fseek(file_, location, SEEK_SET);

        for (uint32_t i = 0; i < size; i++)
        {
            if (fgetc(file_) != kFillByte)
            {
                return false;
            }
        }

        return true;
    }

    bool Write(uint32_t location, const void* src, uint32_t size)
    {
        fseek(file_, location, SEEK_SET);
        uint32_t num = fwrite(src, 1, size, file_);
        fflush(file_);
        return num == size;
    }

    bool Erase(uint32_t location, uint32_t size)
    {
        if ((location % kEraseGranularity) || (size % kEraseGranularity))
        {
            return false;
        }

        fseek(file_, location, SEEK_SET);

        for (uint32_t i = 0; i < size; i++)
        {
            if (fputc(kFillByte, file_) != kFillByte)
            {
                return false;
            }
        }

        fflush(file_);
        return true;
    }

protected:
    FILE* file_;
};

}
