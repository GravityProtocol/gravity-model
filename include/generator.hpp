
#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "../libraries/singularity/include/activity_index_calculator.hpp"
#include "base.hpp"

class generator_abstract
{
public:
    virtual transaction_set generate() = 0;
    time_t test_begin;
    time_t test_end;
    balances_map_t initial_balances;
    uint32_t num_accounts = 100;
    double max_balance = 1000000;
    double max_amount = 10000;
    double min_amount = 1;
    double fee = 20;
    
protected:
    std::string create_random_account_name(std::string prefix, uint32_t count);
    double create_random_amount(double min, double max);
};

class generator_single_rank: public generator_abstract
{
public:
     transaction_set generate();
};

class generator_multi_rank: public generator_abstract
{
public:
     transaction_set generate(); 
};

class generator_attack_1: public generator_abstract
{
public:
    double link_amount; 
    double total_amount; 
    uint32_t node_count;    
    transaction_set generate(); 
};

class generator_attack_2: public generator_abstract
{
public:
    double link_amount; 
    double total_amount; 
    uint32_t node_count;   
    std::string spamming_node;
    transaction_set generate(); 
};

#endif
