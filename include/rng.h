#ifndef RNG_H
#define RNG_H

#include <random>

namespace Rg
{
class Rng
{
public:
    Rng()
        : m_engine(m_rd())
    {
    }

    [[nodiscard]] int rand_int(int start, int end);
    // Each possible value has a predefined probability.
    [[nodiscard]] int rand_prob_int(const std::vector<int>& probabilities);
    [[nodiscard]] bool rand_bool();
    [[nodiscard]] bool one_in(int number);

private:
    std::random_device m_rd;
    std::mt19937 m_engine;
};
}

#endif  // RNG_H
