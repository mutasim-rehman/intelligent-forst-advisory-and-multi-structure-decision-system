#ifndef IFAMDS_CUSTOM_PAIR_H
#define IFAMDS_CUSTOM_PAIR_H

namespace ifamds {

template<typename T1, typename T2>
struct CustomPair {
    T1 first;
    T2 second;

    CustomPair() : first(T1()), second(T2()) {}
    CustomPair(const T1& a, const T2& b) : first(a), second(b) {}
};

} // namespace ifamds

#endif // IFAMDS_CUSTOM_PAIR_H
