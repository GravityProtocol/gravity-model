
#include <iostream>
#include "../include/generator.hpp"
#include "../libraries/singularity/include/singularity.hpp"

using namespace std;
using namespace singularity;

int main(int argc, char** argv)
{
    generator_multi_rank main_generator;
    
    generator_attack_1 attack_generator;
    
    transaction_set main_set = main_generator.generate();
    transaction_set attack_set = attack_generator.generate();

    parameters_t p;
    activity_index_calculator c(p);
    
    for ( auto block: main_set.blocks ) {
        c.add_block(block);
    }
    for ( auto block: attack_set.blocks ) {
        c.add_block(block);
    }
    
    auto r = c.calculate();
    
}
