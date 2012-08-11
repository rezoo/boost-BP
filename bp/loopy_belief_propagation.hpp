#pragma once

#include <vector>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include "properties.hpp"

namespace bp {

template<typename Graph, typename Visitor,
         typename MessagePropertyMap,
         typename BeliefPropertyMap>
void apply_loopy_belief_propagation(const Graph& graph, Visitor visitor,
                                    MessagePropertyMap message_map,
                                    BeliefPropertyMap belief_map,
                                    std::size_t n_iteration) {
    typedef boost::graph_traits<Graph> Traits;
    typedef typename Traits::vertex_descriptor Vertex;
    typedef typename Traits::edge_descriptor Edge;

    // initialize messages
    visitor.init_messages(message_map, graph);
    visitor.init_beliefs(belief_map, graph);

    // make messages
    std::vector<Edge> in_vector;
    for(std::size_t n=0; n<n_iteration; ++n) {
        typename Traits::edge_iterator ei, ei_end;
        boost::tie(ei, ei_end) = boost::edges(graph);
        for(; ei != ei_end; ++ei) {
            in_vector.clear();
            const Vertex source = boost::source(*ei);
            const Vertex target = boost::target(*ei);
            typename Traits::in_edge_iterator ein, ein_end;
            boost::tie(ein, ein_end) = boost::in_edges(source, graph);
            for(; ein != ein_end; ++ein) {
                if(boost::source(*ein, graph) == target)
                    in_vector.push_back(*ein)
            }
            visitor.make_message(
                *ei,
                in_vector.begin(),
                in_vector.end(),
                message_map,
                graph);
        }
    }

    // make beliefs
    typename Traits::vertex_iterator vi, vi_end;
    boost::tie(vi, vi_end) = boost::vertices(graph);
    for(; vi != vi_end; ++vi) {
        typename Traits::in_edge_iterator ein, ein_end;
        boost::tie(ein, ein_end) = boost::in_edges(*vi, graph);
        visitor.make_belief(
            *vi, ein, ein_end,
            message_map,
            belief_map,
            graph);
    }
}

template<typename Graph, typename Visitor>
void apply_loopy_belief_propagation(Graph& graph, Visitor visitor,
                                    std::size_t n_iteration) {
    apply_loopy_belief_propagation(
        graph, visitor,
        boost::get(boost::edge_message, graph),
        boost::get(boost::vertex_belief, graph),
        n_iteration);
}

} // namespace bp
