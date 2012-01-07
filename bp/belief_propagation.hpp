#pragma once

#include <vector>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include "properties.hpp"

namespace bp {

template<typename Graph, typename Visitor,
         typename MessagePropertyMap,
         typename BeliefPropertyMap>
void belief_propagation(const Graph& graph, Visitor visitor,
                        MessagePropertyMap message_map,
                        BeliefPropertyMap belief_map) {
    typedef boost::graph_traits<Graph> Traits;
    typedef typename Traits::vertex_descriptor Vertex;
    typedef typename Traits::edge_descriptor Edge;
    typedef typename Traits::edges_size_type EdgesSizeType;

    // initialize messages
    visitor.init_messages(message_map, graph);
    visitor.init_beliefs(belief_map, graph);

    // initialize the exterior property map
    boost::vector_property_map<unsigned char> initialized_map;
    typename Traits::edge_iterator ei, ei_end;
    boost::tie(ei, ei_end) = boost::edges(graph);
    for(; ei != ei_end; ++ei) {
        initialized_map[boost::get(boost::edge_index, graph, *ei)] = 0;
    }

    // make messages
    EdgesSizeType n_unknown_messages = boost::num_edges(graph);
    typedef std::vector<Edge> EdgeVector;
    typedef typename EdgeVector::iterator EdgeIterator;
    EdgeVector unknown_in_edges;
    EdgeVector known_in_edges;
    EdgeVector unknown_out_edges;
    while(n_unknown_messages > 0) {
        typename Traits::vertex_iterator vi, vi_end;
        boost::tie(vi, vi_end) = boost::vertices(graph);
        for(; vi != vi_end; ++vi) {
            unknown_in_edges.clear();
            known_in_edges.clear();
            unknown_out_edges.clear();

            typename Traits::in_edge_iterator ein, ein_end;
            boost::tie(ein, ein_end) = boost::in_edges(*vi, graph);
            for(; ein != ein_end; ++ein) {
                if(initialized_map[
                        boost::get(boost::edge_index, graph, *ein)] == 1) {
                    known_in_edges.push_back(*ein);
                } else {
                    unknown_in_edges.push_back(*ein);
                }
            }
            typename Traits::out_edge_iterator eout, eout_end;
            boost::tie(eout, eout_end) = boost::out_edges(*vi, graph);
            for(; eout != eout_end; ++eout) {
                if(initialized_map[
                        boost::get(boost::edge_index, graph, *eout)] == 0)
                    unknown_out_edges.push_back(*eout);
            }

            if(unknown_in_edges.size() == 0) {
                EdgeIterator it = unknown_out_edges.begin();
                EdgeIterator it_end = unknown_out_edges.end();
                for(; it != it_end; ++it) {
                    visitor.make_message(
                        *it,
                        known_in_edges.begin(),
                        known_in_edges.end(),
                        message_map,
                        graph);
                    n_unknown_messages--;
                    initialized_map[
                        boost::get(boost::edge_index, graph, *it)] = 1;
                }
            } else if(unknown_in_edges.size() == 1) {
                Vertex target = boost::source(unknown_in_edges[0], graph);
                EdgeIterator it = unknown_out_edges.begin();
                EdgeIterator it_end = unknown_out_edges.end();
                for(; it != it_end; ++it) {
                    if(target == boost::target(*it, graph)) {
                        visitor.make_message(
                            *it,
                            known_in_edges.begin(),
                            known_in_edges.end(),
                            message_map,
                            graph);
                        n_unknown_messages--;
                        initialized_map[
                            boost::get(boost::edge_index, graph, *it)] = 1;
                        break;
                    }
                }
            } // end if statement
        } // end vertex loop
    } // end message loop

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
void belief_propagation(Graph& graph, Visitor visitor) {
    belief_propagation(
        graph, visitor,
        boost::get(boost::edge_message, graph),
        boost::get(boost::vertex_belief, graph));
}

} // namespace bp
