#ifdef ASR_EXPORT_PYTHON

#include <AutoStarRail/Core/ForeignInterfaceHost/PythonHost.h>
#include <gtest/gtest.h>

#include <AutoStarRail/Utils/StringUtils.h>
#include <boost/nowide/quoted.hpp>

#define ASR_VAR(x) #x ":", x

ASR_NS_ANONYMOUS_DETAILS_BEGIN

auto LogAndGetRelativePath(const std::filesystem::path& input)
    -> std::filesystem::path
{
    const auto relative_path = std::filesystem::relative(input);
    std::cout << ASR_VAR(boost::nowide::quoted(relative_path));
    return relative_path;
}

ASR_NS_ANONYMOUS_DETAILS_END

TEST(PythonHost, PathToPackageNameTest1)
{
    const auto current_path = std::filesystem::current_path();
    const auto path = current_path / "test1.py";

    const auto relative_path = Details::LogAndGetRelativePath(path);

    const auto expected_package_name = ASR::Core::ForeignInterfaceHost::
        PythonHost::PythonRuntime::ResolveClassName(relative_path);

    EXPECT_TRUE(expected_package_name);

    if (expected_package_name)
    {
        const auto package_name = reinterpret_cast<const char*>(
            expected_package_name.value().c_str());
        EXPECT_TRUE(
            std::strcmp(
                package_name,
                ASR_UTILS_STRINGUTILS_DEFINE_U8STR("test1"))
            == 0);
    }
}

TEST(PythonHost, PathToPackageNameTest2)
{
    auto path = std::filesystem::current_path() / "test";
    path /= "test2.py";

    const auto expected_package_name =
        ASR::Core::ForeignInterfaceHost::PythonHost::PythonRuntime::
            ResolveClassName(Details::LogAndGetRelativePath(path));

    EXPECT_TRUE(expected_package_name);

    if (expected_package_name)
    {
        const auto package_name = reinterpret_cast<const char*>(
            expected_package_name.value().c_str());
        EXPECT_TRUE(
            std::strcmp(
                package_name,
                ASR_UTILS_STRINGUTILS_DEFINE_U8STR("test.test2"))
            == 0);
    }
}

#endif // ASR_EXPORT_PYTHON