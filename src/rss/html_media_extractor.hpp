#pragma once

#include <string>
#include <vector>
#include <regex>
#include <string_view>
#include <algorithm>

namespace media::html {
    
    struct extracted_media {
        std::string url;
        std::string type; // "video", "audio", or "unknown"
        std::string format; // "mp4", "webm", "m3u8", etc.
        
        extracted_media(std::string_view url_val, std::string_view type_val, std::string_view format_val)
            : url(url_val), type(type_val), format(format_val) {}
    };
    
    std::vector<extracted_media> extract_media_urls(std::string_view html_content);
    bool is_media_url(std::string_view url);
    std::string extract_format(std::string_view url);
    std::string normalize_media_url(std::string_view url);
}

// --- Implementation ---

namespace media::html {
    
    inline std::vector<extracted_media> extract_media_urls(std::string_view html_content) {
        std::vector<extracted_media> results;
        try {
            std::string content_str(html_content);
            
            std::vector<std::pair<std::regex, std::string>> patterns = {
                {std::regex(R"(<iframe[^>]+src\s*=\s*[\"']([^\"']*\.(?:mp4|webm|avi|mov|mkv|m4v|3gp|flv)[^\"']*)[\"'][^>]*>)", 
                           std::regex_constants::icase), "video"},
                {std::regex(R"(<video[^>]+src\s*=\s*[\"']([^\"']+)[\"'][^>]*>)", 
                           std::regex_constants::icase), "video"},
                {std::regex(R"(<source[^>]+src\s*=\s*[\"']([^\"']+)[\"'][^>]*type\s*=\s*[\"']video/[^\"']*[\"'][^>]*>)", 
                           std::regex_constants::icase), "video"},
                {std::regex(R"(<audio[^>]+src\s*=\s*[\"']([^\"']+)[\"'][^>]*>)", 
                           std::regex_constants::icase), "audio"},
                {std::regex(R"(<source[^>]+src\s*=\s*[\"']([^\"']+)[\"'][^>]*type\s*=\s*[\"']audio/[^\"']*[\"'][^>]*>)", 
                           std::regex_constants::icase), "audio"},
                {std::regex(R"((?:youtube\.com/embed/|youtu\.be/)([a-zA-Z0-9_-]+))", 
                           std::regex_constants::icase), "video"},
                {std::regex(R"(vimeo\.com/(?:video/)?(\d+))", 
                           std::regex_constants::icase), "video"},
                {std::regex(R"((https?://[^\s<>"']+\.(?:mp4|webm|avi|mov|mkv|m4v|3gp|flv|mp3|wav|ogg|aac|m4a|wma)(?:\?[^\s<>"']*)?))", 
                           std::regex_constants::icase), "unknown"}
            };
            
            for (const auto& [pattern, media_type] : patterns) {
                std::sregex_iterator iter(content_str.begin(), content_str.end(), pattern);
                std::sregex_iterator end;
                
                for (; iter != end; ++iter) {
                    std::string url = iter->str(1);
                    
                    if (media_type == "video" && url.find("youtube") != std::string::npos) {
                        if (url.find("/embed/") != std::string::npos || url.find("youtu.be/") != std::string::npos) {
                            std::regex yt_id_regex(R"((?:embed/|youtu\.be/)([a-zA-Z0-9_-]+))");
                            std::smatch match;
                            if (std::regex_search(url, match, yt_id_regex)) {
                                url = "https://www.youtube.com/watch?v=" + match[1].str();
                            }
                        }
                    }
                    
                    if (media_type == "video" && url.find("vimeo") != std::string::npos) {
                        std::regex vimeo_id_regex(R"(vimeo\.com/(?:video/)?(\d+))");
                        std::smatch match;
                        if (std::regex_search(url, match, vimeo_id_regex)) {
                            url = "https://vimeo.com/" + match[1].str();
                        }
                    }
                    
                    url = normalize_media_url(url);
                    std::string format = extract_format(url);
                    
                    bool duplicate = false;
                    for (const auto& existing : results) {
                        if (existing.url == url) {
                            duplicate = true;
                            break;
                        }
                    }
                    
                    if (!duplicate && !url.empty()) {
                        results.emplace_back(url, media_type, format);
                    }
                }
            }
        }
        catch (...) {
            // Gracefully ignore complexity/stack errors from std::regex on long HTML content
        }
        return results;
    }
    
    inline bool is_media_url(std::string_view url) {
        static const std::vector<std::string> media_extensions = {
            ".mp4", ".webm", ".avi", ".mov", ".mkv", ".m4v", ".3gp", ".flv",
            ".mp3", ".wav", ".ogg", ".aac", ".m4a", ".wma", ".m3u8", ".ts"
        };
        
        std::string url_lower(url);
        std::transform(url_lower.begin(), url_lower.end(), url_lower.begin(), ::tolower);
        
        for (const auto& ext : media_extensions) {
            if (url_lower.find(ext) != std::string::npos) {
                return true;
            }
        }
        
        return url_lower.find("youtube.com") != std::string::npos ||
               url_lower.find("youtu.be") != std::string::npos ||
               url_lower.find("vimeo.com") != std::string::npos ||
               url_lower.find("twitch.tv") != std::string::npos;
    }
    
    inline std::string extract_format(std::string_view url) {
        std::string url_lower(url);
        std::transform(url_lower.begin(), url_lower.end(), url_lower.begin(), ::tolower);
        
        if (url_lower.find(".mp4") != std::string::npos) return "mp4";
        if (url_lower.find(".webm") != std::string::npos) return "webm";
        if (url_lower.find(".avi") != std::string::npos) return "avi";
        if (url_lower.find(".mov") != std::string::npos) return "mov";
        if (url_lower.find(".mkv") != std::string::npos) return "mkv";
        if (url_lower.find(".m4v") != std::string::npos) return "m4v";
        if (url_lower.find(".3gp") != std::string::npos) return "3gp";
        if (url_lower.find(".flv") != std::string::npos) return "flv";
        if (url_lower.find(".mp3") != std::string::npos) return "mp3";
        if (url_lower.find(".wav") != std::string::npos) return "wav";
        if (url_lower.find(".ogg") != std::string::npos) return "ogg";
        if (url_lower.find(".aac") != std::string::npos) return "aac";
        if (url_lower.find(".m4a") != std::string::npos) return "m4a";
        if (url_lower.find(".wma") != std::string::npos) return "wma";
        if (url_lower.find(".m3u8") != std::string::npos) return "hls";
        if (url_lower.find(".ts") != std::string::npos) return "ts";
        
        if (url_lower.find("youtube") != std::string::npos) return "youtube";
        if (url_lower.find("vimeo") != std::string::npos) return "vimeo";
        
        return "unknown";
    }
    
    inline std::string normalize_media_url(std::string_view url) {
        std::string normalized(url);
        
        normalized.erase(0, normalized.find_first_not_of(" \t\n\r"));
        normalized.erase(normalized.find_last_not_of(" \t\n\r") + 1);
        
        if (normalized.starts_with("//")) {
            normalized = "https:" + normalized;
        }
        
        return normalized;
    }
}
