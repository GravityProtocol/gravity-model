
#include "include/result_data_set.hpp"
#include <fstream>
#include "include/base.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace singularity;
namespace pt = boost::property_tree;

void result_data_set::save(string dir)
{
    ofstream file(dir + "result_pakage.json");
    
    pt::ptree result_package;

     pt::ptree activity_nodes;
     for (auto activity: activities) {
         pt::ptree activity_node;
         string timestamp = format_time(activity.time);
         string file_name = "activity_" + timestamp + ".csv";
        activity_node.put("timestamp", timestamp);
        activity_node.put("csv",file_name);
        activity_nodes.push_back(make_pair("", activity_node));
        save_activity(activity.activity_data, dir + file_name);
     }
     result_package.add_child("activities", activity_nodes);


    pt::ptree emission_nodes;
    for (auto emission: emissions) {
        pt::ptree emission_node;
        string timestamp = format_time(emission.time);
        string file_name = "emission_" + timestamp + ".csv";
        emission_node.put("timestamp", timestamp);
        emission_node.put("csv",file_name);
        emission_nodes.push_back(make_pair("", emission_node));
        save_emission(emission.emission_data, dir + file_name);
    }
    result_package.add_child("emissions", emission_nodes);
    
    string global_emission_file_name = "global_emission.csv";
    
    save_global_emission( dir + global_emission_file_name );
    result_package.put("global_emission.csv", global_emission_file_name);
    
     pt::write_json(file, result_package);
}

void result_data_set::save_activity(account_activity_index_map_t activity_data, string file_name)
{
    ofstream file(file_name);
    
    for (auto row: activity_data) {
        file << row.first << ";" << row.second << endl;
    }
}

void result_data_set::save_emission(map<std::string, emission_row> emission_data, std::string file_name)
{
    ofstream file(file_name);
    
    for (auto row: emission_data) {
        file << row.first << 
        ";" << row.second.balance << 
        ";" << row.second.balance_share << 
        ";" << row.second.activity << 
        ";" << row.second.importance << 
        ";" << row.second.emission << 
        ";" << row.second.total_emission << 
        ";" << row.second.comission << 
        endl;
    }
}

void result_data_set::save_global_emission(std::string file_name)
{
    ofstream file(file_name);
        file << "time" << 
        ";" << "last_emission" << 
        ";" << "current_total_emission" << 
        ";" << "current_supply" << 
        ";" << "current_activity" << 
        ";" << "last_peak_activity" << 
        ";" << "current_total_supply_growth" << 
        endl;

    for (auto row: global_emission_data) {
        file << format_time(row.time) << 
        ";" << row.last_emission << 
        ";" << row.current_total_emission << 
        ";" << row.current_supply << 
        ";" << row.current_activity << 
        ";" << row.last_peak_activity << 
        ";" << row.current_total_supply_growth << 
        endl;
    }
   
}


