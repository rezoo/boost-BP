#pragma once

#include <utility>
#include <iterator>

#include <boost/optional.hpp>
#include <boost/graph/graph_traits.hpp>

namespace bp {

template<typename Message,
         typename Graph,
         typename OutputIterator,
         typename Visitor>
void sum_product(const Graph& graph,
                 OutputIterator belief,
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

    size_t n_unknown_messages = boost::num_edges(graph);
    typedef std::vector<Edge> EdgeVector;
    typedef typename EdgeVector::iterator EdgeIterator;
    EdgeVector unknown_in_edges;
    EdgeVector known_in_edges;
    EdgeVector unknown_out_edges;
    while(n_unknown_messages > 0) {
        typename Traits::vertex_iterator vi, viend;
        boost::tie(vi, viend) = boost::vertices(graph);
        for(; vi != viend; ++vi) {
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
                        *vi,
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
                        *vi,
                        graph);
                    n_unknown_messages--;
                }
            }
        }
    }
}

} // namespace bp
