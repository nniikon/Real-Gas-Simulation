<<<<<<< HEAD
CFLAGS = -O3 -Wall \
=======
CFLAGS = -O0 -g -Wall \
>>>>>>> c9325d0 (added -g to config.mk)
-Wmissing-declarations -Wcast-align -Wcast-qual \
-Wchar-subscripts -Wconversion \
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
-Wformat=2 -Winline \
-Wpacked -Wpointer-arith -Winit-self \
-Wredundant-decls -Wshadow \
-Wstrict-overflow=2 \
-Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code \
-Wunused -Wvariadic-macros -Wno-nonnull \
-Wno-missing-field-initializers -Wno-narrowing \
-Wno-varargs -Wstack-protector -fcheck-new \
-fstack-protector -fstrict-overflow \
-fno-omit-frame-pointer \

CFLAGS += -I../../glad/include/ -I../../common/ -I../include/ -I../../glm/glm/ \
		  -I../../engine/include -I./glad/include

CFLAGS += -D NDEBUG
CFLAGS += -D NLOG

export CFLAGS

export BUILD_DIR = ${CURDIR}/build
export EXEC_NAME = VDV 

export GXX = g++
