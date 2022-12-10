#include <iostream>
#include "repository/memory.h"
#include "controller/controller.h"
#include "service/ratingservice.h"
using namespace rating;

int main(int argc, char* argv[])
{
    auto repository = std::make_unique<Repository>();
    auto controller = std::make_unique<Controller>(std::move(repository));
    const string_t addr = "http://localhost:8082/rating";
    auto service = std::make_unique<RatingService>(std::move(controller), addr);
    ucout << utility::string_t(U("Rating Service Listening for requests at: ")) << addr << std::endl;

    service->start().wait();

    std::string line;
    std::getline(std::cin, line);

    service->stop().wait();

    return 0;
}