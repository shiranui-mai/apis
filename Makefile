all:
	make -C ./nginx/
	@mkdir -p ./logs/
	@mkdir -p ./bin/
	@cp ./nginx/objs/nginx ./bin
