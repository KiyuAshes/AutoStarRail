#include "AsrOrt.h"

ASR_CORE_ORTWRAPPER_NS_BEGIN

class PPOcr : public AsrOrt
{
    using Base = AsrOrt;
    Ort::Session session_;

public:
    PPOcr();
};

ASR_CORE_ORTWRAPPER_NS_END