# Hadoop的学习

1. 连接`HDFS`
`hdfsFS hdfsConnect(const char* host, tPort port)`
不建议使用这个API(我尚不知道原因...)

`hdfsFS hdfsBuilderConnect(struct hdfsBuilder *bld)`
根据`bld`结构体中指定的参数连接到文件系统
使用这个API连接到hdfs,出错返回NULL

```
    string m_strNodeName = "hdfs://172.17.0.8"; //hadoop集群地址
    string m_strMaster = "172.17.0.8:9000";  //hadoop热备份主节点
    
    //创建一个HDFS Builder
    //如果无法为新hdfsBuilder结构分配内存，则返回ENOMEM 
    struct hdfsBuilder* pbld = hdfsNewBuilder(); 
    
    // 该地址为hadoop的core-site.xml中fs.defaultFS的配置
    hdfsBuilderSetNameNode(pbld, m_strNodeName.c_str());  
    
    //该端口为hadoop的core-site.xml中fs.defaultFS的配置 
    hdfsBuilderSetNameNodePort(pbld, nPort);  
    
    //使用hdfsBuilderConfSetStr()修改/新增相关环境配置，该函数提供key-value形式的配置方式, 这两个参数可以为NULL，或者不要这个参数，老实说我没有明白这个函数的作用
    hdfsBuilderConfSetStr(pbld, "fs.hdfs.impl", "org.apache.hadoop.hdfs.DistributedFileSystem");

    //创建TCP连接
    hdfsFS m_hdfsfs;
    m_hdfsfs = hdfsBuilderConnect(pbld);
```

2. 打开文件
`hdfsFile hdfsOpenFile(hdfsFS fs, const char* path, int flags, int bufferSize, short replication, tSize blocksize)`
以指定模式打开文件,如果文件和中间目录不存在,则创建它们
模式即:
```
>成功返回文件句柄,出错返回NULL

后三个参数为0即可
O_RDONLY
以只读模式打开文件,当前偏移量为0
O_RDWR
以读写模式打开文件,如果文件已经存在,则将其截断为偏移量0,从而有效地删除文件内容,使其偏移量为0
O_RDWR | O_APPEND
以读写模式打开文件，当前偏移量为0。使用写入文件hdfsWrite()会将写入的数据附加到文件末尾。写入的数据 hdfsPwrite()未附加，而是从该API调用中指定的偏移量开始写入。
O_WRONLY
以只写模式打开文件。如果文件已存在，则将其截断为偏移量0，从而有效删除文件的内容。
O_WRONLY | O_APPEND
以仅写模式打开文件，当前偏移量为0。使用写入文件hdfsWrite()会将写入的数据附加到文件末尾。写入的数据 hdfsPwrite()未附加，而是从该API调用中指定的偏移量开始写入
```


3. 写文件
`tsize hdfsWrite(hdfsFS fs, hdfsFile file, const void* buffer, tSize length)`
>成功返回写入的字节数，出错返回NULL
length是写入的字节数，不能为0

写入指定的打开文件
如果以仅写模式打开文件，则写操作将从偏移量0开始
如果以附加模式打开文件，则数据将附加到文件末尾
如果存在以相同偏移量开始的并发写入，则仅最后完成的写入会持续存在

4. 读文件
`tSize hdfsRead(hdfsFS fs, hdfsFile file, void* buffer, tSize length)`
>成功返回实际读取的字节数，失败返回-1
length为缓冲区长度

5. 重命名文件
`int hdfsRename(hdfsFS fs, const char* oldPath, const char* newPath)`
>成功返回0,出错返回-1

6. 删除指定目录或文件
`int hdfsDelete(hdfsFS fs, const char* path, int recursive)`
>成功返回0，失败返回-1

fs是要删除的文件或目录所在的文件系统的句柄
recursive为0时：目录为空删除指定目录，不为空返回错误
不为0时：删除指定目录及其所有子目录
指定对象为文件不是目录时，忽略recursive参数
检查errno以获得错误代码和含义。一些关键错误是ESTALE，EACCES和EPERM。

7. 创建文件夹
`int hdfsCreateDirectory(hdfsFS fs, const char* path)`
>成功返回0，出错返回-1

