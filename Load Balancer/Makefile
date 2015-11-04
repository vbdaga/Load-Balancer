LIB = -pthread

%.o : %.c 
	@gcc -c -o $@ $< $(LIB) -w

all : client balancer server1 server2 server3 clean

client : client.o 
	@gcc -o $@ $< $(LIB) -w

balancer : balancer.o
	@gcc -o $@ $< $(LIB) -w

server1 : server1.o
	@gcc -o $@ $< $(LIB) -w

server2 : server2.o
	@gcc -o $@ $< $(LIB) -w

server3 : server3.o
	@gcc -o $@ $< $(LIB) -w

clean :
	@rm -f *.o