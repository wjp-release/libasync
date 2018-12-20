#include "gtest/gtest.h"
#include <boost/hof.hpp>
#include <string>

using namespace boost::hof;

struct sum
{
    template<class T, class U>
    T operator()(T x, U y, U z) const
    {
        return x+y+z;
    }
};

class HofTOT : public ::testing::Test {
protected:
	HofTOT() {}
	virtual ~HofTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(HofTOT, pipable) {
    constexpr pipable_adaptor<sum> evil_sum{sum()};
    EXPECT_EQ(6 , (1 | pipable(sum())(2,3))) ;
    EXPECT_EQ(6 , pipable(sum())(1, 2, 3));
    EXPECT_EQ(6 , evil_sum(1, 2, 3));
    EXPECT_EQ(6 , 1 | evil_sum(2, 3));
}

// C++17 inline variable: https://en.cppreference.com/w/cpp/language/inline

static inline constexpr pipable_adaptor<sum> inline_sum{sum()};

TEST_F(HofTOT, inline_pipable) {
    EXPECT_EQ(6 , inline_sum(1, 2, 3));
    EXPECT_EQ(6 , 1 | inline_sum(2, 3));
}



template <class R>
struct templated_sum
{
    template<class T, class U>
    R operator()(T x, U y, U z) const
    {
        return R(std::to_string(x+y+z));
    }
};

// C++14 variable template: https://en.cppreference.com/w/cpp/language/variable_template

template < class R >
static inline constexpr pipable_adaptor<templated_sum<R>> t_sum{templated_sum<R>()};

TEST_F(HofTOT, templated_pipable) {
    EXPECT_EQ("6" , t_sum<std::string>(1, 2, 3));
    EXPECT_EQ("6" , 1 | t_sum<std::string>(2, 3));
}


