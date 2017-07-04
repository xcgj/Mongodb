#include <cstdint>
#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <string>
#include <vector>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>    //官方文档缺失的头文件
#include <mongocxx/result/insert_one.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;
using namespace std;

/*mongodb API练习*/
//命令行：g++ --std=c++11 test.cpp -o test.out -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0 -L/usr/local/lib -lmongocxx -lbsoncxx

int main()
{
	//创建 mongo 实例
	mongocxx::instance instance{};
	//创建客户端连接
    mongocxx::client client{mongocxx::uri{}};//uri不填参数将默认连接到在本地运行的，端口号为27017的服务器上
	//获取数据库
	mongocxx::database db = client["testdb"];
	//获取集合
	mongocxx::collection coll = db["testC2"];
	//创建流构建器
    auto builder = bsoncxx::builder::stream::document{};//专门用来生成文档
	//创建一个文档
    bsoncxx::document::value doc_value = builder << "name" << "aaa" << "age" << 10 << finalize;
    //如果要使用value(只读属性)， 需要先转成view
    bsoncxx::document::view view = doc_value.view();
        //获取view中的元素
    bsoncxx::document::element element = view["name"];
    std::string name = element.get_utf8().value.to_string();
    cout << name << endl;

    int i = 0;

    /*调用集合是实例 coll 操作API*/
    //=====插入单个文档=====
    //数据库名.集合名.insert({name:"fff",age:15});
    /*  doc_value是只读属性，是个左值，对其插入数据需要右值引用转变类型
    //doc_value = builder << "name" << "fff" << "age" << 15 << finalize;
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one((bsoncxx::v_noabi::document::value&&)doc_value);
    //获取插入后的id
    const bsoncxx::types::value& id_value = mongocxx::result::insert_one::inserted_id(coll);
    */

    //=====插入多个文档=====
    //数据库名.集合名.insert({name:i,age:i});   多次插入
    /*
    vector<bsoncxx::document::value> documents;
    for(i=0; i<3; ++i)
    {
        documents.push_back(bsoncxx::builder::stream::document{}<<"name"<<i<<"age"<<i<<finalize);
    }
    coll.insert_many(documents);
    */

    //=====查询个文档=====
    //数据库名.集合名.find({$or:[{name:'bbb'},{name:'ccc'}]})   只匹配第一条
    /*
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = coll.find_one(document{}<<"$or"<<open_array<<open_document<<"name"<<"bbb"<<close_document<<open_document<<"name"<<"ccc"<<close_document<<close_array<<finalize);
    if(maybe_result)
    {
        cout<<bsoncxx::to_json(*maybe_result)<<endl;
    }
    */

    //=====cursor查询多条文档=====
    //数据库名.集合名.find({$or:[{name:'ccc'},{name:'aaa'}]})   尽可能多匹配
    /*
    mongocxx::cursor cursor = coll.find(document{}<<"$or"<<open_array<<open_document<<"name"<<"ccc"<<close_document<<open_document<<"name"<<"aaa"<<close_document<<close_array<<finalize);
    for(auto doc:cursor)    //遍历游标，把符合条件的文档都打印出来
    {
        cout<<bsoncxx::to_json(doc)<<endl;
    }
    */

    //=====更新单个文档=====
    //数据库名.集合名.update({name:'aaa'},{$set:{age:20}},是否建新文档,是否更新所有)   只匹配第一条
    /*  builder等同于document{}
    coll.update_one(builder<<"name"<<"aaa"<<finalize,builder<<"$set"<<open_document<<"age"<<20<<close_document<<finalize);
    */

    //=====更新多个文档=====
    //数据库名.集合名.update({name:'aaa'},{$set:{age:+30}},是否建新文档,是否更新所有)   尽可能多匹配
    /*
    //给匹配到的文档，年龄加上30
    bsoncxx::stdx::optional<mongocxx::result::update> result = coll.update_many(builder<<"name"<<"aaa"<<finalize,builder<<"$inc"<<open_document<<"age"<<30<<close_document<<finalize);
    if(result)
    {
        cout << result->modified_count() <<endl;//显示被更改的行数
    }
    */

    //=====删除单个文档=====
    //数据库名.集合名.remove({name:"fff"});
    //coll.delete_one(document{}<<"name"<<"fff"<<finalize);

    //=====删除多条文档=====
    //数据库名.集合名.remove({name:"aaa",false,writeConcern.NONE});
    /*
    bsoncxx::stdx::optional<mongocxx::result::delete_result> result = coll.delete_many(document{}<<"name"<<"aaa"<<finalize);
    if (result)
    {
        cout << result->deleted_count() <<endl;//显示被更改的行数
    }
    */

    //====创建集合索引====
    //数据库名.集合名.ensureIndex({name:1,age:1});
    coll.create_index(document{}<<"name"<<1<<"age"<<1<<finalize);

    return 0;
}


