
#ifndef INPUT_DATA_SET_HANDLER_HPP
#define INPUT_DATA_SET_HANDLER_HPP

#include <vector>
#include <map>
#include "inputdataset.hpp"
#include "result_data_set.hpp"
#include "../libraries/singularity/include/singularity.hpp"

class input_data_set_handler
{
public:
    input_data_set ids;
    
    result_data_set rds;
    
    std::shared_ptr<singularity::emission_calculator> p_ec;
    
    std::shared_ptr<singularity::activity_index_calculator> p_c;
    
    std::vector<singularity::transaction_t> current_block;
    
    uint32_t last_peak_activity = 0;
    double total_emission = 0;
    double last_emission = 0;
    
    std::shared_ptr<singularity::activity_period> p_ap;
    
    balances_map_t balances;
    balances_map_t emission_parts;
    balances_map_t commission_parts;
    
    balances_map_t total_emission_for_accounts;
    balances_map_t last_emission_for_accounts;
    
    double current_supply = 0;
    
    singularity::emission_state_t emission_state;
    
    time_t current_time;
    
    std::map<time_t, bool> block_time_map;
    std::map<time_t, bool> activity_time_map;
    std::map<time_t, bool> emission_time_map;
    
    singularity::account_activity_index_map_t current_activity_index_map;
    singularity::account_activity_index_map_t current_gravity_index_map;
    
    void run();
    double sum(balances_map_t v_balances);
    void adjust_balances(singularity::transaction_t transaction);
    std::map<time_t, bool> get_time_map(const std::vector<time_t>& times);
    std::map<std::string, emission_row> prepare_emission_data();
    void distribute_emission();
    void time_action(time_t t);
    std::map<time_t, bool> generate_time_map(time_t begin, time_t end, time_t interval);
};

#endif /* INPUT_DATA_SET_HANDLER_HPP */

