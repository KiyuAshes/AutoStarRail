#ifndef ASR_ASRPTR_HPP
#define ASR_ASRPTR_HPP

#include <AutoStarRail/IAsrBase.h>
#include <functional>
#include <utility>

ASR_NS_BEGIN

struct take_ownership_t
{
};

constexpr auto take_ownership = take_ownership_t{};

template <class T>
class AsrPtr
{
    template <class U>
    friend class AsrPtr;

protected:
    T* ptr_;

    void InternalAddRef() const
    {
        if (ptr_)
        {
            ptr_->AddRef();
        }
    }

    void InternalRelease() const
    {
        if (ptr_)
        {
            ptr_->Release();
        }
    }

    template <class U>
    void InternalCopy(U* p_other)
    {
        if (p_other != ptr_)
        {
            InternalRelease();
            ptr_ = p_other;
            InternalAddRef();
        }
    }

public:
    AsrPtr() noexcept : ptr_(nullptr) {}
    AsrPtr(decltype(nullptr)) noexcept : ptr_(nullptr) {}
    AsrPtr(T* p) : ptr_(p) { InternalAddRef(); }
    AsrPtr(const AsrPtr& other) : ptr_(other.Get()) { InternalAddRef(); }
    template <class U>
    AsrPtr(const AsrPtr<U>& other) : ptr_(other.Get())
    {
        InternalAddRef();
    }
    AsrPtr& operator=(const AsrPtr& other)
    {
        InternalCopy(other.ptr_);
        return *this;
    }
    template <class U>
    AsrPtr& operator=(const AsrPtr<U>& other)
    {
        InternalCopy(other.ptr_);
        return *this;
    }
    AsrPtr(AsrPtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr))
    {
    }
    template <class U>
    AsrPtr(AsrPtr<U>&& other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr))
    {
    }
    AsrPtr& operator=(AsrPtr&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            InternalRelease();
            ptr_ = std::exchange(other.ptr_, nullptr);
        }
        return *this;
    }
    template <class U>
    AsrPtr& operator=(AsrPtr<U>&& other) noexcept
    {
        InternalRelease();
        ptr_ = std::exchange(other.ptr_, nullptr);
        return *this;
    }
    ~AsrPtr() noexcept { InternalRelease(); }
    T*   operator->() const noexcept { return ptr_; }
    T&   operator*() const noexcept { return *ptr_; }
    bool operator==(const AsrPtr<T>& other) const noexcept
    {
        return ptr_ == other.ptr_;
    }
    explicit operator bool() const noexcept { return Get() != nullptr; }
    template <class Other>
    Other* As(const AsrGuid& id) const
    {
        void* result = nullptr;
        if (ptr_)
        {
            ptr_->QueryInterface(id, &result);
            ptr_->AddRef();
        }
        return static_cast<Other*>(result);
    }
    template <class Other>
    AsrResult As(AsrPtr<Other>& other) const
    {
        void* result = nullptr;
        if (ptr_)
        {
            const auto query_interface_result =
                ptr_->QueryInterface(AsrIidOf<Other>(), &result);
            if (IsFailed(query_interface_result))
            {
                return query_interface_result;
            }
            other = {static_cast<Other*>(result)};
            return ASR_S_OK;
        }
        return ASR_E_INVALID_POINTER;
    }
    template <class Other>
    AsrResult As(Other** pp_out_other) const
    {
        if (ptr_)
        {
            ptr_->QueryInterface(
                AsrIidOf<Other>(),
                reinterpret_cast<void**>(pp_out_other));
            (*pp_out_other)->AddRef();
            return ASR_S_OK;
        }
        return ASR_E_NO_INTERFACE;
    }
    T* Reset()
    {
        InternalRelease();
        return std::exchange(ptr_, nullptr);
    }
    T*  Get() const noexcept { return ptr_; }
    T** Put()
    {
        InternalRelease();
        ptr_ = nullptr;
        return &ptr_;
    }
    void**      PutVoid() { return reinterpret_cast<void**>(Put()); }
    friend void swap(AsrPtr& lhs, AsrPtr& rhs) noexcept
    {
        std::swap(lhs.ptr_, rhs.ptr_);
    }
    auto operator<=>(const AsrPtr& other) const noexcept
    {
        return other.ptr_ <=> ptr_;
    };
    static AsrPtr Attach(T* p)
    {
        AsrPtr result{nullptr};
        *result.Put() = p;
        return result;
    }
};

template <class T, class... Args>
auto MakeAsrPtr(Args&&... args)
{
    return AsrPtr<T>(new T(std::forward<Args>(args)...));
}

template <class Base, class T, class... Args>
auto MakeAsrPtr(Args&&... args)
{
    return AsrPtr<Base>(new T(std::forward<Args>(args)...));
}

#if __cplusplus >= 201703L
template <class T>
AsrPtr(T*) -> AsrPtr<T>;
#endif // __cplusplus >= 201703L

ASR_NS_END

template <class T>
struct std::hash<ASR::AsrPtr<T>>
{
    size_t operator()(const ASR::AsrPtr<T>& ptr) const
    {
        return std::hash<T*>()(ptr.Get());
    }
};

#endif // ASR_ASRPTR_HPP
