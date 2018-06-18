#include <ctime>
#include <iostream>
#include "../libraries/singularity/include/singularity.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace singularity;
namespace pt = boost::property_tree;

time_t parse_time_iso(const std::string t)
{
    struct tm time_info;

    std::istringstream ts(t);
    ts >> std::get_time(&time_info, "%Y-%m-%dT%X");

    return mktime(&time_info);

}

string format_time_iso(const time_t t)
{
    struct tm* p_time_info = localtime(&t);

    char buffer[80];

    strftime(buffer, 80, "%Y-%m-%dT%X", p_time_info);

    return std::string(buffer);
}

double deserialize_double(string string_value)
{
    uint64_t int_value;
    std::istringstream iss(string_value);
    iss >> int_value;
    double double_value = *(double*)&int_value;
    return double_value;
}

int main(int argc, char** argv)
{
    //read the args
    if (argc != 3) {
        cout << "Usage: ./scenario_handler input_dir output_dir" << endl;
        return 1;
    }
    string input_dir(argv[1]);
    string output_dir(argv[2]);

    //prepare the tools
    parameters_t activity_params;
    activity_index_calculator ac(activity_params);

    activity_period ap;
    emission_parameters_t emission_params;
    emission_state_t emission_state;
    emission_calculator ec(emission_params, emission_state);

    vector<singularity::transaction_t> current_block;

    //read the scenario line by line
    int block_counter = 0;
    int transaction_counter = 0;
    ifstream infile(input_dir + "scenario.txt");
    string str_line;
    while (getline(infile, str_line))
    {
        //parse the line
        std::stringstream ss;
        ss << str_line;
        pt::ptree line;
        pt::read_json(ss, line);

        //reading the command
        string command = line.get<string>("command");

        if(command == "transaction")
        {
            transaction_t tran(
                    deserialize_double(line.get<string>("amount")),
                    deserialize_double(line.get<string>("fee")),
                    line.get<string>("from"),
                    line.get<string>("to"),
                    parse_time_iso(line.get<string>("time")),
                    deserialize_double(line.get<string>("from_balance")),
                    deserialize_double(line.get<string>("to_balance"))
                    );

            current_block.push_back(tran);
            cout << "transaction pushed" << endl;
        }

        if(command == "block")
        {
            block_counter++;
            transaction_counter += current_block.size();

            ac.add_block(current_block);
            ap.add_block(current_block);
            current_block.clear();
        }

        if(command == "activity")
        {
            string activity_timestamp = line.get<string>("time");
            string filename = output_dir + "activity_" + activity_timestamp + ".csv";

            auto activity_index = ac.calculate();

            std::ofstream activity_output;
            activity_output.open(filename, ofstream::out | ofstream::trunc);

            activity_output << "account_amount_threshold = " << activity_params.account_amount_threshold << endl;
            activity_output << "transaction_amount_threshold = " << activity_params.transaction_amount_threshold << endl;
            activity_output << "outlink_weight = " << activity_params.outlink_weight << endl;
            activity_output << "interlevel_weight = " << activity_params.interlevel_weight << endl;
            activity_output << "clustering_m = " << activity_params.clustering_m << endl;
            activity_output << "clustering_e = " << activity_params.clustering_e << endl;
            activity_output << "decay_period = " << activity_params.decay_period << endl;
            activity_output << "decay_koefficient = " << activity_params.decay_koefficient << endl;
            activity_output << "num_threads = " << activity_params.num_threads << endl;
            activity_output << "token_usd_rate = " << activity_params.token_usd_rate << endl;

            for( auto it = activity_index.begin(); it != activity_index.end(); it++ )
            {
                activity_output << it->first << ";" << it->second << endl;
            }

            activity_output.close();

            exit(1);
        }

        if(command == "activity_settings")
        {
            string ap_filename = line.get<string>("params_file");
            ifstream file(input_dir + ap_filename);
            pt::ptree ap_params;
            pt::read_json(file, ap_params);

            activity_params.account_amount_threshold = ap_params.get<int>("account_amount_threshold");
            activity_params.transaction_amount_threshold = ap_params.get<int>("transaction_amount_threshold");
            activity_params.outlink_weight = ap_params.get<double>("outlink_weight");
            activity_params.interlevel_weight= ap_params.get<double>("interlevel_weight");
            activity_params.clustering_m = ap_params.get<int>("clustering_m");
            activity_params.clustering_e = ap_params.get<double>("clustering_e");
            activity_params.decay_period = ap_params.get<int>("decay_period");
            activity_params.decay_koefficient = ap_params.get<double>("decay_koefficient");
            activity_params.num_threads = ap_params.get<int>("num_threads");
            activity_params.token_usd_rate = ap_params.get<double>("token_usd_rate");

            ac.set_parameters(activity_params);
        }

        if(command == "emission_settings")
        {
            string em_filename = line.get<string>("params_file");
            ifstream file(input_dir + em_filename);
            pt::ptree em_params;
            pt::read_json(file, em_params);

            emission_params.initial_supply = em_params.get<long>("initial_supply");
            emission_params.year_emission_limit = em_params.get<int>("year_emission_limit");
            emission_params.emission_scale = em_params.get<int>("emission_scale");
            emission_params.emission_event_count_per_year = em_params.get<int>("emission_event_count_per_year");
            emission_params.delay_koefficient = em_params.get<double>("delay_koefficient");

            ec.set_parameters(emission_params);
        }

        cout << " blocks " << block_counter
             << " transactions " << transaction_counter
             << " command " << command << endl;
    }
}
