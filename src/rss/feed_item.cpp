#include "feed_item.hpp"
#include <exception>
#include <functional>

namespace media::rss {
    feed_item::feed_item(std::string_view title_param, std::string_view link_param, std::string_view description_param,
                 std::string_view enclosure_param, std::string_view image_url_param,
                 std::chrono::system_clock::time_point updated_param)
        : title(title_param), link(link_param), description(description_param),
          enclosure(enclosure_param), image_url(image_url_param), updated(updated_param) {}

    std::string feed_item::get_best_media_url() const {
        if (!enclosure.empty()) {
            return enclosure;
        }
        
        for (const auto& media : extracted_media_urls) {
            if (media.type == "video" && 
                (media.format == "mp4" || media.format == "webm" || media.format == "youtube" || media.format == "vimeo")) {
                return media.url;
            }
        }
        
        for (const auto& media : extracted_media_urls) {
            if (media.type == "audio" && 
                (media.format == "mp3" || media.format == "wav" || media.format == "ogg" || media.format == "aac")) {
                return media.url;
            }
        }
        
        for (const auto& media : extracted_media_urls) {
            if (!media.url.empty()) {
                return media.url;
            }
        }
        
        if (link.find("youtube.com") != std::string::npos || link.find("youtu.be") != std::string::npos) {
            return link;
        }
        
        return "";
    }
    
    bool feed_item::has_media() const {
        return !enclosure.empty() || 
               !extracted_media_urls.empty() || 
               link.find("youtube.com") != std::string::npos ||
               link.find("youtu.be") != std::string::npos ||
               link.find("vimeo.com") != std::string::npos;
    }
}
