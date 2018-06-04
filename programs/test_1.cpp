
#include <singularity.hpp>

using namespace singularity;
using namespace std;

shared_ptr<matrix_t> create_random_matrix(matrix_t::size_type size, matrix_t::size_type nnz)
{
    shared_ptr<matrix_t> m(new matrix_t(size, size));

    m->reserve(nnz);

    for (uint32_t n = 0; n < nnz; n++) {
        uint32_t i = (int) std::floor(size * drand48());
        uint32_t j = (int) std::floor(size * drand48());
        (*m)(i,j) = 1;
    }
    
    return m;
}

int main(int argc, char** argv)
{
    shared_ptr<matrix_t> m = create_random_matrix(100000, 50000);
    
    m->reserve(50000);
    
    cout << "resize. nnz=" << m->nnz() << endl;

    shared_ptr<matrix_t> m_resized = matrix_tools::resize(*m, 200000, 200000);

    cout << "done." << endl;
}
