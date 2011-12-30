#pragma once

#include <utility>
#include <iterator>

#include <boost/optional.hpp>
#include <boost/graph/graph_traits.hpp>

namespace detail {

template<typename Edge,
         typename Message,
         typename Vertex,
         typename Graph>
inline typename Edge::value_type::first_type&
select_in_message(const Edge& edge,
                  Message& message,
                  const Vertex& target,
                  const Graph& graph) {
    if(boost::get(boost::vertex_index, boost::target(edge, graph), graph) ==
       boost::get(boost::vertex_index, target, graph)) {
        return message[edge].first;
    } else {
        return message[edge].second;
    }
}


template<typename Message>
std::pair<boost::optional<Message>, boost::optional<Message> >
make_empty_message() {
    typedef boost::optional<Message> OMessage;
    typedef std::pair<OMessage, OMessage> EdgeMessage;
    return EdgeMessage(OMessage(), OMessage());
}

} // namespace detail

template<typename Graph,
         typename Message,
         typename OutputIterator>
void sum_product(const Graph& graph,
                 Message init,
                 OutputIterator belief) {
    typedef typename boost::graph_traits<Graph> Traits;
    typedef typename Traits::vertex_descriptor Vertex;
    typedef typename Traits::edge_descriptor Edge;

    typedef typename boost::property_map<
                        Graph,
                        boost::edge_index_t>::const_type EdgeIndexMap;
    typedef boost::optional<Message> OMessage;
    typedef std::pair<OMessage, OMessage> EdgeMessage;
    typedef std::vector<EdgeMessage> MessageVector;
    typedef boost::iterator_property_map<
                typename MessageVector::const_iterator,
                EdgeIndexMap> MessageMap;

    // make the exterior property
    EdgeIndexMap edge_map = get(boost::edge_index, graph);
    MessageVector messages(boost::num_edges(graph));
    std::fill(
        messages.begin(),
        messages.end(),
        detail::make_empty_message<Message>());
    MessageMap message_map(messages.begin(), edge_map);

    // print
    typename Traits::edge_iterator ei, eiend;
    boost::tie(ei, eiend) = boost::edges(graph);
    for(; ei != eiend; ++ei) {
        const Vertex& in = boost::source(*ei, graph);
        const Vertex& out = boost::target(*ei, graph);
        std::cout << boost::get(boost::vertex_index, graph, in)
                  << " -> "
                  << boost::get(boost::vertex_index, graph, out)
                  << std::endl;
    }
}
