#ifndef AUTOSTARRAIL_ASRSWIGPTR_HPP
#define AUTOSTARRAIL_ASRSWIGPTR_HPP

#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/AsrPtr.hpp>

class AsrSwigPtr{
    ASR::AsrPtr<IAsrSwigBase> p_impl_{};
public:
    static AsrSwigPtr Attach(IAsrSwigBase* p_swig_base){
        return {ASR::AsrPtr<IAsrSwigBase>::Attach(p_swig_base)};
    }
};

#endif // AUTOSTARRAIL_ASRSWIGPTR_HPP
