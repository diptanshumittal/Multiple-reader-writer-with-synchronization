all: writer.c
	gcc -o writer writer.c -lpthread  
	gcc -o server server.c -lpthread 
	gcc -o reader reader.c -lpthread 





