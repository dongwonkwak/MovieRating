#pragma once

#include <memory>
#include <string>


namespace config
{
    class Config
    {
    public:
        /**
         * @brief Config객체를 생성하는 helper 함수.
         * 
         * @param filename yaml포맷 형식인 설정 파일 경로.
         * @return std::shared_ptr<config::Config> 
         */
        static std::shared_ptr<config::Config> Create(const std::string& filename = std::string());

        /**
         * @brief 설정파일 경로를 설정한다.
         * 
         * @param filename 
         */
        void Load(const std::string& filename);

        /**
         * @brief 설정파일의 키에 해당되는 값을 리턴하는 함수. subtree를 .으로 구분한다.
         *  application:
         *      name: 
         *  위와 같은 경우 application.name을 키로 값을 얻는다.
         * 
         * @tparam T 리턴 타입 
         * @param k 키값
         * @return T 키에 해당되는 값을 리턴한다.
         */
        template <typename T>
        T get(const std::string& k);
    private:
        Config();
        Config(const std::string& filename);        
    };
}

