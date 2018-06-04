#include "include/inputdataset.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include "include/base.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include <optional>

using namespace std;
using namespace singularity;
namespace pt = boost::property_tree;

void input_data_set::save(string dir)
{
    ofstream file(dir + "input_package.json");
    
    pt::ptree input_package;

    input_package.put("test_begin", format_time(test_begin));
    input_package.put("test_end", format_time(test_end));

    pt::ptree transaction_set_nodes;
    for (vector<transaction_set>::size_type i = 0; i < transaction_sets.size(); i++) {
        pt::ptree transaction_set_node;
        pt::ptree csv_file_nodes;
        transaction_set ts = transaction_sets[i];
        for (uint32_t j = 0; j<ts.blocks.size(); j++) {
            pt::ptree csv_file_node;
            string transactions_file_name = "transactions-" + to_string(i) + "-" + to_string(j) + ".csv";
            csv_file_node.put("", transactions_file_name);
            csv_file_nodes.push_back(make_pair("", csv_file_node));
            save_transactions(ts.blocks[j], dir + transactions_file_name);
        }
        transaction_set_node.put("name", ts.name);
        transaction_set_node.add_child("csv_files", csv_file_nodes);
        transaction_set_nodes.push_back(make_pair("", transaction_set_node));
    }
    input_package.add_child("transaction_sets", transaction_set_nodes);
    
    string balances_file_name = "balances.csv";
    
    pt::ptree inital_balance_csv_nodes;
    pt::ptree inital_balance_csv_node;
    inital_balance_csv_node.put("", balances_file_name);
    inital_balance_csv_nodes.push_back(make_pair("", inital_balance_csv_node));
    input_package.add_child("initial_balance.csv_files", inital_balance_csv_nodes);
    
    save_initial_balance(dir + balances_file_name);
    
    input_package.put("gravity_index.last_gravity_index_processing_time", format_time(last_gravity_index_processing_time));
    input_package.put("gravity_index.gravity_index_period", gravity_index_period);
    input_package.put("gravity_index.total_handled_blocks_count", total_handled_blocks_count);
    input_package.put("gravity_index.activity_weight", activity_weight);
    input_package.put("gravity_index.parameters.outlink_weight", parameters.outlink_weight);
    input_package.put("gravity_index.parameters.interlevel_weight", parameters.interlevel_weight);
    
    input_package.put("emission.last_emission_processing_time", format_time(last_emission_processing_time));
    input_package.put("emission.emission_period", emission_period);

    input_package.put("emission.parameters.emission_scale", emission_parameters.emission_scale); 
    input_package.put("emission.parameters.initial_supply", emission_parameters.initial_supply); 
    input_package.put("emission.parameters.year_emission_limit", emission_parameters.year_emission_limit); 
    input_package.put("emission.parameters.delay_koefficient", emission_parameters.delay_koefficient); 
    input_package.put("emission.parameters.emission_event_count_per_year", emission_parameters.emission_event_count_per_year); 
    
    
    pt::write_json(file, input_package);
    
    file.close();
    
}

void input_data_set::save_transactions(vector<transaction_t> transactions, string file_name)
{
    ofstream file(file_name);
    
    for (vector<transaction_t>::size_type i=0; i<transactions.size(); i++) {
        transaction_t t = transactions[i];
        file << format_time(t.timestamp) << ";" << t.source_account << ";" << t.target_account << ";" << t.amount << ";" << t.comission << std::endl;
    }
    
    file.close();
}

void input_data_set::save_initial_balance(string file_name)
{
    ofstream file(file_name);
    
    for (balances_map_t::const_iterator it = initial_balances.cbegin(); it != initial_balances.cend(); it++) {
        file << it->first << ";" << it->second << std::endl;
    }
    
    file.close();
}

