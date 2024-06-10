CFLAGS = -O0 -march=native -g -Wall \
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
-fno-omit-frame-pointer -flto\

# CFLAGS += -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

CFLAGS += -I../../glad/include/ -I../../common/ -I../include/ -I../../glm/glm/ \
		  -I../../engine/include -I../../glad_fork/include -I./glad_fork/include
# fix inline warnings
CFLAGS += --param=max-inline-insns-single=100000 --param=large-function-growth=1000

CFLAGS += -D DEBUG
CFLAGS += -D NLOG
CFLAGS += -D NGRAPH_DEBUG

export CFLAGS 

export BUILD_DIR = ${CURDIR}/build
export EXEC_NAME = VDV 

export GXX = g++
