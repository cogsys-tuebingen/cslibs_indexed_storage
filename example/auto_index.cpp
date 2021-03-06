#include <cslibs_indexed_storage/storage/auto_index_storage.hpp>
#include <cslibs_indexed_storage/backend/simple/map.hpp>
#include <cslibs_indexed_storage/interface/index/index_std.hpp>

#include <iostream>

using namespace cslibs_indexed_storage;
using namespace cslibs_indexed_storage::backend::simple;

struct Data
{
    const float x;
    const float y;

    inline void merge(const Data& other)
    {
        std::cout << "Index collision: " << x << ", " << y << " with " << other.x << ", " << other.y << std::endl;
    }
};

namespace cslibs_indexed_storage
{
template<>
struct auto_index<Data>
{
    using index_t = std::array<int, 2>;

    inline index_t index(const Data& data) const
    {
        return { int(data.x), int(data.y) };
    }
};
}


int main()
{
    using Storage = AutoIndexStorage<Data, Map>;
    Storage storage;

    storage.insert({ 3.4, 5 });
    storage.insert({ 2, 5 });

    // these to have the same index
    storage.insert({ 3.5, 4.6 });
    storage.insert({ 3.4, 4.8 });

    storage.traverse([](const Storage::index_t&, const Data& data)
                     {
                         std::cout << "Data: " << data.x << ", " << data.y << std::endl;
                     });
}