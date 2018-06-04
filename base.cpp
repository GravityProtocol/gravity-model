#include "include/base.hpp"
#include <ctime>

std::string create_random_gravity_account_name()
{
    char result[20];
    uint16_t n1 = 10000 * drand48();
    uint16_t n2 = 10000 * drand48();
    uint16_t n3 = 10000 * drand48();
    char c1 = 'a' + (uint8_t) (26 * drand48());
    char c2 = 'a' + (uint8_t) (26 * drand48());
    std::sprintf(result, "g%04u%c%04u%c%04u", n1, c1, n2, c2, n3);
    
    return std::string(result);
}

std::string format_time(const time_t t)
{
    struct tm* p_time_info = localtime(&t);
    
    char buffer[80];
    
    strftime(buffer, 80, "%Y-%m-%dT%X", p_time_info);
    
    return std::string(buffer);
}

time_t parse_time(const std::string t)
{
    struct tm time_info;

    std::istringstream ts(t);
    ts >> std::get_time(&time_info, "%Y-%m-%dT%X");
    
    return mktime(&time_info);
    
}

balances_map_t generate_initial_balances(uint32_t num, double balance)
{
    balances_map_t initial_balances;
    
    for (uint32_t i = 0; i < num; i++) {
        initial_balances.insert(balances_map_t::value_type(create_random_gravity_account_name(), balance));
    }
    
    return initial_balances;
}

