#include <kirsch/signal.hpp>

#include <cstdlib>
#include <iostream>

static bool called_void_function = false;
static int called_int_function = 0;

static void void_function() {
    called_void_function = true;
}

static void int_function(int value) {
    called_int_function = value;
}

static bool called_void_method = false;
static int called_int_method = 0;

class FooClass {
    public:

        void void_method() {
            called_void_method = true;
        }

        void int_method(int value) {
            called_int_method = 1;
        }

};


int main() {
    {
        FooClass instance;
        kirsch::signal<void()> signal;
        
        signal.connect<void_function>();
        signal.connect<FooClass, &FooClass::void_method>(&instance);
        signal();
    }

    {
        FooClass instance;
        kirsch::signal<void(int)> signal;

        signal.connect<int_function>();
        signal.connect<FooClass, &FooClass::int_method>(&instance);
        signal(1);
    }

    bool passed = true;

    if(!called_void_function) {
        std::cerr << "void_function was not called." << std::endl;
        passed = false;
    }

    if(!called_void_method) {
        std::cerr << "void_method was not called." << std::endl;
        passed = false;
    }

    if(called_int_function == 0) {
        std::cerr << "int_function was not called." << std::endl;
        passed = false;
    }

    if(called_int_method == 0) {
        std::cerr << "int_method was not called." << std::endl;
        passed = false;
    }

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}