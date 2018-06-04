
#ifndef RESULTDATASET_HPP
#define RESULTDATASET_HPP

#include "../libraries/singularity/include/singularity.hpp"
#include <string>
#include <map>

class activity_info
{
public:
    time_t time;
    singularity::account_activity_index_map_t activity_data;
};

class global_emission_info
{
public:
    time_t time;
    double current_supply;
    double current_activity;
    double last_peak_activity;
    double last_emission;
    double current_total_emission;
    double current_total_supply_growth;
};

class emission_row
{
public:
  double balance;
  double balance_share;
  double activity;
  double importance;
  double emission; 
  double total_emission;
  double comission;
};

class emission_info
{
public:
    time_t time;
    std::map<std::string, emission_row> emission_data;
};

class result_data_set 
{
public:
    std::vector<activity_info> activities;
    std::vector<emission_info> emissions;
    std::vector<global_emission_info> global_emission_data;
    void save(std::string dir);
    void save_activity(singularity::account_activity_index_map_t activity_data, std::string file_name);
    void save_emission(std::map<std::string, emission_row> emission_data, std::string file_name);
    void save_global_emission(std::string file_name);
};

#endif /* RESULTDATASET_HPP */
