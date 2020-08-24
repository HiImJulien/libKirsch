#ifndef KIRSCH_DELEGATE_HPP
#define KIRSCH_DELEGATE_HPP

//==============================================================================
// This module provides a simple, allocation-free, header-only implementation 
// of the delegate concept in C++ 11.
//==============================================================================

#include <type_traits>
#include <stdexcept>

namespace kirsch {

    template<typename return_type, typename... parameter_types>
    class delegate;

    //
    // A delegate.
    //
    // Delegates allow you to bind arbitrary functions and methods,
    // as long as the signature matches.
    //
    template<typename return_type, typename... parameter_types>
    class delegate<return_type(parameter_types...)> {
        public:

            delegate() noexcept
            : _instance{nullptr}
            , _func{nullptr}
            {}

            //
            // Binds a function to this delegate.
            //
            template<return_type(*function)(parameter_types...)>
            void bind();

            //
            // Binds a method to this delegate.
            //
            template<class class_type, return_type(class_type::*method)(parameter_types...)>
            void bind(class_type* instance);

            //
            // Calls the bound function.
            //
            // Note:
            // - Raises std::runtime_error if no function is bound.
            //
            return_type operator()(parameter_types... args);
            
            //
            // Returns whether a function is bound or not.
            //
            bool is_bound() const noexcept;

            //
            // Clears the bound function.
            //
            void clear() noexcept;

        private:
            using instance_ptr_t = void*;
            using func_t = return_type(*)(instance_ptr_t, parameter_types...);

            template<return_type(*function)(parameter_types...)>
            static inline return_type function_stub(void*, parameter_types... args) {
                return function(args...);
            }

            template<class class_type, return_type(class_type::*method)(parameter_types...)>
            static inline return_type method_stub(void* instance, parameter_types... args) {
                return (static_cast<class_type*>(instance)->*method)(args...);
            }

            instance_ptr_t _instance;
            func_t _func;
    };

    //==========================================================================
    // The following section implements the methods declared above.
    //==========================================================================

    template<typename return_type, typename... parameter_types>
    template<return_type(*function)(parameter_types...)>
    void delegate<return_type(parameter_types...)>::bind() {
        _instance = nullptr;
        _func = &function_stub<function>;
    }

    template<typename return_type, typename... parameter_types>
    template<class class_type, return_type(class_type::*method)(parameter_types...)>
    void delegate<return_type(parameter_types...)>::bind(class_type* instance) {
        _instance = instance;
        _func = &method_stub<class_type, method>;
    }

    template<typename return_type, typename... parameter_types>
    return_type delegate<return_type(parameter_types...)>::operator()(parameter_types... args) {
        if(_func)
            return _func(_instance, args...);
        else
            throw std::runtime_error{"Cannot call delegate without any bound functions."};
    }

    template<typename return_type, typename... parameter_types>
    bool delegate<return_type(parameter_types...)>::is_bound() const noexcept {
        return _func != nullptr;
    }

    template<typename return_type, typename... parameter_types>
    void delegate<return_type(parameter_types...)>::clear() noexcept {
        _instance = _func = nullptr;
    }

}

#endif  // KIRSCH_DELEGATE_HPP