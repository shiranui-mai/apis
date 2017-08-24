all:
	make -C ./nginx/
	@mkdir -p ./logs/
	@mkdir -p ./bin/
	@cp ./nginx/objs/nginx ./bin

clean:
	make clean -C ./nginx/
	@rm -rf ./logs/*
