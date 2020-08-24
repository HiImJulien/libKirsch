#include <kirsch/delegate.hpp>

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

static int square_function(int value) {
    return value * value;
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

        int square_method(int value) {
            return value * value;
        }

};

int main() {
    {
        kirsch::delegate<void()> delegate;
        delegate.bind<void_function>();
        delegate();
    }

    {
        kirsch::delegate<void(int)> delegate;
        delegate.bind<int_function>();
        delegate(1);
    }

    int square = 0;
    {
        kirsch::delegate<int(int)> delegate;
        delegate.bind<square_function>();
        square = delegate(5);
    }

    {
        kirsch::delegate<void()> delegate;
        delegate.bind<void_function>();
        delegate();
    }

    { 
        FooClass instance;
        kirsch::delegate<void()> delegate;

        delegate.bind<FooClass, &FooClass::void_method>(&instance);
        delegate();
    }

    {
        FooClass instance;
        kirsch::delegate<void(int)> delegate;

        delegate.bind<FooClass, &FooClass::int_method>(&instance);
        delegate(1);
    }

    int method_square = 0;
    {
        FooClass instance;
        kirsch::delegate<int(int)> delegate;

        delegate.bind<FooClass, &FooClass::square_method>(&instance);
        method_square = delegate(5);
    }

    bool passed = true;

    {
        kirsch::delegate<void()> delegate;
        try {
            delegate();
            
            std::cerr << "delegate bound to nothing didn't throw." << std::endl;
            passed = false;
        } catch(...) {}
    }

    if(!called_void_function) {
        std::cerr << "void_function was not called." << std::endl;
        passed = false;
    }

    if(called_int_function == 0) {
        std::cerr << "int_function was not called." << std::endl;
        passed = false;
    }

    if(square != 5 * 5) {
        std::cerr << "delegate bound to square_function returned wrong value." << std::endl;
        passed = false;
    }

    if(!called_void_method) {
        std::cerr << "void_method was not called." << std::endl;
        passed = false;
    }

    if(called_int_method == 0) {
        std::cerr << "int_method was not called." << std::endl;
        passed = false;
    }

    if(method_square != 5*5) {
        std::cerr << "delegate bound to square_method returned wrong value." << std::endl;
        passed = false;
    }

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}