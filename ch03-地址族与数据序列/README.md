# ch03 地址族与数据序列

## 1. 分配给套接字的IP地址与端口号

### *1. 网络地址*

IP地址分两类：

- IPv4，4字节地址族。
- IPv6，16字节地址族。

### *2. 网络地址分类与主机地址边界*

- A类地址的首字节范围：0\~127，即首位以0开始。
- B类地址的首字节范围：128\~191，即前2位以10开始。
- C类地址的首字节范围：192\~223，即前3位以110开始。

CIDR了解一下？

### *3. 用于区分套接字的端口号*

端口号由16位构成，可分配的端口号范围为0\~65535。虽然端口号不能重复，但TCP套接字和UDP套接字不会共用端口号，所以允许重复。例如，某TCP套接字使用8888端口号，则其它TCP套接字就无法使用该端口号，但UDP套接字可以使用。

## 2. 地址信息的表示

### *1. 表示IPv4地址的结构体*

```c
struct sockaddr_in
{
    sa_family_t sin_family; // 地址族(Address Family)
    uint16_t sin_port; // 16位TCP/UDP端口号
    struct in_addr sin_addr; // 32位IP地址
    char sin_zero[8]; // 不使用
}
// 该结构体中提到的另一个结构体 in_addr 定义如下，它用来存放32位IP地址。
struct in_addr
{
    in_addr_t s_addr; // 32 位IPv4地址。
}
```

数据类型参考如下POSIX定义表。

|数据类型名称|数据类型说明|声明的头文件|
| :------------: | :------------------------: | :--------------: |
|int8_t|signed 8 bit int|sys/types.h|
|uint8_t|unsigned 8 bit int(char)|sys/types.h|
|int16_t|signed 16 bit int|sys/types.h|
|uint16_t|unsigned 16 bit int(unsigned short)|sys/types.h|
|int32_t|signed 32 bit int|sys/types.h|
|uint32_t|unsigned 32 bit int(unsigned long)|sys/types.h|
|sa_family_t|地址族(Address Family)|sys/socket.h|
|socklen_t|长度(length of struct)|sys/socket.h|
|in_addr_t|IP地址，声明为uint32_t|netinet/in.h|
|in_port_t|端口号，声明为uint16_t|netinet/in.h|

### 2. 结构体 `sockaddr_in` 的成员分析

#### *1. 成员 `sin_family`*

每种协议适用的地址族均不同。比如，IPv4使用4字节地址族，IPv6使用16字节地址族。

|地址族|含义|
| :---------: | :-----------------------: |
|AF_INET|IPv4网络协议中使用的地址族|
|AF_INET6|IPv6网络协议中使用的地址族|
|AF_LOCAL|本地通信中采用的UNIX协议的地址族|

#### *2. 成员 `sin_port`*

以网络字节序（大端）保存16位端口号。

#### *3. 成员 `sin_addr`*

以网络字节序（大端）保存32位IP地址。结构体 `in_addr` 声明为uint32_t，因此只需当作32位整数即可。

#### *4. 成员 `sin_zero`*

无特殊含义，只是为了使结构体 `sockaddr_in` 的大小与 `sockaddr` 结构体保持一致而插入的成员。必须填充为0，否则无法得到想要的结果。参考如下 `bind` 函数，重点关注参数传递和类型转换部分。

```c
if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handing("bind() error");
```

此处重要的是第二个参数的传递。实际上 `bind` 函数的第二个参数期望得到 `sockaddr` 结构体变量的地址值，包括地址族、端口号、IP地址等。从下列代码可以看出，直接向 `sockaddr` 结构体填充这些信息会很麻烦。

```c
struct sockaddr
{
    sa_family_t sin_family; // 地址族（Address Family）
    char sa_data[14]; // 地址信息
}
```

