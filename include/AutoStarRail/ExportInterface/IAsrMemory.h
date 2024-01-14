#ifndef ASR_MEMORY_H
#define ASR_MEMORY_H

#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/IAsrBase.h>

// {0E8E6129-512E-4AAB-A4A6-327ADCACB019}
ASR_DEFINE_GUID(
    ASR_IID_MEMORY,
    IAsrMemory,
    0xe8e6129,
    0x512e,
    0x4aab,
    0xa4,
    0xa6,
    0x32,
    0x7a,
    0xdc,
    0xac,
    0xb0,
    0x19);
SWIG_IGNORE(IAsrMemory)
ASR_INTERFACE IAsrMemory : public IAsrBase
{
    ASR_METHOD GetData(unsigned char** pp_out_data) = 0;
    ASR_METHOD GetSize(size_t * p_out_size) = 0;
    ASR_METHOD SetBeginOffset(ptrdiff_t offset) = 0;
};

SWIG_IGNORE(CreateIAsrMemory)
AsrResult CreateIAsrMemory(size_t size_in_byte, IAsrMemory** pp_out_memory);

#ifdef __cplusplus

class AsrMemory
{
    Asr::AsrPtr<IAsrMemory> p_data_;

public:
    AsrMemory(size_t size_in_bytes)
    {
        CreateIAsrMemory(size_in_bytes, p_data_.Put());
    }
    ~AsrMemory() = default;

    unsigned char* GetData() const
    {
        unsigned char* p_data{nullptr};
        p_data_->GetData(&p_data);
        return p_data;
    }

    size_t GetSize() const
    {
        size_t size{};
        p_data_->GetSize(&size);
        return size;
    }

    void SetBeginOffset(const ptrdiff_t offset)
    {
        p_data_->SetBeginOffset(offset);
    }

    // stl-like api
    unsigned char& operator[](size_t size_in_bytes)
    {
        return *(GetData() + size_in_bytes);
    }

    void resize(size_t new_size)
    {
        if (new_size > GetSize())
        {
            CreateIAsrMemory(new_size, p_data_.Put());
        }
    }

    IAsrMemory* GetImpl() const noexcept { return p_data_.Get(); };
};

#endif // __cplusplus

#endif // ASR_MEMORY_H
