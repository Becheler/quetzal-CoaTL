#include "quetzal/quetzal.hpp"

#include <cassert>
#include <filesystem>
#include <ranges>

namespace geo = quetzal::geography;
using namespace mp_units::si::unit_symbols; // SI system: km, m, s

using kernel_type = quetzal::demography::dispersal_kernel::exponential_power<>;

struct edge_info
{
    using probability_type = kernel_type::pdf_result_type;
    probability_type weight;
    auto compute(auto source, auto target, const auto & grid)
    {
        auto r = grid.to_latlon(source).great_circle_distance_to(grid.to_latlon(target));
        return kernel_type::pdf( r, { .a=200.*km , .b=5.5 } );
    }
    edge_info(auto source, auto target, const auto & grid): weight(compute(source, target, grid)) {}
    edge_info() = default;
};

int main()
{
    auto file1 = std::filesystem::current_path() / "data/bio1.tif";
    auto file2 = std::filesystem::current_path() / "data/bio12.tif";

    // The raster have 10 bands that we will assign to 2001 ... 2010.
    std::vector<int> times(10);
    std::iota(times.begin(), times.end(), 2001);

    // Initialize the landscape: for each var a key and a file, for all a time series.
    using landscape_type = geo::landscape<>;
    auto land = landscape_type::from_file({{"bio1", file1}, {"bio12", file2}}, times);

    // Edges will store the result of the kernel's probability distribution function
    using vertex_info = geo::no_property;

    // We convert the grid to a graph, passing our assumptions
    auto graph = geo::from_grid(land, 
                                vertex_info(), 
                                edge_info(), 
                                geo::connect_fully(), 
                                geo::isotropy(),
                                geo::mirror());

    // Print the result
    for (const auto& e : graph.edges() ) {
        std::cout << e << " : " << graph[e].weight << '\n';
    }
}
