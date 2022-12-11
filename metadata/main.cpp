#include <iostream>
#include "repository/repository.h"
#include "controller/controller.h"
#include "service/metadataservice.h"
using namespace metadata;

#include <rapidcsv.h>

int main(int argc, char* argv[])
{
    auto repository = std::make_unique<Repository>();
    rapidcsv::Document doc("../data/metadata.csv");
    auto count = doc.GetRowCount();
    for (size_t i = 0; i < count; i++)
    {
        Metadata data;
        auto row = doc.GetRow<std::string>(i);
        data.id = row[0];
        data.title = row[1];
        data.director = row[2];
        data.description = row[3];
        repository->Put(data.id, data);
    }

    //repository->Put("1234", data);
    auto controller = std::make_unique<Controller>(std::move(repository));
    const string_t addr = "http://localhost:8081/metadata";
    auto service = std::make_unique<MetadataService>(std::move(controller), addr);
    ucout << utility::string_t(U("Metadata Service Listening for requests at: ")) << addr << std::endl;
    service->start().wait();

    std::string line;
    std::getline(std::cin, line);

    service->stop().wait();
    return 0;
}