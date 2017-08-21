# apis
Access requests, Include inside&amp;outside. Use your own nginx modules


 git clone --recursive https://github.com/shiranui-mai/apis.git
 
 apt-get install libpcre3-dev zlib1g-dev libprotobuf-c-dev protobuf-c-compiler libzookeeper-st-dev

DEP:
	

Protocol:
	HEAD:
		version:8
		reserve1:8
		reserve1:8
		magic:8
		crc:32
		len:32
			head - method:
				version:8
				server:16
				method:8
				uid:[64]
				timestamp:[64]
				traceid:[64]
				token: [string]
			body:... ...
