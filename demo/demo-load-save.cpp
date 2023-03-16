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

// Demonstrates load and save functionality of Persist using a file as
// nonvolatile memory. The file path is demo_load_save.bin, either in the
// current directory or in the directory specified by the optional first
// argument passed to the program.

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

    FileMemory nvmem(file_dir / "demo_load_save.bin");



    // Our persistent data structure.
    struct SaveData
    {
        uint32_t number;

        void Init(void)
        {
            number = 0;
        }
    };

    persist::Result result;

    // Instantiate and initialize Persist.
    persist::Persist<FileMemory, SaveData, 0> persist{nvmem};
    result = persist.Init();
    assert(result == persist::RESULT_SUCCESS);

    // We initialize our data structure before attempting to load from Persist.
    // If the load fails, our initialized data will be left intact.
    SaveData save_data;
    save_data.Init();
    result = persist.Load(save_data);

    if (result == persist::RESULT_FAIL_NO_DATA)
    {
        printf("No valid saved data found.\n");
    }
    else
    {
        printf("Found saved data: %u.\n", save_data.number);
        save_data.number++;
    }

    printf("Saving data: %u.\n", save_data.number);
    result = persist.Save(save_data);
    assert(result == persist::RESULT_SUCCESS);



    return EXIT_SUCCESS;
}

}
