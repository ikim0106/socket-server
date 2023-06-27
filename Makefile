client:
	gcc client.c -oclient.out
	./client.out

server:
	gcc server.c -oserver.out
	./server.out 8080 resources

clear:
	rm ./*.out