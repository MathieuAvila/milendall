#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/program_options.hpp>
using namespace boost::program_options;

using namespace nlohmann;
using namespace std;

int main(int argc, char* argv[])
{
    variables_map vm;

    options_description desc("Allowed options");
        desc.add_options()
        // First parameter describes option name/short name
        // The second is parameter to option
        // The third is description
        ("help,h", "print usage message")
        ("recreate,r", "force creation of file if exist, otherwise create or append")
        ("inputbin,i",  value< vector<string> >(), "pathname for input json")
        ("outputbin,o", value< vector<string> >(), "pathname for output bin")
        ("outputjson,j", value< vector<string> >(), "pathname for output json")
        ;
    try {    
        store(parse_command_line(argc, argv, desc), vm);
    }
    catch(exception& e) {
        cerr << e.what() << "\n";
    }
    if (vm.count("help")) {  
        cout << desc << "\n";
        return 0;
    }
    string input_filename = vm["inputbin"].as< vector<string> >()[0];
    string output_filename_bin = vm["outputbin"].as< vector<string> >()[0];
    string output_filename_json = vm["outputjson"].as< vector<string> >()[0];
    cout << input_filename << "\n";
    
    try {
        std::ifstream i(input_filename);
        json j;
        i >> j;
        cout << j << std::endl;

        std::ofstream o_bin(output_filename_bin);
        std::ofstream o_json(output_filename_json);

        json j_out = 
            {
               { 
                    "bufferViews" ,
                    {
                        {"buffer", 0},
                        {"byteOffset", 0},
                        {"byteLength", 12}
                    }
               },
               {
                    "accessors",
                    {
                       {"bufferView", 0},
                       {"byteOffset", 0},
                       {"componentType", 5123},
                       {"count", 6},
                       {"type", "SCALAR"},
                       {"max", { 3 }},
                       {"min", { 0 }},
                    }
               }
            };
        o_json << std::setw(4) << j_out << std::endl;

    }
    catch(exception& e) {
        cerr << e.what() << "\n";
    }
    return 0;
}