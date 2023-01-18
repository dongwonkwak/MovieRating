#pragma once

#include "common/expected.h"

#include <string>
#include <vector>

namespace discovery
{
    struct Registry
    {
        virtual ~Registry() = default;
        /**
         * @brief 서비스를 등록 인터페이스. 서비스 디스커버리 기능에서 사용한다.
         * 
         * @param serviceId 서비스 고유 아이디. 아래 GenerateServiceID함수를 통해 랜덤하게 생성한다.
         * @param serviceName 서비스 이름
         * @param hostPort 서비스 네트워크 주소
         */
        virtual void Register(
            const std::string& serviceId, 
            const std::string& serviceName, 
            const std::string& hostPort) = 0;
        /**
         * @brief 서비스 등록을 해지하는 함수.
         * 
         * @param serviceId 등록을 해지할 서비스 고유 아이디. Register에서 등록한 값과 동일하다.
         */
        virtual void Deregister(const std::string& serviceId) = 0;
        /**
         * @brief 서비스 가용 여부를 위해 지정된 시간마다 health check를 위해 호출하는 함수
         * 
         * @param serviceId 
         */
        virtual void ReportHealthyState(const std::string& serviceId) = 0;
        /**
         * @brief 가용한 서비스의 네트워크 주소목록을 가져온다.
         * 
         * @param serviceName 조회할 서비스의 이름
         * @return common::expected<std::vector<std::string>> 
         */
        virtual common::expected<std::vector<std::string>> ServiceAddress(const std::string& serviceName) = 0;
    };

    std::string GenerateServiceID(const std::string& serviceName);
}