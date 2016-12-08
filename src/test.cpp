#include "../include/cslibs_clustering/storage.hpp"
#include "../include/cslibs_clustering/backend/nested_component_map.hpp"

#include <iostream>
#include <array>

namespace cc = cslibs_clustering;

namespace
{
struct DataType
{
    int value;
};

using IndexType = std::array<int, 2>;

}

int main(int argc, char* argv[])
{
    cc::Storage<DataType, IndexType, cc::backend::NestedComponentMap> storage;

    storage.insert({0, 1}, {2});
    storage.get({0, 0}).value = 1;

    std::cout << storage.get({0, 0}).value << std::endl;
    std::cout << storage.get({0, 1}).value << std::endl;
}
