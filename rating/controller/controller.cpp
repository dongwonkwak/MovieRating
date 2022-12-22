#include "rating/controller/controller.h"
#include "rating/repository/memory.h"
#include "rating/ingester/kafka/ingester.h"

namespace rating::controller
{
    Controller::Controller(
        std::unique_ptr<rating::repository::Repository> repository,
        std::shared_ptr<rating::ingester::kafka::Ingester> ingester)
        : repository_{std::move(repository)}
        , ingester_(ingester)
    {

    }

    Controller::~Controller()
    {
        Stop();
    }

    auto Controller::Get(const RecordID& recordID, const RecordType& recordType)
        -> common::expected<std::vector<common::expected<model::Rating>>> 
    {
        return repository_->Get(recordID, recordType);
    }

    void Controller::Put(const RecordID& recordId, const RecordType& recordType, const model::Rating& rating)
    {
        repository_->Put(recordId, recordType, rating);
    }

    void Controller::StartIngestion()
    {
        if (ingester_ == nullptr || isStart_ == true)
        {
            return;
        }

        ingestThread_ = std::jthread([&](const std::stop_token& token)
                        {
                            doIngestion(token);
                        });
        isStart_ = true;
    }

    void Controller::doIngestion(const std::stop_token& token)
    {
        bool done = false;

        while (!done)
        {
            auto records = ingester_->Poll();
            records
                .map([&](const auto& record) {
                    std::cout << record.size() << std::endl;
                    for (const auto& r : record)
                    {
                        model::Rating rating;
                        rating.userId = r.userId;
                        rating.ratingValue = r.value;
                        Put(r.recordId, r.recordType, rating);
                    }
                });

            if (token.stop_requested())
            {
                done = true;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void Controller::Stop()
    {
        if (isStart_ == false)
        {
            return;
        }

        ingestThread_.request_stop();
        ingestThread_.join();
        isStart_ = false;
    }
}