#include <cslibs_indexed_storage/storage/auto_index_storage.hpp>
#include <cslibs_indexed_storage/backend/array/array.hpp>
#include <cslibs_indexed_storage/backend/kdtree/kdtree.hpp>
#include <cslibs_indexed_storage/interface/index/index_std.hpp>

#include <iostream>
#include <cslibs_indexed_storage/storage.hpp>

using namespace cslibs_indexed_storage;
using namespace cslibs_indexed_storage::backend::array;
using namespace cslibs_indexed_storage::backend::kdtree;

struct Data
{
    float x;
    float y;

    inline void merge(const Data& other)
    {
        std::cout << "Index collision: " << x << ", " << y << " with " << other.x << ", " << other.y << std::endl;
    }
};

using Index = std::array<int, 2>;

namespace cslibs_indexed_storage
{
template<>
struct auto_index<Data>
{
    inline Index index(const Data& data) const
    {
        return { int(data.x), int(data.y) };
    }
};
}
int main()
{
    std::vector<Data> data;
    data.push_back({ 3.4, 5 });
    data.push_back({ 2, 5 });
    data.push_back({ 3.5f, 4.6f });
    data.push_back({ 3.4f, 4.8f });

    // manual indexing
    {
        using Storage = Storage<interface::non_owning<Data>, Index, KDTree, option::array_size<20, 20>, option::array_offset<int, -10, -10>, option::merge_strategy<option::MergeStrategy::MERGE>>;
        Storage storage;

        storage.insert({ 3, 5 }, &data[0]);
        storage.insert({ 2, 5 }, &data[1]);

        // these to have the same index
        storage.insert({ 3, 4 }, &data[2]);
        storage.insert({ 3, 4 }, &data[3]);

        storage.traverse([](const Storage::index_t&, const Storage::data_t& data)
                         {
                             std::cout << "Data: " << data.x << ", " << data.y << std::endl;
                         });
    }

    // auto indexing
    {
        using Storage = AutoIndexStorage<interface::non_owning<Data>, Array, option::array_size<20, 20>, option::array_offset<int, -10, -10>, option::merge_strategy<option::MergeStrategy::MERGE>>;
        Storage storage;

        storage.insert(&data[0]);
        storage.insert(&data[1]);

        // these to have the same index
        storage.insert(&data[2]);
        storage.insert(&data[3]);

        storage.traverse([](const Storage::index_t&, const Storage::data_t& data)
                         {
                             std::cout << "Data: " << data.x << ", " << data.y << std::endl;
                         });
    }
}