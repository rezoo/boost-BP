#pragma once

#include <boost/graph/properties.hpp>

namespace boost {
    enum edge_message_t { edge_message };
    enum vertex_belief_t { vertex_belief };
    BOOST_INSTALL_PROPERTY(edge, message);
    BOOST_INSTALL_PROPERTY(vertex, belief);
}
