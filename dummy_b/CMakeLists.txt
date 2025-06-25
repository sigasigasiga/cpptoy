add_library(dummy_b)

target_sources(
    dummy_b

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    module.cppm
)

