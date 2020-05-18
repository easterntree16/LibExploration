
#include <iostream>
#include "xmsg_head.pb.h"
#include <fstream>
using namespace std;
using namespace xmsg;
void main_first()
{
    std::cout << "test protobuf!\n"; 
	XMsgHead msg1;
    msg1.set_msg_size(1024);
    msg1.set_msg_str("test0001");

    cout << "msg2 size=" << msg1.msg_size() << endl;
    cout << "msg2 msg_str=" << msg1.msg_str() << endl;
    
    //序列化到string
    string str1;
    msg1.SerializeToString(&str1);
    cout << "str1 size = " << str1.size() << endl;
    cout << str1 << endl;

    //序列化到文件
    ofstream ofs;
    ofs.open("test.txt", ios::binary);
    msg1.SerializePartialToOstream(&ofs);
    ofs.close();

    //从文件反序列化
    ifstream ifs;
    ifs.open("test.txt", ios::binary);
    XMsgHead msg2;
    cout<<msg2.ParseFromIstream(&ifs)<<endl;
    cout << "msg2 str = " << msg2.msg_str() << endl;

 
    //从string中反序列化
    msg2.set_msg_str("change msg2 str");
    string str2;
    msg2.SerializeToString(&str2);
    cout << "str2 size = " << str2.size() << endl;

    XMsgHead msg3;
    msg3.ParseFromArray(str2.data(), str2.size());
    cout << "msg3 str="<<msg3.msg_str() << endl;

}