如果输入参数无效，则将errno设置为EINVAL；如果目录已存在，则将errno设置为EEXIST；如果父目录不允许用户具有写许可权，则将EACCES设置为EACCES。

8. 关闭打开文件
刷新文件的所有挂起的写缓冲区，并释放与文件关联的资源
`int hdfsCloseFile(hdfsFS fs, hdfsFile file)`
>成功返回0,出错返回-1

9. 刷新指定文件的写缓冲区
`int hdfsFlush(hdfsFS fs, hdfsFile file)`
>成功返回0,出错返回-1

如果输入参数无效，则将errno设置为EINVAL

10. 以chown命令的方式更改文件或目录的所有权
`int hdfsChown(hdfsFS fs, const char* path, const char *owner, const char *group)`
>成功返回0，错误返回-1

后两个参数为NULL时保持原状
如果输入参数无效，则将errno设置为EINVAL。
如果进程没有足够的特权执行操作，则将errno设置为EPERM

11. 以chmod命令的方式更改文件或目录的权限
`int hdfsChmod(hdfsFS fs, const char* path, short mode)`
>成功返回0，错误返回-1。

如果输入参数无效，则将errno设置为EINVAL
如果进程没有足够的特权执行操作，则将errno设置为EPERM

12. 将当前偏移量移动到指定文件中的另一个偏移量
`int hdfsSeek(hdfsFS fs, hdfsFile file, tOffset desiredPos)`
>成功返回0，错误返回-1

13. 获得指定路径的文件/目录信息
- 获取给定路径的文件和目录列表。以动态分配的hdfsFileInfo结构数组返回信息
`hdfsFileInfo *hdfsListDirectory(hdfsFS fs, const char* path, int *numEntries)`
>成功返回结构体，错误返回 NULL
numEntries存储的是path中的文件/目录数，不能为0或者NULL

- 获取给定路径的信息
`hdfsFileInfo * hdfsGetPathInfo(hdfsFS fs, const char* path)`
>功返回动态分配的结构，错误返回 NULL
对于无效的参数，将errno设置为EINVAL，对于无效的访问，将errno设置为EACCES

```
struct hdfsFileInfo的信息包括:
ParameterstObjectKind mKind 指定对象是文件还是目录
char *mName                 指定对象的名称。
tTime mLastMod              指定上次修改对象的时间（以毫秒为单位）。
tOffset mSize               指定对象的大小（以字节为单位）。
short mReplication          指定对象的副本数。
tOffset mBlockSize          指定对象的块大小。
char *mOwner                指定对象的所有者。
char *mGroup                指定与对象关联的组。
short mPermissions          指定对象的权限。
tTime mLastAccess           指定创建对象的时期时间（以毫秒为单位）
```

当不再使用时,需要使用释放hdfsFileInfo分配的hdfsFileInfo结构体:
`void hdfsFreeFileInfo(hdfsFileInfo *hdfsInfo, int numEntries)`

14. 获取文件中的当前偏移量（以字节为单位）
`tOffset hdfsTell(hdfsFS fs, hdfsFile file)`
>成功返回当前偏移量(单位:字节), 失败返回-1

15.与指定的文件系统断开连接,即使出现错误，与文件系统句柄关联的资源也会被释放
`int hdfsDisconnect(hdfsFS fs)`
>成功返回0，错误返回-1

16. 设置工作目录。所有相对路径将相对于它进行解析
`int hdfsSetWorkingDirectory(hdfsFS fs, const char* path)`
>成功返回0，错误返回-1

例如，如果调用此API将工作目录设置为/mycluster/myvolume，然后hdfsOpenFile()使用path 进行调用， /temp/tmp.txt则假定要打开的文件的完整路径为/mycluster/myvolume/temp/tmp.txt

17. 获取文件系统的当前工作目录。在调用此方法之前，应用程序必须已调用hdfsSetWorkingDirectory()
`char* hdfsGetWorkingDirectory(hdfsFS fs, char *buffer, size_t bufferSize)`
>成功返回缓冲区，错误返回NULL。
无效参数将errno设置为EINVAL