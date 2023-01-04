enable_testing()

function(AddTests target)
    find_package(GTest CONFIG REQUIRED)    
    target_link_libraries(${target} PUBLIC GTest::gtest GTest::gtest_main)
endfunction()