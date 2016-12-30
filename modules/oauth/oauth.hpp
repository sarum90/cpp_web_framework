#pragma once

#include "reactor.hpp"
#include "http_client.hpp"
#include "mestring.hpp"

namespace meoauth {

future<std::string> authenticate(
    http::client* cl,
    mes::mestring_cat scopes,
    mes::mestring_cat credentials_json_file
);

}