此结构体成员 `sa_data` 保存的地址信息中需包含IP地址和端口号，剩余部分应填充为0，这也是 `bind` 函数要求的。而这对于包含地址信息来讲非常麻烦，继而就有了新的结构体 `sockaddr_in`。若按照之前的讲解填充 `sockaddr_in` 结构体，则将生成符合 `bind` 函数要求的字节流。最后转换为 `sockaddr` 型的结构体变量，再传递给 `bind` 函数即可。

> **知识补给站：** `sockaddr_in` 是保存IPv4地址信息的结构体，那为何还要通过 `sin_family` 来单独指定地址族信息呢？这与 `sockaddr` 结构体有关。结构体 `sockaddr` 并非只为 IPv4 设计，这从保存地址信息的数组 `sa_data` 长度为14字节也可看出。因此，结构体 `sockaddr` 要求在 `sin_family` 中指定地址族信息。为了与 `sockaddr` 保持一致，`sockaddr_in` 结构体中也有地址族信息。

## 3. 网络字节序与地址变换

### *1. 字节序与网络字节序*

- 大端序（Big Endian）（网络字节序为大端序）：高位字节存放在低位地址。
- 小端序（Little Endian）：高位字节存放在高位地址。

### *2. 字节序转换（Endian Conversations）*

- htons
- ntohs
- htonl
- ntohl

其中，h代表主机（host）字节序，n代表网络（network）字节序。s 指的是short(unsigned short)，l指的是long(unsigned long，Linux中long占4个字节)。比如，`htons` 是把short型数据从主机字节序转换为网络字节序。  
以s作为后缀的函数中，s代表两个字节short，因此用于端口号转换；以l作为后缀的函数中，l代表4个字节，因此用于IP地址转换。

[endian_conv.c](./endian_conv.c)

```bash
lxc@Lxc:~/C/tcpip_src/ch03-地址族与数据序列$ bin/endian_conv 
Host Byte Order Port: 0x1234
NewWork Byte Order Port: 0x3412
Host Byte Order Address: 0x12345678
NewWork Byte Order Address: 0x78563412
```

这是在小端字节序CPU中运行的结果。Intel和AMD系列的CPU都采用小端序标准。

> **知识补给站：** 数据在传输之前都要经过转换吗？数据收发过程中有自动转换机制。除了向 `sockaddr_in` 结构体变量填充数据以外，其他情况无须考虑字节序问题。

## 4. 网络地址的初始化与分配

### *1. 将字符串信息转换为网络字节序的整数型*

对于IP地址的表示，我们熟悉的是点分十进制表示法（Dotted Decimal Notation），而非整数型数据表示法。幸运的是，有个函数会帮我们将字符串形式的IP地址转换为32位整数型数据。此函数在转换类型的同时还进行网络字节序的转化。

```c
#include <arpa/inet.h>
in_addr_t inet_addr(const char* string);
// 成功时返回32位大端序整数型值，失败时返回 INADDR_NONE。
// 详见手册
```

[inet_add.c](./inet_addr.c)

```bash
lxc@Lxc:~/C/tcpip_src/ch03-地址族与数据序列$ bin/inet_addr 
NewWork Byte Order Integer Address: 0x6fc6a87f
Error Occured!
```

从运行结果可以看出，`inet_addr` 函数不仅可以把IP地址转成32位整数型，而且可以检测无效的IP地址。另外，从输出结果可以验证确实转换为网络字节序。  

`inet_aton` 函数与 `inet_addr` 函数在功能上完全相同，也将字符串形式IP地址转换为32位网络字节序整数并返回。只不过该函数利用了 `in_addr` 结构体，且其使用频率更高。

```c
#include <arpa/inet.h>
int inet_aton(const char* string, struct in_addr* addr)
// 成功时返回1（true），失败时返回0（false)。
```

