
#include "include/input_data_set_handler.hpp"

using namespace singularity;
using namespace std;

map<time_t, bool> input_data_set_handler::get_time_map(const vector<time_t>& times)
{
    map<time_t, bool> result;
    for (auto t:times) {
        auto item = result.find(t);
        if (item == result.cend()) {
            result.insert(map<time_t, bool>::value_type(t, true));
        }
    }
    
    return result;
}


void input_data_set_handler::distribute_emission()
{
    last_emission_for_accounts.clear();
    for (auto balance: balances) {
        auto activity_index_for_account = current_activity_index_map[balance.first];
        gravity_index_calculator gc(ids.activity_weight, current_supply);
        double gravity_index_for_account = gc.calculate_index(balance.second, activity_index_for_account);
        
//         cout << " CS " <<  current_supply << " B " <<  balance.second << " GI " << gravity_index_for_account << endl;
        current_gravity_index_map[balance.first] = gravity_index_for_account;
        double emission_distributed = last_emission * gravity_index_for_account;
        last_emission_for_accounts[balance.first] = emission_distributed;
        balance.second += emission_distributed;
        emission_parts[balance.first] += emission_distributed;
    }
    
    current_supply += last_emission;
}

map<std::string, emission_row> input_data_set_handler::prepare_emission_data()
{
    
    map<std::string, emission_row> result;
    for (auto balance: balances) {
        emission_row row;
        row.balance = balance.second;
        row.balance_share = balance.second;
        row.activity = current_activity_index_map[balance.first];
        row.importance = current_gravity_index_map[balance.first];
        row.emission = last_emission_for_accounts[balance.first];
        row.total_emission = emission_parts[balance.first];
        row.comission = commission_parts[balance.first];
        result[balance.first] = row;
    }
    
    return result;
}


void input_data_set_handler::time_action(time_t t)
{
    auto block_time = block_time_map.find(t);
    if (block_time != block_time_map.cend()) {
        p_c->add_block(current_block);
        p_ap->add_block(current_block);
        current_block.clear();
    }
    auto activity_time = activity_time_map.find(t);
    if (activity_time != activity_time_map.cend()) {
        cout << "Activity calculation " << format_time(t) << endl;
        current_activity_index_map = p_c->calculate();
        activity_info a;
        a.time = t;
        a.activity_data = current_activity_index_map;
        rds.activities.push_back(a);
    }
    auto emission_time = emission_time_map.find(t);
    if (emission_time != emission_time_map.cend()) {
        std::cout << "Emission calculation " << format_time(emission_time->first) << std::endl;
        uint32_t last_activity = p_ec->get_emission_state().last_activity;
        uint32_t current_activity = p_ap->get_activity();
        last_emission = p_ec->calculate(total_emission, *p_ap);
        p_ap->clear();
        distribute_emission();
        emission_info ei;
        ei.emission_data = prepare_emission_data();
        ei.time = t;
        rds.emissions.push_back(ei);
        if (current_activity > last_peak_activity) {
            last_peak_activity = current_activity;
        }
        total_emission += last_emission;
        
        double current_supply = ids.emission_parameters.initial_supply + total_emission;
        double supply_growth = total_emission / ids.emission_parameters.initial_supply * 100;
        
        global_emission_info gei;
        
        gei.time = t;
        gei.current_supply = current_supply;
        gei.last_peak_activity = last_activity;
        gei.current_activity = current_activity;
        gei.last_emission = last_emission;
        gei.current_total_emission = total_emission;
        gei.current_total_supply_growth = supply_growth;
        
        rds.global_emission_data.push_back(gei);
        
        cout << "Last emission=" << last_emission 
        << "; total_emission=" << total_emission 
        << "; current_supply=" << current_supply
        << "; supply_growth=" << supply_growth << "%"
        << endl;
    }
    
}


void input_data_set_handler::run()
{
    p_ec = make_shared<emission_calculator>(ids.emission_parameters, emission_state);
    
    p_c = make_shared<activity_index_calculator>(ids.parameters);
    
    p_ap = make_shared<activity_period>();
    

    if (ids.block_times.size() > 0) {
        block_time_map = get_time_map(ids.block_times);
    } else {
        block_time_map = generate_time_map(ids.test_begin, ids.test_end, ids.block_duration);
    }
    
    if (ids.activity_times.size() > 0) {
        activity_time_map = get_time_map(ids.activity_times);
    } else {
        activity_time_map = generate_time_map(ids.test_begin, ids.test_end, ids.block_duration * ids.gravity_index_period);
    }
    
    if (ids.emission_times.size() > 0) {
        emission_time_map = get_time_map(ids.emission_times);
    } else {
        emission_time_map = generate_time_map(ids.test_begin, ids.test_end, ids.block_duration * ids.emission_period);
    }
    
    current_time = ids.test_begin;
    
    balances = ids.initial_balances;
    
    current_supply = sum(balances);
    
    for (auto ts: ids.transaction_sets) {
        for (auto block: ts.blocks ) {
            for(transaction_t t: block) {
                while (current_time < t.timestamp) {
                    time_action(current_time);
                    current_time++;
                }
                t.source_account_balance = balances[t.source_account];
                t.target_account_balance = balances[t.target_account];
//                 cout << "T: " << t.timestamp << " = " <<  format_time(t.timestamp) << endl;
                current_block.push_back(t);
                adjust_balances(t);
            }
        }
    }
    
    while (current_time < ids.test_end) {
        time_action(current_time);
        current_time++;
    }
}

double input_data_set_handler::sum(balances_map_t v_balances)
{
    double sum = 0;
    
    for (auto balance: v_balances) {
        sum += balance.second;
    }
    
    return sum;
}

void input_data_set_handler::adjust_balances(singularity::transaction_t transaction)
{
    if (balances[transaction.source_account] < transaction.amount + transaction.comission) {
         cout << "Warning: unsufficient balance: " << transaction.source_account << endl;
    }
    balances[transaction.source_account] -= transaction.amount + transaction.comission;
    balances[transaction.target_account] += transaction.amount;
    
    commission_parts[transaction.source_account] += transaction.comission;
}

map<time_t, bool> input_data_set_handler::generate_time_map(time_t begin, time_t end, time_t interval)
{
    map<time_t, bool> result;
    
    for (time_t current = begin; current < end; current += interval) {
        result[current] = true;
    }
    
    return result;
}

