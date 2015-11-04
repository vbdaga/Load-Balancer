# Load-Balancer
This is a simulation of load balancer using round robin algorithm. There are three servers all doing same tasks and a balancer to schedule the task to the servers. Client sends two random number as message to load balancer which schedules the task to one of the server and the server outputs the sum of the two numbers. The connection between two process is done by using sockets and threads.

##Installing the project
Run the following command in the installation directory.
```sh
make
```
This will build the files.
To simulate the project run the following commands, each in different terminal and in the same order.
```sh
server1
```
```sh
server2
```
```sh
server3
```
```sh
balancer
```
Now you can run different instances of client program in different terminals to see how the balancer schedules the tasks to different servers.
