
//  compiles with g++ -o example example1.cpp -std=c++14 -Wall -I/usr/include/gdal  -L/usr/lib/ -lgdal

#include "../../../quetzal.h"

#include <random>

class GenerativeModel{

private:

  struct Params{
    auto r() const {return m_r;}
    auto sigma() const {return m_sigma;}
    auto mu() const {return m_mu;}

    unsigned int m_r;
    double m_sigma;
    double m_mu;
  };

public:

  using param_type = Params;
  using generator_type = std::mt19937;
  using time_type = unsigned int;
  using key_type = std::string;
  using env_type = quetzal::geography::DiscreteLandscape<key_type, time_type>;
  using coord_type = typename env_type::coord_type;
  using N_type = unsigned int;
  using pop_size_type = quetzal::demography::PopulationSize<coord_type, time_type, N_type>;
  using flux_type = quetzal::demography::PopulationFlux<coord_type, time_type, N_type>;

  auto prior() const {
    auto prior = [](auto& gen){
      Params params;
      params.r(std::uniform_real_distribution<double>(1.,5.)(gen));
      params.sigma(std::uniform_real_distribution<double>(1.,50.)(gen));
      params.mu(std::uniform_real_distribution<double>(1.,10.)(gen));
      return params;
    };
    return prior;
  }

  std::pair<pop_size_type, flux_type>
  operator()(generator_type& gen, Params const& param) const {
      return generateRandomDemography(gen, param);
  }

  std::pair<pop_size_type, flux_type>
  generateRandomDemography(generator_type& gen, Params const& param) const {

    // Real" environment
  	env_type E( {{"bio1","test_data/bio1.tif"},{"bio12","test_data/bio12.tif"}},
  	                    {2001,2002,2003,2004,2005,2006,2007,2008,2009,2010} );

    const auto& X = E.geographic_definition_space();
    const auto& T = E.temporal_definition_space();

    using N_type = unsigned int;
    pop_size_type N;
    N(X.front(), T.front()) = 10;

    flux_type Phi;

    // Building mathematical expressions of space and time
    using quetzal::expressive::literal_factory;
    using quetzal::expressive::use;

    quetzal::expressive::literal_factory<coord_type, time_type> lit;

    // Growth expressions
    auto r = lit(param.r());
    auto k = (use(std::cref(E["bio1"]))+use(std::cref(E["bio12"])))/lit(2);
    auto N_expr = use(std::cref(N));
    auto g = N_expr*(lit(1)+r)/ (lit(1)+((r * N_expr)/k));

    auto sim_N_tilde = [g](generator_type& gen, coord_type const& x, time_type t){
      std::poisson_distribution<N_type> poisson(g(x,t));
      return poisson(gen);
    };

    // Dispersal expressions
    auto gaussian  = [&param](coord_type::km d){
      auto sigma = param.sigma();
      auto mu = param.mu();
      double pi = 3.14159265358979323846;
      return ( 1./(sigma*sqrt(2.*pi))*exp(-pow(d-mu,2.)/(2.*pow(sigma, 2.))));
    };

    auto distance = [](coord_type const& x, coord_type const& y){
      return x.great_circle_distance_to(y);
    };

    // Dispersal law
    auto m = quetzal::expressive::compose(gaussian, distance);
    auto kernel = quetzal::random::make_transition_kernel(X, m);

    // Demographic process
    for(auto t : T){
      for(auto x : X){
        auto N_tilde = sim_N_tilde(gen, x, t);
        for(unsigned int i = 1; i <= N_tilde; ++i){
          coord_type y = kernel(gen, x);
          Phi(x,y,t) += 1;
          time_type t2 = t; ++t2;
          N(y,t2) += 1;
        }
      }
    }
    return std::make_pair(N,Phi);
  }

}; // GenerativeModel

int main(){

  std::mt19937 gen;
  GenerativeModel model;
  //auto abc = abc::make_ABC(model, model.prior());
  //auto table = abc.sample_prior_predictive_distribution(30, gen);

  return 0;
}
