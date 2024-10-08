/*
 * Copyright (c) 2014-2021 Kevin Huck
 * Copyright (c) 2014-2021 University of Oregon
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#include "event_filter.hpp"
#include <regex>
#include <iostream>
#include <fstream>
#include <rapidjson/istreamwrapper.h>

namespace perfstubs {
namespace python {

event_filter::event_filter() : have_filter(false) {
    try {
        const char * filter_file = getenv("PERFSTUBS_PYTHON_FILTER_FILENAME");
        // did the user specify a filter file?
        if (filter_file == nullptr) { return; }
        // try to parse the filter file...
        std::string tmpstr{filter_file};
        std::ifstream cfg(tmpstr);
        if (!cfg.good()) {
            // fail silently, nothing to do but use defaults
            return;
        }
        rapidjson::IStreamWrapper file_wrapper(cfg);
        configuration.ParseStream(file_wrapper);
        cfg.close();
        have_filter = true;
    } catch (...) {
        // fail silently, nothing to do but use defaults
        return;
    }
}

void handle_error(std::regex_error& e) {
    switch (e.code()) {
        case std::regex_constants::error_collate:
            std::cerr << "collate" << std::endl;
            break;
        case std::regex_constants::error_ctype:
            std::cerr << "ctype" << std::endl;
            break;
        case std::regex_constants::error_escape:
            std::cerr << "escape" << std::endl;
            break;
        case std::regex_constants::error_backref:
            std::cerr << "backref" << std::endl;
            break;
        case std::regex_constants::error_brack:
            std::cerr << "brack" << std::endl;
            break;
        case std::regex_constants::error_paren:
            std::cerr << "paren" << std::endl;
            break;
        case std::regex_constants::error_brace:
            std::cerr << "brace" << std::endl;
            break;
        case std::regex_constants::error_badbrace:
            std::cerr << "badbrace" << std::endl;
            break;
        case std::regex_constants::error_range:
            std::cerr << "range" << std::endl;
            break;
        case std::regex_constants::error_space:
            std::cerr << "space" << std::endl;
            break;
        case std::regex_constants::error_badrepeat:
            std::cerr << "badrepeat" << std::endl;
            break;
        case std::regex_constants::error_complexity:
            std::cerr << "complexity" << std::endl;
            break;
        case std::regex_constants::error_stack:
            std::cerr << "stack" << std::endl;
            break;
        default:
            std::cerr << "unknown" << std::endl;
            break;
    }
}

bool event_filter::_exclude(const std::string &name, const std::string &filename) {
    // check if this timer should be explicitly ignored
    if (configuration.HasMember("exclude_timers")) {
        auto & exclude_filter = configuration["exclude_timers"];
        for(auto itr = exclude_filter.Begin(); itr != exclude_filter.End(); ++itr) {
            std::string needle(itr->GetString());
            needle.erase(std::remove(needle.begin(),needle.end(),'\"'),needle.end());
            try {
                std::regex re(needle);
                std::string haystack(name);
                if (std::regex_search(haystack, re)) {
                    return true;
                }
            } catch (std::regex_error& e) {
                std::cerr << "Error: '" << e.what() << "' in regular expression: "
                          << needle << std::endl;
                handle_error(e);
            }
        }
        // not found in the exclude filters
        // ...but don't assume anything yet - check for include list
    }
    // check if this timer should be explicitly ignored based on filename
    if (configuration.HasMember("exclude_files")) {
        auto & exclude_filter = configuration["exclude_files"];
        for(auto itr = exclude_filter.Begin(); itr != exclude_filter.End(); ++itr) {
            std::string needle(itr->GetString());
            needle.erase(std::remove(needle.begin(),needle.end(),'\"'),needle.end());
            try {
                std::regex re(needle);
                std::string haystack(filename);
                if (std::regex_search(haystack, re)) {
                    return true;
                }
            } catch (std::regex_error& e) {
                std::cerr << "Error: '" << e.what() << "' in regular expression: "
                          << needle << std::endl;
                handle_error(e);
            }
        }
        // not found in the exclude filters
        // ...but don't assume anything yet - check for include list
    }
    bool ignored = false;
    // check if this timer should be implicitly ignored
    if (configuration.HasMember("include_timers")) {
        auto & include_filter = configuration["include_timers"];
        for(auto itr = include_filter.Begin(); itr != include_filter.End(); ++itr) {
            std::string needle(itr->GetString());
            needle.erase(std::remove(needle.begin(),needle.end(),'\"'),needle.end());
            try {
                std::regex re(needle);
                std::string haystack(name);
                if (std::regex_search(haystack, re)) {
                    return false;
                }
            } catch (std::regex_error& e) {
                std::cerr << "Error: '" << e.what() << "' in regular expression: "
                          << needle << std::endl;
                handle_error(e);
            }
        }
        // not found in the whitelist
        ignored = true;
    }
    // check if this timer should be implicitly ignored
    if (configuration.HasMember("include_files")) {
        auto & include_filter = configuration["include_files"];
        for(auto itr = include_filter.Begin(); itr != include_filter.End(); ++itr) {
            std::string needle(itr->GetString());
            needle.erase(std::remove(needle.begin(),needle.end(),'\"'),needle.end());
            try {
                std::regex re(needle);
                std::string haystack(filename);
                if (std::regex_search(haystack, re)) {
                    return ignored;
                }
            } catch (std::regex_error& e) {
                std::cerr << "Error: '" << e.what() << "' in regular expression: "
                          << needle << std::endl;
                handle_error(e);
            }
        }
        // not found in the whitelist
        ignored = true;
    }
    return ignored; // no filters
}

bool event_filter::exclude(const std::string &name, const std::string &filename) {
    return instance()._exclude(name, filename);
}

event_filter& event_filter::instance(void) {
    static event_filter _instance;
    return _instance;
}


} // namespace python
} // namespace perfstubs
