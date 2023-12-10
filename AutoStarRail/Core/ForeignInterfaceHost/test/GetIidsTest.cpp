#include <gtest/gtest.h>
#include <AutoStarRail/Utils/GetIids.hpp>
#include <AutoStarRail/PluginInterface/IAsrTask.h>


/**
 * @brief 由于Utils没有单独的测试，这个模块和这个测试比较相关，就先这么放了
 */
TEST(GetIidsTest, Default)
{
    ASR::AsrPtr<IAsrIidVector> p_iids;
    ASR::Utils::GetIids<ASR::Utils::IAsrTaskInheritanceInfo>(p_iids.Put());
    AsrSwigIidVector iids_wrapper{p_iids};
    ASSERT_EQ(iids_wrapper.Size().value, 1);
    ASSERT_EQ(iids_wrapper.At(0).value, ASR_IID_TASK);
}