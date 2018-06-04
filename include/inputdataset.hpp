
#ifndef INPUTDATASET_HPP
#define INPUTDATASET_HPP


#include <ctime>
#include <string>
#include "../libraries/singularity/include/singularity.hpp"
#include "base.hpp"

class input_data_set
{
public:
//     input_data_set(
//         time_t test_begin, 
//         time_t test_end, 
//         double poi_weight, 
//         poi::parameters_t parameters, 
//         poi::EmissionParameters emission_parameters_t):
//         test_begin(test_begin),
//         test_end(test_end),
//         poi_weight(poi_weight),
//         parameters(parameters),
//         emission_parameters_t(emission_parameters_t) {}
    
    void save(std::string dir);
    void save_transactions(std::vector<singularity::transaction_t> transactions, std::string file_name);   
    void save_initial_balance(std::string file_name);
    
    void load(std::string dir);
    std::vector<singularity::transaction_t> load_transactions(std::string file_name);
    std::vector<time_t> load_times(std::string file_name);
    void load_initial_balances(std::string file_name);
    double calculate_supply();

    time_t test_begin;
    time_t test_end;
    
    balances_map_t initial_balances;

    time_t last_gravity_index_processing_time;
    uint32_t gravity_index_period = 86400;
    uint32_t total_handled_blocks_count = 0;
    double activity_weight = 0.1;
    
    singularity::parameters_t parameters;
    singularity::emission_parameters_t emission_parameters;
    
    time_t last_emission_processing_time;
    uint32_t emission_period = 3600;
    uint32_t block_duration = 1;
//     double token_usd_rate = 1;
//     double account_amount_threshold = 10000;
//     double transaction_amount_threshold = 1000;
    
    std::vector<transaction_set> transaction_sets;
    
    std::vector<time_t> emission_times;
    std::vector<time_t> activity_times;
    std::vector<time_t> block_times;
    
};

#endif /* INPUTDATASET_HPP */
