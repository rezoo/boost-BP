#pragma once

#include <vector>

#include <boost/graph/graph_traits.hpp>
#include <boost/optional.hpp>

#include "sum_product.hpp"

namespace bp {

template<typename Message, typename Belief>
struct optional_visitor {

    typedef Message message_type;
    typedef Belief belief_type;
    typedef boost::optional<Message> optional_type;
    typedef boost::property<vertex_belief_t, belief_type>
                belief_property_type;
    typedef boost::property<edge_message_t, optional_type>
                message_property_type;

    template<typename MessagePropertyMap, typename Graph>
    void init_messages(MessagePropertyMap message_map,
                       const Graph& graph) {
        typedef boost::graph_traits<Graph> Traits;

        typename Traits::edge_iterator ei, ei_end;
        boost::tie(ei, ei_end) = boost::edges(graph);
        for(; ei != ei_end; ++ei) {
            boost::put(message_map, *ei, optional_type());
        }
    }

    template<typename BeliefPropertyMap, typename Graph>
    void init_beliefs(BeliefPropertyMap, const Graph&) {}

    template<typename Edge,
             typename MessagePropertyMap,
             typename Graph>
    bool is_initialized(Edge edge,
                        MessagePropertyMap message_map,
                        const Graph&) {
        return boost::get(message_map, edge);
    }
};

} // namespace bp
