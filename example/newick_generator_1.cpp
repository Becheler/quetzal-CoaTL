#include "quetzal/quetzal.hpp"

int main()
{
  // Declare a random number generator
  std::mt19937 rng{std::random_device{}()};
  // Generate a random tree from a graph V(10,30)
  auto [tree,root] = quetzal::get_random_spanning_tree<>(10, 30, rng);
  // Generate the newick string
  using Flavor = quetzal::format::newick::TreeAlign;
  auto s = quetzal::format::newick::generate_from(tree, Flavor());

  std::cout << s << std::endl;

  return 0;
}
