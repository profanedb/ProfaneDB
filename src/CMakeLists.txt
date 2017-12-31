add_subdirectory(profanedb)

# Only install headers and proto files
install(DIRECTORY profanedb DESTINATION include
        FILES_MATCHING REGEX "^.*\\.(hpp|h|proto)$")
