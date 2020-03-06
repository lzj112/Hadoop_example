Hadoop的接口一些例子

```
1. 建立、关闭与HDFS连接：hdfsConnect()、hdfsConnectAsUser()、hdfsDisconnect()。hdfsConnect()实际上是直接调用hdfsConnectAsUser。

2. 打开、关闭HDFS文件：hdfsOpenFile()、hdfsCloseFile()。当用hdfsOpenFile()创建文件时，可以指定replication和blocksize参数。写打开一个文件时，隐含O_TRUNC标志，文件会被截断，写入是从文件头开始的。

3. 读HDFS文件：hdfsRead()、hdfsPread()。两个函数都有可能返回少于用户要求的字节数，此时可以再次调用这两个函数读入剩下的部分（类似APUE中的readn实现）；只有在两个函数返回零时，我们才能断定到了文件末尾。

4. 写HDFS文件：hdfsWrite()。HDFS不支持随机写，只能是从文件头顺序写入。

5. 查询HDFS文件信息：hdfsGetPathInfo()

6. 查询和设置HDFS文件读写偏移量：hdfsSeek()、hdfsTell()

7. 查询数据块所在节点信息：hdfsGetHosts()。返回一个或多个数据块所在数据节点的信息，一个数据块可能存在多个数据节点上。
libhdfs中的函数是通过jni调用Java虚拟机，在虚拟机中构造对应的HDFS的JAVA类，然后反射调用该类的功能函数。总会发生JVM和程序之间内存拷贝的动作，性能方面值得注意。
HDFS不支持多个客户端同时写入的操作，无文件或是记录锁的概念。
建议只有超大文件才应该考虑放在HDFS上，而且较好对文件的访问是写一次，读多次。小文件不应该考虑放在HDFS上，得不偿失！
```