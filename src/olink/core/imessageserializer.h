#pragma once

#include "types.h"
#include "olink_common.h"
#include <memory>
#include <string>

namespace ApiGear
{
namespace ObjectLink {

class OLINK_EXPORT IMessageReader
{
public:
    virtual ~IMessageReader() = default;
    virtual void readNext(MsgType& arg) = 0;
    virtual void readNext(std::string& value) = 0;
    virtual void readNext(OLinkContent& props) = 0;
    virtual void readNext(int value) = 0;
    virtual bool validate(std::string& out_error) const = 0;
    virtual std::string getAsString() const = 0;
};

class OLINK_EXPORT INetworkFormatConverter
{
public:
    virtual ~INetworkFormatConverter() = default;
    /**
    * Unpacks message received from network according to selected message format.
    * @param message A message received from network.
    * @return Unpacked message in json format.
    */
    virtual OLinkMessage fromNetworkFormat(const std::string& message) = 0;
    /**
    * Formats message to selected network message format.
    * @param j Message to send, not formated.
    * @return message in network message format.
    */
    virtual std::string toNetworkFormat(const OLinkMessage& j) = 0;
};


class OLINK_EXPORT IMessageWriter
{
public:
    virtual ~IMessageWriter() = default;
    virtual OLinkMessage& writeNext(MsgType msgType) = 0;
    virtual OLinkMessage& writeNext(const std::string& value) = 0;
    virtual OLinkMessage& writeNext(const OLinkContent& props) = 0;
    virtual OLinkMessage& writeNext(int id) = 0;
};

class  OLINK_EXPORT IMessageSerializer : public INetworkFormatConverter
{
public:
    virtual ~IMessageSerializer() = default;
    virtual std::unique_ptr<IMessageReader> createReader(const OLinkMessage& ) = 0;
    virtual std::unique_ptr<IMessageWriter> createWriter() = 0;
};



}} // namespace ApiGear::ObjectLink
