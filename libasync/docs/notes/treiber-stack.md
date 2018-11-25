## Treiber Stack
Treiber stack是古老而简单的无锁算法。S

### 标准实现
标准实现里包括push和pop两个接口，用cas+数组+top数字实现无锁的stack，也可以把数组和数字索引替换成链表和结点指针。
都用while循环加上CAS来解决race condition。

