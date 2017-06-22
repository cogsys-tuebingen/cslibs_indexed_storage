#include <eigen3/Eigen/Core>
#include <iostream>
#include <memory>

#include <cslibs_indexed_storage/interface/data/align/aligned_array.hpp>
#include <cslibs_indexed_storage/interface/data/align/aligned_allocator.hpp>

namespace
{
struct DataInnerA
{
    Eigen::Matrix<double, 2, 1> a;
    Eigen::Matrix<double, 2, 2> b;
    double                      c;
    double                      d;
    Eigen::Matrix<double, 2, 2> e;
    Eigen::Matrix<double, 2, 2> f;
    Eigen::Matrix<double, 2, 1> g;
    Eigen::Matrix<double, 2, 2> h;
    double                      i;
    bool                        j;
    bool                        k;
};

struct DataInnerB
{
    bool                 a;
    double               b;
    std::complex<double> c;
    double               d;
    double               e;
};

template<typename Contained, std::size_t Alignment = alignof(Contained), std::size_t ParentAlign = 16/*alignof(std::max_align_t)*/>
struct alignas(Alignment) HardAlign
{
    char data[sizeof(Contained)];

    template<typename... Args>
    HardAlign(Args&&... args)
    {
        new (&data)Contained(std::forward<Args>(args)...);
    }

    ~HardAlign()
    {
        reinterpret_cast<Contained&>(data).~Contained();
    }
};

struct Data
{
    int a;
    std::vector<const void*> b;
//    std::aligned_storage<sizeof(DataInnerA), 32>::type c;
    DataInnerA c;
    DataInnerB d;

    Data()
    {
//        std::cout << reinterpret_cast<std::size_t>(&c) % 32 << std::endl;
    }


    virtual ~Data() {}
};

struct Test1 {int a; };
struct Test2 {Eigen::Vector2d x; };
struct Test3 {Eigen::Matrix4d x; };
struct Test4 : Test2, Test3 {};
}

int main()
{
    int a[0];
//    std::cout << EIGEN_DEFAULT_ALIGN_BYTES << std::endl;
//    std::cout << EIGEN_MALLOC_ALREADY_ALIGNED << std::endl;
//    std::cout << sizeof(Data) << std::endl;
//    std::cout << sizeof(Data::a) << std::endl;
//    std::cout << sizeof(Data::b) << std::endl;
//    std::cout << sizeof(Data::c) << std::endl;
//    std::cout << sizeof(Data::d) << std::endl;
//    using Wrap = HardAlign<Data, 32>;

//    std::cout << alignof(Wrap) << std::endl;
//    std::cout << alignof(Data) << std::endl;
//    std::cout << sizeof(Wrap::padding) << std::endl;
//    std::cout << sizeof(Wrap::data) << std::endl;

    std::cout << alignof(Test1) << std::endl;
    std::cout << alignof(Test2) << std::endl;
    std::cout << alignof(Test3) << std::endl;
    std::cout << alignof(std::pair<int, Test4>) << std::endl;

    cslibs_indexed_storage::interface::aligned_array<Data> array(24255);

//    auto array2 = new Data[24255];
    std::cout << &(array[0]) << std::endl;
}
