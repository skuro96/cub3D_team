# all:
# 		gcc src/*.c -I include

all:
		gcc -g -L minilibx -lmlx -framework OpenGL -framework AppKit src/*.c -I include
		./a.out map/test06.cub

wall:
		gcc -L minilibx -lmlx -framework OpenGL -framework AppKit src/*.c -I include -Wall
		./a.out map/test01.

linux:
	gcc src/*.c ./minilibx/libmlx.a -I include -Lmlx -lXext -lX11 -lm
	./a.out map/test01.cub
