#include <cslibs_clustering/storage/auto_index_storage.hpp>
#include <cslibs_clustering/backend/array/array.hpp>
#include <cslibs_clustering/interface/index/index_std.hpp>

#include <iostream>

using namespace cslibs_clustering;
using namespace cslibs_clustering::backend::array;

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


int main()
{
    using Storage = Storage<Data*, Index, Array, option::array_size<20, 20>, option::array_offset<int, -10, -10>, option::merge_strategy<option::MergeStrategy::MERGE>>;
    Storage storage;

    std::vector<Data> data;
    data.push_back({3.4, 5});
    data.push_back({2, 5});
    data.push_back({3.5f, 4.6f});
    data.push_back({3.4f, 4.8f});

    storage.insert({ 3, 5 }, &data[0]);
    storage.insert({ 2, 5 }, &data[1]);

    // these to have the same index
    storage.insert({ 3, 4 }, &data[2]);
    storage.insert({ 3, 4 }, &data[3]);

    storage.traverse([](const Storage::index_t&, const Storage::data_t& data)
                     {
                         std::cout << "Data: " << data->x << ", " << data->y << std::endl;
                     });
}