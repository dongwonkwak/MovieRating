function(Doxygen input output)
  find_package(Doxygen)
  if (NOT DOXYGEN_FOUND)
    add_custom_target(doxygen COMMAND false
                              COMMENT "Doxygen not found")
    return()
  endif()

  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_HTML_OUTPUT   ${output})

  doxygen_add_docs(doxygen
      ${PROJECT_SOURCE_DIR}/${input}
      ALL
      COMMENT "Generate HTML documentation"
  )
endfunction()
