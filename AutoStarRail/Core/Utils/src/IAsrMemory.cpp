#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/ExportInterface/IAsrMemory.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <memory>
#include <cstring>

namespace
{
    class AsrMemoryImpl final : public IAsrMemory
    {
        unsigned char*                   p_offset_data_;
        size_t                           size;
        std::unique_ptr<unsigned char[]> up_data_{};

    public:
        AsrMemoryImpl(const size_t size_in_bytes) : size{size_in_bytes}
        {
            up_data_ = std::make_unique<unsigned char[]>(size_in_bytes);
            p_offset_data_ = up_data_.get();
        }

        ASR_UTILS_IASRBASE_AUTO_IMPL(AsrMemoryImpl);

        ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object) override
        {
            return ASR::Utils::QueryInterface<IAsrMemory>(this, iid, pp_object);
        }

        ASR_IMPL GetData(unsigned char** pp_out_data) override
        {
            ASR_UTILS_CHECK_POINTER(pp_out_data)

            *pp_out_data = p_offset_data_;
            return ASR_S_OK;
        }

        ASR_IMPL GetRawData(unsigned char** pp_out_data) override
        {
            ASR_UTILS_CHECK_POINTER(pp_out_data);

            *pp_out_data = up_data_.get();
            return ASR_S_OK;
        }

        ASR_IMPL GetSize(size_t* p_out_size) override
        {
            ASR_UTILS_CHECK_POINTER(p_out_size)

            *p_out_size = size;
            return ASR_S_OK;
        }

        ASR_IMPL SetOffset(ptrdiff_t offset) override
        {
            if (const auto ptrdiff_t_size = static_cast<ptrdiff_t>(size);
                offset < ptrdiff_t_size) [[likely]]
            {
                p_offset_data_ = up_data_.get() + offset;
                return ASR_S_OK;
            }

            ASR_CORE_LOG_ERROR(
                "Invalid offset detected: input {} should be less than {}.",
                offset,
                size);

            return ASR_E_OUT_OF_RANGE;
        }

        ASR_IMPL GetOffset(ptrdiff_t* p_out_offset) override
        {
            ASR_UTILS_CHECK_POINTER(p_out_offset)

            *p_out_offset = p_offset_data_ - up_data_.get();

            return ASR_S_OK;
        }

        ASR_IMPL Resize(size_t new_size_in_byte) override
        {
            if (new_size_in_byte > size)
            {
                auto up_new_data = std::make_unique<unsigned char[]>(size);

                std::memcpy(up_new_data.get(), up_data_.get(), size);

                up_data_ = std::move(up_new_data);
                size = new_size_in_byte;

                return ASR_S_OK;
            }
            return ASR_S_FALSE;
        }
    };
} // namespace

AsrResult CreateIAsrMemory(size_t size_in_byte, IAsrMemory** pp_out_memory)
{
    ASR_UTILS_CHECK_POINTER(pp_out_memory)

    try
    {
        auto* const p_memory = new AsrMemoryImpl(size_in_byte);
        p_memory->AddRef();
        *pp_out_memory = p_memory;
        return ASR_S_OK;
    }
    catch (std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}
