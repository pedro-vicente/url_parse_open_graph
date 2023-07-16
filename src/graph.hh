#ifndef OG_HH_
#define OG_HH_

#include <string>

// https://ogp.me/
// 
// The four basic open graph tags that are required for each page are og:title, og:type, og:image, and og:url
// og:title - The title of your object as it should appear within the graph, e.g., "The Rock".
// og:type - The type of your object, e.g., "video.movie". Depending on the type you specify, other properties may also be required.
// og:image - An image URL which should represent your object within the graph.
// og:url - The canonical URL of your object that will be used as its permanent ID in the graph, e.g., "https://www.imdb.com/title/tt0117500/".

struct og_t
{
  std::string title;
  std::string type;
  std::string image;
  std::string url;

};

#endif