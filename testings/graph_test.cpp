#include <gtest/gtest.h>

#include <iostream>
#include <boost/optional.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <bp/graph.hpp>

using namespace std;
using namespace boost;

typedef adjacency_list<
            vecS, vecS, bidirectionalS,
            no_property,
            property<edge_index_t, std::size_t> > Graph;
typedef graph_traits<Graph> GraphTraits;
typedef GraphTraits::edge_iterator EdgeIterator;
typedef GraphTraits::edge_descriptor Edge;
typedef property_map<Graph, edge_index_t>::type EdgeIndexMap;

typedef int MessageType;
typedef optional<MessageType> OptionalType;
typedef std::pair<OptionalType, OptionalType> EdgeMessageType;
typedef std::vector<EdgeMessageType> MessageVectorType;
typedef iterator_property_map<
            MessageVectorType::iterator,
            EdgeIndexMap> IterMap;

inline EdgeMessageType make_message() {
    return EdgeMessageType(OptionalType(), OptionalType());
}

inline EdgeMessageType make_message(MessageType v1,
                                    MessageType v2) {
    return EdgeMessageType(OptionalType(v1), OptionalType(v2));
}

TEST(graphTest, construction)
{
    // make graph
    Graph graph;
    for(int i=0; i<5; ++i) {
        add_vertex(graph);
    }
    for(int i=0; i<5-1; ++i) {
        add_edge(i, i+1, i, graph);
    }

    // print edge
    EdgeIterator ei, ei_end;
    for(tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei) {
        std::cout << get(edge_index, graph, *ei) << std::endl;
    }
    // create exterior map
    EdgeIndexMap edge_map = get(edge_index, graph);
    MessageVectorType props;
    props.push_back(make_message(0, 0));
    props.push_back(make_message(1, 1));
    props.push_back(make_message(2, 2));
    props.push_back(make_message(3, 3));
    IterMap message_map(props.begin(), edge_map);

    sum_product(graph, (int)0, props.begin());

    ASSERT_TRUE(1 == 1);
}
