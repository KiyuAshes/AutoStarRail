#ifndef ASR_IINPUT_H
#define ASR_IINPUT_H

#include <AutoStarRail/IAsrInspectable.h>

typedef enum AsrClickType
{
    ASR_CLICK_LEFT = 1,
    ASR_CLICK_RIGHT = 2,
    ASR_CLICK_MIDDLE = 4
} AsrClickType;

typedef struct _asr_Point
{
    int32_t x;
    int32_t y;
} AsrPoint;

// {02F6A16A-01FD-4303-886A-9B60373EBE8C}
ASR_DEFINE_GUID(
    IAsrInput,
    ASR_IID_INPUT,
    0x2f6a16a,
    0x1fd,
    0x4303,
    0x88,
    0x6a,
    0x9b,
    0x60,
    0x37,
    0x3e,
    0xbe,
    0x8c);
ASR_INTERFACE IAsrInput : public IAsrInspectable
{
    /**
     * @brief If the implementation is a mouse, then the function assumes that
     * the left button is pressed.
     *
     * @param x
     * @param y
     * @return ASR_METHOD
     */
    ASR_METHOD Click(const int32_t x, const int32_t y);
};

// {DDB17BB3-E6B2-4FD8-8E06-C037EEF18D65}
ASR_DEFINE_GUID(
    IAsrTouch,
    ASR_IID_TOUCH,
    0xddb17bb3,
    0xe6b2,
    0x4fd8,
    0x8e,
    0x6,
    0xc0,
    0x37,
    0xee,
    0xf1,
    0x8d,
    0x65);
ASR_INTERFACE IAsrTouch : public IAsrInput
{
    ASR_METHOD Swipe(AsrPoint from, AsrPoint to, const int32_t duration_ms);
};

#endif // ASR_IINPUT_H
