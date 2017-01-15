//
// Copyright (c) 2017. See AUTHORS file.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef PIXL_UTILS_H
#define PIXL_UTILS_H

#include <utility>

#include "types.h"

namespace pixl {

    inline void aswap(u8* x, u8* y, i32 len) {
        for (i32 i = 0; i < len; i++) {
            std::swap(x[i], y[i]);
        }
    }
}

#endif