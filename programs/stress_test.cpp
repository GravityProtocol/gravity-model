
#include <singularity.hpp>

using namespace singularity;
using namespace std;

void add_random_transactions(activity_index_calculator& ic, uint32_t num_accounts, uint32_t num_blocks, uint32_t block_size, double max_amount)
{
    time_t now = time(nullptr);
    
    for (uint32_t i = 0; i < num_blocks; i++) {
        std::vector<transaction_t> transactions;
        
        for (uint32_t j = 0; j < block_size; j++) {
            std::string src_account  = "A" + std::to_string((int)std::floor(num_accounts * drand48()));
            std::string target_account  = "A" + std::to_string((int)std::floor(num_accounts * drand48()));
            double amount = std::floor(max_amount * drand48());
            if (src_account == target_account || amount < 10) {
                continue;
            }
            transactions.push_back( transaction_t (amount, 0, src_account, target_account, now, 100000, 100000));
        }
        
        cout << "Adding block " << i << endl; 
        
        ic.add_block(transactions);
    }
}

int main(int argc, char** argv)
{
    parameters_t p;
    
    p.num_threads = 4;
    p.account_amount_threshold = 0;
    p.transaction_amount_threshold = 0;
    
    activity_index_calculator c(p);
    
    add_random_transactions(c, 1000000, 100000, 1000, 100000);
    
    cout << "Transactions added" << endl;
    
    account_activity_index_map_t r = c.calculate();
    
    cout << "Complete" << endl;
    cout << r.size() << endl;
}
