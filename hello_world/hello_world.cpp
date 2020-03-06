#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>

#include <ctime>
#include <string>
#include <iostream>

#include "hdfs.h" 
using namespace std;


bool ConnectToHdfs();

hdfsFS m_hdfsfs;
string m_strNodeName = "hdfs://172.17.0.8"; //hadoop集群地址
string m_strMaster = "172.17.0.8:9000";  //hadoop热备份主节点
int nPort = 9000;

int main(int argc, char **argv) 
{

    cout << "Begin to connect hdfs,cur time:" << time(NULL) << endl;

    //连接hdfs
    if( !ConnectToHdfs() )
    {
        cout << __LINE__ << ":Connect to hdfs failed,cur time:" << time(NULL) << endl;
        return -1;
    }
    cout << __LINE__ << ":Connect to hdfs success,cur time:" << time(NULL) << endl;

    //创建文件夹
    if( 0 != hdfsCreateDirectory(m_hdfsfs, "/datacenter/data"))
    {
        cout << "Create Directory failed" << endl;
    }

    string path = "/datacenter/test/tmp/test.nb" ;
    //根据flag O_WRONLY|O_CREAT 知，若文件存在，则打开;不存在，则新建
    hdfsFile hdfsfd = hdfsOpenFile(m_hdfsfs, path.c_str(), O_WRONLY|O_CREAT, 0, 0, 0);
    string strDataBuf = "";
    for(int i = 0 ;i < 1024;++i)
    {
        strDataBuf += ('A' + i%26);
    }
    strDataBuf += '\n';
    //写hdfs
    tSize tNumBytes = hdfsWrite(m_hdfsfs, hdfsfd, strDataBuf.c_str(), strDataBuf.size());
    cout << "success write size:" << tNumBytes<<endl;
    cout << "hdfsFlush:" << hdfsFlush(m_hdfsfs, hdfsfd ) << endl;
    cout << "hdfsCloseFile:" << hdfsCloseFile(m_hdfsfs, hdfsfd) << endl;

    //
    if ( 0 != hdfsRename(m_hdfsfs, "/datacenter/test/tmp/test.nb", "/datacenter/data/test.nb"))
    {
        cout << "move file from /datacenter/test/tmp/test.nb to /datacenter/data/test.nb failed" << endl;
    }

    int FileNum;
    hdfsFileInfo* fileinfo;
    /*
     *试图浏览一个HDFS目录，并尝试打开该文件，如果打开失败，尝试删除该文件；如果打开成功，则获取文件的大小
     */
    if (NULL != (fileinfo = hdfsListDirectory(m_hdfsfs, "/datacenter/data", &FileNum)))
    {
        for (int i = 0; i < FileNum; i++,fileinfo++)
        {
            string mname = fileinfo->mName;
            cout << "ID:" << i << ",Name:" << mname << endl;
            hdfsFile hdfsTmpFile = hdfsOpenFile(m_hdfsfs, mname.c_str(), O_RDONLY, 0, 2, 0);
            if (!hdfsTmpFile)
            {
                hdfsCloseFile(m_hdfsfs,hdfsTmpFile);
                cout << "Open file ERROR:" << mname << endl;
                if(0 != hdfsDelete(m_hdfsfs, mname.c_str(),0))
                {
                    cout << "File in tmp dir open failed, del failed:" + mname << endl;
                }
                continue;
            }
            long long nTmpSize = hdfsAvailable(m_hdfsfs, hdfsTmpFile); 
            cout << "filesize:" << nTmpSize << "," << mname << endl;
        }

    }

    if ( 0 != hdfsDisconnect(m_hdfsfs) )
    {
        cout << __LINE__ << "hdfsDisconnect failed" << endl;
    }

    return 0;
}


bool ConnectToHdfs( )
{
    //创建一个HDFS Builder
    struct hdfsBuilder* pbld = hdfsNewBuilder(); 
    // 该地址为hadoop的core-site.xml中fs.defaultFS的配置
    hdfsBuilderSetNameNode(pbld, m_strNodeName.c_str());  
    //该端口为hadoop的core-site.xml中fs.defaultFS的配置 
    hdfsBuilderSetNameNodePort(pbld, nPort);  
    //使用hdfsBuilderConfSetStr()修改/新增相关环境配置，该函数提供key-value形式的配置方式，ex：
    hdfsBuilderConfSetStr(pbld, "fs.hdfs.impl", "org.apache.hadoop.hdfs.DistributedFileSystem");

    //创建TCP连接
    m_hdfsfs = hdfsBuilderConnect(pbld);
    //检查是否存在根目录，来判断是否连接成功
    int CheckHDFS = hdfsExists(m_hdfsfs,"/");   

    if (!m_hdfsfs || 0 != CheckHDFS)
    {   
        return false ;
    }
    else
    {
        return true ;
    }
}