
#include "include/generator.hpp"

using namespace std;
using namespace singularity;


string generator_abstract::create_random_account_name(string prefix, uint32_t count)
{
    return prefix + to_string( (uint32_t) (count * drand48()));
}

double generator_abstract::create_random_amount(double min, double max)
{
    return min + floor((max - min) * drand48());
}

transaction_set generator_single_rank::generate()
{
    time_t current_time = test_begin;
    
    time_t duration = test_end - test_begin;
    
    uint32_t num_steps = 10;
    
    time_t step = duration / num_steps;
    
    transaction_set ts;
    ts.name = "single rank set";
    
    vector<account_info> accounts;
    
    
    vector<transaction_t> block;
    
    
    
    for (balances_map_t::const_iterator it = initial_balances.cbegin(); it != initial_balances.cend(); it++) {
        accounts.push_back(account_info(it->first, it->second));
    }
    
    for (uint32_t i = 0; i < num_accounts; i++) {
        double balance = floor(max_balance * drand48());
        accounts[0].balance -= balance;
        account_info new_account(create_random_gravity_account_name(), balance); 
        accounts.push_back(new_account);
        block.push_back(transaction_t(balance, 0, accounts[0].name, new_account.name, current_time, 0, 0));
    }
    ts.blocks.push_back(block);
    
    for (uint32_t n = 1; n <= num_steps; n++ ) {
        current_time += step;
        vector<transaction_t> block;
        
        for (uint32_t i = 0; i < num_accounts; i++) {
            double amount = floor(min(accounts[i].balance, max_amount) * drand48());
            uint32_t target = accounts.size() * drand48();
            if (target == i || amount < min_amount || accounts[i].balance < amount + fee) {
                continue;
            }
            accounts[i].balance -= amount + fee;
            accounts[target].balance += amount;
            block.push_back(transaction_t(amount, fee, accounts[i].name, accounts[target].name, current_time, 0, 0));
        }
        
        ts.blocks.push_back(block);
    }
    
    return ts;
    
}


transaction_set generator_multi_rank::generate()
{
    transaction_set ts;
    ts.name = "multi rank set";
    
    vector<transaction_t> transactions;
    
    uint32_t total_value(1000000);
    
    time_t now = time(nullptr);
    
    uint32_t users_count(1000), shop_count(10), xchange_count(5);
    
    double current_value(0);
    
    current_value = 0;
    while (current_value < total_value) {
        double amount = create_random_amount(10, 1000);
        string src = create_random_account_name("u", users_count);
        string trgt = create_random_account_name("s", shop_count);
        transactions.push_back(transaction_t(amount, 0, src, trgt, now, 0, 0));
        
        current_value += amount;
    }

    current_value = 0;
    while (current_value < total_value) {
        double amount = create_random_amount(1000, 100000);
        string src = create_random_account_name("s", shop_count);
        string trgt = create_random_account_name("x", xchange_count);
        transactions.push_back(transaction_t(amount, 0, src, trgt, now, 0, 0));
        
        current_value += amount;
    }

    current_value = 0;
    while (current_value < total_value) {
        double amount = create_random_amount(10, 1000);
        string src = create_random_account_name("x", xchange_count);
        string trgt = create_random_account_name("u", users_count);
        transactions.push_back(transaction_t(amount, 0, src, trgt, now, 0, 0));
        
        current_value += amount;
    }
    
    ts.blocks.push_back(transactions);
    
    return ts;
    
}

transaction_set generator_attack_1::generate()
{
    transaction_set ts;
    time_t now = test_begin;
    
    vector<transaction_t> attacker_block;
        double amount = total_amount / node_count;
    
    for (uint32_t i=0; i < node_count; i++) {
        uint32_t j = (i+1) % node_count;
        string src = "a" + to_string(i);
        string trgt = "a" + to_string(j);
        
        attacker_block.push_back(transaction_t(amount, 0, src, trgt, now, 0, 0));
    }

    attacker_block.push_back(transaction_t(link_amount, 0, "a0", "s1", now, 0, 0));
    attacker_block.push_back(transaction_t(link_amount, 0, "x1", "a0", now, 0, 0));
    
    ts.blocks.push_back(attacker_block);
    
    return ts;
}


transaction_set generator_attack_2::generate()
{
    transaction_set ts;
    vector<transaction_t> attacker_block;
    
    time_t now = test_begin;
    double amount = total_amount / node_count;
    
    for (uint32_t i=0; i < node_count; i++) {
        string src = spamming_node;
        string trgt = "a" + to_string(i);
        
        if (i % 2 == 0) {
            attacker_block.push_back(transaction_t(amount, 0, src, trgt, now, 0, 0));
        } else {
            attacker_block.push_back(transaction_t(amount, 0, trgt, src, now, 0, 0));
        }
    }
    
    ts.blocks.push_back(attacker_block);
    
    return ts;
}
