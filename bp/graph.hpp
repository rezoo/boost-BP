#pragma once

#include <utility>
#include <iterator>
#include <vector>

#include <boost/optional.hpp>
#include <boost/graph/graph_traits.hpp>

enum edge_message_t { edge_message };
enum vertex_belief_t { vertex_belief };
namespace boost {
    BOOST_INSTALL_PROPERTY(edge, message);
    BOOST_INSTALL_PROPERTY(vertex, belief);
}

namespace bp {

template<typename Graph, typename Visitor>
void sum_product(Graph& graph, Visitor visitor) {
    typedef boost::graph_traits<Graph> Traits;
    typedef typename Traits::vertex_descriptor Vertex;
    typedef typename Traits::edge_descriptor Edge;
    typedef typename Traits::edges_size_type EdgesSizeType;

    // initialize messages
    visitor.init_messages(graph);

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
            boost::tie(ein, ein_end) =
                boost::in_edges(*vi, graph);
            for(; ein != ein_end; ++ein) {
                if(visitor.is_initialized(*ein, graph)) {
                    known_in_edges.push_back(*ein);
                } else {
                    unknown_in_edges.push_back(*ein);
                }
            }
            typename Traits::out_edge_iterator eout, eout_end;
            boost::tie(eout, eout_end) =
                boost::out_edges(*vi, graph);
            for(; eout != eout_end; ++eout) {
                if(!visitor.is_initialized(*eout, graph))
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
                        graph);
                    n_unknown_messages--;
                }
            } else if(unknown_in_edges.size() == 1) {
                Vertex target =
                    boost::source(unknown_in_edges[0], graph);
                EdgeIterator it = unknown_out_edges.begin();
                EdgeIterator it_end = unknown_out_edges.end();
                for(; it != it_end; ++it) {
                    if(target == boost::target(*it, graph)) {
                        visitor.make_message(
                            *it,
                            known_in_edges.begin(),
                            known_in_edges.end(),
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
        visitor.make_belief(*vi, ein, ein_end, graph);
    }
}

/*
template<typename Message,
         typename Graph,
         typename OutputIterator,
         typename PropertyMap,
         typename Visitor>
void sum_product(const Graph& graph,
                 OutputIterator belief,
                 PropertyMap& message_map,
                 Visitor visitor) {
    typedef boost::graph_traits<Graph> Traits;
    typedef typename Traits::vertex_descriptor Vertex;
    typedef typename Traits::edge_descriptor Edge;

    typedef typename boost::property_map<
                Graph,
                boost::edge_index_t>::const_type EdgeIndexMap;
    typedef boost::optional<Message> OMessage;
    typedef std::vector<OMessage> MessageVector;
    typedef boost::iterator_property_map<
                typename MessageVector::iterator,
                EdgeIndexMap> MessageMap;

    // make the exterior property
    EdgeIndexMap edge_map = get(boost::edge_index, graph);
    MessageVector messages(boost::num_edges(graph), OMessage());
    MessageMap message_map(messages.begin(), edge_map);

    // make messages
    size_t n_unknown_messages = boost::num_edges(graph);
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
            boost::tie(ein, ein_end) =
                boost::in_edges(*vi, graph);
            for(; ein != ein_end; ++ein) {
                if(!message_map[*ein]) {
                    unknown_in_edges.push_back(*ein);
                } else {
                    known_in_edges.push_back(*ein);
                }
            }

            typename Traits::out_edge_iterator eout, eout_end;
            boost::tie(eout, eout_end) =
                boost::out_edges(*vi, graph);
            for(; eout != eout_end; ++eout) {
                if(!message_map[*eout])
                    unknown_out_edges.push_back(*eout);
            }

            if(unknown_in_edges.size() == 0) {
                EdgeIterator it = unknown_out_edges.begin();
                EdgeIterator it_end = unknown_out_edges.end();
                for(; it != it_end; ++it) {
                    visitor.make_message(
                        message_map[*it],
                        *it,
                        known_in_edges.begin(),
                        known_in_edges.end(),
                        graph);
                    n_unknown_messages--;
                }
            } else if(unknown_in_edges.size() == 1) {
                const Vertex& target =
                    boost::source(unknown_in_edges[0], graph);
                EdgeIterator it = unknown_out_edges.begin();
                EdgeIterator it_end = unknown_out_edges.end();
                EdgeIterator message_it = it_end;
                for(; it != it_end; ++it) {
                    const Vertex& target2 =
                        boost::target(*it, graph);
                    if(target == target2) {
                        message_it = it;
                        break;
                    }
                }
                if(message_it != it_end) {
                    visitor.make_message(
                        message_map[*message_it],
                        *message_it,
                        known_in_edges.begin(),
                        known_in_edges.end(),
                        graph);
                    n_unknown_messages--;
                }
            }
        }
    }
    // make belief
    typename Traits::vertex_iterator vi, vi_end;
    boost::tie(vi, vi_end) = boost::vertices(graph);
    for(; vi != vi_end; ++vi) {
        known_in_edges.clear();
        typename Traits::in_edge_iterator ein, ein_end;
        boost::tie(ein, ein_end) =
            boost::in_edges(*vi, graph);
        for(; ein != ein_end; ++ein) {
            known_in_edges.push_back(*ein);
        }
    }
}
*/

} // namespace bp
