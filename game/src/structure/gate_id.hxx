#pragma once

#include <string>

struct GateIdentifier {
    std::string gate;
    std::string connect;
    GateIdentifier();
    GateIdentifier(std::string _gate, std::string _connect);
    bool operator< (const GateIdentifier& b) const;
    bool operator== (const GateIdentifier& b) const;
};
