#include "olinkcontent.h"

namespace ApiGear {
namespace ObjectLink {


inline void from_json(const nlohmann::json& j, InitialProperty& p) {

    p.propertyName = j[0].get<std::string>();
    p.propertyValue.content = j[1];
}

inline void to_json(nlohmann::json& j, const InitialProperty& p) {
    j = nlohmann::json{ p.propertyName, p.propertyValue.content };
}

bool operator==(const OLinkContent& lhs, const OLinkContent& rhs)
{
    return lhs.content == rhs.content;
}


void fillContent(nlohmann::json& content_array, size_t current)
{
    (void)content_array;
    (void)current;
}


std::string toString(const OLinkContent& content)
{
    return content.content.dump();
}


}} //namespace ApiGear::ObjectLink