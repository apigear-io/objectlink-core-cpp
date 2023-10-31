#pragma once

#include "types.h"
#include "olink_common.h"
#include "nlohmann/json.hpp"
#include "olinkcontent.h"
#include <string>
#include "imessageserializer.h"
#include "defaultcontentserializer.h"


namespace ApiGear {
namespace ObjectLink {

/**
* Choose one of the available message formats for object link protocol messages.
* It describes network packing only, the message is still formatted with nlohmann::json
*/
enum MessageFormat
{
    JSON = 1,
    BSON = 2,
    MSGPACK = 3,
    CBOR = 4
};

class NlohmannMessageWriter: public IMessageWriter
{
public:
    NlohmannMessageWriter()
    {
        message.message = nlohmann::json::array();
    }

    OLinkMessage& writeNext(MsgType msgType) final
    {
        message.message.emplace_back(msgType);
        return message;
    }
    
    OLinkMessage& writeNext(int value) final
    {
        message.message.emplace_back(value);
        return message;
    }

    OLinkMessage& writeNext(const std::string& value) final
    {
        message.message.emplace_back(value);
        return message;
    }

    OLinkMessage& writeNext(const OLinkContent& value) final
    {
        message.message.emplace_back(value.content);
        return message;
    }


private:
    OLinkMessage message;
};



class OLINK_EXPORT NlohmannMessageConverter : public INetworkFormatConverter
{
public:
    /**ctor
    * @param format network message format used for packing messages
    */
    NlohmannMessageConverter(MessageFormat format)
        : m_format(format)
    {}
    /**
    * Change network format used for message packing.
    * @param format Requested message format.
    */
    void setMessageFormat(MessageFormat format)
    {
        m_format = format;
    }

    OLinkMessage fromNetworkFormat(const std::string& message)
    {
        OLinkMessage olinkMessage;
        switch (m_format) {
        case MessageFormat::JSON:
            olinkMessage.message = nlohmann::json::parse(message); break;
        case MessageFormat::BSON:
            olinkMessage.message = nlohmann::json::from_bson(message); break;
        case MessageFormat::MSGPACK:
            olinkMessage.message = nlohmann::json::from_msgpack(message); break;
        case MessageFormat::CBOR:
            olinkMessage.message = nlohmann::json::from_cbor(message); break;
        }

        return olinkMessage;
    }

    ::std::string toNetworkFormat(const OLinkMessage& j)
    {
        switch (m_format) {
        case MessageFormat::JSON:
            return j.message.dump();
        case MessageFormat::BSON:
        {
            auto bsonData = nlohmann::json::to_bson(j.message);
            return std::string(bsonData.begin(), bsonData.end());
        }
        case MessageFormat::MSGPACK:
        {
            auto msgPackData = nlohmann::json::to_msgpack(j.message);
            return std::string(msgPackData.begin(), msgPackData.end());
        }
        case MessageFormat::CBOR:
        {
            auto cbotData = nlohmann::json::to_cbor(j.message);
            return std::string(cbotData.begin(), cbotData.end());
        }
        }
        return std::string();
    }
private:
    /**Currently used network message format*/
    MessageFormat m_format;
};

class NlohmannMessageReader : public IMessageReader
{
public:
    NlohmannMessageReader(const OLinkMessage& message)
        :m_message(message)
    {}

    void readNext(std::string& value) final
    {
        getNext(value);
    }

    void readNext(OLinkContent& value) final
    {
        value.content = m_message.message[currentIndex].get<nlohmann::json>();
        currentIndex++;
    }

    void readNext(int value) final
    {
        getNext(value);
    }


    virtual std::string getAsString() const
    {
        return m_message.message.dump();
    }

    void readNext(MsgType& arg)
    {
        arg = static_cast<MsgType>(m_message.message[currentIndex].get<int>());
        currentIndex++;
    }

    bool validate(std::string& out_error) const
    {
        if (!m_message.message.is_array()) {
            out_error = "message must be array";
            return false;
        }
        return true;
    }
private:

    template<typename ArgType>
    void getNext(ArgType& arg)
    {
        arg = m_message.message[currentIndex].get<ArgType>();
        currentIndex++;
    }

    const OLinkMessage& m_message;
    size_t currentIndex = 0;
};

class NlohmannMessageSerializer : public IMessageSerializer
{
public:
    NlohmannMessageSerializer(MessageFormat format = MessageFormat::JSON)
        : converter(format)
    {
    }

    void setMessageFormat(MessageFormat format)
    {
        converter.setMessageFormat(format);
    }

    std::unique_ptr<IMessageReader> createReader(const OLinkMessage& msg) final
    {
        return std::make_unique< NlohmannMessageReader>(msg);
    }
    std::unique_ptr<IMessageWriter> createWriter() final
    {
        return std::make_unique< NlohmannMessageWriter>();
    }
    OLinkMessage fromNetworkFormat(const std::string& message) final
    {
        return converter.fromNetworkFormat(message);
    }
    std::string toNetworkFormat(const OLinkMessage& j)final
    {
        return converter.toNetworkFormat(j);
    }
private:
    NlohmannMessageConverter converter;
    size_t index = 0;
};

}} //ApiGear::ObjectLink