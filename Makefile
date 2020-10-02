%: %.c
	gcc $@.c -o $@ -Wall -Werror -lm -fsanitize=address,leak
