
#include <iostream>
#include "../libraries/singularity/include/singularity.hpp"
#include "../include/inputdataset.hpp"
#include "../include/result_data_set.hpp"
#include "../include/input_data_set_handler.hpp"

using namespace std;
using namespace singularity;

int main(int argc, char** argv)
{
    if (argc != 3) {
        cout << "Usage: input_data_set_handle input_dir output_dir" << endl;
        
        return 1;
    }
    string input_dir(argv[1]);
    string output_dir(argv[2]);
    
    input_data_set ids;
    
    ids.load(input_dir);
    
    input_data_set_handler idsh;
    
    idsh.ids = ids;
    
    idsh.run();
    
    result_data_set rds = idsh.rds;
    
    rds.save(output_dir);
    
}
