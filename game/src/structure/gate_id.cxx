#include "gate_id.hxx"

GateIdentifier::GateIdentifier(std::string _gate, std::string _connect):
gate(_gate), connect(_connect)
{
}

GateIdentifier::GateIdentifier() {}

bool GateIdentifier::operator< (const GateIdentifier& b) const
{
    if (gate == b.gate)
        return (connect < b.connect);
    return gate < b.gate;
};

bool GateIdentifier::operator== (const GateIdentifier& b) const
{
    return (gate == b.gate) && (connect == b.connect);
};

bool GateIdentifier::operator!= (const GateIdentifier& b) const
{
    return !(operator==(b));
}

std::string to_string(GateIdentifier g)
{
    return g.gate + ":" + g.connect;
}