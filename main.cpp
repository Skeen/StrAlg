#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>

//#define DEBUG_PRINT

#include "SuffixTree.hpp"

void print_usage()
{
    std::cout << "Usage: search.exe INPUT_FILE SEARCH_STRING" << std::endl;
}

std::string read_file(const std::string& filename)
{
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
    return str;
}

int main(int argv, char* argc[])
{
    // Check the enough arguments have been supplied
    if (argv < 3)
    {
        print_usage();
        return -1;
    }
    // Convert input, to strings
    std::string input_file(argc[1]);
    std::string search_string(argc[2]);

#ifdef DEBUG_PRINT
    // Output debug information
    std::cout << "INPUT FILE:" << std::endl;
    std::cout << input_file << std::endl;
    
    std::cout << "SEARCH STRING:" << std::endl;
    std::cout << search_string << std::endl;
#endif //DEBUG_PRINT

    // Read the entire file into a string
    std::string file_contents = read_file(input_file);

#ifdef DEBUG_PRINT
    // Output debug information
    std::cout << "FILE CONTENTS:" << std::endl;
    std::cout << file_contents << std::endl;
#endif //DEBUG_PRINT

    // Build the suffix tree
    SuffixTree stree = SuffixTree::naiveConstruct(file_contents);
    std::cout << "Tree constructed" << std::endl;

    /*
    stree.to_dot("a.dot");
    */
    /*
    bool is_substring = stree.isSubstring(search_string);
    std::cout << "is_substring: " << is_substring << std::endl;
    */
    /*
    bool is_suffix = stree.isSuffix(search_string);
    std::cout << "is_suffix: " << is_suffix << std::endl;
    */
    // Do the search
    std::vector<std::string> matches = stree.findOccurences(search_string);
    std::cout << "occurs: ";
    for(const auto str : matches)
    {
        // Add one, as our indexes are 0 indexed
        std::cout << str << " ";
    }
    std::cout << std::endl;
}
