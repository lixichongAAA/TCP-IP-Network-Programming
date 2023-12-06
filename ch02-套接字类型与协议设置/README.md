# ch02 套接字类型与协议设置

## 1. 套接字协议及其数据传输特性

### *1. 创建套接字*

```c
int socket(int domain, int type, int protocol)
// Create a new socket of type TYPE in domain DOMAIN, using
// protocol PROTOCOL. If PROTOCOL is zero, one is chosen automatically.
// Returns a file descriptor for the new socket, or -1 for errors.
```

*domain* ：套接字中使用的协议族（Protocol Family）信息。*type* ：套接字数据传输类型信息。*protocol* ：计算机间通信使用的协议信息。

### *2. 协议族(Protocol Family)*

协议族主要有以下几类。通过 `socket` 函数的第一个参数传递。

|名称|协议族|
| :--------: | :--------------------: |
|PF_INET|IPv4互联网协议族|
|PF_INET6|IPv6互联网协议族|
|PF_LOCAL|本地通信的UNIX协议族|
|PF_PACKET|底层套接字的协议族|
|PF_IPX|IPX Novell协议族|

### 3. 套接字类型(Type)

套接字类型指的是套接字的数据传输方式，通过 `socket` 函数的第二个参数传递。

#### *1. 面向连接的套接字(SOCK_STREAM)*

可靠、有序、基于字节的面向连接的数据传输方式的套接字。套接字必须一一对应。这种套接字称为TCP套接字。

#### *2. 面向消息的套接字(SOCK_DGRAM)*

不可靠、无序、以数据的高速传输为目的的套接字。这种套接字成为UDP套接字。

#### *3. 协议的最终选择*

`socket` 函数的第三个参数决定最终采用的协议。传递前两个参数即可创建所需套接字，所以大部分情况下可以向第三个参数传递0，除非在同一协议族中存在多个数据传输方式相同的协议。这个时候，数据传输方式相同，协议不同，此时需要第三个参数具体指定协议信息。

### *3. TCP套接字示例*

[tcp_client.c](./tcp_client.c)。该示例验证TCP套接字具有以下特性：传输的数据不存在边界。为验证这一点，需要让 `write` 函数的调用次数不同于 `read` 函数的调用次数。因此在客户端中分多次调用 `read` 函数以接收服务器端发送的全部数据。tcp_server.c 与第一章的hello_server.c相比无变化。