# nbtview
A C++ library for fast, read-only access to data in the NBT format (as used by Minecraft)

The library itself is working but its API is subject to change.

The basic concept is to provide a non-owning view of the data in the NBT structure.

The NBT data is read over to the extent of creating an object for each tag.

## Tag Payloads
* The tag payloads of simple
numeric tags (Int, Long, Float, etc.) are copied.
* The contents of Compound tags and List tags are
rendered as `std::vector<std::unique_ptr<Tag>>`, where Tag is a base class for NBT tags.
* String tags provide a `std::string_view` of their payload.
* Numeric array tags provide a `std::span<T>` of an appropriate numeric type.
  However, the user of this view must take care of the byte ordering themselves.
  
## Tag Names
* The names of all tags are
  provided as a `std::optional<std::string_view>`.
