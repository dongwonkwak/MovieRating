#include <spdlog/spdlog.h>
#include "rating/controller/controller.h"
#include "rating/repository/memory/memory.h"
#include "rating/ingester/kafka/ingester.h"

namespace rating::controller
{
    Controller::Controller(
        std::shared_ptr<rating::repository::IRepository> repository,
        std::shared_ptr<rating::ingester::kafka::Ingester> ingester)
        : repository_(repository)
        , ingester_(ingester)
    {

    }

    Controller::~Controller()
    {
        Stop();
    }

    auto Controller::Get(const RecordID& recordID, const RecordType& recordType)
        -> common::expected<RatingSet> 
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
        RatingEventSet events;

        while (!done)
        {
            ingester_->Poll(events);
            if (!events.empty())
            {
                spdlog::info("[Controller::doIngestion] got event");
                for (const auto& event : events)
                {
                    if (event.eventType == model::RatingEventTypePut)
                    {
                        model::Rating rating;
                        rating.userId = event.userId;
                        rating.ratingValue = event.value;
                        rating.recordId = event.recordId;
                        Put(event.recordId, event.recordType, rating);
                    }
                }
            }

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
        isStart_ = false;

        ingestThread_.request_stop();
        if (ingestThread_.joinable())
        {
            ingestThread_.join();
        }
    }
}