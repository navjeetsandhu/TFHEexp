#pragma once

#include <cmath>
#include <limits>

#include "cloudkey.hpp"
#include "detwfa.hpp"
#include "keyswitch.hpp"
#include "params.hpp"
#include "trlwe.hpp"
#include "utils.hpp"
#include "blindrotate.hpp"

namespace TFHEpp {

template <class P>
void GateBootstrappingTLWE2TLWEFFT(
    TLWE<typename P::targetP> &res, const TLWE<typename P::domainP> &tlwe,
    const BootstrappingKeyFFT<P> &bkfft,
    const Polynomial<typename P::targetP> &testvector)
{
    alignas(64) TRLWE<typename P::targetP> acc;
    BlindRotate<P>(acc, tlwe, bkfft, testvector);
    SampleExtractIndex<typename P::targetP>(res, acc, 0);
}

template <class P, uint32_t num_out>
void GateBootstrappingManyLUT(
    std::array<TLWE<typename P::targetP>, num_out> &res,
    const TLWE<typename P::domainP> &tlwe, const BootstrappingKeyFFT<P> &bkfft,
    const Polynomial<typename P::targetP> &testvector)
{
    alignas(64) TRLWE<typename P::targetP> acc;
    BlindRotate<P, num_out>(acc, tlwe, bkfft, testvector);
    for (int i = 0; i < num_out; i++)
        SampleExtractIndex<typename P::targetP>(res[i], acc, i);
}

template <class P, typename P::T mu>
constexpr Polynomial<P> mupolygen()
{
    Polynomial<P> poly;
    for (typename P::T &p : poly) p = mu;
    return poly;
}

template <class iksP, class bkP, typename bkP::targetP::T mu>
void GateBootstrapping(TLWE<typename iksP::domainP> &res,
                       const TLWE<typename iksP::domainP> &tlwe,
                       const EvalKey &ek)
{
    alignas(64) TLWE<typename iksP::targetP> tlwelvl0;
    IdentityKeySwitch<iksP>(tlwelvl0, tlwe, ek.getiksk<iksP>());
    GateBootstrappingTLWE2TLWEFFT<bkP>(res, tlwelvl0, ek.getbkfft<bkP>(),
                                       mupolygen<typename bkP::targetP, mu>());
}

}  // namespace TFHEpp