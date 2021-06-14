#include "rng.h"

namespace Rg
{
int Rng::rand_int(int start, int end)
{
    std::uniform_int_distribution<> rand{start, end};
    return rand(m_engine);
}

int Rng::rand_prob_int(const std::vector<int>& probabilities)
{
    std::discrete_distribution<int> rand(probabilities.begin(), probabilities.end());
    return rand(m_engine);
}

bool Rng::rand_bool()
{
    std::bernoulli_distribution rand{0.5};
    return rand(m_engine);
}

[[nodiscard]] bool Rng::one_in(int number)
{
    std::uniform_int_distribution<> rand{1, number};
    if (rand(m_engine) == 1)
        return true;
    return false;
}

}
