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

#include <torquescript/platformcontext.hpp>
#include <torquescript/standardconsoleobjectregistry.hpp>

namespace TorqueScript
{
    /**
     *  @brief A structure representing overall interpreter runtime configuration. Some settings can be
     *  changed at runtime while others are static once initialized.
     */
    struct InterpreterConfiguration
    {
        InterpreterConfiguration(PlatformContext* platform = new PlatformContext(), ConsoleObjectRegistryBase* registry = new StandardConsoleObjectRegistry()) :
                                 mPlatform(platform), mConsoleObjectRegistry(registry), mMaxRecursionDepth(1024), mCaseSensitive(false)
        {

        }

        //! The platform context used for handling ie. logging, File I/O and so on. While this can be reassigned at runtime, it is not recommended.
        PlatformContext* const mPlatform;

        //! The ConsoleObjectRegistry associated with this interpreter. It is used to store all ConsoleObject instances associated with the interpreter.
        ConsoleObjectRegistryBase* mConsoleObjectRegistry;

        //! Maximum call stack depth. If set to 0, no maximum call depth is enforced.
        unsigned int mMaxRecursionDepth;

        //! Whether or not the interpreter should be case sensitive. While this can be reassigned at runtime, it is not recommended.
        bool mCaseSensitive;
    };
}
