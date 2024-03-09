#ifndef ASR_UTILS_CONFIG_H
#define ASR_UTILS_CONFIG_H

#include <AutoStarRail/AsrConfig.h>

#define ASR_UTILS_NS_BEGIN                                                     \
    ASR_NS_BEGIN namespace Utils                                               \
    {

#define ASR_UTILS_NS_END                                                       \
    }                                                                          \
    namespace Core                                                             \
    {                                                                          \
        namespace Utils                                                        \
        {                                                                      \
            using namespace ASR::Utils;                                        \
        }                                                                      \
    }                                                                          \
    ASR_NS_END

#endif // ASR_UTILS_CONFIG_H
