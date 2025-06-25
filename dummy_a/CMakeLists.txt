add_library(dummy_a)

target_sources(
    dummy_a

    PUBLIC

    FILE_SET
    CXX_MODULES FILES

    module.cppm
)

