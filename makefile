SOURCE = graphics/main.cpp graphics/source/graphics.cpp glad/src/glad.c

INCLUDE = -Iglad/include/ -Icommon/ -Igraphics/include/ -Iglm/glm/

SAN = -fsanitize=address,bool,bounds,enum,float-cast-overflow,$\
float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,$\
object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,$\
undefined,unreachable,vla-bound,vptr

all:
	@g++ $(SOURCE) $(INCLUDE) $(SAN) -Og -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -o test