void input_data_set::load(string dir)
{
    ifstream file(dir + "input_package.json");

    pt::ptree input_package;
    
    pt::read_json(file, input_package);
    
    test_begin = parse_time(input_package.get<string>("test_begin"));
    test_end = parse_time(input_package.get<string>("test_end"));
    
    block_duration = input_package.get<int>("block_duration");

    for( auto init_balance_csv:input_package.get_child("initial_balance.csv_files")) {
        string initial_balances_file_name = init_balance_csv.second.data();
        load_initial_balances(dir + initial_balances_file_name);
    }
    
    last_gravity_index_processing_time = parse_time(input_package.get<string>("gravity_index.last_gravity_index_processing_time"));
    gravity_index_period = input_package.get<int>("gravity_index.gravity_index_period");
    total_handled_blocks_count = input_package.get<int>("gravity_index.total_handled_blocks_count");
    activity_weight = input_package.get<double>("gravity_index.activity_weight");
    parameters.outlink_weight = input_package.get<double>("gravity_index.parameters.outlink_weight");
    parameters.interlevel_weight = input_package.get<double>("gravity_index.parameters.interlevel_weight");
    parameters.account_amount_threshold = input_package.get<int>("gravity_index.parameters.account_amount_threshold");
    parameters.transaction_amount_threshold = input_package.get<int>("gravity_index.parameters.transaction_amount_threshold");
    parameters.token_usd_rate = input_package.get<double>("gravity_index.parameters.token_usd_rate");
    
    last_emission_processing_time =  parse_time(input_package.get<string>("emission.last_emission_processing_time"));
    emission_period = input_package.get<int>("emission.emission_period");
    emission_parameters.emission_scale = input_package.get<int>("emission.parameters.emission_scale"); 
    
    boost::optional<uint64_t> initial_supply_optional = input_package.get_optional<uint64_t>("emission.parameters.initial_supply"); 
    if (initial_supply_optional) {
        cout << "initial_supply parameter is found" << endl;
        emission_parameters.initial_supply = initial_supply_optional.get();
    } else {
        emission_parameters.initial_supply = calculate_supply();
        cout << "initial_supply parameter is not found, caclulated value is " << emission_parameters.initial_supply << endl;
    }
    
    emission_parameters.year_emission_limit = input_package.get<int>("emission.parameters.year_emission_limit"); 
    emission_parameters.delay_koefficient = input_package.get<double>("emission.parameters.delay_koefficient"); 
    
    boost::optional<int> emission_event_count_per_year_optinal = input_package.get_optional<int>("emission.parameters.emission_event_count_per_year");
    
    if (emission_event_count_per_year_optinal) {
        cout << "emission_event_count_per_year parameter is found" << endl;
        emission_parameters.emission_event_count_per_year = emission_event_count_per_year_optinal.get();
    } else {
        emission_parameters.emission_event_count_per_year = (3600 * 24 * 365) / (emission_period * block_duration);
        cout << "emission_event_count_per_year parameter is not found, caclulated value is " << emission_parameters.emission_event_count_per_year << endl;
    }
    
    for (auto ts_data: input_package.get_child("transaction_sets")) {
        transaction_set ts;
        ts.name = ts_data.second.get<string>("name");
        for (auto csv_file: ts_data.second.get_child("csv_files")) {
            string csv_file_name = csv_file.second.data();
            ts.blocks.push_back(load_transactions(dir + csv_file_name));
        }
        transaction_sets.push_back(ts);
    }
    
    boost::optional<string> activity_times_optional = input_package.get_optional<string>("activity_times");
    if (activity_times_optional) {
        cout << "activity_times data found" << endl;
        string activity_times_file_name = activity_times_optional.get();
        activity_times = load_times(dir + activity_times_file_name);
    } else {
        cout << "activity_times data not found, using default" << endl;
    }

    boost::optional<string> block_times_optional = input_package.get_optional<string>("block_times");
    if (block_times_optional) {
        cout << "block_times data found" << endl;
        string block_times_file_name = block_times_optional.get();
        block_times = load_times(dir + block_times_file_name);
    } else {
        cout << "block_times data not found, using default" << endl;
    }

    boost::optional<string> emission_times_optional = input_package.get_optional<string>("emission_times");
    if (emission_times_optional) {
        cout << "emission_time data found" << endl;
        string emission_times_file_name = emission_times_optional.get();
        emission_times = load_times(dir + emission_times_file_name);
    } else {
        cout << "emission_time data not found, using default" << endl;
    }
}

vector<singularity::transaction_t> input_data_set::load_transactions(string file_name)
{
    vector<singularity::transaction_t> transactions;
    
    ifstream file(file_name);
    while (file.good()) {
        string line;
    
        getline(file, line);
        
        vector<string> parsed_line;
        
        istringstream sline(line);
        
        while (sline.good()) {
            string value;
            getline(sline, value, ';');
            parsed_line.push_back(value);
        }
        
        if (parsed_line.size() == 5) {
            transaction_t t(stod(parsed_line[3]), stod(parsed_line[4]), parsed_line[1], parsed_line[2], parse_time(parsed_line[0]), 0, 0);
            transactions.push_back(t);
            
//              cout << "Time: " << parsed_line[0] <<  " - " << t.timestamp <<  " - " << format_time(t.timestamp) << endl;
        }
    }
    
    return transactions;
}

std::vector<time_t> input_data_set::load_times(std::string file_name)
{
    vector<time_t> result;
    ifstream file(file_name);
    while (file.good()) {
        string line;
    
        getline(file, line);
        
        result.push_back(parse_time(line));
    }
    
    return result;
    
}

void input_data_set::load_initial_balances(std::string file_name)
{
    ifstream file(file_name);
    
    while (file.good()) {
        string line;
        
        getline(file, line);

        vector<string> parsed_line;
        
        istringstream sline(line);
        
        while (sline.good()) {
            string value;
            getline(sline, value, ';');
            parsed_line.push_back(value);
        }
        
        if (parsed_line.size() == 2) {
            initial_balances[parsed_line[0]] = stod(parsed_line[1]);
            
//             cout << parsed_line[0] << ">>" << initial_balances[parsed_line[0]] << " : " << parsed_line[1] <<  endl;
        }
    }
}

double input_data_set::calculate_supply()
{
    double result = 0;
    for (auto account: initial_balances) {
        result += account.second;
    }
    
    return result;
}




