#pragma once

#include <map>
#include <vector>
#include <memory>

namespace test
{
struct Verifier
{
    std::map<int, std::vector<int>> merges;
};

struct Data
{
    std::shared_ptr<Verifier> verifier;

    int id;
    /*
    Data()
    {
        std::cout << "Default Construct" << std::endl;
    }
    Data(Verifier* verifier, int id) : verifier(verifier), id(id)
    {
        std::cout << "Construct: " << id << std::endl;
    }
    Data(const Data& other) : id(other.id), verifier(other.verifier)
    {
        std::cout << "Copy Construct: " << other.id << std::endl;
    }
    Data(Data&& other) : id(other.id), verifier(other.verifier)
    {
        std::cout << "Move Construct: " << other.id << std::endl;
    }

    Data& operator=(const Data& other)
    {
        this->id = other.id;
        this->verifier = other.verifier;
        std::cout << "Copy Assign: " << other.id << std::endl;
        return *this;
    }
    Data& operator=(Data&& other)
    {
        this->id = other.id;
        this->verifier = other.verifier;
        std::cout << "Move Assign: " << other.id << std::endl;
        return *this;
    }
    */

    inline void merge(const Data& other)
    {
        verifier->merges[id].push_back(other.id);
    }
};

using Index = std::array<int, 2>;

template<typename T>
inline std::shared_ptr<Verifier> insert_owning_data(T& storage)
{
    auto verifier = std::make_shared<Verifier>();

    // 1 merge
    storage.insert({ 1, 1 }, { verifier, 1 });
    storage.insert({ 1, 1 }, { verifier, 2 });

    // 0 merge
    storage.insert({ 1, 2 }, { verifier, 3 });

    // 0 merge
    storage.insert({ 2, 1 }, { verifier, 4 });

    // 2 merges
    storage.insert({ 2, 2 }, { verifier, 5 });
    storage.insert({ 2, 2 }, { verifier, 6 });
    storage.insert({ 2, 2 }, { verifier, 7 });

    return verifier;
}

template<typename T>
inline std::shared_ptr<Verifier> insert_nonowning_data(T& storage, std::vector<Data>& data)
{
    auto verifier = std::make_shared<Verifier>();

    data.push_back({ verifier, 1 });
    data.push_back({ verifier, 2 });
    data.push_back({ verifier, 3 });
    data.push_back({ verifier, 4 });
    data.push_back({ verifier, 5 });
    data.push_back({ verifier, 6 });
    data.push_back({ verifier, 7 });

    // 1 merge
    storage.insert({ 1, 1 }, &data[0]);
    storage.insert({ 1, 1 }, &data[1]);

    // 0 merge
    storage.insert({ 1, 2 }, &data[2]);

    // 0 merge
    storage.insert({ 2, 1 }, &data[3]);

    // 2 merges
    storage.insert({ 2, 2 }, &data[4]);
    storage.insert({ 2, 2 }, &data[5]);
    storage.insert({ 2, 2 }, &data[6]);

    return verifier;
}


}