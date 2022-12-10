#include "movie/gateway/metadata.h"
#include "movie/gateway/rating.h"
#include "movie/controller/controller.h"
#include "movie/service/movieservice.h"

#include <iostream>
#include <memory>
#include <string>

using namespace movie;

int main(int argc, char* argv[])
{
    // create meta gateway
    auto metadataGateway = std::make_unique<MetadataGateway>("http://localhost:8081");
    auto ratingGateway = std::make_unique<RatingGateway>("http://localhost:8082");
    auto controller = std::make_unique<Controller>(
        std::move(ratingGateway),
        std::move(metadataGateway));
    const string_t addr = "http://localhost:8083/movie";
    auto service = std::make_unique<MovieService>(std::move(controller), addr);

    ucout << utility::string_t(U("Movie Service Listening for requests at: ")) << addr << std::endl;
    service->start().wait();

    std::string line;
    std::getline(std::cin, line);

    service->stop().wait();
    // create rating gateway
    // create controller
    // create service
    
    //movie::MovieDetails movie;
    return 0;
}