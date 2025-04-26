#include "hjson.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

HJson* TestDeserialize(const char* json_file) {
    // Reading file
    std::ifstream in(json_file);
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string file_content = buffer.str();
    std::cout
        << file_content
        << std::endl;
    // Deserialize
    return HJson_parse(file_content.c_str());
}

void TestOperator(HJson* node) {
    HJson* p = node->at(0);
    HJson* id = p->get("actor")->get("id");
    std::cout << (id == nullptr ? 100 : id->ToInt()) << std::endl;
}

void TestSerialize(HJson* node) {
    // Serialize
    int out_len = 0;
    const char* ret = HJson_write(node, out_len);
    std::cout
        << "Serialize result: "
        << '\n'
        << ret
        << std::endl;
    delete ret;
}

void TestCreateArray() {
    HJson* array_node = HJson_createArray();
    HJson* object_node = HJson_createObject();
    HJson* string_node = HJson_createString("Value-1");
    HJson_addItemToObject(object_node, "Key-1", string_node);
    HJson* string_node1 = HJson_createString("Value-2");
    HJson_addItemToObject(object_node, "Key-2", string_node1);
    HJson_addItem(array_node, object_node);
    TestSerialize(array_node);
    HJson_delete(array_node);
}

int main(int argc, char const *argv[])
{
    HJson* root_node = 0;
    root_node = TestDeserialize("events.json");
    TestOperator(root_node);
    TestSerialize(root_node);
    TestCreateArray();
    HJson_delete(root_node);
    return 0;
}