其中，*string* 参数为需要转换的IP地址信息的字符串地址值。*addr* 将保存转换结果的 `in_addr` 结构体变量的地址值。在使用 `inet_addr` 函数时，需将转换后的IP地址信息代入 `sockaddr_in` 结构体变量中声明的 `in_addr` 结构体变量。而 `inet_aton` 函数则不需要此过程。原因在于，若传递 `in_addr` 结构体变量地址值，函数会自动把结果填入该结构体变量。

[inet_aton.c](./inet_aton.c)

```bash
lxc@Lxc:~/C/tcpip_src/ch03-地址族与数据序列$ bin/inet_aton 
Newwork ordered integer addr: 0xdeea7c7b
```

### *2. 将网络字节序的整数型转换为字符串*

`inet_ntoa` 看到名字你应该就知道这个函数是干嘛的了，此函数将网络字节序的整数型转换为字符串。

```c
#include <arpa/inet.h>
char *inet_ntoa(struct in_addr adr)
// 成功时返回字符串地址，失败时返回-1
```

在调用该函数时要小心，返回值为char类型的指针。返回字符串地址意味着字符串已保存到内存空间，但该函数未向程序员要求分配内存，而是在内部申请了内存并保存了字符串。也就是说，调用完该函数后，应立即将字符串信息复制到其他内存空间。因为，若再次调用该函数，则会覆盖之前保存的字符串信息。

[inet_ntoa.c](./inet_ntoa.c)

```bash
lxc@Lxc:~/C/tcpip_src/ch03-地址族与数据序列$ bin/inet_ntoa 
Dotted-Decimal notation1: 1.2.3.4
Dotted-Decimal notation2: 1.1.1.1
Dotted-Decimal notation3: 1.2.3.4
```

### *3. 网络地址初始化*

结合前面所学的内容，现在介绍套接字创建过程中常见的网络地址信息初始化的方法。

```c
struct sockaddr_in addr;
char *serv_ip = "123.234.123.111" // 声明IP地址字符串
char *serv_port = "9999" // 声明端口号字符串
memset(&addr, 0, sizeof(addr)); // 结构体变量 addr 的所有成员赋值为0
addr.sin_family = AF_INET; // 指定地址族
addr.sin_addr.s_addr = inet_addr(serv_ip); // 基于字符串的IP地址初始化
addr.sin_port = htons(atoi(serv_port)); // 基于字符串的端口号初始化
```

上述代码中 `memset` 将每个字节都初始化为一个值，第一个参数为结构体变量 `addr` 的地址值，即初始化对象为 `addr`；第二个参数为0，因此初始化为0；最后一个参数传入 `addr` 的长度，因此 `addr` 的所有字节均初始化为0。这么做是为了将 `sockaddr_in` 结构体成员 `sin_zero` 初始化为0。另外，最后一行代码调用的 `atoi` 函数把字符串类型的值转换为整数型。总之，上述代码利用字符串格式的IP地址和端口号初始化了 `sockaddr_in` 结构体。

### *4. 客户端地址信息初始化*

服务器端的准备工作通过 `bind` 函数完成，而客户端则通过 `connect` 函数完成。服务器端声明 `sockadr_in` 结构体变量，将其赋予服务器端IP和套接字的端口号，然后调用 `bind` 函数；而客户端则声明 `sockaddr_in` 结构体，并初始化为要与之连接的服务器端套接字的IP和端口号，然后调用 `connect` 函数。

### *5. `INADDR_ANY`*

每次创建服务器端套接字时都输入IP地址有些繁琐，此时可如下初始化地址信息。

```c
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
```

利用常数 `INADDR_ANY` 分配服务器端的IP地址。若采用这种方法，则可自动获取运行服务器端的IP地址，不必亲自输入。而且，若同一计算机中已分配多个IP地址（多宿主(Multi-homed)计算机，一般路由器属于这一类），则只要端口号一致，就可以从不同IP地址接收数据信息。因此，服务器端优先考虑这种方式。而客户端中除非带有一部分服务器端功能，否则不会采用。