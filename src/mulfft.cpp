#include "mulfft.hpp"

namespace TFHEpp {
using namespace std;

template <class P>
void TwistFFT(Polynomial<P> &res, const PolynomialInFD<P> &a)
{
    if constexpr (std::is_same_v<typename P::T, uint32_t>)
        fftplvl1.execute_direct_torus32(res.data(), a.data());
    else if constexpr (std::is_same_v<typename P::T, uint64_t>)
        fftplvl2.execute_direct_torus64(res.data(), a.data());
    else
        static_assert(false_v<typename P::T>, "Undefined TwistFFT!");
}
#define INST(P) \
    template void TwistFFT<P>(Polynomial<P> & res, const PolynomialInFD<P> &a)
TFHEPP_EXPLICIT_INST_WRT_LVL0_1_2(INST)
#undef INST

template <class P>
void TwistIFFT(PolynomialInFD<P> &res, const Polynomial<P> &a)
{
    if constexpr (std::is_same_v<typename P::T, uint32_t>)
        fftplvl1.execute_reverse_torus32(res.data(), a.data());
    else if constexpr (std::is_same_v<typename P::T, uint64_t>)
        fftplvl2.execute_reverse_torus64(res.data(), a.data());
    else
        static_assert(false_v<typename P::T>, "Undefined TwistIFFT!");
}
#define INST(P) \
    template void TwistIFFT<P>(PolynomialInFD<P> & res, const Polynomial<P> &a)
TFHEPP_EXPLICIT_INST_WRT_LVL0_1_2(INST)
#undef INST

template <class P>
void PolyMul(Polynomial<P> &res, const Polynomial<P> &a, const Polynomial<P> &b)
{
    if constexpr (std::is_same_v<typename P::T, uint32_t>) {
        PolynomialInFD<P> ffta;
        TwistIFFT<P>(ffta, a);
        PolynomialInFD<P> fftb;
        TwistIFFT<P>(fftb, b);
        MulInFD<P::n>(ffta, ffta, fftb);
        TwistFFT<P>(res, ffta);
    }
    else if constexpr (std::is_same_v<typename P::T, uint64_t>) {
        for (int i = 0; i < P::n; i++) {
            typename P::T ri = 0;
            for (int j = 0; j <= i; j++)
                ri += static_cast<typename make_signed<typename P::T>::type>(
                          a[j]) *
                      b[i - j];
            for (int j = i + 1; j < P::n; j++)
                ri -= static_cast<typename make_signed<typename P::T>::type>(
                          a[j]) *
                      b[P::n + i - j];
            res[i] = ri;
        }
    }
    else
        static_assert(false_v<typename P::T>, "Undefined PolyMul!");
}
#define INST(P)                                                           \
    template void PolyMul<P>(Polynomial<P> & res, const Polynomial<P> &a, \
                             const Polynomial<P> &b)
TFHEPP_EXPLICIT_INST_WRT_LVL0_1_2(INST)
#undef INST

}  // namespace TFHEpp