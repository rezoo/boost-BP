#pragma once

#include <vector>

#include <boost/graph/graph_traits.hpp>

enum edge_message_t { edge_message };
enum vertex_belief_t { vertex_belief };
namespace boost {
    BOOST_INSTALL_PROPERTY(edge, message);
    BOOST_INSTALL_PROPERTY(vertex, belief);
}

namespace bp {

template<typename Graph, typename Visitor,
         typename MessagePropertyMap,
         typename BeliefPropertyMap>
void sum_product(const Graph& graph, Visitor visitor,
                 MessagePropertyMap message_map,
                 BeliefPropertyMap belief_map) {
    typedef boost::graph_traits<Graph> Traits;
    typedef typename Traits::vertex_descriptor Vertex;
    typedef typename Traits::edge_descriptor Edge;
    typedef typename Traits::edges_size_type EdgesSizeType;

    // initialize messages
    visitor.init_messages(message_map, graph);
    visitor.init_beliefs(belief_map, graph);

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
                if(visitor.is_initialized(*ein, message_map, graph)) {
                    known_in_edges.push_back(*ein);
                } else {
                    unknown_in_edges.push_back(*ein);
                }
            }
            typename Traits::out_edge_iterator eout, eout_end;
            boost::tie(eout, eout_end) = boost::out_edges(*vi, graph);
            for(; eout != eout_end; ++eout) {
                if(!visitor.is_initialized(*eout, message_map, graph))
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
                        break;
                    }
                }
            } // end if statement
        } // end vertex loop
    } // end message loop

    // make belief
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
void sum_product(Graph& graph, Visitor visitor) {
    sum_product(
        graph, visitor,
        boost::get(edge_message, graph),
        boost::get(vertex_belief, graph));
}

} // namespace bp
