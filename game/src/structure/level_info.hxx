#pragma once

#include "file_library.hxx"
#include "i18n_string.hxx"

/** @brief Just load a few fields */
class LevelInfo
{
    public:

        unsigned recommended_time;
        I18NString name;
        std::string section;
        FileLibrary::UriReference ref;

        /** @brief Extract level information
         * @param ref Level file to load */
        LevelInfo(FileLibrary::UriReference _ref);
};

