set(FLAGS_TARGET flags)

add_library(${FLAGS_TARGET} INTERFACE)

target_compile_features(${FLAGS_TARGET}
    INTERFACE
        cxx_std_20
)

target_compile_options(${FLAGS_TARGET}
    INTERFACE
        -fdiagnostics-color=always

        # -Wno-interference-size

        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wunused
        -Wformat=2
        -Wnon-virtual-dtor
        -Wswitch-enum

        -fstack-protector-strong # inserts checks for buffer overflow
        # -fPIE -pie

        $<$<CONFIG:Debug>:
            -Og
            -g3
            -ggdb
            -fsanitize=address,leak,undefined,float-divide-by-zero
            -fno-omit-frame-pointer
        >

        $<$<CONFIG:Release>:
            -g
            -O2
            -march=native
            -flto
            -DNDEBUG
        >
)

target_link_options(${FLAGS_TARGET}
    INTERFACE
        -fdiagnostics-color=always

        -Wall
        -Wextra
        # -fPIE -pie
        -rdynamic

        $<$<CONFIG:Debug>:
            -Og
            -g3
            -ggdb
            -fsanitize=address,leak,undefined,float-divide-by-zero
            -fno-omit-frame-pointer
        >

        $<$<CONFIG:Release>:
            -g
            -O2
            -march=native
            -flto
        >
)

