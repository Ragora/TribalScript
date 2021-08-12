/**
 *  Copyright 2021 Robert MacGregor
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction,
 *  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <tribalscript/function.hpp>

namespace TribalScript
{
    /**
     *  @brief A FunctionRegistry is used to keep track of registered functions in memory by
     *  the packages they are associated with.
     *  @details A FunctionRegistry effectively is a 'package' for registered functions to reside
     *  in. The "" package is the root level package everything is registered to if no package is
     *  otherwise set.
     */
    struct FunctionRegistry
    {
        /**
         *  @brief Constructs a new FunctionRegistry instance.
         *  @param package The name of the package this registry is associated with.
         */
        explicit FunctionRegistry(const std::string& package) : mPackageName(package), mActive(false)
        {

        }

        //! The package this registry belongs to.
        std::string mPackageName;

        //! Whether or not the registry is currently active.
        bool mActive;

        //! A mapping of function namespaces to a mapping of function names to the function object.
        std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Function>>> mFunctions;
    };
}
