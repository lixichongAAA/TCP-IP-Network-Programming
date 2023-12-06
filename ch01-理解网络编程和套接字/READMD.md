# ch01 理解网络编程和套接字

## 1. 理解网络编程和套接字

**网络编程中接受请求的套接字的创建过程如下。**

1. 调用 `socket` 函数创建套接字。

```c
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
// Create a new socket of type TYPE in domain DOMAIN, using
// protocol PROTOCOL. If PROTOCOL is zero, one is chosen automatically.
// Returns a file descriptor for the new socket, or -1 for errors.
```

2. 调用 `bind` 函数分配IP地址和端口号。

```c
#include <sys/socket.h>
int bind(int __fd, const struct sockaddr *__addr, socklen_t __len);
// Give the socket FD the local address ADDR (which is LEN bytes long).
// 成功时返回0，失败时返回-1。
```

3. 调用 `listen` 函数转为可接受请求的状态。

```c
#include <sys/socket.h>
int listen(int __fd, int __n);
// Prepare to accept connections on socket FD.
// N connection requests will be queued before further requests are refused.
// Returns 0 on success, -1 for errors.
```

4. 调用 `accept` 函数受理连接请求。

```c
#include <sys/socket.h>
int accept(int __fd, struct sockaddr *__restrict__ __addr, socklen_t *__restrict__ __addr_len);
// Await a connection on socket FD.
// When a connection arrives, open a new socket to communicate with it,
// set *ADDR (which is *ADDR_LEN bytes long) to the address of the connecting
// peer and *ADDR_LEN to the address's actual length, and return the
// new socket's descriptor, or -1 for errors.

// This function is a cancellation point and therefore not marked with
// __THROW.
```

服务器程序：[hello_server.c](./hello_server.c)

客户端程序只有 "调用 `socket` 函数创建套接字" 和 "调用 `connect` 函数向服务器端发送请求" 这两个步骤。

```c
int connect(int __fd, const struct sockaddr *__addr, socklen_t __len)
// Open a connection on socket FD to peer at ADDR (which LEN bytes long).
// For connectionless socket types, just set the default address to send to
// and the only address from which to accept transmissions.
// Return 0 on success, -1 for errors.

// This function is a cancellation point and therefore not marked with
// __THROW.
```

客户端程序：[hello_client.c](./hello_client.c)

## 2. 基于Linux的文件操作

在Linux世界，socket也被认为是文件的一种，因此在网络数据传输过程中自然可以使用文件I/O的相关函数。关于标准输入输出，[可参考《Linux命令行与shell脚本编程大全》笔记](https://github.com/lixichongAAA/Linux_Shell-Bash-/blob/master/ch15-%E5%91%88%E7%8E%B0%E6%95%B0%E6%8D%AE/README.md#1-理解输入输出)。

### *1. 打开文件*

```c
int open(const char *__file, int __oflag, ...)
// 第一个参数是文件名和路径信息，第二个参数是文件打开模式
// 成功时返回文件描述符，失败时返回-1
// 详见手册
```

[low_open.c](./low_open.c)

|打开模式|含义|
| :----------: | :---------------------: |
|O_CREAT|必要时创建文件|
|O_TRUNC|删除全部现有数据|

### *2. 关闭文件*

```c
#include <unistd.h>
int close(int fd);
// 成功时返回0，失败时返回-1
```

### *3. 将数据写入文件*

```c
#include <unistd.h>
ssize_t write(int __fd, const void *__buf, size_t __n)
// Write N bytes of BUF to FD. Return the number written, or -1.

// This function is a cancellation point and therefore not marked with
// __THROW.
```

> **知识补给站：**`size_t` 是通过 `typedef` 定义的 `unsigned int` 类型。`ssize_t` 前面多加的s代表signed，即 `ssize_t` 是通过 `typedef` 定义的 `signed int` 类型。这些类型都是基本数据类型的别名。
人们普遍认为int是32位的，因为主流的操作系统和计算机仍采用32位。而在过去16位操作系统时代，int是16位的。根据系统的不同、时代的变化，数据类型的表现也随之改变，需要修改程序中使用的数据类型。如果之前已在需要声明4字节数据类型之处使用了 `size_t` 或 `ssize_t`，则将大大减少代码的改动，因为只需要修改并编译 `size_t` 和 `ssize_t` 类型的 `typedef` 声明即可。在项目中，为了给基本数据类型赋予别名，一般会添加大量 `typedef` 声明。而为了与程序员定义的新数据类型加以区分，操作系统定义的数据类型会添加后续。

### *4. 读取文件中的数据*

```c
#include <unistd.h>
ssize_t read(int __fd, void *__buf, size_t __nbytes)
// Read NBYTES into BUF from FD. Return the
// number read, -1 for errors or 0 for EOF.

// This function is a cancellation point and therefore not marked with
// __THROW.
```

[low_read.c](./low_read.c)

### *5. 文件描述符与套接字*

[fd_seric.c](./fd_seri.c)

```bash
lxc@Lxc:~/C/tcpip_src/ch01-理解网络编程和套接字$ bin/fd_seri 
file descriptor 1: 3
file descriptor 2: 4
file descriptor 3: 5
```

从输出的文件描述符数值可以看出，描述符从3开始由小到大的顺序编号，因为0、1、2是分配给标准I/O的描述符。