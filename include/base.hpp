
#ifndef BASE_HPP
#define BASE_HPP
#include <string>
#include "../libraries/singularity/include/singularity.hpp"

typedef std::map<std::string, double> balances_map_t;

class account_info
{
public:
    std::string name;
    double balance;
    account_info(std::string name, double balance): name(name), balance(balance) {};
};

class transaction_set
{
public:
    std::string name;
    std::vector<std::vector<singularity::transaction_t>> blocks;
};

std::string create_random_gravity_account_name();

std::string format_time(const time_t t);
time_t parse_time(const std::string t);

balances_map_t generate_initial_balances(uint32_t num, double balance);

#endif
