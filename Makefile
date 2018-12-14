all : dfc.cpp dfs.cpp
	##gcc server.c -o server
	g++ -c dfc.cpp
	g++ -c dfs.cpp
	g++ -no-pie -pthread -lssl -lcrypto -g -o dfc dfc.o /usr/lib/x86_64-linux-gnu/libcrypto.a
	g++ -no-pie -pthread -lssl -lcrypto -g -o dfs dfs.o /usr/lib/x86_64-linux-gnu/libcrypto.a
	##gcc server.cpp -o server -no-pie -pthread -lssl -lcrypto -g

clean:
	$(RM) *.o client server
