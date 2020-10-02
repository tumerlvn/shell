%: %. c
	gcc $@ . c -o $@ - Wall - Werror - lm -fsanitize=address
	cpplint -- filter = - legal / copyright $@ . c
