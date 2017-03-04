Created by:
- [Leiver Andrés Campeón Benjumea](https://github.com/leiverandres)
- [Juan Sebastián Vega Patiño](https://github.com/sebasvega95)

Depends on
- [JSON for Modern C++](https://github.com/nlohmann/json). Included in lib/
- [base64](https://github.com/tkislan/base64). Included in lib/
- [MinIndexedPQ](https://github.com/kartikkukreja/blog-codes/blob/master/src/Indexed%20Min%20Priority%20Queue.cpp). Included in lib/
- [ØMQ](http://zeromq.org/). **Please install it in /usr/include and /usr/lib**
- [zmqpp](https://github.com/zeromq/zmqpp). **Please install it in /usr/include and /usr/lib**

To compile: `make`

Then, to run:
- Broker: `./broker.o`
- Server: `./server.o  <broker-ip> <server-ip> <server-port>`.`broker-ip` and `server-ip` can be `localhost` to test locally
- Client: `./client.o <broker-ip>`. `broker-ip` can be `localhost` to test locally
