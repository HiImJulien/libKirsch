#ifndef KIRSCH_SIGNAL_HPP
#define KIRSCH_SIGNAL_HPP

//==============================================================================
// This module provides a simple, header-only implementation of the signals
// and slots concept written in pure C++11.
//
// Author: Julian Kirsch
//==============================================================================

#include <algorithm>
#include <vector>
#include <type_traits>

namespace kirsch {
    template <typename return_type, typename... parameter_types>
    class signal;

    //
    // A signal.
    //
    // Signals allow you to dispatch events to receivers.
    // In this particular implementation, methods, lambdas or functions
    // act as receivers.
    //
    template <typename return_type, typename... parameter_types>
    class signal<return_type(parameter_types...)> {
        public:
            using func_t = return_type(*)(parameter_types...);

            template<typename lambda_type>
            using is_stateless = typename std::is_convertible<lambda_type, func_t>;
            
            //
            // Connects a function with the signature
            //   return_type function(parameter_types...);
            //
            template<return_type(*function)(parameter_types...)>
            void connect();

            //
            // Connects a method with the signature
            //   return_type class_type::method(parameter_types...);
            //
            template<class class_type, return_type(class_type::*method)(parameter_types...)>
            void connect(class_type* instance);

            //
            // Connects a lambda with signature
            //   return_type l(parameter_types...);
            //
            template<class lambda_type>
            typename std::enable_if<is_stateless<lambda_type>::value>::type
            connect(lambda_type type) {
                _owned_slots.emplace_back(nullptr, &lambda_stub<lambda_type>);
            } 

            template<class lambda_type>
            typename std::enable_if<!is_stateless<lambda_type>::value>::type
            connect(lambda_type type) {
                _owned_slots.emplace_back(new lambda_type{type}, &lambda_stub<lambda_type>);
            } 

            //
            // Disconnects all slots with matching instance.
            //
            void disconnect(void* instance);

            //
            // Disconnects all slots with matching function.
            //
            // Note:
            // - Only for functions; removes multiple to mitigate the 
            //   the defect with connect allowing to multiple conections.
            //
            template<return_type(*function)(parameter_types...)>
            void disconnect();

            //
            // Disconnects a specific slot with specific instance.
            //
            // Note:
            // - If instance is `nullptr`, disconnects all specified methods.
            //
            template<class class_type, return_type(class_type::*method)(parameter_types...)>
            void disconnect(class_type* instance);

            //
            // Dispatches the signal to every connected slot.
            //
            void operator()(parameter_types... arguments);
        
        private:
            using callable_t = void(*)(void*, parameter_types...);
            using slot_t = std::pair<void*, callable_t>;

            template<return_type(*function)(parameter_types...)>
            static inline void function_stub(void*, parameter_types... args) {
                function(args...); 
            }

            template<class class_type, return_type(class_type::*method)(parameter_types...)>
            static inline void method_stub(void* instance, parameter_types... args) {
                (static_cast<class_type*>(instance)->*method)(args...);
            }

            template<class lambda_type>
            static inline void lambda_stub(void* instance, parameter_types... args) {
                static_cast<lambda_type*>(instance)->operator()(args...);
            }
        
            std::vector<slot_t> _slots;
            std::vector<slot_t> _owned_slots;

    }; 

    //==========================================================================
    // The following section implements the signal class declared above.
    //==========================================================================

    template<typename return_type, typename... parameter_types>
    template<return_type(*function)(parameter_types...)>
    void signal<return_type(parameter_types...)>::connect() {
        callable_t cb = &function_stub<function>;
        _slots.emplace_back(nullptr, cb);
    }

    template<typename return_type, typename... parameter_types>
    template<class class_type, return_type(class_type::*function)(parameter_types...)>
    void signal<return_type(parameter_types...)>::connect(class_type* instance) {
        callable_t cb = &method_stub<class_type, function>;
        _slots.emplace_back(instance, cb);
    }

    template<typename return_type, typename... parameter_types>
    void signal<return_type(parameter_types...)>::disconnect(void* instance) {
        const auto it = std::remove_if(_slots.begin(), _slots.end(), [instance](const slot_t& slot) {
            return slot.first == instance;
        });

        if(it != _slots.end())
            _slots.erase(it, _slots.end());
    }

    template<typename return_type, typename... parameter_types>
    template<return_type(*function)(parameter_types...)>
    void signal<return_type(parameter_types...)>::disconnect() {
        const auto it = std::remove_if(_slots.begin(), _slots.end(), [](const slot_t& slot) {
            return slot.second == &function_stub<function>;
        });

        if(it != _slots.end())
            _slots.erase(it);
    }

    template<typename return_type, typename... parameter_types>
    template<class class_type, return_type(class_type::*method)(parameter_types...)> 
    void signal<return_type(parameter_types...)>::disconnect(class_type* instance) {
        const auto it = std::remove_if(
            _slots.begin(),
            _slots.end(),
            [instance](const slot_t& slot) {
                const bool no_instance = (instance == nullptr);
                return slot.second == &method_stub<class_type, method>
                    && no_instance ? true : slot.first == instance;
            }
        );

        if(it != _slots.end())
            _slots.erase(it, _slots.end());
    }

    template<typename return_type, typename... parameter_types>
    void signal<return_type(parameter_types...)>::operator()(parameter_types... arguments) {
        for(auto& slot: _slots)
            slot.second(slot.first, arguments...);
    }


}

#endif  // KIRSCH_SIGNAL_HPP
