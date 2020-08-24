libKirsch
---
A simple, header-only library providing delegates and signals.

## Features
- Generic signals, allowing to bind functions, methods and lambdas.
- Generic **allocation-free** delegates.

## Integrating
### Generic Route
Copy the directory "kirsch" to your project and make sure, that the directory
is specified in the include directories.

### CMake
This project provides a simple and easy to use CMakeLists, allowing you to include
the library without any hassles.
Just copy this repository somewhere into your project and add the subdirectory.

```
add_subdirectory(dir/kirsch)

add_executable(my_app main.cpp)
link_target_libraries(
    my_app
    PRIVATE
        kirsch::kirsch
)
```

### CMake + FetchContent
Luckily, the CMakeLists is kept modern, thus is suitable to be used via CMake`s
`FetchContent`.

> For those who don't know:
> 
> Since CMake 3.11 an extension is being provided, namely `FetchContent`,
> allowing you to download and configure projects from arbitrary sources.

Just copy this into your CMakeLists. Bear in mind, that you'll require CMake 3.11 or
higher.
```
include(FetchContent)

FetchContent_Declare(
    kirsch
    GIT_REPOSITORY https://github.com/HiImJulien/fundament.git
    GIT_TAG 1.0
)

FetchContent_GetProperties(kirsch)
if(NOT kirsch_POPULATED)
    FetchContent_Populate(kirsch)
    add_subdirectory(${kirsch_SOURCE_DIR} ${kirsch_BINARY_DIR})
endif()
```

### Meson
This repository provides an `build.meson` file, allowing to use this repository
as an subproject.

## Usage
Below are some basic samples. See `test/` for more samples.

### Delegates

```cpp
#include <kirsch/delegate.hpp>

void function(int i) { ... };

class Foo {
    public:
        void method(int i) { ... };
};

int main() {
    {
        kirsch::delegate<void(int)> delegate;
        delegate.bind<function>();
        delegate(1234);
    }

    {
        Foo f;
        kirsch::delegate<void(int)> delgate;
        delegate.bind<Foo, &Foo::method>(&f);
        delegate(1234);
    }

    return 0;
}

```

### Signals

```cpp
#include <kirsch/signal.hpp>

// function and class from above.
int main() {
    Foo f;
    kirsch::signal<void(int)> signal;
    
    signal.bind<function>();
    signal.bind<Foo, &Foo::method>(&f);

    signal(12345);
}
```