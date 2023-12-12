# ch06 基于UDP的服务器端/客户端

## 1. 理解UDP

...略

## 2. 实现基于UDP的服务器端/客户端

### *1. UDP中的服务器端和客户端没有连接*

UDP服务器端/客户端不像TCP那样在连接状态下交换数据，因此与TCP不同，无需经过连接过程。也就是说，不必调用TCP连接过程中调用的 `listen` 函数和 `accept` 函数。UDP中只有创建套接字的过程和数据交换过程。

### *2. UDP服务器端和客户端均只需一个套接字*

TCP中套接字之间应该是一对一的关系。若要向10个客户端提供服务，则除了守门的服务器套接字外，还需要10个服务器端套接字。但在UDP中，不管是服务器端还是客户端都只需要1个套接字。也就是说，只需要1个UDP套接字就能和多台主机通信。

### *3. 基于UDP的数据I/O函数*

创建好TCP套接字后，传输数据时无需再添加地址信息。因为TCP套接字将保持与对方套接字的连接。换言之，TCP套接字知道目标地址信息。但UDP套接字不会保持连接状态，因此，每次传输数据都要添加目标地址信息。

```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
              const struct sockaddr *dest_addr, socklen_t addrlen);
// 成功时返回传输的字节数，失败时返回-1。
// 详见手册
```

- *sockfd* ：用于传输数据的UDP套接字文件描述符；
- *buf* ：保存待传输数据的缓冲地址值；
- *len* ：待传输的数据长度，以字节为单位；
- *flags* ：可选项参数，若没有则传递0；
- *dest_addr* ：存有目标地址信息的 `sockaddr` 结构体变量的地址值；
- *addrlen* ：传递给参数 *dest_addr* 的结构体变量的长度。

```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, socklen_t *addrlen);
// 成功时返回接收的字节数，失败时返回-1
// 详见手册
```

- *sockfd* ：用于接收数据的UDP套接字文件描述符；
- *buf* ：保存接收数据的缓冲地址值；
- *len* ：可接收的最大字节数，故无法超过参数 *buff* 所指的缓冲大小；
- *flags* ：可选项参数，若没有则传0；
- *src_addr* ：存有发送端地址信息的 `sockaddr` 结构体变量的地址值；
- *addrlen* ：保存参数 *src_addr* 的结构体变量长度的变量地址值；

### *4. 基于UDP的回声服务器端/客户端*

需要注意的是，UDP不同于TCP，不存在请求连接和受理过程，因此在某种意义上无法明确区分服务器端和客户端。只是因其提供服务而称为服务器端。

[uecho_server.c](./uecho_server.c) [uecho_client.c](./uecho_client.c)

### *5. UDP客户端套接字的地址分配*

> TCP客户端调用 `connect` 函数完成IP和端口号的分配，UDP客户端在何时分配IP和端口号呢？

UDP程序中，调用 `sendto` 函数传输数据前应完成对套接字的地址分配工作，因此调用 `bind` 函数。当然，`bind` 函数在TCP程序中出现过，但 `bind` 函数不区分TCP和UDP，也就是说，在UDP程序中同样可以调用。另外，如果调用 `sendto` 函数时发现尚未分配地址信息，则在首次调用 `sendto` 函数时给相应套接字自动分配IP和端口。而且此时分配的地址一直保留到程序结束为止，因此也可用来与其他UDP套接字进行数据交换。当然，IP用主机IP，端口号选尚未使用的任意端口号。  

综上所述，调用 `sendto` 函数时自动分配IP和端口号，因此，UDP客户端通常无需额外的地址分配过程。所以之前的示例省略了该过程，这也是普遍的实现方式。

## 3. UDP的数据传输特性和调用 `connect` 函数

TCP传输的数据不存在数据边界，UDP数据传输中存在数据边界。

### *1. 存在数据边界的UDP套接字*

TCP传输的数据不存在数据边界，这表示 **“数据传输过程中调用I/O函数的次数不具有任何意义。”** UDP是具有数据边界的协议，传输中调用I/O函数的次数非常重要。因此，**输入函数的调用次数和输出函数的调用次数完全一致，这样才能保证接收全部已发送数据。**

[bound_host1.c](./bound_host1.c) [bound_host2.c](./bound_host2.c)

### *2. 已连接（connected）UDP套接字与未连接（unconnected）UDP套接字*

TCP套接字中需注册待传输数据的目标IP和端口号，而UDP中则无需注册。因此，通过 `sendto` 函数传输数据的过程大致可分为以下3个阶段。

1. 向UDP套接字注册目标IP和端口号
2. 传输数据
3. 删除UDP套接字中注册的目标地址信息

每次都变更目标地址，因此可以重复利用同一UDP套接字向不同目标传输数据。这种未注册目标地址信息的套接字称为未连接套接字，反之，注册了目标地址的套接字称为已连接套接字。显然，UDP套接字属于未连接套接字。
  
在与同一主机进行长时间通信时，将UDP套接字变成已连接套接字会提高效率。上述三个阶段中，第一个阶段和第三个阶段占整个通信过程近1/3的时间，缩短这部分时间将大大提高性能。

### *3. 创建已连接UDP套接字*

创建已连接UDP套接字只需针对UDP套接字调用 `connect` 函数即可。

```c
sock = socket(PF_INET, SOCK_DGRAM, 0);
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = ....
addr.sin_port = ....
connect(sock, (struct sockaddr*)&addr, sizeof(addr));
```

针对UDP套接字调用 `connect` 函数并不意味着要与对方UDP套接字连接，这只是向UDP套接字注册目标IP和端口信息。之后就与TCP套接字一样了，每次调用 `sendto` 函数时只需传输数据。因为已经指定了收发对象，所以不仅可以使用 `sendto`、`recvfrom` 函数，还可以使用 `write`、`read` 函数进行通信。  
下面示例改自 `uecho_client.c`，可以结合 `uecho_server.c` 程序运行。

[uecho_con_client.c](./uecho_con_client.c)