#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>

#include <ctime>
#include <string>
#include <iostream>

#include "hdfs.h" 
using namespace std;

hdfsFS m_hdfsfs;
string m_strNodeName = "hdfs://172.17.0.8"; //hadoop集群地址
string m_strMaster = "172.17.0.8:9000";  //hadoop热备份主节点
int nPort = 9000;

int main(int argc, char **argv) 
{

    cout << "Begin to connect hdfs,cur time:" << time(NULL) << endl;

    //创建一个HDFS Builder
    struct hdfsBuilder* pbld = hdfsNewBuilder(); 
    // 该地址为hadoop的core-site.xml中fs.defaultFS的配置
    hdfsBuilderSetNameNode(pbld, m_strNodeName.c_str());  
    //该端口为hadoop的core-site.xml中fs.defaultFS的配置 
    hdfsBuilderSetNameNodePort(pbld, nPort);  
    //使用hdfsBuilderConfSetStr()修改/新增相关环境配置，该函数提供key-value形式的配置方式，ex：
    // hdfsBuilderConfSetStr(pbld, "NULL", "NULL");

    //创建TCP连接
    m_hdfsfs = hdfsBuilderConnect(pbld);
    //检查是否存在根目录，来判断是否连接成功
    int CheckHDFS = hdfsExists(m_hdfsfs,"/");   

    if (!m_hdfsfs || 0 != CheckHDFS)
    {   
        cout << __LINE__ << ":Connect to hdfs failed,cur time:" << time(NULL) << endl;
        return -1;
    }
    else
    {
        cout << __LINE__ << ":Connect to hdfs success,cur time:" << time(NULL) << endl;
    }

    return 0;
}

