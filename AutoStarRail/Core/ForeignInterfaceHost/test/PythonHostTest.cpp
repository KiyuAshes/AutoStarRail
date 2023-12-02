#include <gtest/gtest.h>
#include "../src/PythonHost.h"

#include <AutoStarRail/Utils/StringUtils.h>
#include <boost/nowide/quoted.hpp>

#define ASR_VAR(x) #x ":", x

TEST(PythonHost, PathToPackageNameTest)
{
    const auto current_path = std::filesystem::current_path();
    const auto path = current_path / "test1.py";

    const auto relative_path = std::filesystem::relative(path);
    std::cout << ASR_VAR(boost::nowide::quoted(relative_path));

    const auto exptected_package_name = ASR::Core::ForeignInterfaceHost::
        PythonHost::PythonRuntime::ResolveClassName(relative_path);

    EXPECT_TRUE(exptected_package_name);

    if (exptected_package_name)
    {
        const auto package_name = reinterpret_cast<const char*>(
            exptected_package_name.value().c_str());
        EXPECT_TRUE(
            std::strcmp(package_name, ASR_UTILS_STRINGUTILS_DEFINE_U8STR("test1.py")));
    }
}