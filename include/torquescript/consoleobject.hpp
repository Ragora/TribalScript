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
#include <vector>
#include <unordered_map>

#include <torquescript/storedvaluereference.hpp>
#include <torquescript/storedvalue.hpp>

namespace TorqueScript
{
    class Interpreter;
    class ExecutionScope;
    class Interpreter;
    class ConsoleObjectDescriptor;
    struct ObjectInstantiationDescriptor;

    typedef ConsoleObject* (*InitializeConsoleObjectFromDescriptorPointer)(Interpreter* interpreter, struct ObjectInstantiationDescriptor& descriptor);

    extern std::unordered_map<std::string, ConsoleObjectDescriptor*>* sConsoleObjectDescriptors;
    static std::unordered_map<std::string, ConsoleObjectDescriptor*>* getConsoleObjectDescriptors()
    {
        if (sConsoleObjectDescriptors)
        {
            return sConsoleObjectDescriptors;
        }
        return sConsoleObjectDescriptors = new std::unordered_map<std::string, ConsoleObjectDescriptor*>();
    }

    class ConsoleObjectDescriptor
    {
        public:
            ConsoleObjectDescriptor(const std::string& name, const std::string& parentName, InitializeConsoleObjectFromDescriptorPointer initializePointer) : mName(name), mParentName(parentName), mInitializePointer(initializePointer)
            {
                std::unordered_map<std::string, ConsoleObjectDescriptor*>* descriptors = getConsoleObjectDescriptors();

                assert(descriptors->find(name) == descriptors->end());
                descriptors->insert(std::make_pair(name, this));
            }

            std::string mName;
            std::string mParentName;
            std::vector<std::string> mHierarchy;
            InitializeConsoleObjectFromDescriptorPointer mInitializePointer;
    };

    static std::vector<std::string> relinkNamespace(const std::string& space)
    {
        std::unordered_map<std::string, ConsoleObjectDescriptor*>* descriptors = getConsoleObjectDescriptors();

        auto search = descriptors->find(space);
        if (search == descriptors->end())
        {
            throw std::runtime_error("Fatal error in relink namespaces!");
        }

        ConsoleObjectDescriptor* currentDescriptor = search->second;

        std::vector<std::string> result;
        result.push_back(currentDescriptor->mParentName);

        // ConsoleObject won't have an entry
        if (currentDescriptor->mParentName == "ConsoleObject")
        {
            return result;
        }

        std::vector<std::string> children = relinkNamespace(currentDescriptor->mParentName);
        result.insert(result.end(), children.begin(), children.end());
        return result;
    }

    static void relinkNamespaces()
    {
        std::unordered_map<std::string, ConsoleObjectDescriptor*>* descriptors = getConsoleObjectDescriptors();

        for (auto&& entry : *descriptors)
        {
            // Reset the hierarchy of this namespace
            entry.second->mHierarchy = relinkNamespace(entry.second->mName);
            entry.second->mHierarchy.insert(entry.second->mHierarchy.begin(), entry.second->mName);
        }
    }

    template <typename classType>
    struct TypeInformation
    {

    };

    #define DECLARE_CONSOLE_OBJECT(type, super)                                         \
        template<>                                                                      \
        struct TypeInformation<type>                                                    \
        {                                                                               \
            typedef TypeInformation<super> ParentInfo;                                  \
            static std::string getName()                                                \
            {                                                                           \
                return #type;                                                           \
            }                                                                           \
            static std::vector<std::string> getHierarchy()                              \
            {                                                                           \
                const std::string current = #type;                                      \
                std::vector<std::string> result;                                        \
                result.push_back(current);                                              \
                const std::vector<std::string> upper = ParentInfo::getHierarchy();      \
                result.insert(result.end(), upper.begin(), upper.end());                \
                return result;                                                          \
            }                                                                           \
            static ConsoleObjectDescriptor* Descriptor;                                 \
        };

    #define DECLARE_CONSOLE_OBJECT_BODY()                                               \
        public:                                                                         \
            virtual std::string getClassName() override;                                \

    #define IMPLEMENT_CONSOLE_OBJECT(type, super)                                                                   \
        ConsoleObjectDescriptor* TypeInformation<type>::Descriptor = new ConsoleObjectDescriptor(#type, #super, type::instantiateFromDescriptor);    \
        std::string type::getClassName()                                                                            \
        {                                                                                                           \
            return #type;                                                                                           \
        }

    /**
     *  @brief Base class for object instances recognized by the interpreter. These object instances
     *  may contain member fields addressable the interpreter as well as tagged fields which are arbitrarily
     *  mapped values.
     */
    class ConsoleObject
    {
        public:
            ConsoleObject(Interpreter* interpreter);
            virtual ~ConsoleObject();

            /**
             *  @brief Retrieves a tagged field by name from the object.
             *  @param The tagged field name to retrieve, which is case insensitive.
             *  @return A StoredValue pointer of the value currently in that tagged field
             *  slot.
             */
            StoredValueReference getTaggedField(const std::string& name);

            /**
             *  @brief Sets a tagged field by name on the object.
             *  @param name The tagged field name to set, which is case insensitive.
             *  @param value The value to set.
             */
            void setTaggedField(const std::string& name, StoredValue value);

            /**
             *  @brief Retrieves the class name of this ConsoleObject instance.
             *  @return A string representing the class name of this ConsoleObject.
             */
            virtual std::string getClassName() = 0;

            virtual bool addChild(ConsoleObject* child);

        protected:
            Interpreter* mInterpreter;

            //! A mapping of tagged field names to their stored values.
            std::unordered_map<std::string, StoredValue> mTaggedFields;
    };

    template<>
    struct TypeInformation<ConsoleObject>
    {
        static std::string getName()
        {
            return "ConsoleObject";
        }

        static std::vector<std::string> getHierarchy()
        {
            std::vector<std::string> result;
            result.push_back("ConsoleObject");
            return result;
        }
    };
}
