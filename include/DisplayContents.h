#ifndef DISPLAYCONTENTS_H
#include <string>
#include <deque>
#include <Arduino.h>

namespace DisplayContents
{
    enum MessageType
    {
        Status,
        Error,
        Label,
        Debug   
    };

    struct DisplayMessage
    {
        MessageType type;
        std::string message;
        uint8_t height;
    };

    class Content
    {
    private:
        const uint16_t width;
        const uint16_t height;
        /* data */
        std::deque<DisplayMessage> messages;
        uint16_t currentContentHeight();
        uint16_t currentContentWidth();
    public:
        Content(uint16_t width=128, uint16_t height=32);
        ~Content();
        void addMessage(DisplayMessage message);
    };
     
    

};

#endif // !DISPLAYCONTENTS_H
