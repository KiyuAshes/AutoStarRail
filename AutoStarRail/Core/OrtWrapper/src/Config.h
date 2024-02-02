#ifndef ASR_CORE_IMPL_CONFIG_H
#define ASR_CORE_IMPL_CONFIG_H

#include <AutoStarRail/AsrConfig.h>

#define ASR_CORE_ORTWRAPPER_NS_BEGIN                                                 \
    ASR_NS_BEGIN namespace Core                                                \
    {                                                                          \
        namespace OrtWrapper                                                         \
        {

#define ASR_CORE_ORTWRAPPER_NS_END                                                   \
    }                                                                          \
    }                                                                          \
    ASR_NS_END

#endif // ASR_CORE_IMPL_CONFIG_H
