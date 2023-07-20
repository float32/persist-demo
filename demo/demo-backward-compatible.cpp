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

// Demonstrates backward compatibility using Persist's datatype_version and
// LoadLegacy function. The program behaves differently depending on the
// existence of two files in the current directory or in the directory
// specified by the optional first argument passed to the program:
// - If demo_load_save.bin exists but demo_backward_compatible.bin does not
//   exist, the first is copied to the second and backward compatibility is
//   demonstrated.
// - Otherwise, the program behaves much the same as demo-load-save, but using
//   the file demo_backward_compatible.bin.

#include <cstring>
#include <cassert>
#include <cstdio>
#include <filesystem>

#include "persist/persist.h"
#include "util/file_memory.h"

namespace demo
{

extern "C"
int main(int argc, const char* argv[])
{
    std::filesystem::path file_dir = ".";

    if (argc >= 2)
    {
        file_dir = argv[1];
    }

    std::filesystem::path source = file_dir / "demo_load_save.bin";
    std::filesystem::path dest = file_dir / "demo_backward_compatible.bin";

    if (std::filesystem::is_regular_file(source))
    {
        std::filesystem::copy_file(source, dest,
            std::filesystem::copy_options::skip_existing);
    }

    FileMemory nvmem(dest);



    // Version 0 data structure
    struct SaveData0
    {
        uint32_t number;

        void Init(void)
        {
            number = 0;
        }
    };

    // Version 1 data structure
    struct SaveData1
    {
        uint32_t number;
        uint8_t byte;

        void Init(void)
        {
            number = 0;
            byte = 0xFF;
        }

        SaveData1() = default;
        explicit SaveData1(const SaveData0& data0)
        {
            printf("Converting data from version 0 to version 1.\n");
            number = data0.number;
            byte = 0xFF;
        }
    };

    using Persist0 = persist::Persist<FileMemory, SaveData0, 0>;
    using Persist1 = persist::Persist<FileMemory, SaveData1, 1>;

    persist::Result result;

    // Instantiate and initialize Persist for version 1.
    Persist1 persist1{nvmem};
    result = persist1.Init();
    assert(result == persist::RESULT_SUCCESS);

    // Load version 1 data, converting from version 0 if necessary.
    SaveData1 save_data1;
    save_data1.Init();
    result = persist1.LoadLegacy<Persist0>(save_data1);

    if (result == persist::RESULT_FAIL_NO_DATA)
    {
        printf("No valid saved data found.\n");
    }
    else
    {
        printf("Found saved data: %u, %u.\n",
            save_data1.number, save_data1.byte);
        save_data1.number++;
        save_data1.byte--;
    }

    printf("Saving version 1 data: %u, %u.\n",
        save_data1.number, save_data1.byte);
    result = persist1.Save(save_data1);
    assert(result == persist::RESULT_SUCCESS);

    return EXIT_SUCCESS;
}

}
