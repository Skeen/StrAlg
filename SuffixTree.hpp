#ifndef SUFFIX_TREE_HPP
#define SUFFIX_TREE_HPP

#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <string>

class SuffixTreeNode
{
    public:
        SuffixTreeNode(int offset = 0)
            : offset(offset)
        {
        }

        int offset;
        std::map<char, SuffixTreeNode*> children;
};

class SuffixTree
{
    private:
        SuffixTreeNode* root;
        std::string& input_string;

    private:
        SuffixTree(std::string str)
            : root(new SuffixTreeNode()), input_string(str)
        {
        }

    public:
        static SuffixTree naiveConstruct(std::string str)
        {
            // Push back our special terminator
            str.push_back('$');
            // Create a nulled tree
            SuffixTree stree(str);
            // For each suffix
            for(std::string::iterator it=str.begin(); it!=str.end(); ++it)
            {
                // Build the suffix
                std::string suffix(it, str.end());
#ifdef DEBUG_PRINT
                // Debug print the suffix
                std::cout << suffix << std::endl;
#endif //DEBUG_PRINT
                // Find the current node
                SuffixTreeNode* current = stree.root;
                // Loop through all characters in sequence
                for(char c : suffix)
                {
                    // Check if the map has an outgoing edge, corresponding to what we're adding
                    if(current->children.find(c) == current->children.end())
                    {
                        int offset = std::distance(str.begin(), it);
                        // If not, add one
                        current->children.emplace(c, new SuffixTreeNode(offset));
                    }
                    current = current->children.at(c);
                }
            }
            return stree;
        }

        // Follow the path given by str, return the Node we end in, or nullptr if we fall off
        SuffixTreeNode* followPath(std::string str)
        {
            SuffixTreeNode* current = root;
            for(char c : str)
            {
                // If we cannot follow an edge
                if(current->children.find(c) == current->children.end())
                {
                    return nullptr;
                }
                current = current->children.at(c);
            }
            return current;
        }

        bool isSubstring(std::string str)
        {
            return (followPath(str) != nullptr);
        }

        bool isSuffix(std::string str)
        {
            SuffixTreeNode* current = followPath(str);
            return (current != nullptr) && (current->children.find('$') != current->children.end());
        }

        std::vector<SuffixTreeNode*> findLeafNodes(SuffixTreeNode* parent)
        {
            std::vector<SuffixTreeNode*> leafs;

            for(std::pair<char, SuffixTreeNode*> child : parent->children)
            {
                // This is a leaf
                if(std::get<0>(child) == '$')
                {
                    leafs.push_back(std::get<1>(child));
                }
                else // This is not a leaf
                {
                    std::vector<SuffixTreeNode*> subleaves = findLeafNodes(std::get<1>(child));
                    leafs.insert(leafs.end(), subleaves.begin(), subleaves.end());
                }
            }

            return leafs;
        }

        std::vector<int> findOccurences(std::string str)
        {
            std::vector<int> occurences;

            SuffixTreeNode* current = followPath(str);
            // There are occourences
            if (current != nullptr)
            {
                // We just need to find them
                std::vector<SuffixTreeNode*> leafs = findLeafNodes(current);
                for(SuffixTreeNode* leaf : leafs)
                {
                    occurences.push_back(leaf->offset);
                }
            }
            std::sort(occurences.begin(), occurences.end());
            return occurences;
        }

        void to_dot(std::stringstream& dot, SuffixTreeNode* parent, int& id)
        {
            int parent_id = id;
            for(std::pair<char, SuffixTreeNode*> child : parent->children)
            {
                id++;
                char c = std::get<0>(child);
                if(c == '\n')
                {
                    c = '#';
                }

                dot << "n" << id << "[label=\"" << std::get<1>(child)->offset << "\"];" << std::endl;
                dot << "n" << parent_id << " -> n" << id << " [label=\"" << c << "\"];" << std::endl;

                to_dot(dot, std::get<1>(child), id);
            }
        }

        void to_dot(std::string output_file)
        {
            int id = 0;

            std::stringstream dot;
            dot << "digraph g {" << std::endl;
            dot << "n0 [label=\"ROOT\"];" << std::endl;
            to_dot(dot, root, id);
            dot << "}";

            std::ofstream dot_file;
            dot_file.open(output_file);
            dot_file << dot.str();
            dot_file.close();
        }
};

#endif //SUFFIX_TREE_HPP
