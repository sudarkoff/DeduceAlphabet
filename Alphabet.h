#ifndef _ALPHABET_H_5EF10239_98DE_41F1_AAE0_B6135EFD0439_DEFINED_
#define _ALPHABET_H_5EF10239_98DE_41F1_AAE0_B6135EFD0439_DEFINED_

#include <iostream>

#ifdef _USE_BGL_
#include <boost/graph/graph_traits.hpp>
#else
#include "Graph.h"
#endif

namespace amz {

class CaseSensitive {
public:
    char operator() (char character) { return character; }
};

class CaseInsensitive {
public:
    char operator() (char character) { return toupper(character); }
};

template <class CasePolicy>
class Alphabet {
    public:
        Alphabet() {}
            virtual ~Alphabet() {}

        void deduce(std::istream& is)
        {
            // recursively collect and connect all the unique letters
            process_words(is, 0);
#ifdef _DEBUG
            // export graph to dot-file
            graph_.export_dot("Initial.dot");
#endif
            
            // reduce the number of connection
            graph_.eliminate_shortcuts();
            // sort the graph
            graph_.sort();
#ifdef _DEBUG
            // export graph to dot-file
            graph_.export_dot("Final.dot");
#endif
        }
        
        void output(std::ostream& os)
        {
            // output the alphabet
            graph_.output(os);
        }
        
    private:
        /// @todo Refactor to use boost::graph library.
        amz::graph<char> graph_;
        CasePolicy case_policy_;
        
        void process_words(std::istream& is, int column)
        {
            while (!is.eof()) {
                // get the character and add it to the alphabet
                char curr = case_policy_(is.get());
                insert_character(curr);
                
                // read the character immediately under the current one
                std::ios::pos_type pos = is.tellg();
                is.ignore(INT_MAX, '\n');
                std::string word = "";
                std::getline(is, word);
                is.seekg(pos);
                char next = case_policy_(word[column]);
                if (is.eof()) return;
                
                // if the character below is the same, go to the next column
                if ((next != '\0') && (curr == next) && (is.peek() != '\n')) {
                    process_words(is, column + 1);
                }
                // if not the same
                else if (curr != next) {
                    // add the character below
                    insert_character(next);
                    // add the connection
                    graph_.insert_edge(curr, next);
                    
                    // add the rest of the word to the alphabet
                    std::string rest = "";
                    std::getline(is, rest);
                    for (size_t i = 0; i < rest.length(); ++i) {
                        insert_character(case_policy_(rest[i]));
                    }
                }
                
                // skip to the next line
                if (column > 0) return;
                if (is.peek() == '\n') {
                    is.ignore(INT_MAX, '\n');
                }
            }
        }

        void insert_character(char c)
        {
            if (!graph_.node_exists(c)) {
                graph_.insert_node(c);
            }
        }
};

} // namespace amz

#endif // _ALPHABET_H_5EF10239_98DE_41F1_AAE0_B6135EFD0439_DEFINED_
