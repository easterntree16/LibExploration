
#include <iostream>
#include "xmsg_head.pb.h"
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include <fstream>
using namespace std;
using namespace xmsg;
using namespace google;
using namespace protobuf;
using namespace compiler;
int main()
{
    std::cout << "test reflect protobuf!\n"; 
   
    //1 通过message 对象和字段名称设置和获取内容
    XMsgHead msg;

    //描述对象
	auto descriptor = msg.GetDescriptor();

    //反射对象
    auto reflecter = msg.GetReflection();

    //属性对象
    string fname = "msg_size";
    auto size_field = descriptor->FindFieldByName(fname);
    if (!size_field)
    {
        cerr << "FindFieldByName " << fname << " failed!" << endl;
        return -1;

    }

    //设置属性的值
    reflecter->SetInt32(&msg, size_field, 1024);

    //获取属性的值
    cout<<fname<<"= "<<reflecter->GetInt32(msg, size_field)<<endl;

    /////////////////////////////////////////////////////////
    ///运行时解析proto文件
    // 准备文件系统
    DiskSourceTree source_tree;
    //设置一个字符串，用来做路径替换
    source_tree.MapPath("protobuf_root", "./");

    //创建动态编译器
    Importer importer(&source_tree, NULL);
    string proto_filename = "xmsg_head.proto";
    //动态编译proto文件，如果已经编译过，直接返回缓冲对象
    string path = "protobuf_root/";
    path += proto_filename;
    //返回的文件描述对象，包含文件依赖关系
    auto file_desc = importer.Import(path);
    if (!file_desc)
    {
        cerr << "importer.Import " << path << " failed!" << endl;
        return -1;
    }
    //获取Message类型
    auto message_desc = importer.pool()->FindMessageTypeByName("xmsg.XMsgHead");

    //用消息工厂，创建消息对象
    DynamicMessageFactory factory;
    //创建一个类型原型
    auto message_proto = factory.GetPrototype(message_desc);
    auto msg_test = message_proto->New();
    {
        //描述对象
        auto descriptor = msg_test->GetDescriptor();

        //反射对象
        auto reflecter = msg_test->GetReflection();

        //属性对象
        string fname = "msg_str";
        auto str_field = descriptor->FindFieldByName(fname);
        if (!str_field)
        {
            cerr << "FindFieldByName " << fname << " failed!" << endl;
            return -1;

        }

        //设置属性的值
        reflecter->SetString(msg_test, str_field, "test dy proto str");

        //获取属性的值
        cout <<"test dy proto "<< fname << "= " << reflecter->GetString(*msg_test, str_field) << endl;
    }



}

